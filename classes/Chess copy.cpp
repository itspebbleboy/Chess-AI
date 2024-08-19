#include "Chess.h"
#include "ChessSquare.h"


const int pieceSize = 64;
//
// add a helper to Square so it returns out FEN chess notation in the form p for white pawn, K for black king, etc.
// this version is used from the top level board to record moves
//
const char Chess::bitToPieceNotation(int row, int column) const
{
    if (row < 0 || row >= 8 || column < 0 || column >= 8) {
        return '0';
    }

    const char *bpieces = { "?PNBRQK" };
    const char *wpieces = { "?pnbrqk" };
    unsigned char notation = '0';
    Bit *bit = _grid[row][column].bit();
    if (bit) { // 0-128 = white pieces, 128-
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()&127];
    } else {
        notation = '0';
    }
    return notation;
}

std::string Chess::pieceNotation(int row, int col) const{
    const char *pieces = {"?PNBRQK"};
    std::string notation = "";
    Bit *bit = _grid[row][col].bit();
    if(bit){
        notation += bit->gameTag() <128 ? "W" : "B";
        notation += pieces[bit->gameTag()&127];
    }else{
        notation = "00";
    }
    return notation; // notation is /COLOR/ & then the /PIECE CHAR/
}

const char Chess::gameTagToPieceNotation(int gameTag) const{
    const char *bpieces = { "?PNBRQK" };
    const char *wpieces = { "?pnbrqk" };
    return gameTag < 128 ? wpieces[gameTag] : bpieces[gameTag&127];
}

// Convert row and column index to chess notation
std::string Chess::indexToNotation(int row, int col) 
{
    return std::string(1, 'a' + col) + std::string(1, '8' - row);
}

int Chess::rowColToIndex(int row, int col){
    return row*8+col;
}
int Chess::notationToIndex(std::string notation) 
{
    return (8*(8-(int)(notation[1]-48)) + (int)(notation[0]-97));
}

int Chess::notationToRow(std::string notation){
    return (8-(notation[1]-48));
}
int Chess::notationToCol(std::string notation){
    return (notation[0]-97);
}

void Chess::printMove(Chess::Move move){
    std::cout<<move.from<<" -> "<<move.to<<std::endl;
}
void Chess::printMoves(){
    std::string lastSrc = "";
    for(Move move: _moves){
        if(lastSrc!= move.from){
            lastSrc=move.from;
            std::cout<<std::endl<<move.from<<" -> "<<move.to<<", ";
        }else{
            std::cout<<" -> "<<move.to;
        }
    }
    std::cout<<std::endl;
}

void Chess::printStateString(std::string state) {
    // The state string represents an 8x8 board
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            char piece = state[row * 8 + col];
            // Check if the square is empty (denoted by '0' in the state string)
            if (piece == '0') {
                std::cout << ".";
            } else {
                std::cout << piece;
            }
            std::cout << " ";
        }
        std::cout << std::endl; // Move to the next row after printing each row
    }
}


Chess::Chess()
{
}

Chess::~Chess()
{
}

//
// make a chess piece for a player
//
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char *pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit *bit = new Bit();
    // should possibly be cached from player class?
    const char *pieceName = pieces[piece - 1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "b_" : "w_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize,pieceSize);
    return bit;
}

void Chess::setUpBoard()
{
    const ChessPiece initialBoard[2][8] = {
        {Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook},  // 1st Rank
        {Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn, Pawn}  // 2nd Rank
    };

    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    for (int y=0; y<_gameOptions.rowY; y++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
            _grid[y][x].initHolder(ImVec2((float)(pieceSize*x + pieceSize),(float)(pieceSize*y + pieceSize)), "boardsquare.png", x, y);
            _grid[y][x].setGameTag(0);
            _grid[y][x].setSize(pieceSize,pieceSize);
            _grid[y][x].setNotation( indexToNotation(y, x) );
        }
    }

    for (int rank=0; rank<2; rank++) {
        for (int x=0; x<_gameOptions.rowX; x++) {
            ChessPiece piece = initialBoard[rank][x];
            Bit *bit = PieceForPlayer(0, initialBoard[rank][x]);
            bit->setPosition(_grid[rank][x].getPosition());
            bit->setParent(&_grid[rank][x]);
            bit->setGameTag( piece + 128);
            _grid[rank][x].setBit( bit );

            bit = PieceForPlayer(1, initialBoard[rank][x]);
            bit->setPosition(_grid[7-rank][x].getPosition()); // 8th & 7th row
            bit->setParent(&_grid[7-rank][x]);
            bit->setGameTag( piece );
            _grid[7-rank][x].setBit( bit );
        }
    }

    if (gameHasAI()) {
        setAIPlayer(_gameOptions.AIPlayer);
    }
    _currPlayer = 'W';
    _moves = genValMoves(stateString(), 'W', _stats, _lastMove);
    printMoves();
    _bestMove = {"", ""};
    startGame();
}

//return true if it is legal for the given bit to be moved from its current holder
bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src)
{
    //Generate list of possible locations for a bit to move from 
    //if there's a valid move for the piece, it can move
    for (std::pair<int,int> pair : _highlightedMoves) {
        const int col = pair.first, row = pair.second;
        getHolderAtC(col, row).setMoveHighlighted(false);
        (static_cast<ChessSquare&>(getHolderAt(col, row))).setMoveHighlighted(false);
    }
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    char pieceType = bitToPieceNotation(srcSquare.getRow(),srcSquare.getColumn());
    if((std::islower(pieceType)&&_currPlayer=='B') ||(std::isupper(pieceType)&&_currPlayer =='W')){
        return false;
    }
    std::vector<Chess::Move> pMoves;
    std::string currN = indexToNotation(srcSquare.getRow(),srcSquare.getColumn());
    for(Chess::Move move : _moves){
        if (move.from == currN){
            pMoves.push_back(move);
            int x = notationToCol(move.to), y = notationToRow(move.to);
            _highlightedMoves.emplace_back(std::make_pair(x, y));
            getHolderAtC(x,y).setMoveHighlighted(true);
        }
    }
    if(pMoves.empty()){
        return false;
    }
    return true;
}

bool Chess::clickedBit(Bit& bit)
{
    return true;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    ChessSquare sbit = static_cast<ChessSquare&>(src);
    ChessSquare dbit = static_cast<ChessSquare&>(dst);
    std::string sourceN = indexToNotation(sbit.getRow(),sbit.getColumn());
    std::string destN = indexToNotation(dbit.getRow(),dbit.getColumn());
    for(Chess::Move move : _moves){
        if (move.from == sourceN && move.to == destN){ 
            return true;
        }
    }
    return false;
}

bool Chess::moveVal(int r, int c, char p, std::string board){ 
    //p is the char representing the orignal piece and r & c are the new proposed location
    if (r >= 0 && r < 8 && c >= 0 && c < 8) {
        // Check if the square is empty or contains an opponent's piece
        if (board[r * 8 + c] == '0' || (islower(p) && isupper(board[r * 8 + c])) || (isupper(p) && islower(board[r * 8 + c]))) {
            return true;
        }
    }
    return false;
}

bool Chess::genKing(std::vector<Move>& moves, int row, int col, char piece, std::string board, Chess::HasMoved stats) {
    // Possible moves for a king
    static const int movesRow[]= {-1, -1, -1,  0, 0,  1, 1, 1};
    static const int movesCol[]= {-1,  0,  1, -1, 1, -1, 0, 1};

    // Check each possible move and add if it's a valid move
    for (int i = 0; i < 8; i++) {
        int r = row + movesRow[i];
        int c = col + movesCol[i];

        if (moveVal(r, c, piece, board)) { // Valid in the basic sense, not considering pinning or other factors
            moves.push_back({indexToNotation(row, col), indexToNotation(r, c)});
        }
    }

    // Castling - White King
    if (piece == 'k' && !stats.WKing) {
        // Check if castling to the right is possible
        if (!stats.WRook && board[63] == 'r' && board[62] == '0' && board[61] == '0') {
            moves.push_back({"e1", "g1"});
        }
        // Check if castling to the left is possible
        if (!stats.WRook && board[56] == '0' && board[57] == '0' && board[58] == '0' && board[59] == 'r') {
            moves.push_back({"e1", "c1"});
        }
    }

    // Castling - Black King
    if (piece == 'K' && !stats.BKing) {
        // Check if castling to the right is possible
        if (!stats.BRook && board[7] == 'R' && board[6] == '0' && board[5] == '0') {
            moves.push_back({"e8", "g8"});
        }
        // Check if castling to the left is possible
        if (!stats.BRook && board[0] == '0' && board[1] == '0' && board[2] == '0' && board[3] == 'R') {
            moves.push_back({"e8", "c8"});
        }
    }

    return false;
}


void Chess::linMoves(std::vector<Move>& moves, int row, int col, char piece, std::string board){
    // Up
    for (int i = 1; row - i >= 0; ++i) {
        int r = row - i;
        int c = col;
        char targetPiece = board[(row - i) * 8 + col];
        
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if (targetPiece != '0'){
            break;
        }
    }
    // Down
    for (int i = 1; row + i < 8; ++i) {
        int r = row + i;
        int c = col;
        char targetPiece = board[(row + i) * 8 + col];
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if (targetPiece != '0'){
            break;
        }
    }
    // Left
    for (int i = 1; col - i >= 0; ++i) {
        int r = row;
        int c = col - i;
        char targetPiece = board[row * 8 + (col - i)];
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if (targetPiece != '0'){
            break;
        }
    }
    // Right
    for (int i = 1; col + i < 8; ++i) {
        int r = row;
        int c = col + i;
        char targetPiece = board[row * 8 + (col + i)];
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if (targetPiece !='0'){
            break;
        }
    }
}

void Chess::diagMoves(std::vector<Move>& moves, int row, int col, char piece, std::string board){
    // Top-left
    for (int i = 1; row - i >= 0 && col - i >= 0; ++i) {
        int r = row - i, c = col - i;
        char targetPiece = board[(row - i) * 8 + (col - i)];
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if (targetPiece !='0'){
            break;
        }
    }
    // Top-right
    for (int i = 1; row - i >= 0 && col + i < 8; ++i) {
        int r = row - i, c = col + i;
        char targetPiece = board[(row - i) * 8 + (col + i)];
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if(targetPiece != '0'){
            break;
        }
    }
    // Bottom-left
    for (int i = 1; row + i < 8 && col - i >= 0; ++i) {
        int r = row + i, c = col - i;
        char targetPiece = board[(row + i) * 8 + (col - i)];
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if(targetPiece != '0'){
            break;
        }
    }
    // Bottom-right
    for (int i = 1; row + i < 8 && col + i < 8; ++i) {
        int r = row + i, c = col + i;
        char targetPiece = board[(row + i) * 8 + (col + i)];
        if (targetPiece == '0' || (islower(piece) && isupper(targetPiece)) || (isupper(piece) && islower(targetPiece))) {
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }if(targetPiece != '0'){
            break;
        }
    }
}

bool Chess::genQueen(std::vector<Move>& moves, int row, int col, char piece, std::string board){
    linMoves(moves, row, col, piece, board);
    diagMoves(moves, row, col, piece, board);
    return false;
}

bool Chess::genRook(std::vector<Move>& moves, int row, int col, char piece, std::string board){
    linMoves(moves, row, col, piece, board);
    return false;
}

bool Chess::genBishop(std::vector<Move>& moves, int row, int col, char piece, std::string board){
    diagMoves(moves, row, col, piece, board);
    return false;
}

bool Chess::genKnight(std::vector<Move>& moves, int row, int col, char piece, std::string board){
    // Possible moves for a knight
    static const int movesRow[]= {2, 1, -1, -2, -2, -1,  1,  2};
    static const int movesCol[]= {1, 2,  2,  1, -1, -2, -2, -1};

    for(int i = 0; i<8; i++){
        int r = row + movesRow[i];
        int c = col + movesCol[i];

        if(moveVal(r, c, piece, board)){ //valid in the 1st sense, not looking into pinning or anything
            moves.push_back({indexToNotation(row,col),indexToNotation(r,c)});
        }
    }
    return false;
}

bool Chess::genPawn(std::vector<Move>& moves, int row, int col, char piece, std::string board, Chess::Move lastMove){
    if(piece == 'P'){ //black piece
        if (board[(row+1) * 8 + col] == '0' ) { //if square below free
            moves.push_back({indexToNotation(row,col),indexToNotation(row+1,col)}); //add one down to moves
            // Check if the pawn is in the original position
            if (row == 1 && (board[(row + 2) * 8 + col] == '0')) { 
                // if pawn's first move is available & can move downwards two squares
                moves.push_back({indexToNotation(row,col),indexToNotation(row+2,col), PAWN2});
            }
        }
        //opponent's piece in the diagonals
        if (col > 0 && std::islower(board[(row + 1) * 8 + col - 1])) {
            moves.push_back({indexToNotation(row,col),indexToNotation(row+1,col-1)});
        }
        if (col < 7 && std::islower(board[(row + 1) * 8 + col + 1])) {
            moves.push_back({indexToNotation(row,col),indexToNotation(row+1,col+1)});
        }
        //EN PASSANT: if last pieced moved was a pawn && we're in the correct row for en passant
        if(row == 4){
            if(lastMove.moveType == 4 && lastMove.from[1] == '2' && lastMove.to[1]=='4'){
                int lastMoveCol = notationToCol(_lastMove.to);
                if(col > 0 && col-lastMoveCol == 1){
                    moves.push_back({indexToNotation(row,col),indexToNotation(row+1,col-1), ENPASSANT});
                }
                if(col < 7 && lastMoveCol-col == 1){
                    moves.push_back({indexToNotation(row,col),indexToNotation(row+1,col+1), ENPASSANT});
                }
            }
        }

    }
    else{
        if (board[(row-1) * 8 + col] == '0' ) { //if square below free
            moves.push_back({indexToNotation(row,col),indexToNotation(row-1,col)});
            // Check if the pawn is in the original position
            if (row == 6 && (board[(row - 2) * 8 + col] == '0')) { 
                // if pawn's first move & available can move downwards two squares
                moves.push_back({indexToNotation(row,col),indexToNotation(row-2,col), PAWN2});
            }
        }
        //opponent's piece in the diagonals
        if (col > 0 && std::isupper(board[(row - 1) * 8 + col - 1])) {
            moves.push_back({indexToNotation(row,col),indexToNotation(row-1,col-1)});
        }
        if (col < 7 && std::isupper(board[(row - 1) * 8 + col + 1])) {
            moves.push_back({indexToNotation(row,col),indexToNotation(row-1,col+1)});
        }
        //EN PASSANT: if last pieced moved was a pawn && we're in the correct row for en passant
        if(row == 3){
            //if opposing pawn moved 2 spaces
            if(lastMove.moveType == 4 && lastMove.from[1] == '7' && lastMove.to[1]=='5'){
                int lastMoveCol = notationToCol(_lastMove.to);
                if(col > 0 && col-lastMoveCol == 1){
                    moves.push_back({indexToNotation(row,col),indexToNotation(row-1,col-1), ENPASSANT});
                }
                if(col < 7 && lastMoveCol-col == 1){
                    moves.push_back({indexToNotation(row,col),indexToNotation(row-1,col+1), ENPASSANT});
                }
            }
        }
    }
    return false;
}

bool Chess::isKingInCheck(std::string state, char color, std::vector<Chess::Move> moves){
    char king = (color == 'W' ? 'k' : 'K');
    int kingIndex = -1;
    for(int i = 0; i< state.length(); i++){
        if (state[i] == king){
            kingIndex = i;
        }
    }
    for(Chess::Move move : moves){
        if(kingIndex == notationToIndex(move.to)){
            return true;
        }
    }
    return false;
}

void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    ChessSquare srcSq = static_cast<ChessSquare&>(src), dstSq = static_cast<ChessSquare&>(dst);
    //UNHIGHLIGHT MOVES
    for (std::pair<int,int> pair : _highlightedMoves) {
        const int col = pair.first, row = pair.second;
        getHolderAtC(col, row).setMoveHighlighted(false);
        (static_cast<ChessSquare&>(getHolderAt(col, row))).setMoveHighlighted(false);
    }

    std::string destNotation = indexToNotation(dstSq.getRow(),dstSq.getColumn());
    std::string srcNotation = indexToNotation(srcSq.getRow(), srcSq.getColumn());
    if(destNotation == _lastEnPassantMove.to && srcNotation == _lastEnPassantMove.from){
        if(dstSq.getRow() == 2){
            _grid[3][dstSq.getColumn()].destroyBit();
        }if(dstSq.getRow() == 5){
            _grid[4][dstSq.getColumn()].destroyBit(); 
        }
        _lastEnPassantMove = {"",""};
    }
    if(destNotation == _lastCastle.to && srcNotation == _lastCastle.from ){
        //if king side castle,
        if (destNotation[0] == 'g') {
            _grid[dstSq.getRow()][7].destroyBit();
            Bit *bit = new Bit();
            // should possibly be cached from player class?
            const char *pieceName = "rook.png";
            std::string spritePath = std::string("chess/") + (_currPlayer == 'B' ? "b_" : "w_") + pieceName;
            bit->LoadTextureFromFile(spritePath.c_str());
            bit->setOwner(_currPlayer == 'W' ? getPlayerAt(1) : getPlayerAt(0));
            bit->setSize(pieceSize,pieceSize);
            bit->setPosition(_grid[dstSq.getRow()][5].getPosition());
            bit->setParent(&_grid[dstSq.getRow()][5]);
            bit->setGameTag(_currPlayer == 'W' ? Rook : Rook + 128);
            _grid[dstSq.getRow()][5].setBit(bit);
        }
        //if queen side
        if (destNotation[0] == 'c') {
            _grid[dstSq.getRow()][0].destroyBit();
            Bit *bit = new Bit();
            // should possibly be cached from player class?
            const char *pieceName = "rook.png";
            std::string spritePath = std::string("chess/") + (_currPlayer == 'B' ? "b_" : "w_") + pieceName;
            bit->LoadTextureFromFile(spritePath.c_str());
            bit->setOwner(_currPlayer == 'W' ? getPlayerAt(1) : getPlayerAt(0));
            bit->setSize(pieceSize,pieceSize);
            bit->setPosition(_grid[dstSq.getRow()][3].getPosition());
            bit->setParent(&_grid[dstSq.getRow()][3]);
            bit->setGameTag(_currPlayer == 'W' ? Rook : Rook + 128);
            _grid[dstSq.getRow()][3].setBit(bit);
        }

        _lastCastle = {"",""};
    }

    std::string state= stateString();
    //saving the last move & piece last moved.
    _lastMove = {indexToNotation(srcSq.getRow(), srcSq.getColumn()),indexToNotation(dstSq.getRow(),dstSq.getColumn())};
    _lastPiece = state[rowColToIndex(dstSq.getRow(),dstSq.getColumn())];

    if (_lastPiece == 'k'){ _stats.WKing = true;}
    if (_lastPiece == 'K'){ _stats.BKing = true;}
    if (_lastPiece == 'r'){ _stats.WRook = true;}
    if (_lastPiece == 'R'){ _stats.BRook = true;}

    std::cout<< "Last Move: " << _lastMove.from << " -> " <<_lastMove.to << " Last Piece: "<< _lastPiece <<std::endl;
    _currPlayer = _currPlayer == 'W' ? 'B' : 'W';
    std::cout<< "CURR PLAYER FOR TURN: "<<_currPlayer<< " state: "<<std::endl;
    printStateString(stateString());
    _moves = genValMoves(stateString(), _currPlayer, _stats, _lastMove);
    printMoves();
    endTurn();
}

std::vector<Chess::Move> Chess::genValMoves(std::string state, char currPlayer, Chess::HasMoved stats, Chess::Move lastMove, bool recordLastMove){
    
    std::vector<Chess::Move> allMoves = generateMoves(state, currPlayer, stats, lastMove);
    auto it = allMoves.begin();
    while (it != allMoves.end()) {
        Chess::Move move = *it;
        //simulating move
        std::string newState = state;
        //string manipulation for castling & en passant
        newState = enactMove(state, currPlayer, move);
        if(move.moveType == 1){
            if(recordLastMove) _lastEnPassantMove = move;
        }
        if(move.moveType == 2){
            if(recordLastMove) _lastCastle = move;
        }
        if(move.moveType == 3){
            if(recordLastMove)_lastCastle = move;
        }
        std::vector<Chess::Move> oppMoves = generateMoves(newState, currPlayer == 'W' ? 'B' : 'W', stats, move);
        bool check = isKingInCheck(newState, currPlayer, oppMoves);
        
        if(check){
            it = allMoves.erase(it);
        }
        else{
            ++it;
        }
    }
    return allMoves;
}

std::vector<Chess::Move> Chess::generateMoves(std::string state, char color, Chess::HasMoved stats, Chess::Move lastMove){
    std::vector<Chess::Move> moves;
    std::string board = state;
    for(int row = 0; row < 8;row++){
        for(int col = 0; col < 8; col++){
            char piece = board[(row*8 + col)];
            //there's a piece there & it belongs to the player inputted
            if(piece != '0' && ( (std::islower(piece) && color == 'W') || (std::isupper(piece) && color == 'B'))){
                switch(piece){
                    case 'K':
                    case 'k':
                        genKing(moves, row, col, piece, board, stats);
                        break;
                    case 'Q':
                    case 'q':
                        genQueen(moves, row, col, piece, board);
                        break;
                    case 'R':
                    case 'r':
                        genRook(moves, row, col, piece, board);
                        break;
                    case 'B':
                    case 'b':
                        genBishop(moves, row, col, piece, board);
                        break;
                    case 'N':
                    case 'n':
                        genKnight(moves, row, col, piece, board);
                        break;
                    case 'P':
                    case 'p':
                        genPawn(moves, row, col, piece, board, lastMove);
                        break;
                }
            }
        }
    }
    return moves;
}

//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

Player* Chess::checkForWinner()
{
    if(_moves.empty()){
        std::cout<<" moves empty "<<std::endl;
        std::string state = stateString();

        std::vector<Chess::Move> oppMoves = generateMoves(state, _currPlayer == 'W' ? 'B' : 'W', _stats, _lastMove);
        if(isKingInCheck(state, _currPlayer, oppMoves)){ //black == 0, white == 1
            return _currPlayer == 'W' ? getPlayerAt(1) : getPlayerAt(0);
        }
    }
    return nullptr;
}

bool Chess::checkForDraw(){
    if(_moves.empty()){
        std::string state = stateString();
        std::vector<Chess::Move> oppMoves = generateMoves(state, _currPlayer == 'W' ? 'B' : 'W', _stats, _lastMove);
        if(isKingInCheck(state, _currPlayer, oppMoves)){ //black == 0, white == 1
            return false;
        }
        return true;
    }
    return false;
}

//
// state strings
//
std::string Chess::initialStateString()
{
    return stateString();
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString() const
{
    std::string s;
    for (int y=0; y<8; y++) {
        for (int x=0; x<8; x++) {
            s += bitToPieceNotation( y, x );
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    // not implemented here
}


// CHESS AI METHODS
std::string Chess::enactMove(std::string state, char currPlayer, Chess::Move move){
    int indexFrom = notationToIndex(move.from), indexTo = notationToIndex(move.to);
    std::string newState = state;
    newState[indexTo] = newState[indexFrom];
    newState[indexFrom] = '0';
    switch(move.moveType){
        case 0:
            break;
        case 1:
            if(currPlayer == 'B'){
                newState[indexTo+8] = '0';
            }else{
                newState[indexTo-8] = '0';
            }
            break;
        case 2:
            if(currPlayer == 'B'){
                newState[indexTo-1] = 'R';
            }else{
                newState[indexTo-1] = 'r';
            }
            break;
        case 3:
            if(currPlayer == 'B'){
                newState[indexTo+1] = 'R';
            }else{
                newState[indexTo+1] = 'r';
            }
            break;
    }
    return newState;
}

//sorts ascending
bool customAscend(const std::pair<int, Chess::Move>& a, const std::pair<int, Chess::Move>& b) {
    return a.first < b.first;
}
//sorts descending
bool customDescend(const std::pair<int, Chess::Move>& a, const std::pair<int, Chess::Move>& b) {
    return a.first > b.first;
}

void Chess::AIMoveBit(Chess::Move& move){
    ChessSquare &src = _grid[notationToRow(move.from)][notationToCol(move.from)];
    ChessSquare &dst = _grid[notationToRow(move.to)][notationToCol(move.to)];
    Bit *bit = src.bit();
    dst.dropBitAtPoint(bit, dst.getPosition());
    src.draggedBitTo(bit, nullptr);
    bitMovedFromTo(*bit, src, dst);
}

void Chess::updateAI(){
    
    if(_AIInProgress) { return; }

    if(_AIDone){
        AIMoveBit(_bestMove);
        _AIDone = false;
        return;
    }

    _AIInProgress = true;
    std::vector<std::pair<int, Chess::Move>> moveScores; // store score and move
    std::string state = stateString();
    if(!_moves.empty()){
        for(Chess::Move move : _moves){
            std::string newState = enactMove(state, _currPlayer, move);
            int score = minimaxAlphaBetaSorted(newState, 0, false, INT_MIN, INT_MAX, _stats, move);
            moveScores.push_back({score, move});
        }
        std::sort(moveScores.begin(), moveScores.end(), customDescend);
        //after sort get first elem make move, _bestMove
        _bestMove = moveScores.front().second;
        std::cout<< "ai chose move w/ score: "<< moveScores.front().first <<" move: ";
        printMove(_bestMove);
        _AIDone = true;
        _AIInProgress = false;
    }else{
        _bestMove = {"",""};
        _AIDone=true;
        _AIInProgress = false;
    }
}




int Chess::minimaxAlphaBetaSorted(std::string state, int depth, bool maximizingPlayer, int alpha, int beta, Chess::HasMoved stats, Chess::Move lastMove) 
{
    char currPlayer = maximizingPlayer? AI : nonAI;
    char currOpponent = maximizingPlayer? nonAI : AI;
    //std::cout<<"minimaxA/B called: "<< "depth: "<<depth<< ", currPlayer: "<< currPlayer <<" maxim player?: " << maximizingPlayer<< " move: ";
    //printMove(lastMove);
    int score = AI == 'B' ? -1*evalBoard(state, AI) : evalBoard(state,AI);
    
    depth++;
    // IF DEPTH > MAXDEPTH || KING IS IN CHECK RETURN SCORE
    if (depth > maxDepth) {
        //std::cout<<"reached max depth, depth: "<<depth<< ", currPlayer: "<< currPlayer <<" maxim player?: " << maximizingPlayer<< " move: ";
        //printMove(lastMove);
        return score;
    }
    std::vector<Chess::Move> moves = genValMoves(state, currPlayer, stats, lastMove, false);
    if(moves.empty()){
        std::vector<Chess::Move> oppMoves = genValMoves(state, currOpponent, stats, lastMove, false);
        if(isKingInCheck(state, currPlayer, oppMoves)){
            std::cout << "our king in check, score: " << score <<" maxim player? " << maximizingPlayer<<" depth: "<<depth<< " state:"<<std::endl;
            printStateString(state);
            return score;
        }
    }
    /*if(oppMoves.empty()){
        if(isKingInCheck(state, currOpponent, moves)){
            std::cout << "their king in check, score: " << score <<" maxim player? " << maximizingPlayer<<" depth: "<<depth<< " state:"<<std::endl;
            printStateString(state);
            return score+10;
        }
    }*/
    std::vector<std::pair<int, Chess::Move>> moveScores; // store score and move
    for (auto& move : moves){
        std::string newState = enactMove(state, currPlayer, move);
        int moveScore = AI == 'B' ? evalBoard(newState, currPlayer) : -1*evalBoard(newState, currPlayer);
        //std::cout<<"for player: "<<currPlayer<<" pushing move score: " << moveScore<<" for move: ";
        //printMove(move);
        moveScores.push_back({moveScore, move});
    }
    // for each: Sort moves: If isMaximizingPlayer, sort descending. Otherwise, sort ascending
    //      init bestVal
    std::string bestValState = "";
    if(maximizingPlayer){
        std::sort(moveScores.begin(), moveScores.end(), customDescend);
        int bestVal = -1000;
        for (auto& move : moveScores) {
            std::string newState = enactMove(state, currPlayer, move.second);
            HasMoved newStats = stats;
            switch (newState[notationToIndex(move.second.from)]){
                case 'k':
                    newStats.WKing = true;
                    break;
                case 'K':
                    newStats.BKing = true;
                    break;
                case 'r':
                    newStats.WRook = true;
                    break;
                case 'R':
                    newStats.BRook = true;
                    break;
            }
            int val =  minimaxAlphaBetaSorted(newState, depth, false, alpha, beta, newStats, move.second);
            if(val > bestVal){
                bestValState = newState;
                bestVal = val;
            }
            //bestVal = std::max(bestVal, minimaxAlphaBetaSorted(newState, depth, false, alpha, beta, newStats, move.second));
            alpha = std::max(alpha, bestVal);
            if (beta <= alpha) {
                //std::cout<<"a/b cutoff: depth: "<<depth<< ", currPlayer: "<< currPlayer <<" maxim player?: " << maximizingPlayer<< " bestVal: "<<bestVal <<" curr move: ";
                //printMove(move.second);
                return bestVal; // Beta cut-off
            }
        }
        if(depth ==1){
            std::cout<<"reached end, depth: "<<depth<< ", currPlayer: "<< currPlayer <<" maxim player?: " << maximizingPlayer<< " bestVal: "<<bestVal <<" move: ";
            printMove(lastMove);
            //std::cout<<"best val state"<<std::endl;
            //printStateString(bestValState);
        }
        return bestVal;
    }else{
        std::sort(moveScores.begin(), moveScores.end(), customAscend);
        int bestVal = 1000;
        for (auto& move : moveScores) {
            std::string newState = enactMove(state, currPlayer, move.second);
            HasMoved newStats = stats;
            switch (newState[notationToIndex(move.second.from)]){
                case 'k':
                    newStats.WKing = true;
                    break;
                case 'K':
                    newStats.BKing = true;
                    break;
                case 'r':
                    newStats.WRook = true;
                    break;
                case 'R':
                    newStats.BRook = true;
                    break;
            }            
            int val = minimaxAlphaBetaSorted(newState, depth, true, alpha, beta, newStats, move.second);
            if(val < bestVal){
                bestValState = newState;
                bestVal = val;
            }
            //bestVal = std::min(bestVal, minimaxAlphaBetaSorted(newState, depth, true, alpha, beta, newStats, move.second));
            beta = std::min(beta, bestVal);
            if (beta <= alpha) {
                //std::cout<<"a/b cutoff: depth: "<<depth<< ", currPlayer: "<< currPlayer <<" maxim player?: " << maximizingPlayer<< " bestVal: "<<bestVal <<" curr move: ";
                //printMove(move.second);
                return bestVal; // Beta cut-off
            }
        }
        if(depth == 1) {
            std::cout<<"reached end, depth: "<<depth<< ", currPlayer: "<< currPlayer <<" maxim player?: " << maximizingPlayer<< " bestVal: "<<bestVal <<" move: ";
            printMove(lastMove);
            //std::cout<<"best val state"<<std::endl;
            //printStateString(bestValState);
        }
        return bestVal;
    }
    std::cout<< "Something broke?"<<std::endl;
    return 0;
}

int Chess::evaluateBoard(const char* stateString){
    int score = 0;
    for(int i =0; i<64; i++){
        char p = stateString[i];
        if (p != '0') {
            int mod = i % 8;
            int pos = std::islower(p)? i : 56-(i-mod)+mod;
            int coeff = std::islower(p)? 1 : -1;
            switch(p){
            case 'K':
            case 'k':
                score += coeff*kingTable[pos];
                score += coeff*2000;
                break;
            case 'Q':
            case 'q':
                score += coeff*queenTable[pos];
                score += coeff*900;
                break;
            case 'R':
            case 'r':
                score += coeff*rookTable[pos];
                score += coeff*500;
                break;
            case 'B':
            case 'b':
                score += coeff*bishopTable[pos];
                score += coeff*330;
                break;
            case 'N':
            case 'n':
                score += coeff*knightTable[pos];
                score +=coeff*320;
                break;
            case 'P':
            case 'p':
                score += coeff*pawnTable[pos];
                score += coeff*100;
                break;
            }
        }        
    }
    return score;
}

int Chess::evalBoard(std::string state, char color)
{
    int score = 0;
    for(int i = 0; i <64; i++){
        char p = state[i];
        if (p != '0') {
            int mod = i % 8;
            int pos = std::islower(p)? i : 56-(i-mod)+mod;
            int coeff = std::islower(p)? 1 : -1;
            switch(p){
            case 'K':
            case 'k':
                score += coeff*kingTable[pos];
                score += coeff*2000;
                break;
            case 'Q':
            case 'q':
                score += coeff*queenTable[pos];
                score += coeff*900;
                break;
            case 'R':
            case 'r':
                score += coeff*rookTable[pos];
                score += coeff*500;
                break;
            case 'B':
            case 'b':
                score += coeff*bishopTable[pos];
                score += coeff*330;
                break;
            case 'N':
            case 'n':
                score += coeff*knightTable[pos];
                score +=coeff*320;
                break;
            case 'P':
            case 'p':
                score += coeff*pawnTable[pos];
                score += coeff*100;
                break;
            }
        }        
    }
    return score;
}




#include "Chess.h"
#include "ChessSquare.h"
#include "BitBoard.h"
#include "utility.h"

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

void Chess::printMove(bMove move){
    std::cout<<SQSTR[move.from]<<" -> "<<SQSTR[move.to]<<std::endl;
}

void Chess::printMoves(){
    //std::sort(_moves.begin(), _moves.end(), mCustomAscend);
    std::string lastSrc = "";
    for(bMove move: _moves){
        if(lastSrc!= SQSTR[move.from]){
            lastSrc=SQSTR[move.from];
            std::cout<<std::endl<<SQSTR[move.from]<<" -> "<<SQSTR[move.to]<<", ";
        }else{
            std::cout<<" -> "<<SQSTR[move.to];
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

    //_moves = genValMoves(stateString(), 'W', _stats, _lastMove);
    //printStateString(stateString());
    //_board = new Bitboard();
    init_sqs_between();
    _board.createStateFromString(stateString());
    //std::cout<<stateString()<<std::endl;
    _bestMove = {64, 64};
    _board.generateLegalMoves(_moves, 0);

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
    _highlightedMoves.erase(_highlightedMoves.begin(),_highlightedMoves.end());
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    char pieceType = bitToPieceNotation(srcSquare.getRow(),srcSquare.getColumn());
    if((std::islower(pieceType)&&_currPlayer=='B') ||(std::isupper(pieceType)&&_currPlayer =='W')){
        return false;
        std::cout<<"wrong player \n";
    }
    std::string currN = indexToNotation(srcSquare.getRow(),srcSquare.getColumn());
    for(bMove move : _moves){
        if (SQSTR[move.from] == currN){
            int x = notationToCol((SQSTR[move.to])), y = notationToRow((SQSTR[move.to]));
            _highlightedMoves.emplace_back(std::make_pair(x, y));
            getHolderAtC(x,y).setMoveHighlighted(true);
        }
    }
    if(_highlightedMoves.empty()){
        return false;
        std::cout<<"move empty\n";
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
    for(bMove move : _moves){
        if (SQSTR[move.from] == sourceN && SQSTR[move.to] == destN){ 
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
bool Chess::isKingInCheck(std::string state, char color, std::vector<bMove> moves){
    char king = (color == 'W' ? 'k' : 'K');
    int kingIndex = -1;
    for(int i = 0; i< state.length(); i++){
        if (state[i] == king){
            kingIndex = i;
        }
    }
    for(bMove move : moves){
        if(kingIndex == move.to){
            return true;
        }
    }
    return false;
}

void Chess::updateBoard(){
    for (int row = 0; row < 8; ++row) { //when i=0, row=7 & col=0 : when i= 63, row=0 & col=7
        int r = 56-row*8;
        for (int col = 0; col < 8; ++col) {
            int i = r + col;
            char p = _board.getPieceAt(i);
            int add = std::islower(p) ? 0 : 128;
            Bit *bit = new Bit();
            const char *pieceName;
            std::string spritePath;
            switch (p){
                case '0': //no piece
                    bit = NULL;
                    _grid[row][col].destroyBit();
                    break;
                case 'P':
                case 'p':
                    pieceName = "pawn.png";
                    spritePath = std::string("chess/") + (p  == 'P' ? "b_" : "w_") + pieceName;
                    bit->LoadTextureFromFile(spritePath.c_str());
                    bit->setGameTag(Pawn + add);
                    break;
                case 'N':
                case 'n':
                    pieceName = "knight.png";
                    spritePath = std::string("chess/") + (p == 'N' ? "b_" : "w_") + pieceName;
                    bit->LoadTextureFromFile(spritePath.c_str());
                    bit->setGameTag(Knight + add);
                    break;
                case 'B':
                case 'b':
                    pieceName = "bishop.png";
                    spritePath = std::string("chess/") + (p == 'B' ? "b_" : "w_") + pieceName;
                    bit->LoadTextureFromFile(spritePath.c_str());
                    bit->setGameTag(Bishop + add);
                    break;
                case 'R':
                case 'r':
                    pieceName = "rook.png";
                    spritePath = std::string("chess/") + (p == 'R' ? "b_" : "w_") + pieceName;
                    bit->LoadTextureFromFile(spritePath.c_str());
                    bit->setGameTag(Rook + add);
                    break;
                case 'Q':
                case 'q':
                    pieceName = "queen.png";
                    spritePath = std::string("chess/") + (p == 'Q' ? "b_" : "w_") + pieceName;
                    bit->LoadTextureFromFile(spritePath.c_str());
                    bit->setGameTag(Queen + add);
                    break;
                case 'K':
                case 'k':
                    pieceName = "king.png";
                    spritePath = std::string("chess/") + (p == 'K' ? "b_" : "w_") + pieceName;
                    bit->LoadTextureFromFile(spritePath.c_str());
                    bit->setGameTag(King + add);
                    break;         
            }
            //if(bit.gameTag() != 0){
            if(bit != NULL){
                bit->setOwner(add == 0 ? getPlayerAt(1) : getPlayerAt(0));
                bit->setSize(pieceSize,pieceSize);
                bit->setPosition(_grid[row][col].getPosition());
                bit->setParent(&_grid[row][col]);
                _grid[row][col].setBit(bit);
            }
        }
    }
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

    MoveType moveType = DEFAULT;
    int srcI = 56-srcSq.getRow()*8 + srcSq.getColumn();
    int dstI = 56-dstSq.getRow()*8 + dstSq.getColumn();
    std::string destNotation = indexToNotation(dstSq.getRow(),dstSq.getColumn());
    std::string srcNotation = indexToNotation(srcSq.getRow(), srcSq.getColumn());
    bMove newMove = _board.findMove(_moves, srcI, dstI);
    _board.move(newMove);
    _lastMove = newMove;
    _board._lastMove = newMove;
    updateBoard();
    std::cout<< "Last Move: " << SQSTR[_lastMove.from] << " -> " <<SQSTR[_lastMove.to] << " move type "<< _lastMove.moveType <<std::endl;
    _currPlayer = _currPlayer == 'W' ? 'B' : 'W';
    _moves.erase(_moves.begin(),_moves.end());
    _board.generateLegalMoves(_moves, _currPlayer == 'W' ? 0 : 1);
    printMoves();
    endTurn();
}
//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

Player* Chess::checkForWinner()
{   
    std::cout<<"in check for winner\n";
    if(!(_moves.empty())) return nullptr;
    std::cout<<"moves empty\n";
    if(_board.isKingInCheck( _currPlayer == 'W' ? '0' : '1')){
        return _currPlayer == 'W' ? getPlayerAt(1) : getPlayerAt(0);  //black == 1, white == 0
    }
    else return nullptr;
}

bool Chess::checkForDraw(){
    if(_moves.empty()){
        if(_board.isKingInCheck( _currPlayer == 'W' ? '0' : '1')){
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
std::string Chess::enactMove(std::string state, char currPlayer, bMove move){
    int indexFrom = notationToIndex(SQSTR[move.from]), indexTo = notationToIndex(SQSTR[move.to]);
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
bool customAscend(Chess::scoreMove& a, Chess::scoreMove& b) {
    return a.score < b.score;
}
//sorts descending
bool customDescend(Chess::scoreMove& a, Chess::scoreMove& b) {
    return a.score > b.score;
}

void Chess::AIMoveBit(bMove& move){
    ChessSquare &src = _grid[notationToRow(SQSTR[move.from])][notationToCol(SQSTR[move.from])];
    ChessSquare &dst = _grid[notationToRow(SQSTR[move.to])][notationToCol(SQSTR[move.to])];
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
    std::vector<scoreMove> moveScores; // store score and move
    std::string state = stateString();

    if(!_moves.empty()){
        for(bMove move : _moves){
            Bitboard *newState = new Bitboard();
            *newState = _board.copy();
            //std::string newState = enactMove(state, _currPlayer, move);
            newState->move(move);
            int score = minimaxAlphaBetaSorted(*newState, 0, false, INT_MIN, INT_MAX);
            moveScores.push_back({score, move});
            delete newState;
        }
        std::sort(moveScores.begin(), moveScores.end(), customDescend);
        //after sort get first elem make move, _bestMove
        _bestMove = moveScores.front().move;
        std::cout<< "ai chose move w/ score: "<< moveScores.front().score <<" move: ";
        printMove(_bestMove);
        _AIDone = true;
        _AIInProgress = false;
    }else{
        _bestMove = {64,64};
        _AIDone=true;
        _AIInProgress = false;
    }
}

int Chess::minimaxAlphaBetaSorted(Bitboard state, int depth, bool maximizingPlayer, int alpha, int beta) {
    int currPlayer = maximizingPlayer ? _AI : _nonAI;
    depth++;
    if (depth > maxDepth) {// eval score when hit depth limit
        return (maximizingPlayer ? 1 : -1)* (state.evalBoard(currPlayer));
    }
    std::vector<bMove> moves;
    state.generateLegalMoves(moves, currPlayer);
    if (moves.empty()) {// check for checkmate or stalemate
        if (state.isKingInCheck(currPlayer)) {
            return (maximizingPlayer ? 1 : -1)* ((state.evalBoard(currPlayer)));
        }
        return 0;  // ret 0 if stalemate
    }
    std::vector<scoreMove> moveScores;
    for (auto& move : moves) {
        Bitboard *newState = new Bitboard();
        *newState = state.copy();
        newState->move(move);
        int moveScore = newState->evalBoard(currPlayer);
        moveScores.push_back({moveScore, move, newState});
    }
    bMove bestMove;
    if(maximizingPlayer){
        std::sort(moveScores.begin(), moveScores.end(), customDescend);
        int bestVal = -1000;
        for (auto& moveScore : moveScores) {
            Bitboard newState = state;
            int val = minimaxAlphaBetaSorted(*moveScore.state, depth, false, alpha, beta);
            if (val > bestVal) {
                bestVal = val;
                bestMove = moveScore.move;
            }
            alpha = std::max(alpha, bestVal);
            if (beta <= alpha) {
                return bestVal;
            }
        }
        if (depth == 1) {
            std::cout << "Reached end, depth: " << depth << ", currPlayer: " << currPlayer
                    << " maxim player?: " << maximizingPlayer << " bestVal: " << bestVal 
                    << " for move: ";
            printMove(state._lastMove);
        }
        return bestVal;
    }
    else{
        std::sort(moveScores.begin(), moveScores.end(), customAscend);
        int bestVal = 1000;
        for (auto& moveScore : moveScores) {
            Bitboard newState = state;  // Use copy constructor or assignment operator
            int val = minimaxAlphaBetaSorted(*moveScore.state, depth, true, alpha, beta);
            if (val < bestVal) {
                bestVal = val;
                bestMove = moveScore.move;
            }
            beta = std::min(beta, bestVal);
            if (beta <= alpha) {
                return bestVal;
            }
        }
        if (depth == 1) {
            std::cout << "Reached end, depth: " << depth << ", currPlayer: " << currPlayer
                    << " maxim player?: " << maximizingPlayer << " bestVal: " << bestVal 
                    << " best move: ";
            printMove(state._lastMove);
        }
        return bestVal;
    }
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
    //std::cout<<state<<"\n";
    //printStateString(state);
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
    //std::cout<<"score: "<<score<<"\n";
    return score;
}




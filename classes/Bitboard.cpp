#include "Chess.h"
#include "BitBoard.h"
#include <bitset>

    char Bitboard::getPieceAt(uint64_t square) {
        // Check if the square contains a piece and return its character
        if (get_bit(boards[wPAWN], square)) return 'p';
        if (get_bit(boards[bPAWN]  , square)) return 'P';
        if (get_bit(boards[wKNIGHT], square)) return 'n';
        if (get_bit(boards[bKNIGHT], square)) return 'N';
        if (get_bit(boards[wBISHOP], square)) return 'b';
        if (get_bit(boards[bBISHOP], square)) return 'B';
        if (get_bit(boards[wROOK]  , square)) return 'r';
        if (get_bit(boards[bROOK]  , square)) return 'R';
        if (get_bit(boards[wQUEEN] , square)) return 'q';
        if (get_bit(boards[bQUEEN] , square)) return 'Q';
        if (get_bit(boards[wKING]  , square)) return 'k';
        if (get_bit(boards[bKING]  , square)) return 'K';
        return '0'; // Empty square
    }

    void Bitboard::printBoard() {
        std::cout<<"printing bitboard\n";
        
        uint64_t bit = 1ULL << 63; // Start from the top-left corner
        //uint64_t bit = 1ULL; // Start from the top-left corner

        for (int i = 56; i >= 0; i -= 8) {
		    for (int j = 0; j < 8; j++){
                char piece = getPieceAt(i+j); // Get the piece at this square
                std::cout << piece << " ";
            }
            std::cout<<"\n";
        }
    }
    void Bitboard::printBoardStraight() {
        std::cout<<"printing bitboard\n";
        
        //uint64_t bit = 1ULL << 63; // Start from the top-left corner
        uint64_t bit = 1ULL; // Start from the top-left corner

        for (int i = 0; i < 64 ; i ++) {
            char piece = getPieceAt(i); // Get the piece at this square
            std::cout << piece << " ";
        }
        std::cout<<"\n";
    }

    void Bitboard::printBitBoard(uint64_t bitBoard) {
        for (int i = 0; i < 64 ; i ++) {
            //std::cout << ((bitBoard << i) & 1 ? 'X' : '.') << ' ' ;
            //std::cout << get_bit(bitBoard, i) << ' ' ;
            std::cout << ((bitBoard >> i) & 1 ? 'X' : '.')<< ' ';
        }
        std::cout << std::endl;
    }
    
    //sorts ascending
    bool bCustomAscend(bMove& a, bMove& b) {
        return a.from < b.from;
    }
    //sorts descending
    bool bCustomDescend(bMove& a, bMove& b) {
        return a.from > b.from;
    }

    void printMoves(std::vector<bMove> &moves){
        std::sort(moves.begin(),moves.end(),bCustomAscend);
        int lastSrc = NO_SQUARE;
        for(bMove move: moves){
            if(lastSrc!= move.from){
                lastSrc=move.from;
                std::cout<<std::endl<<SQSTR[move.from]<<" -> "<<SQSTR[move.to]<<", ";
            }else{
                std::cout<<" -> "<<SQSTR[move.to];
            }
        }
        std::cout<<std::endl;
    }
    
    void Bitboard::setPiece(char p, int square) {
        uint64_t bit = 1ULL << square;
        switch (p) {
            case 'p': boards[wPAWN]     |= bit; break;
            case 'n': boards[wKNIGHT]   |= bit; break;
            case 'b': boards[wBISHOP]   |= bit; break;
            case 'r': boards[wROOK]     |= bit; break;
            case 'q': boards[wQUEEN]    |= bit; break;
            case 'k': boards[wKING]     |= bit; break;
            case 'P': boards[bPAWN]     |= bit; break;
            case 'N': boards[bKNIGHT]   |= bit; break;
            case 'B': boards[bBISHOP]   |= bit; break;
            case 'R': boards[bROOK]     |= bit; break;
            case 'Q': boards[bQUEEN]    |= bit; break;
            case 'K': boards[bKING]     |= bit; break;
            //case '0': 0x0000000000000000 &= bit break;
            default: break;
        }
    }

    PieceType Bitboard::getPieceTypeAt(uint64_t square) {
        // Check if the square contains a piece and return its character
        if (get_bit(boards[wPAWN]  , square)) return wPAWN;
        if (get_bit(boards[wKNIGHT], square)) return wKNIGHT;
        if (get_bit(boards[wBISHOP], square)) return wBISHOP;
        if (get_bit(boards[wROOK]  , square)) return wROOK;
        if (get_bit(boards[wQUEEN] , square)) return wQUEEN;
        if (get_bit(boards[wKING]  , square)) return wKING;
        if (get_bit(boards[bPAWN]  , square)) return bPAWN;
        if (get_bit(boards[bKNIGHT], square)) return bKNIGHT;
        if (get_bit(boards[bBISHOP], square)) return bBISHOP;
        if (get_bit(boards[bROOK]  , square)) return bROOK;
        if (get_bit(boards[bQUEEN] , square)) return bQUEEN;
        if (get_bit(boards[bKING]  , square)) return bKING;
        return EMPTY; // Empty square
    }

    int Bitboard::colorAt(uint64_t square) {
        // Check if the square contains a piece and return its character
        if (get_bit(boards[wPAWN]  , square)) return 0;
        if (get_bit(boards[bPAWN]  , square)) return 1;
        if (get_bit(boards[wKNIGHT], square)) return 0;
        if (get_bit(boards[bKNIGHT], square)) return 1;
        if (get_bit(boards[wBISHOP], square)) return 0;
        if (get_bit(boards[bBISHOP], square)) return 1;
        if (get_bit(boards[wROOK]  , square)) return 0;
        if (get_bit(boards[bROOK]  , square)) return 1;
        if (get_bit(boards[wQUEEN] , square)) return 0;
        if (get_bit(boards[bQUEEN] , square)) return 1;
        if (get_bit(boards[wKING]  , square)) return 0;
        if (get_bit(boards[bKING]  , square)) return 1;
        return -1;
    }

    bMove Bitboard::findMove(std::vector<bMove> &moves, int src, int dst){
        for(bMove move: moves){
            if(src == move.from){
                if(dst == move.to) return move;
            }
        }
        return {};
    }

    void Bitboard::clearBitboards() {
        for (int i = 0; i < 12; ++i) {
            boards[i] = 0ULL; // Set all bitboards to 0
        }
    }

    // Function to convert state string to Bitboard instance
    void Bitboard::createStateFromString(const std::string& state) {
        clearBitboards();
        int square = 0;
        for (int i = 56; i >= 0; i -= 8) {
		    for (int j = 0; j < 8; j++){
                char piece = state[64-(i+(8-j))];
                this -> setPiece(piece, i+j);
            }
        }
    }
    std::string Bitboard::stateString(){
        //uint64_t bit = 1ULL; // Start from the top-left corner
        std::string ret = "0000000000000000000000000000000000000000000000000000000000000000";
        for (int i = 56; i >= 0; i -= 8) {
		    for (int j = 0; j < 8; j++){
                ret[64-(i+(8-j))]= getPieceAt(i+j);
            }
        }
        return ret;
    }
    bool Bitboard::isKingInCheck(int side){ //white == 0, black == 1
        int opponent = side == 0 ? 1 : 0;
        uint64_t us = friendly_occupants(side);
        uint64_t them = friendly_occupants(opponent);
        uint64_t all = us | them;
        int kingsq = get_LSB(boards[wKING+side*6]);
        int oppCoeff = opponent*6;
        //uint64_t danger = 0;
        //danger |= all_pawn_attacks(boards[wPAWN+oppCoeff], them, us, opponent) | king_attacks(kingsq, all); 
        uint64_t checkers = (knight_attacks(kingsq, us) & boards[wKNIGHT + oppCoeff]) | (pawn_attacks(kingsq, side) & boards[wPAWN + oppCoeff]);

        uint64_t their_bishop_sliders = bishop_attacks(kingsq, all);
        uint64_t their_rook_sliders = rook_attacks(kingsq, all);

        uint64_t candidates = (their_bishop_sliders & (boards[wBISHOP + oppCoeff] | boards[wQUEEN + oppCoeff])) |
                                (their_rook_sliders & (boards[wROOK + oppCoeff] | boards[wQUEEN + oppCoeff]));
        while (candidates) {
            int s = pop_LSB(candidates);
            uint64_t b1 = SQ_BETWEEN_BB[kingsq][s] & us;
            if (b1 == 0){
                checkers ^= SQ_BB[s];
            }
        }
        return checkers != 0;
    }

    bool Bitboard::legalMove(bMove move){
        Bitboard bitboard = *this;
        bitboard.move(move);
        if(bitboard.isKingInCheck(bitboard.colorAt(move.to))){
            return false;
        }
        return true;
    }
    void Bitboard::generateLegalMoves(std::vector<bMove> &moves, int color){
        std::vector<bMove> everymove;
        generatePsuedoMoves(everymove, color);
        for(auto &move: everymove){
            if(legalMove(move)) moves.push_back(move);
            if(move.moveType == ENPASSANT) _lastEnPassantMove = move;
            if(move.moveType == QSCASTLE || KSCASTLE) _lastCastle = move;
        }
    }
    void Bitboard::move(bMove& move){
        switch(getPieceTypeAt(move.from)){
            case wPAWN:
            case bPAWN:
                movePawn(move);
                _lastMove = move;
                return;
            case wKNIGHT:
            case bKNIGHT:
                moveKnight(move);
                _lastMove = move;
                return;
            case wBISHOP:
            case bBISHOP:
                moveBishop(move);
                _lastMove = move;
                return;
            case wROOK:
            case bROOK:
                moveRook(move);
                _lastMove = move;
                return;
            case wQUEEN:
            case bQUEEN:
                moveQueen(move);
                _lastMove = move;
                return;
            case wKING:
            case bKING:
                moveKing(move);
                _lastMove = move;
                return;
        }
    }
    void Bitboard::movePawn(bMove& move){
        int from = move.from;
        int to = move.to;
        MoveType type = move.moveType;
        int side = colorAt(from);
        int opponent = side == 0 ? 1 : 0;
        int coeff = side*6;
        clear_bit(boards[wPAWN + coeff], from); //clear the from bit in corresponding pawn
        PieceType capturedPiece = (getPieceTypeAt(to));//see if we've captured a piece
        if(capturedPiece != EMPTY){ //if so clear the opponent's corresponding board's square bit
            clear_bit(boards[capturedPiece], to);
        }
        if (type == ENPASSANT) { //if so clear the opponent's corresponding board's square bit 
            //(square cleared should be row above or below our to square depending on the side)
                int capturedSquare = side == 0 ? (to - 8) : (to + 8);
            clear_bit(boards[wPAWN + opponent*6], capturedSquare);
            set_bit(boards[wPAWN + coeff], to);
        }else if (type == PROMOTION) {
            set_bit(boards[wQUEEN + coeff], to); //set to bit to a queen.
            // Else just set the from square of the corresponding color's pawn bitboard to a 0
        }else{
            set_bit(boards[wPAWN + coeff], to);// if not promotion set corresponding board's to square bit
        }
    }
    
    void Bitboard::moveKnight(bMove& move){
        int from = move.from;
        int to = move.to;
        int side = colorAt(from);
        int coeff = side*6;
        clear_bit(boards[wKNIGHT + coeff], from);
        PieceType capturedPiece = (getPieceTypeAt(to)); //see if we've captured a piece
        if(capturedPiece != EMPTY){ //if so clear the opponent's corresponding board's square bit
            clear_bit(boards[capturedPiece], to);
        }
        set_bit(boards[wKNIGHT + coeff], to);
    }

    void Bitboard::moveBishop(bMove& move){
        int from = move.from;
        int to = move.to;
        int side = colorAt(from);
        int coeff = side*6;
        clear_bit(boards[wBISHOP + coeff], from);
        PieceType capturedPiece = (getPieceTypeAt(to));//see if we've captured a piece
        if(capturedPiece != EMPTY){ //if so clear the opponent's corresponding board's square bit
            clear_bit(boards[capturedPiece], to);
        }
        set_bit(boards[wBISHOP + coeff], to);
    }
    void Bitboard::moveRook(bMove& move){
        int from = move.from;
        int to = move.to;
        int side = colorAt(from);
        int coeff = side*6;
        clear_bit(boards[wROOK + coeff], from);
        PieceType capturedPiece = (getPieceTypeAt(to));//see if we've captured a piece
        if(capturedPiece != EMPTY){ //if so clear the opponent's corresponding board's square bit
            clear_bit(boards[capturedPiece], to);
        }
        set_bit(boards[wROOK + coeff], to);
    }
    void Bitboard::moveQueen(bMove& move){
        int from = move.from;
        int to = move.to;
        int side = colorAt(from);
        int coeff = side*6;
        clear_bit(boards[wQUEEN + coeff], from);
        PieceType capturedPiece = (getPieceTypeAt(to));//see if we've captured a piece
        if(capturedPiece != EMPTY){ //if so clear the opponent's corresponding board's square bit
            clear_bit(boards[capturedPiece], to);
        }
        set_bit(boards[wQUEEN + coeff], to);
    }
    void Bitboard::moveKing(bMove& move){
        int from = move.from;
        int to = move.to;
        MoveType type = move.moveType;
        int side = colorAt(from);
        int coeff = side*6;
        clear_bit(boards[wKING + coeff], from);
        PieceType capturedPiece = (getPieceTypeAt(to));//see if we've captured a piece
        if(capturedPiece != EMPTY){ //if so clear the opponent's corresponding board's square bit
            clear_bit(boards[capturedPiece], to);
        }
        set_bit(boards[wKING + coeff], to);
        if (type == KSCASTLE || type == QSCASTLE) {//castling move
            int rookFrom, rookTo;
            if (type == KSCASTLE) {
                if(side == 0){
                    rookFrom = h1;
                    rookTo   = f1;
                    _hasMoved.WKing = true;
                    _hasMoved.k_WRook = true;
                }else{
                    rookFrom = h8;
                    rookTo   = f8;
                    _hasMoved.BKing = true;
                    _hasMoved.k_BRook = true;
                }
            }else if(type == QSCASTLE){ //type == QSCASTLE
                if(side == 0){
                    rookFrom = a1;
                    rookTo   = d1;
                    _hasMoved.WKing = true;
                    _hasMoved.q_WRook = true;
                }else{
                    rookFrom = a8;
                    rookTo   = d8;
                    _hasMoved.BKing = true;
                    _hasMoved.q_BRook = true;
                }
            }
            clear_bit(boards[wROOK + coeff], rookFrom);
            set_bit(boards[wROOK + coeff], rookTo);
        }
    }
    uint64_t Bitboard::friendly_occupants(int color){
        return color == 0 ? (boards[wPAWN] | boards[wKNIGHT] | boards[wBISHOP] | boards[wROOK] | boards[wQUEEN] | boards[wKING] ) :
            boards[bPAWN] | boards[bKNIGHT] | boards[bBISHOP] | boards[bROOK] | boards[bQUEEN] | boards[bKING];
    }
    uint64_t Bitboard::enemy_occupants(int color){
        return color == 1 ? (boards[wPAWN] | boards[wKNIGHT] | boards[wBISHOP] | boards[wROOK] | boards[wQUEEN] | boards[wKING] ) :
            boards[bPAWN] | boards[bKNIGHT] | boards[bBISHOP] | boards[bROOK] | boards[bQUEEN] | boards[bKING];
    }

    void Bitboard::generatePsuedoMoves(std::vector<bMove> &moves, int color) {
        uint64_t friendlies = friendly_occupants(color);
        uint64_t enemies = enemy_occupants (color);
        uint64_t all = friendlies | enemies;
        int coeff=color*6;
        generatePawnMoves(moves, boards[wPAWN+coeff], friendlies, enemies, color);
        generateKnightMoves(moves, boards[wKNIGHT+coeff], friendlies);
        generateRookMoves(moves, boards[wROOK+coeff], all, ~friendlies);
        generateBishopMoves(moves, boards[wBISHOP+coeff], all, ~friendlies);
        generateQueenMoves(moves, boards[wQUEEN+coeff], all, ~friendlies);
        generateKingMoves(moves, boards[wKING+coeff], ~friendlies, all);
    }

    void Bitboard::generatePawnMoves(std::vector<bMove> &moves, uint64_t pawns, uint64_t friendlies, uint64_t enemies, int color){
        
        if (color == 0) {
            generateWhitePawnMoves(moves, pawns, friendlies, enemies);
        } else {
            generateBlackPawnMoves(moves, pawns, friendlies, enemies);
        }
    }

    void Bitboard::generateWhitePawnMoves(std::vector<bMove> &moves, uint64_t pawns, uint64_t friendlies, uint64_t enemies){
        uint64_t promotions = pawns & MASK_RANK[6]; // Rank 7 for promotions for white pawns
        uint64_t non_promotions = pawns & ~MASK_RANK[6];
        uint64_t empties = ~(friendlies | enemies);
        uint64_t singles, doubles, up_promotions, w_promotions, e_promotions, w_captures, e_captures;
        singles = north(non_promotions) & empties;
        doubles = north(singles) & MASK_RANK[3] & empties;

        if (promotions) {
            up_promotions = north(promotions) & empties;
            w_promotions = north_west(promotions) & enemies; // Capture to the left
            e_promotions = north_east(promotions) & enemies; // Capture to the right
        }
        //west debugs
        w_captures = north_west(non_promotions) & enemies;
        e_captures = north_east(non_promotions) & enemies;
        while (singles) {
            int to = pop_LSB(singles);
            moves.push_back({ to - 8, to}); // single move forward
        }
        while (doubles) {
            int to = pop_LSB(doubles);
            moves.push_back({ to - 16, to, MoveType::PAWN2 }); // Double push forward
        }
        while (promotions && up_promotions) {
            int to = pop_LSB(up_promotions);
            moves.push_back({ to - 8, to, MoveType::PROMOTION }); // Regular pushes
        }   
        while (promotions && w_promotions) {
            int to = pop_LSB(w_promotions);
            moves.push_back({ to - 7, to, MoveType::PROMOTION }); // Capture to the left
        }
        while (promotions && e_promotions) {
            int to = pop_LSB(e_promotions);
            moves.push_back({ to - 9, to, MoveType::PROMOTION }); // Capture to the right
        }
        while (w_captures) {
            int to = pop_LSB(w_captures);
            moves.push_back({ to - 7, to }); // Capture to the left
        }
        while (e_captures) {
            int to = pop_LSB(e_captures);
            moves.push_back({ to - 9, to }); // Capture to the right
        }        
        
        if (_lastMove.moveType == PAWN2) {
            int lastMoveTo = _lastMove.to;
            // Check if en passant is possible on the left or right side
            
            if((get_bit(boards[wPAWN] & ~MASK_FILE[0], lastMoveTo -1) & 1)){//left
                moves.push_back({lastMoveTo-1, lastMoveTo + 8, ENPASSANT });
            } 
            if((get_bit(boards[wPAWN] & ~MASK_FILE[7], lastMoveTo+1) & 1)){//right
                moves.push_back({lastMoveTo+1, lastMoveTo + 8, ENPASSANT });
            }             
        }

    }

    void Bitboard::generateBlackPawnMoves(std::vector<bMove> &moves, uint64_t pawns, uint64_t friendlies, uint64_t enemies){
        uint64_t promotions = pawns & MASK_RANK[1]; // Rank 2 for promotions for black pawns
        uint64_t non_promotions = pawns & ~MASK_RANK[1];
        uint64_t empties = ~(friendlies | enemies);
        uint64_t singles, doubles, down_promotions, w_promotions, e_promotions;
        uint64_t w_captures, e_captures;

        singles = south(non_promotions) & empties;
        doubles = south(singles) & MASK_RANK[4] & empties;

        if (promotions!=0ULL) {
            down_promotions = south(promotions) & empties;
            w_promotions = south_west(promotions) & enemies; // Capture to the left
            e_promotions = south_east(promotions) & enemies; // Capture to the right
        }
        w_captures = south_west(non_promotions) & enemies;
        e_captures = south_east(non_promotions) & enemies;

        while (singles) {
            int to = pop_LSB(singles);
            moves.push_back({ to + 8, to}); // single move forward
        }

        while (doubles) {
            int to = pop_LSB(doubles);
            moves.push_back({ to + 16, to, MoveType::PAWN2 }); // Double push forward
        }

        while (promotions && down_promotions) {
            int to = pop_LSB(down_promotions);
            moves.push_back({ to + 8, to, MoveType::PROMOTION }); // Regular pushes
        }

        while (promotions && w_promotions) {
            int to = pop_LSB(w_promotions);
            moves.push_back({ to + 9, to, MoveType::PROMOTION }); // Capture to the left
        }

        while (promotions && e_promotions) {
            int to = pop_LSB(e_promotions);
            moves.push_back({ to + 7, to, MoveType::PROMOTION }); // Capture to the right
        }

        while (w_captures) {
            int to = pop_LSB(w_captures);
            moves.push_back({ to + 9, to }); // Capture to the left
        }

        while (e_captures) {
            int to = pop_LSB(e_captures);
            moves.push_back({ to + 7, to }); // Capture to the right
        }
        //previous logic
        
        if (_lastMove.moveType == PAWN2) {
            int lastMoveTo = _lastMove.to;
            // Check if en passant is possible on the left or right side
            if ((lastMoveTo % 8 != 7) && get_bit(boards[bPAWN], lastMoveTo + 1) ) { // Check left side (black's perspective)
                moves.push_back({lastMoveTo + 1, lastMoveTo - 8, ENPASSANT});
            }
            if ((lastMoveTo % 8 != 0)&& get_bit(boards[bPAWN], lastMoveTo - 1)) { // Check right side (black's perspective)
                moves.push_back({lastMoveTo - 1, lastMoveTo - 8, ENPASSANT});
            }
        }
    }

    void Bitboard::generateKnightMoves(std::vector<bMove> &moves, uint64_t knights, uint64_t friendlies) {
        while (knights) {
            int from = pop_LSB(knights); //curr knight position
            uint64_t attacks = nATTACKS[from] & ~friendlies; //attack locations
            while (attacks) moves.push_back({from, pop_LSB(attacks)});
        }
    }
    
    void Bitboard::generateBishopMoves(std::vector<bMove> &moves, uint64_t bishops, uint64_t occ, uint64_t targets) {
        while (bishops) {
            int from = pop_LSB(bishops); // bishop's position
            uint64_t attacks = bishop_attacks(from, occ) & targets; // potential attack locations
            while (attacks) moves.push_back({from,pop_LSB(attacks)});
        }
    }
    
    void Bitboard::generateRookMoves(std::vector<bMove> &moves, uint64_t rooks, uint64_t occ, uint64_t targets) {
        while (rooks) {
            int from = pop_LSB(rooks); // rook's position
            uint64_t attacks = rook_attacks(from, occ) & targets; // potential attack locations
            while (attacks) moves.push_back({from, pop_LSB(attacks)});

        }
    }
    
    void Bitboard::generateQueenMoves(std::vector<bMove> &moves, uint64_t queens, uint64_t occ, uint64_t targets){
        while (queens) { // for each of our queens
            int from = pop_LSB(queens); // queen's position
            uint64_t attacks = queen_attacks(from, occ) & targets;
            while (attacks) moves.push_back({from, pop_LSB(attacks)}); // add a move for e/a attack
        }
    }

    void Bitboard::generateKingMoves(std::vector<bMove> &moves, uint64_t kings, uint64_t targets, uint64_t all) {
        while (kings) {
            int from = pop_LSB(kings); // king's position
            uint64_t attacks = kATTACKS[from] & targets; // potential attack locations
            while (attacks) moves.push_back({from, pop_LSB(attacks)});
            if (from == e1 && !_hasMoved.WKing && !isKingInCheck(0)) {
                // Kingside castling
                if (!_hasMoved.k_WRook && (all & (SQ_BB[f1] | SQ_BB[g1])) == 0) {
                    moves.push_back({e1, g1, KSCASTLE});
                }
                // Queenside castling
                if (!_hasMoved.q_WRook && (all & (SQ_BB[b1] | SQ_BB[c1] | SQ_BB[d1])) == 0) {
                    moves.push_back({e1, c1, QSCASTLE});
                }
            }else if (from == e8 && !_hasMoved.BKing && !isKingInCheck(1)) {
                // Kingside castling
                if (!_hasMoved.k_BRook && (all & (SQ_BB[f8] | SQ_BB[g8])) == 0) {
                    moves.push_back({e8, g8, KSCASTLE});
                }
                // Queenside castling
                if (!_hasMoved.q_BRook && (all & (SQ_BB[b8] | SQ_BB[c8] | SQ_BB[d8])) == 0) {
                    moves.push_back({e8, c8, QSCASTLE});
                }
            }
        }
    }

int Bitboard::evalBoard(int side){
    return side == 0 ? evaluateBoard() : -1*evaluateBoard();
}
int Bitboard::evaluateBoard() {
    int score = 0;
    // iter through e/a piece type using  bitboards
    for (int p = 0; p < 6; ++p) {
        // Get the bitboard for both white and black pieces of this type
        uint64_t whitePieces = boards[p];
        uint64_t blackPieces = boards[p + 6]; // Assuming black pieces are offset by 6

        while (whitePieces){ 
            int i = pop_LSB(whitePieces);
            score += pValAndPos(p, i, true); // true for white
            //score += mobilityScore(p, i, 0);
        } 
        while (blackPieces){ 
            int i =  pop_LSB(blackPieces);
            score -= pValAndPos(p, i, false); // false for black
            //score += mobilityScore(p, i, 1);
        } 
    }
    //score += pawnStructureScore(0) - pawnStructureScore(1);
    //score += kingSafetyScore(0) - kingSafetyScore(1);
    return score;
}

int Bitboard::pValAndPos(int p, int pos, bool isWhite) {
    // Adjust position for black pieces if necessary
    if (!isWhite) {
        int mod = pos % 8;
        pos = 56 - (pos - mod) + mod;
    }
    return scoreTables[p][pos] + pieceValues[p];
}


//other eval functions

int Bitboard::pawnStructureScore(int side) {
    int score = 0;
    uint64_t pawns = side == 0 ? boards[wPAWN] : boards[bPAWN];
    uint64_t enemyPawns = side == 0 ? boards[bPAWN] : boards[wPAWN];

    while (pawns) {
        int sq = pop_LSB(pawns);
        uint64_t pawnBB = SQ_BB[sq];

        // doubled Pawns
        if (south(pawnBB) & pawns) {
            score -= 5; // Penalty for doubled pawns
        }
        // isolated Pawns
        uint64_t adjacentFiles = (east(pawnBB) | west(pawnBB)) & file(sq);
        if (!(adjacentFiles & pawns)) {
            score -= 5; // Penalty for isolated pawns
        }

        // passed Pawns
        uint64_t forwardSquares = (side == 0 ? north(pawnBB) : south(pawnBB)) & file(sq);
        if (!(forwardSquares & enemyPawns)) {
            score += 5; // Bonus for passed pawns
        }
        // pawn Chains
        uint64_t supportingPawns = (side == 0 ? south_east(pawnBB) | south_west(pawnBB)
                                              : north_east(pawnBB) | north_west(pawnBB)) & pawns;
        if (supportingPawns) {
            score += 15; // bonus for pawn chains
        }
    }

    return score;
}

int Bitboard::kingSafetyScore(int side) {
    int score = 0;
    uint64_t kingBB = side == 0 ? boards[wKING] : boards[bKING];
    uint64_t pawns = side == 0 ? boards[wPAWN] : boards[bPAWN];
    int kingsq = get_LSB(kingBB);

    // Shielding Pawns
    uint64_t shieldingPawns = (side == 0 ? north(kingBB) : south(kingBB)) & pawns;

    if (!shieldingPawns) {
        score -= 10; // penalty for no shielding pawns
    } else {
        score += 5 * popcount(shieldingPawns); // bonus for each shielding pawn
    }

    // king's Exposure
    uint64_t kingZone = king_attacks(kingsq, ~pawns); // squares around the king
    uint64_t openFiles = (kingZone & ~pawns) & file(kingsq);
    score -= 15 * popcount(openFiles); // Penalty for open files around the king
    return score;
}

int Bitboard::mobilityScore(int p, int pos, int side) {
    uint64_t ownPieces = side == 0 ? friendly_occupants(0) : friendly_occupants(1);
    uint64_t moves = 0ULL;
    int mobility = 0;

    switch (p) {
        case wPAWN: // Fall through
        case bPAWN:
            moves = pawn_attacks(pos, side);
            break;
        case wKNIGHT: // Fall through
        case bKNIGHT:
            moves = nATTACKS[pos];
            break;
        case wBISHOP: // Fall through
        case bBISHOP:
            moves = bishop_attacks(pos, ownPieces);
            break;
        case wROOK: // Fall through
        case bROOK:
            moves = rook_attacks(pos, ownPieces);
            break;
        case wQUEEN: // Fall through
        case bQUEEN:
            moves = queen_attacks(pos, ownPieces);
            break;
        case wKING: // Fall through
        case bKING:
            moves = king_attacks(pos, ownPieces);
            break;
    }
    moves &= ~ownPieces;
    mobility = popcount(moves); // Count legal moves excluding own pieces

    return mobility;
}

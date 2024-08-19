#pragma once
#include "Game.h"
#include "ChessSquare.h"
#include "BitBoard.h"
#include <string>
#include <vector>
#include <algorithm>

//
// the classic game of chess
//

const int pieceSize = 64;

enum ChessPiece {
    Pawn = 1,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

//
// the main game class
//
class Chess : public Game
{
public:
    Chess();
    ~Chess();
    
    //enum MoveType{ DEFAULT, ENPASSANT, QSCASTLE, KSCASTLE, PAWN2};

    //ADDED STRUCT MOVE
    struct scoreMove{
        int score;
        bMove move;
        Bitboard *state;
    };

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() const override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override {return false; }
    bool        canBitMoveFrom(Bit& bit, BitHolder& src) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void        updateBoard();
    void        bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    bool	    clickedBit(Bit& bit) override;
    void        stopGame() override;
    int         evaluateBoard(const char* stateString);
    int         evalBoard(std::string state, char color);

    BitHolder &getHolderAt(const int x, const int y) override { return _grid[y][x]; }
    ChessSquare &getHolderAtC(const int x, const int y) { return _grid[y][x]; }
    
    //ADDED METHODS

    void printMove(bMove move);
    void printMoves();
    void printStateString(std::string state);
    std::vector<std::pair<int,int>> validMovesForPiece(char piece, ChessSquare& src);
    std::vector<std::pair<int,int>> _highlightedMoves;
    std::string pieceNotation(int row, int col) const;
    bool moveVal(int r, int c, char p, std::string board);
   
    bool isKingInCheck(std::string state, char color, std::vector<bMove> moves);
    bool isKingInCheck(int row, int col, char color);
    void setAI(char ai){
        AI = ai == 'W' ? 'W' : 'B';
        _AI = ai == 'W' ? 0 : 1;
        nonAI = ai == 'W' ? 'B' : 'W';
        _nonAI = ai == 'W' ? 1 : 0;
        std::cout<<"ai: "<<AI<<" nonAI: "<<nonAI<<std::endl;
    }
    //CHESS AI FUNCTIONS
    std::string enactMove(std::string state, char currPlayer, bMove move);
    void AIMoveBit(bMove& move);
    void updateAI();
    //bool customCompare(const std::pair<int, Chess::Move>& a, const std::pair<int, Chess::Move>& b);
    //bool isCheckmate(char color) {return false;};
    //bool isStalemate(char color) {return false;};
    int minimaxAlphaBetaSorted(Bitboard state, int depth, bool isMaximizingPlayer, int alpha, int beta);
private:

    Bitboard _board;
    const char  bitToPieceNotation(int row, int column) const;
    const char  gameTagToPieceNotation(int gameTag) const;
    std::string indexToNotation(int row, int col);
    int rowColToIndex(int row, int col);
    int notationToIndex(std::string notation);
    int notationToRow(std::string notation);
    int notationToCol(std::string notation);
    std::pair<int,int> indexToNotation(std::string notation);

    Bit *       PieceForPlayer(const int playerNumber, ChessPiece piece);
    char _currPlayer;
    ChessSquare         _grid[8][8];
    std::vector<bMove> _moves;
    bMove _bestMove = {64,64};
    bMove _lastMove = {64,64};
    char _lastPiece;
    bMove _lastEnPassantMove = {64,64};
    bMove _lastCastle = {64,64};
    HasMoved _stats;
    //priv CHESS AI vars
    char AI = 'W'; //color the AI is
    int _AI = 0;
    int _nonAI = 1;
    char nonAI = 'B';
    bool _AIInProgress = false;
    bool _AIDone = false;
    int maxDepth = 3; //max depth minmax search will do
};
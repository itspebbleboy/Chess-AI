#pragma once
#include "Chess.h"
#include <bitset>
#include "utility.h"


class Bitboard {
private:
    uint64_t boards[12]{ //top 6 are white , pnbrqk, bottom 6 are black PNBRQK
        0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
        0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000000000000,
    };
    HasMoved _hasMoved;
    
public:
    Bitboard() {
    }
    bMove _lastMove = {64,64};
    bMove _lastEnPassantMove = {64, 64};
    bMove _lastCastle = {64,64};
    char getPieceAt(uint64_t square);
    void setPiece(char p, int square);
    PieceType getPieceTypeAt(uint64_t square);
    int colorAt(uint64_t square);
    bMove findMove(std::vector<bMove> &moves, int src, int dst);
    void printBoard();
    void printBoardStraight();
    
    void printBitBoard(uint64_t bitBoard);
    void clearBitboards(); 
    void createStateFromString(const std::string& state);
    Bitboard copy(){
        Bitboard *b = new Bitboard();
        for (int i = 0; i < 12; ++i) {
            b->boards[i] = this->boards[i];
        }
        b->_lastMove = _lastMove;
        b->_lastEnPassantMove = _lastEnPassantMove;
        b->_lastCastle = _lastCastle;
        b->_hasMoved = _hasMoved;
        return *b;
    };
    std::string stateString();

    bool legalMove(bMove move);
    bool isKingInCheck(int side);
    uint64_t friendly_occupants(int color);
    uint64_t enemy_occupants(int color);
    //uint64_t all_occupants();

    void generateLegalMoves(std::vector<bMove> &moves, int color);
    void generatePsuedoMoves(std::vector<bMove> &moves, int color);
    void move(bMove& move);
    void movePawn(bMove& move);
    void moveKnight(bMove& move);
    void moveBishop(bMove& move);
    void moveRook(bMove& move);
    void moveQueen(bMove& move);
    void moveKing(bMove& move);
    void generatePawnMoves(std::vector<bMove> &moves, uint64_t pawns, uint64_t friendlies,uint64_t enemies, int color);
    void generateWhitePawnMoves(std::vector<bMove> &moves, uint64_t pawns, uint64_t friendlies, uint64_t enemies);
    void generateBlackPawnMoves(std::vector<bMove> &moves, uint64_t pawns, uint64_t friendlies, uint64_t enemies);
    void generateKnightMoves(std::vector<bMove> &moves, uint64_t knights, uint64_t friendlies);
    void generateBishopMoves(std::vector<bMove> &moves, uint64_t bishops, uint64_t occ, uint64_t targets);
    void generateRookMoves(std::vector<bMove> &moves, uint64_t rooks, uint64_t occ, uint64_t targets);
    void generateQueenMoves(std::vector<bMove> &moves, uint64_t queens, uint64_t occ, uint64_t targets);
    void generateKingMoves(std::vector<bMove> &moves, uint64_t kings, uint64_t targets, uint64_t all);   

    int pValAndPos(int p, int pos, bool isWhite);
    int evalBoard(int side);
    int evaluateBoard();
    int pawnStructureScore(int side);
    int kingSafetyScore(int side);
    int mobilityScore(int p, int pos, int side);
};



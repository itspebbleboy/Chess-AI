#pragma once
#include <iostream>

//A lookup table for king move bitboards
#pragma region HELPER TABLES

//Lookup tables of square names in algebraic chess notation
//from: https://github.com/nkarve/surge/blob/master/src/types.h
enum SQ : int {
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8,
	NO_SQUARE
};

//from: https://github.com/nkarve/surge/blob/master/src/types.cpp
const uint64_t SQ_BB[65] = {
	0x1, 0x2, 0x4, 0x8,
	0x10, 0x20, 0x40, 0x80,
	0x100, 0x200, 0x400, 0x800,
	0x1000, 0x2000, 0x4000, 0x8000,
	0x10000, 0x20000, 0x40000, 0x80000,
	0x100000, 0x200000, 0x400000, 0x800000,
	0x1000000, 0x2000000, 0x4000000, 0x8000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000,
	0x100000000, 0x200000000, 0x400000000, 0x800000000,
	0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
	0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000,
	0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
	0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000,
	0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
	0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000,
	0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
	0x0
};

const uint64_t MASK_FILE[8] = {
	0x101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808,
	0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
};

//Precomputed rank masks
const uint64_t MASK_RANK[8] = {
	0xff, 0xff00, 0xff0000, 0xff000000,
	0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000
};

//Precomputed diagonal masks
const uint64_t MASK_DIAGONAL[15] = {
	0x80, 0x8040, 0x804020,
	0x80402010, 0x8040201008, 0x804020100804,
	0x80402010080402, 0x8040201008040201, 0x4020100804020100,
	0x2010080402010000, 0x1008040201000000, 0x804020100000000,
	0x402010000000000, 0x201000000000000, 0x100000000000000,
};

//Precomputed anti-diagonal masks
const uint64_t MASK_ANTI_DIAGONAL[15] = {
	0x1, 0x102, 0x10204,
	0x1020408, 0x102040810, 0x10204081020,
	0x1020408102040, 0x102040810204080, 0x204081020408000,
	0x408102040800000, 0x810204080000000, 0x1020408000000000,
	0x2040800000000000, 0x4080000000000000, 0x8000000000000000,
};

#pragma endregion

#pragma region LOOK UP TABLE FOR MOVES (king, knights, white pawns, black pawns)
//from https://github.com/nkarve/surge/blob/master/src/tables.cpp#L100
const uint64_t kATTACKS[64] = {
	0x302, 0x705, 0xe0a, 0x1c14,
	0x3828, 0x7050, 0xe0a0, 0xc040,
	0x30203, 0x70507, 0xe0a0e, 0x1c141c,
	0x382838, 0x705070, 0xe0a0e0, 0xc040c0,
	0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00,
	0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000,
	0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000,
	0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000,
	0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000,
	0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000,
	0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000,
	0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000,
	0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000,
	0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
	0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000,
	0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000,
};

//A lookup table for knight move bitboards
const uint64_t nATTACKS[64] = {
	0x20400, 0x50800, 0xa1100, 0x142200,
	0x284400, 0x508800, 0xa01000, 0x402000,
	0x2040004, 0x5080008, 0xa110011, 0x14220022,
	0x28440044, 0x50880088, 0xa0100010, 0x40200020,
	0x204000402, 0x508000805, 0xa1100110a, 0x1422002214,
	0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040,
	0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400,
	0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000,
	0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000,
	0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000,
	0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000,
	0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
	0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000,
	0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
	0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000,
	0x44280000000000, 0x0088500000000000, 0x0010a00000000000, 0x20400000000000
};

//A lookup table for white pawn move bitboards
const uint64_t PAWN_ATTACKS[2][64] { 
    {
        0x200, 0x500, 0xa00, 0x1400,
        0x2800, 0x5000, 0xa000, 0x4000,
        0x20000, 0x50000, 0xa0000, 0x140000,
        0x280000, 0x500000, 0xa00000, 0x400000,
        0x2000000, 0x5000000, 0xa000000, 0x14000000,
        0x28000000, 0x50000000, 0xa0000000, 0x40000000,
        0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
        0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
        0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
        0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
        0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
        0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
        0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000,
        0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
    },
    {
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x2, 0x5, 0xa, 0x14,
	0x28, 0x50, 0xa0, 0x40,
	0x200, 0x500, 0xa00, 0x1400,
	0x2800, 0x5000, 0xa000, 0x4000,
	0x20000, 0x50000, 0xa0000, 0x140000,
	0x280000, 0x500000, 0xa00000, 0x400000,
	0x2000000, 0x5000000, 0xa000000, 0x14000000,
	0x28000000, 0x50000000, 0xa0000000, 0x40000000,
	0x200000000, 0x500000000, 0xa00000000, 0x1400000000,
	0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
	0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000,
	0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
	0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000,
	0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
    }
};
#pragma endregion LOOKUP TABLES
//from: https://github.com/nkarve/surge/blob/master/src/tables.cpp#L251

extern const char* SQSTR[65];
extern uint64_t SQ_BETWEEN_BB[64][64];

extern const int scoreTables[6][64];
extern const int pieceValues[6];
extern const int pawnTable[64];
extern const int knightTable[64];
extern const int bishopTable[64];
extern const int rookTable[64];
extern const int queenTable[64];
extern const int kingTable[64];

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))
inline int get_LSB(uint64_t b) {
    int count = 0;
    while ((b & 1) == 0) {
        ++count;
        b >>= 1;
    }
    return count;
}
inline int pop_LSB(uint64_t& b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}
inline int popcount(uint64_t& x) {
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}

extern uint64_t reverse(uint64_t b);
extern uint64_t rank(int sq);
extern uint64_t file(int sq);
extern uint64_t diagonal(int sq);
extern uint64_t antidiagonal(int sq);
extern uint64_t sliding_attacks(int sq, uint64_t occ, uint64_t mask);
extern uint64_t king_attacks(int sq, uint64_t friendly);
extern uint64_t queen_attacks(int sq, uint64_t occ);
extern uint64_t rook_attacks(int sq, uint64_t occ);
extern uint64_t bishop_attacks(int sq, uint64_t occ);
extern uint64_t knight_attacks(int sq, uint64_t friendly);
extern uint64_t pawn_attacks(int sq, int color);
extern uint64_t all_pawn_attacks(uint64_t pawns, uint64_t friendly, uint64_t enemy, int color);
extern void init_sqs_between();

//bitwise operation helper functions from https://ameye.dev/notes/chess-engine/

constexpr uint64_t north(const uint64_t& b) { return    (b & ~MASK_RANK[7]) << 8; }
constexpr uint64_t south(const uint64_t& b) { return    (b & ~MASK_RANK[0]) >> 8; }
constexpr uint64_t west(const uint64_t& b) { return     (b & ~MASK_FILE[0]) >> 1; }
constexpr uint64_t east(const uint64_t& b) { return     (b & ~MASK_FILE[7]) << 1; }
constexpr uint64_t north_west(const uint64_t& b) { return (b & ~MASK_FILE[0]) << 7; }
constexpr uint64_t north_east(const uint64_t& b) { return (b & ~MASK_FILE[7]) << 9; }
constexpr uint64_t south_west(const uint64_t& b) { return (b & ~MASK_FILE[0]) >> 9; }
constexpr uint64_t south_east(const uint64_t& b) { return (b & ~MASK_FILE[7]) >> 7; }
//constexpr uint64_t south_east(const uint64_t& b) { return (south(west(b))); }
// constexpr uint64_t south_east(const uint64_t& b) { return (south(east(b))); }
// constexpr uint64_t north_west(const uint64_t& b) { return (north(west(b))); }
// constexpr uint64_t north_east(const uint64_t& b) { return (north(east(b))); }

//cardinal direction helper functions from https://ameye.dev/notes/chess-engine/
enum MoveType{ DEFAULT, ENPASSANT, QSCASTLE, KSCASTLE, PAWN2, PROMOTION};
struct HasMoved{
	bool WKing   = false;
	bool q_WRook = false;
	bool k_WRook = false;
	bool BKing   = false;
	bool q_BRook = false;
	bool k_BRook = false;
};
enum PieceType {
    EMPTY = -1, wPAWN, wKNIGHT, wBISHOP, wROOK, wQUEEN, wKING,
    bPAWN, bKNIGHT, bBISHOP, bROOK, bQUEEN, bKING
};
struct bMove {
    int from;
    int to;
    MoveType moveType = DEFAULT;
};
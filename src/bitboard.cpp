#include "bitboard.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace {
    const uint8_t rookBits[SQUARE_NUM] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };

    const uint8_t bishopBits[SQUARE_NUM] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    Bitboard attacksTable[(1 << 12) * SQUARE_NUM + (1 << 9) * SQUARE_NUM];
}

bool BB::initialized = false;
Bitboard BB::pseudoAttacks[PIECE_TYPE_NUM][SQUARE_NUM];
BB::Magic BB::rookAttacks[SQUARE_NUM];
BB::Magic BB::bishopAttacks[SQUARE_NUM];


template <Direction dir>
static Bitboard SlideAttack(Square square, Bitboard occupancy) {
    Bitboard bb = 0;
    Bitboard squareBB = BB::SquareBB(square);
    while (true) {
        squareBB = BB::Shift<dir>(squareBB);
        bb |= squareBB;
        if (!squareBB || squareBB & occupancy) return bb;
    }
}

static Bitboard ComputeKingAttack(Square square) {
    Bitboard squareBB = BB::SquareBB(square);
    return
        BB::Shift<Direction::UP>(squareBB)        |
        BB::Shift<Direction::DOWN>(squareBB)      |
        BB::Shift<Direction::LEFT>(squareBB)      |
        BB::Shift<Direction::RIGHT>(squareBB)     |
        BB::Shift<Direction::UP_LEFT>(squareBB)   |
        BB::Shift<Direction::UP_RIGHT>(squareBB)  |
        BB::Shift<Direction::DOWN_LEFT>(squareBB) |
        BB::Shift<Direction::DOWN_RIGHT>(squareBB);
}

static Bitboard ComputeRookAttack(Square square, Bitboard occupancy) {
    return
        SlideAttack<Direction::UP>(square, occupancy)   |
        SlideAttack<Direction::DOWN>(square, occupancy) |
        SlideAttack<Direction::LEFT>(square, occupancy) |
        SlideAttack<Direction::RIGHT>(square, occupancy);
}

static Bitboard ComputeBishopAttack(Square square, Bitboard occupancy) {
    return
        SlideAttack<Direction::UP_LEFT>(square, occupancy)  |
        SlideAttack<Direction::UP_RIGHT>(square, occupancy) |
        SlideAttack<Direction::DOWN_LEFT>(square, occupancy)|
        SlideAttack<Direction::DOWN_RIGHT>(square, occupancy);
}

static Bitboard ComputeKightAttack(Square square) {
    Bitboard squareBB = BB::SquareBB(square);
    Bitboard ul = BB::Shift<Direction::UP_LEFT>(squareBB);
    Bitboard ur = BB::Shift<Direction::UP_RIGHT>(squareBB);
    Bitboard dl = BB::Shift<Direction::DOWN_LEFT>(squareBB);
    Bitboard dr = BB::Shift<Direction::DOWN_RIGHT>(squareBB);
    return
        BB::Shift<Direction::UP>(ul)      |
        BB::Shift<Direction::LEFT>(ul)    |
        BB::Shift<Direction::UP>(ur)      |
        BB::Shift<Direction::RIGHT>(ur)   |
        BB::Shift<Direction::DOWN>(dl)    |
        BB::Shift<Direction::LEFT>(dl)    |
        BB::Shift<Direction::DOWN>(dr)    |
        BB::Shift<Direction::RIGHT>(dr);
}

void BB::InitPseudoAttacks() {
    using SquareInt = std::underlying_type<Square>::type;
    for (SquareInt sqInt = 0; sqInt < SQUARE_NUM; sqInt++) {
        Square square = static_cast<Square>(sqInt);
        Bitboard kingAttack = ComputeKingAttack(square);
        Bitboard rookAttack = ComputeRookAttack(square, 0);
        Bitboard bishopAttack = ComputeBishopAttack(square, 0);
        Bitboard knightAttack = ComputeKightAttack(square);

        using PieceTypeInt = std::underlying_type<PieceType>::type;
        pseudoAttacks[static_cast<PieceTypeInt>(PieceType::King)][sqInt]    = kingAttack;
        pseudoAttacks[static_cast<PieceTypeInt>(PieceType::Queen)][sqInt]   = rookAttack | bishopAttack;
        pseudoAttacks[static_cast<PieceTypeInt>(PieceType::Rook)][sqInt]    = rookAttack;
        pseudoAttacks[static_cast<PieceTypeInt>(PieceType::Bishop)][sqInt]  = bishopAttack;
        pseudoAttacks[static_cast<PieceTypeInt>(PieceType::Knight)][sqInt]  = knightAttack;
    }
}

/**
 * A bad random number generator for 64-bit numbers
 */
static uint64_t rnd64() {
  uint64_t u1, u2, u3, u4;
  u1 = (uint64_t)(random()) & 0xFFFF; u2 = (uint64_t)(random()) & 0xFFFF;
  u3 = (uint64_t)(random()) & 0xFFFF; u4 = (uint64_t)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

static uint64_t rnd64_sparse() {
  return rnd64() & rnd64() & rnd64();
}

void BB::InitMagicBitboards() {
    Bitboard* table = attacksTable;
    using SquareInt = std::underlying_type<Square>::type;
    for (SquareInt sqInt = 0; sqInt < SQUARE_NUM; sqInt++) {
        Square square = static_cast<Square>(sqInt);
        table = InitMagicBitboards(PieceType::Rook, square, table);
    }
    for (SquareInt sqInt = 0; sqInt < SQUARE_NUM; sqInt++) {
        Square square = static_cast<Square>(sqInt);
        table = InitMagicBitboards(PieceType::Bishop, square, table);
    }
}

Bitboard* BB::InitMagicBitboards(PieceType pieceType, Square square, Bitboard* tableStart) {
    const auto pieceTypeInt = static_cast<std::underlying_type<PieceType>::type>(pieceType);
    const auto squareInt = static_cast<std::underlying_type<Square>::type>(square);
    const Bitboard squareBB = SquareBB(square);
    const uint8_t* numBits = pieceType == PieceType::Rook ? rookBits : bishopBits;
    const auto computeAttack = pieceType == PieceType::Rook ? ComputeRookAttack : ComputeBishopAttack;
    Magic& attacks = pieceType == PieceType::Rook ? rookAttacks[squareInt] : bishopAttacks[squareInt];

    uint32_t used[1 << 12] = { 0 };
    Bitboard mask = pseudoAttacks[pieceTypeInt][squareInt];
    // Board edges are unnecessary for attacks (unless piece is itself on respective edge)
    if (!(squareBB & FILE_A)) mask &= ~FILE_A;
    if (!(squareBB & FILE_H)) mask &= ~FILE_H;
    if (!(squareBB & RANK_1)) mask &= ~RANK_1;
    if (!(squareBB & RANK_8)) mask &= ~RANK_8;
    
    attacks.table = tableStart;
    attacks.mask = mask;
    attacks.shift = 64 - numBits[squareInt];

    assert(Count1s(mask) == numBits[squareInt]);

    for (uint32_t round = 1; round < 1000000; round++) {
        attacks.mult = rnd64_sparse();

        bool conflict = false;
        for (Bitboard occupancy = mask; !conflict; occupancy = (occupancy - 1) & mask) {
            uint64_t index = attacks.TableIndex(occupancy);
            Bitboard attack = computeAttack(square, occupancy);;
            if (used[index] != round) {
                used[index] = round;
                attacks.table[index] = attack;
            } else if (attacks.table[index] != attack) {
                conflict = true;
            }
            if (!occupancy) break; // finished
        }

        if (!conflict) {
            return tableStart + (1 << numBits[squareInt]);
        }
    }

    assert(false);
    return nullptr;
}

void BB::Init() {
    InitPseudoAttacks();
    InitMagicBitboards();
    initialized = true;
}

int main() {
    BB::Init();
    printf("Rook\n");
    using SquareInt = std::underlying_type<Square>::type;
    for (SquareInt sqInt = 0; sqInt < SQUARE_NUM; sqInt++) {
        printf("{ nullptr, 0x%016lxULL, 0x%016lxULL, %u }\n", BB::rookAttacks[sqInt].mask, BB::rookAttacks[sqInt].mult, BB::rookAttacks[sqInt].shift);
    }
    printf("\n\nBishop\n");
    for (SquareInt sqInt = 0; sqInt < SQUARE_NUM; sqInt++) {
        printf("{ nullptr, 0x%016lxULL, 0x%016lxULL, %u }\n", BB::bishopAttacks[sqInt].mask, BB::bishopAttacks[sqInt].mult, BB::bishopAttacks[sqInt].shift);
    }
}
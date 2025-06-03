#include "bitboard.hpp"

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

void BB::Init() {
    InitPseudoAttacks();
    initialized = true;
}
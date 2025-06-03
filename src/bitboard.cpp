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

bool BBOp::initialized = false;
BBOp::Magic BBOp::rookAttacks[SQUARE_NUM];
BBOp::Magic BBOp::bishopAttacks[SQUARE_NUM];


template <Direction dir>
static Bitboard SlideAttack(Square square, Bitboard occupancy) {
    Bitboard bb = 0;
    Bitboard squareBB = BBOp::SquareBB(square);
    while (true) {
        squareBB = BBOp::Shift<dir>(squareBB);
        bb |= squareBB;
        if (!squareBB || squareBB & occupancy) return bb;
    }
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
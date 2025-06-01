#pragma once

#include <cstdint>
#include "types.hpp"

using Bitboard = uint64_t;

constexpr Bitboard FileBB(BoardFile file) {
    constexpr Bitboard FILE_A = (1 << 8) - 1;
    return FILE_A << (static_cast<uint8_t>(file) * 8);
}

constexpr Bitboard RankBB(BoardRank rank) {
    return 0x0101010101010101 << static_cast<uint8_t>(rank);
}

constexpr Bitboard SquareBB(Square square) {
    return 1ull << static_cast<uint8_t>(square);
}


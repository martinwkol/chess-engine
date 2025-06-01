#pragma once

#include <cstdint>
#include <type_traits>
#include "types.hpp"

using Bitboard = uint64_t;

constexpr Bitboard FileBB(BoardFile file) {
    constexpr Bitboard FILE_A = (1 << 8) - 1;
    return FILE_A << (static_cast<std::underlying_type<BoardFile>::type>(file) * 8);
}

constexpr Bitboard RankBB(BoardRank rank) {
    return 0x0101010101010101 << static_cast<std::underlying_type<BoardRank>::type>(rank);
}

constexpr Bitboard SquareBB(Square square) {
    return 1ull << static_cast<std::underlying_type<Square>::type>(square);
}


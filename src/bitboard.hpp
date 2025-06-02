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

template <Direction dir>
constexpr Bitboard Shift(Bitboard bb) {
    constexpr bool ShiftsUp     = dir == Direction::UP || dir == Direction::UP_LEFT || dir == Direction::UP_RIGHT;
    constexpr bool ShiftsDown   = dir == Direction::DOWN || dir == Direction::DOWN_LEFT || dir == Direction::DOWN_RIGHT;
    constexpr bool ShiftsRight  = dir == Direction::RIGHT || dir == Direction::UP_RIGHT || dir == Direction::DOWN_RIGHT;
    constexpr bool ShiftsLeft   = dir == Direction::LEFT || dir == Direction::UP_LEFT || dir == Direction::DOWN_LEFT;
    constexpr int shift         = (ShiftsUp ? 8 : 0) + (ShiftsDown ? -8 : 0) + (ShiftsRight ? 1 : 0) + (ShiftsLeft ? -1 : 0);
    if constexpr (shift >= 0)   bb <<= shift;
    else                        bb >>= -shift;
    if constexpr (ShiftsRight)  bb &= ~RankBB(BoardRank::R1);
    if constexpr (ShiftsLeft)   bb &= ~RankBB(BoardRank::R8);
    return bb;
}

inline Square Lsb(Bitboard bb) {
    assert(bb);

#if defined(__GNUC__) // gcc, clang, icx

    return static_cast<Square>(__builtin_ctzll(bb));

#elif defined(_MSC_VER)

    #ifdef _WIN64  // MSVC, WIN64

    unsigned long idx;
    _BitScanForward64(&idx, bb);
    return static_cast<Square>(idx);

    #else // MSVC, WIN32

    unsigned long idx;
    if (bb & 0xFFFFFFFF) {
        _BitScanForward(&idx, static_cast<uint32_t>(bb));
    } else {
        _BitScanForward(&idx, static_cast<uint32_t>(bb >> 32));
        idx += 32;
    }
    return static_cast<Square>(idx);

    #endif

#else

    #error "Compiler not supported"

#endif 
}


inline Bitboard LsbBB(Bitboard bb) {
    assert(bb);
    return bb & -bb;
}


inline Square PopLsb(Bitboard& bb) {
    assert(bb);
    Square sq = Lsb(bb);
    bb &= bb - 1;
    return sq;
}
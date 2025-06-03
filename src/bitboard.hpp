#pragma once

#include <cstdint>
#include <type_traits>
#include "types.hpp"

using Bitboard = uint64_t;

/**
 * Static class for bitboard operations
 */
class BB {
public:
    static constexpr Bitboard FILE_A = (1 << 8) - 1;
    static constexpr Bitboard FILE_B = FILE_A << (8 * 1);
    static constexpr Bitboard FILE_C = FILE_A << (8 * 2);
    static constexpr Bitboard FILE_D = FILE_A << (8 * 3);
    static constexpr Bitboard FILE_E = FILE_A << (8 * 4);
    static constexpr Bitboard FILE_F = FILE_A << (8 * 5);
    static constexpr Bitboard FILE_G = FILE_A << (8 * 6);
    static constexpr Bitboard FILE_H = FILE_A << (8 * 7);
    
    static constexpr Bitboard RANK_1 = 0x0101010101010101ull;
    static constexpr Bitboard RANK_2 = RANK_1 << 1;
    static constexpr Bitboard RANK_3 = RANK_1 << 2;
    static constexpr Bitboard RANK_4 = RANK_1 << 3;
    static constexpr Bitboard RANK_5 = RANK_1 << 4;
    static constexpr Bitboard RANK_6 = RANK_1 << 5;
    static constexpr Bitboard RANK_7 = RANK_1 << 6;
    static constexpr Bitboard RANK_8 = RANK_1 << 7;

    static void Init();

    static constexpr Bitboard FileBB(BoardFile file);
    static constexpr Bitboard RankBB(BoardRank rank);
    static constexpr Bitboard SquareBB(Square square);

    template <Direction dir>
    static constexpr Bitboard Shift(Bitboard bb);

    template <PieceType pieceType>
    static Bitboard AttacksBB(Square square);
    template <PieceType pieceType>
    static Bitboard AttacksBB(Square square, Bitboard occupancy);
    static Bitboard AttacksBB(PieceType pieceType, Square square);
    static Bitboard AttacksBB(PieceType pieceType, Square square, Bitboard occupancy);

    static uint32_t Count1s(Bitboard bb);
    static Bitboard LsbBB(Bitboard bb);
    static Square Lsb(Bitboard bb);
    static Square PopLsb(Bitboard& bb);

    BB() = delete;

private:
    struct Magic {
        Bitboard* table;
        Bitboard mask;
        uint64_t mult;
        uint8_t shift;

        uint64_t TableIndex(Bitboard occupancy) {
            return ((occupancy & mask) * mult) >> shift;
        }

        Bitboard Attacks(Bitboard occupancy) {
            return table[TableIndex(occupancy)];
        }
    };

    static bool initialized;

    static Bitboard pseudoAttacks[PIECE_TYPE_NUM][SQUARE_NUM];
    static Magic rookAttacks[SQUARE_NUM];
    static Magic bishopAttacks[SQUARE_NUM];

    static void InitPseudoAttacks();
    static void InitMagicBitboards();
    static Bitboard* InitMagicBitboards(PieceType pieceType, Square square, Bitboard* tableStart);

};

constexpr Bitboard BB::FileBB(BoardFile file) {
    return FILE_A << (ToInt(file) * 8);
}

constexpr Bitboard BB::RankBB(BoardRank rank) {
    return RANK_1 << ToInt(rank);
}

constexpr Bitboard BB::SquareBB(Square square) {
    return 1ull << ToInt(square);
}

template <Direction dir>
constexpr Bitboard BB::Shift(Bitboard bb) {
    constexpr bool ShiftsUp     = dir == Direction::UP || dir == Direction::UP_LEFT || dir == Direction::UP_RIGHT;
    constexpr bool ShiftsDown   = dir == Direction::DOWN || dir == Direction::DOWN_LEFT || dir == Direction::DOWN_RIGHT;
    constexpr bool ShiftsRight  = dir == Direction::RIGHT || dir == Direction::UP_RIGHT || dir == Direction::DOWN_RIGHT;
    constexpr bool ShiftsLeft   = dir == Direction::LEFT || dir == Direction::UP_LEFT || dir == Direction::DOWN_LEFT;
    constexpr int shift         = (ShiftsUp ? 8 : 0) + (ShiftsDown ? -8 : 0) + (ShiftsRight ? 1 : 0) + (ShiftsLeft ? -1 : 0);
    if constexpr (shift >= 0)   bb <<= shift;
    else                        bb >>= -shift;
    if constexpr (ShiftsRight)  bb &= ~RANK_1;
    if constexpr (ShiftsLeft)   bb &= ~RANK_8;
    return bb;
}

template <PieceType pieceType>
inline Bitboard BB::AttacksBB(Square square) {
    static_assert(pieceType != PieceType::Pawn);
    assert(initialized);
    return pseudoAttacks[ToInt(pieceType)][ToInt(square)]; 
}

template <PieceType pieceType>
inline Bitboard BB::AttacksBB(Square square, Bitboard occupancy) {
    static_assert(pieceType != PieceType::Pawn);
    assert(initialized);

    if constexpr (pieceType == PieceType::Queen) {
        return 
            AttacksBB<PieceType::Rook>(square, occupancy) | 
            AttacksBB<PieceType::Bishop>(square, occupancy);
    }
    else if constexpr (pieceType == PieceType::Rook) {
        return rookAttacks[ToInt(square)].Attacks(occupancy);
    }
    else if constexpr (pieceType == PieceType::Bishop) {
        return bishopAttacks[ToInt(square)].Attacks(occupancy);
    } 
    else {
        return pseudoAttacks[ToInt(pieceType)][ToInt(square)];
    }
}

inline Bitboard BB::AttacksBB(PieceType pieceType, Square square) {
    assert(pieceType != PieceType::Pawn);
    assert(initialized);
    return pseudoAttacks[ToInt(pieceType)][ToInt(square)]; 
}

inline Bitboard BB::AttacksBB(PieceType pieceType, Square square, Bitboard occupancy) {
    assert(pieceType != PieceType::Pawn);
    assert(initialized);

    switch (pieceType) {
    case PieceType::Queen:
        return AttacksBB<PieceType::Queen>(square, occupancy);
    
    case PieceType::Rook:
        return AttacksBB<PieceType::Rook>(square, occupancy);

    case PieceType::Bishop:
        return AttacksBB<PieceType::Bishop>(square, occupancy);

    default:
        return pseudoAttacks[ToInt(pieceType)][ToInt(square)];
    }
}

inline uint32_t BB::Count1s(Bitboard bb)
{
    return __builtin_popcountll(bb);
}

inline Square BB::Lsb(Bitboard bb) {
    assert(bb);

#if defined(__GNUC__) // gcc, clang, icx

    return ToSquare(__builtin_ctzll(bb));

#elif defined(_MSC_VER)

    #ifdef _WIN64  // MSVC, WIN64

    unsigned long idx;
    _BitScanForward64(&idx, bb);
    return ToSquare(idx);

    #else // MSVC, WIN32

    unsigned long idx;
    if (bb & 0xFFFFFFFF) {
        _BitScanForward(&idx, static_cast<uint32_t>(bb));
    } else {
        _BitScanForward(&idx, static_cast<uint32_t>(bb >> 32));
        idx += 32;
    }
    return ToSquare(idx);

    #endif

#else

    #error "Compiler not supported"

#endif 
}

inline Bitboard BB::LsbBB(Bitboard bb)
{
    assert(bb);
    return bb & -bb;
}

inline Square BB::PopLsb(Bitboard& bb) {
    assert(bb);
    Square sq = Lsb(bb);
    bb &= bb - 1;
    return sq;
}
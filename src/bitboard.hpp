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
    static constexpr Bitboard FILE_A = 0x0101010101010101ull;
    static constexpr Bitboard FILE_B = FILE_A << 1;
    static constexpr Bitboard FILE_C = FILE_A << 2;
    static constexpr Bitboard FILE_D = FILE_A << 3;
    static constexpr Bitboard FILE_E = FILE_A << 4;
    static constexpr Bitboard FILE_F = FILE_A << 5;
    static constexpr Bitboard FILE_G = FILE_A << 6;
    static constexpr Bitboard FILE_H = FILE_A << 7;
    
    static constexpr Bitboard RANK_1 = (1 << 8) - 1;
    static constexpr Bitboard RANK_2 = RANK_1 << (8 * 1);
    static constexpr Bitboard RANK_3 = RANK_1 << (8 * 2);
    static constexpr Bitboard RANK_4 = RANK_1 << (8 * 3);
    static constexpr Bitboard RANK_5 = RANK_1 << (8 * 4);
    static constexpr Bitboard RANK_6 = RANK_1 << (8 * 5);
    static constexpr Bitboard RANK_7 = RANK_1 << (8 * 6);
    static constexpr Bitboard RANK_8 = RANK_1 << (8 * 7);

    static void Init();

    static constexpr Bitboard FileBB(BoardFile file);
    static constexpr Bitboard RankBB(BoardRank rank);
    static constexpr Bitboard SquareBB(Square square);

    template <Direction dir>
    static constexpr Bitboard Shift(Bitboard bb);

    template <PieceType pieceType>
    static Bitboard Attacks(Square square);
    template <PieceType pieceType>
    static Bitboard Attacks(Square square, Bitboard occupancy);
    static Bitboard Attacks(PieceType pieceType, Square square);
    static Bitboard Attacks(PieceType pieceType, Square square, Bitboard occupancy);
    template <Color color>
    static constexpr Bitboard PawnAttacks(Square square);
    static constexpr Bitboard PawnAttacks(Color color, Square square);

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
    return FILE_A << ToInt(file);
}

constexpr Bitboard BB::RankBB(BoardRank rank) {
    return RANK_1 << (ToInt(rank) * 8);
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
    if constexpr (ShiftsRight)  bb &= ~FILE_A;
    if constexpr (ShiftsLeft)   bb &= ~FILE_H;
    return bb;
}

template <PieceType pieceType>
inline Bitboard BB::Attacks(Square square) {
    static_assert(pieceType != PieceType::Pawn);
    assert(initialized);
    return pseudoAttacks[ToInt(pieceType)][ToInt(square)]; 
}

template <PieceType pieceType>
inline Bitboard BB::Attacks(Square square, Bitboard occupancy) {
    static_assert(pieceType != PieceType::Pawn);
    assert(initialized);

    if constexpr (pieceType == PieceType::Queen) {
        return 
            Attacks<PieceType::Rook>(square, occupancy) | 
            Attacks<PieceType::Bishop>(square, occupancy);
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

template <Color color>
constexpr Bitboard BB::PawnAttacks(Square square) {
    constexpr Bitboard squareBB = SquareBB(square);
    if constexpr (color == Color::White) {
        return Shift<Direction::UP_LEFT>(squareBB) | Shift<Direction::UP_RIGHT>(squareBB);
    } else {
        return Shift<Direction::DOWN_LEFT>(squareBB) | Shift<Direction::DOWN_RIGHT>(squareBB);
    }
}

inline Bitboard BB::Attacks(PieceType pieceType, Square square) {
    assert(pieceType != PieceType::Pawn);
    assert(initialized);
    return pseudoAttacks[ToInt(pieceType)][ToInt(square)]; 
}

inline Bitboard BB::Attacks(PieceType pieceType, Square square, Bitboard occupancy) {
    assert(pieceType != PieceType::Pawn);
    assert(initialized);

    switch (pieceType) {
    case PieceType::Queen:
        return Attacks<PieceType::Queen>(square, occupancy);
    
    case PieceType::Rook:
        return Attacks<PieceType::Rook>(square, occupancy);

    case PieceType::Bishop:
        return Attacks<PieceType::Bishop>(square, occupancy);

    default:
        return pseudoAttacks[ToInt(pieceType)][ToInt(square)];
    }
}

inline constexpr Bitboard BB::PawnAttacks(Color color, Square square) {
    const Bitboard squareBB = SquareBB(square);
    if (color == Color::White) {
        return Shift<Direction::UP_LEFT>(squareBB) | Shift<Direction::UP_RIGHT>(squareBB);
    } else {
        return Shift<Direction::DOWN_LEFT>(squareBB) | Shift<Direction::DOWN_RIGHT>(squareBB);
    }
}

inline uint32_t BB::Count1s(Bitboard bb) {
    // TODO: support more compilers
#if defined(__GNUC__)

    return __builtin_popcountll(bb);

#else
    
    #error "Compiler not supported"

#endif
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

inline Bitboard BB::LsbBB(Bitboard bb) {
    assert(bb);
    return bb & -bb;
}

inline Square BB::PopLsb(Bitboard& bb) {
    assert(bb);
    Square sq = Lsb(bb);
    bb &= bb - 1;
    return sq;
}
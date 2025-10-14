#pragma once

#include <cstdint>
#include "types.hpp"

constexpr uint8_t CASTLING_RIGHTS_NUM = 16;

class CastlingRights {
public:
    static constexpr uint8_t WHITE_QUEENSIDE = 1;
    static constexpr uint8_t WHITE_KINGSIDE = 2;
    static constexpr uint8_t BLACK_QUEENSIDE = 4;
    static constexpr uint8_t BLACK_KINGSIDE = 8;
    static constexpr uint8_t WHITE = WHITE_QUEENSIDE | WHITE_KINGSIDE;
    static constexpr uint8_t BLACK = BLACK_QUEENSIDE | BLACK_KINGSIDE;
    static constexpr uint8_t ALL = WHITE | BLACK;
    static constexpr uint8_t NONE = 0;
    
    CastlingRights() { mRights = ALL; }
    CastlingRights(uint8_t rights) { mRights = rights; }
    operator uint8_t&() { return mRights; }

    template <Color color> void ForbidCastling() { 
        static_assert(IsValid(color));
        mRights &= color == Color::White ? ~WHITE : ~BLACK; 
    }

    template <Color color> void AllowCastlingQueenside() { 
        static_assert(IsValid(color));
        mRights |= color == Color::White ? WHITE_QUEENSIDE : BLACK_QUEENSIDE; 
    }

    template <Color color> void ForbidCastlingQueenside() { 
        static_assert(IsValid(color));
        mRights &= color == Color::White ? ~WHITE_QUEENSIDE : ~BLACK_QUEENSIDE; 
    }

    template <Color color> void AllowCastlingKingside() { 
        static_assert(IsValid(color));
        mRights |= color == Color::White ? WHITE_KINGSIDE : BLACK_KINGSIDE; 
    }

    template <Color color> void ForbidCastlingKingside() { 
        static_assert(IsValid(color));
        mRights &= color == Color::White ? ~WHITE_KINGSIDE : ~BLACK_KINGSIDE; 
    }

    template <Color color> bool CanCastleQueenside() const { 
        static_assert(IsValid(color));
        return (mRights & (color == Color::White ? WHITE_QUEENSIDE : BLACK_QUEENSIDE)); 
    }

    template <Color color> bool CanCastleKingside() const { 
        static_assert(IsValid(color));
        return (mRights & (color == Color::White ? WHITE_KINGSIDE : BLACK_KINGSIDE)); 
    }

    bool AnyCastlingAllowed() const {
        return mRights != NONE;
    }

private:
    uint8_t mRights;
};
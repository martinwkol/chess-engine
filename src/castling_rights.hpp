#pragma once

#include <cstdint>
#include "types.hpp"

class CastlingRights {
public:
    static constexpr uint8_t WHITE_QUEENSIDE = 1;
    static constexpr uint8_t WHITE_KINGSIDE = 2;
    static constexpr uint8_t BLACK_QUEENSIDE = 4;
    static constexpr uint8_t BLACK_KINGSIDE = 8;
    static constexpr uint8_t WHITE = WHITE_QUEENSIDE | WHITE_KINGSIDE;
    static constexpr uint8_t BLACK = BLACK_QUEENSIDE | BLACK_KINGSIDE;
    static constexpr uint8_t ALL = WHITE | BLACK;
    
    CastlingRights() { mRights = ALL; }
    CastlingRights(uint8_t rights) { mRights = rights; }
    operator uint8_t&() { return mRights; }

    template <Color color> void ForbidCastling() { 
        static_assert(IsValid(color));
        mRights &= color == Color::White ? ~WHITE : ~BLACK; 
    }

    template <Color color> void ForbidCastlingQueenside() { 
        static_assert(IsValid(color));
        mRights &= color == Color::White ? ~WHITE_QUEENSIDE : ~BLACK_QUEENSIDE; 
    }

    template <Color color> void ForbidCastlingKingside() { 
        static_assert(IsValid(color));
        mRights &= color == Color::White ? ~WHITE_KINGSIDE : ~BLACK_KINGSIDE; 
    }

    template <Color color> bool CanCastleQueenside() { 
        static_assert(IsValid(color));
        return (mRights & (color == Color::White ? WHITE_QUEENSIDE : BLACK_QUEENSIDE)); 
    }

    template <Color color> bool CanCastleKingside() { 
        static_assert(IsValid(color));
        return (mRights & (color == Color::White ? WHITE_KINGSIDE : BLACK_KINGSIDE)); 
    }


private:
    uint8_t mRights;
};
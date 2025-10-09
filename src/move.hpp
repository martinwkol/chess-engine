#pragma once

#include "types.hpp"
#include <cstdint>

// https://www.chessprogramming.org/Encoding_Moves
class Move {
public:
    static Move NewQuiet(Square from, Square to) {
        return Move((ToInt(from) << FROM_SHIFT) | (ToInt(to) << TO_SHIFT));
    }

    static Move NewCapture(Square from, Square to) {
        return Move((ToInt(from) << FROM_SHIFT) | (ToInt(to) << TO_SHIFT) | CAPTURE_FLAG);
    }

    static Move NewEnPassant(Square from, Square to) {
        return Move((ToInt(from) << FROM_SHIFT) | (ToInt(to) << TO_SHIFT) | EN_PASSANT_FLAGS);
    }

    static Move NewKingsideCastle(Square from, Square to) {
        return Move((ToInt(from) << FROM_SHIFT) | (ToInt(to) << TO_SHIFT) | KINGSIDE_CASTLE_FLAGS);
    }
    
    static Move NewQueensideCastle(Square from, Square to) {
        return Move((ToInt(from) << FROM_SHIFT) | (ToInt(to) << TO_SHIFT) | QUEENSIDE_CASTLE_FLAGS);
    }

    static Move NewPromotionNormal(Square from, Square to, PieceType type) {
        return Move(
            (ToInt(from) << FROM_SHIFT) | (ToInt(to) << TO_SHIFT) | 
            ((ToInt(type) & PROMOTION_TYPE_MASK) << PROMOTION_TYPE_SHIFT) | 
            PROMOTION_FLAG
        );
    }

    static Move NewPromotionCapture(Square from, Square to, PieceType type) {
        return Move(
            (ToInt(from) << FROM_SHIFT) | (ToInt(to) << TO_SHIFT) | 
            ((ToInt(type) & PROMOTION_TYPE_MASK) << PROMOTION_TYPE_SHIFT) | 
            (PROMOTION_FLAG | CAPTURE_FLAG)
        );
    }

    Move() = default;

    Square GetFrom() const { return ToSquare((mMove >> FROM_SHIFT) & FROM_MASK); }
    Square GetTo() const { return ToSquare((mMove >> TO_SHIFT) & TO_MASK); }

    bool IsPromotion() const { return mMove & PROMOTION_FLAG != 0; }
    PieceType GetPromotionType() const { assert(IsPromotion()); return ToPieceType((mMove >> PROMOTION_TYPE_SHIFT) & PROMOTION_TYPE_MASK); }
    
    bool IsCapture() const { return mMove & CAPTURE_FLAG != 0; }
    bool IsEnPassant() const { return mMove & FLAGS_MASK == EN_PASSANT_FLAGS; }
    
    bool IsCastle() const { return mMove & FLAGS_EXCEPT_FIRST == CASTLE_FLAG; }
    bool IsKingsideCastle() const { return mMove & FLAGS_MASK == KINGSIDE_CASTLE_FLAGS; }
    bool IsQueensideCastle() const { return mMove & FLAGS_MASK == QUEENSIDE_CASTLE_FLAGS; }
    
    bool IsDoublePawnPush() const { return mMove & FLAGS_MASK == DOUBLE_PAWN_FLAG; }

    bool IsQuiet() const { return mMove & FLAGS_MASK == 0; }
    

private:
    static constexpr int FROM_SHIFT                 = 0;
    static constexpr int TO_SHIFT                   = 6;
    static constexpr uint16_t FROM_MASK             = (1 << 6) - 1;
    static constexpr uint16_t TO_MASK               = (1 << 6) - 1;

    static constexpr uint16_t FLAGS_MASK            = ((1 << 4) - 1) << 12;
    static constexpr uint16_t FLAGS_EXCEPT_FIRST    = ((1 << 3) - 1) << 13;

    static constexpr uint16_t DOUBLE_PAWN_FLAG      = 1 << 12;
    
    static constexpr uint16_t CASTLE_FLAG           = 1 << 13;
    static constexpr uint16_t KINGSIDE_CASTLE_FLAGS = CASTLE_FLAG;
    static constexpr uint16_t QUEENSIDE_CASTLE_FLAGS= CASTLE_FLAG | (1 << 12);

    static constexpr uint16_t CAPTURE_FLAG          = 1 << 14;
    static constexpr uint16_t EN_PASSANT_FLAGS      = CAPTURE_FLAG | (1 << 12);
    
    static constexpr uint16_t PROMOTION_FLAG        = 1 << 15;
    static constexpr int PROMOTION_TYPE_SHIFT       = 12;
    static constexpr uint16_t PROMOTION_TYPE_MASK   = (1 << 2) - 1;
    
    uint16_t mMove;

    Move(uint16_t move) { mMove = move; }

};
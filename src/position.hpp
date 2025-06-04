#pragma once

#include "types.hpp"
#include "bitboard.hpp"
#include "castling_rights.hpp"

class Position {
public:

private:
    Bitboard mPiecesBB[COLOR_NUM][PIECE_TYPE_NUM];
    Piece mBoard[SQUARE_NUM];
    Color mSideToMove;
    CastlingRights mCastlingRights;
    Square mEnPassant;
    uint32_t mReversableMovesCnt;
};
#pragma once

#include "position.hpp"
#include "move.hpp"
#include "move_generation.hpp"

class MoveList {
public:
    MoveList(const Position& pos) { mEnd = GenerateMoves(mMoves.begin(), pos); }

    Move* begin()               { return mMoves.begin(); }
    Move* end()                 { return mEnd; }
    const Move* begin() const   { return mMoves.begin(); }
    const Move* end() const     { return mEnd; }

private:
    static constexpr int MAX_NUM_MOVES = 256;

    Array<Move, MAX_NUM_MOVES> mMoves;
    Move* mEnd;
};
#pragma once

#include "position.hpp"
#include "move.hpp"
#include "move_generation.hpp"

class MoveList {
public:
    MoveList(const Position& pos) { mEnd = GenerateMoves(mMoves, pos); }

    Move* begin() { return mMoves; }
    Move* end() { return mEnd; }
    const Move* begin() const { return mMoves; }
    const Move* end() const { return mEnd; }

private:
    static constexpr int MAX_NUM_MOVES = 256;

    Move mMoves[MAX_NUM_MOVES];
    Move* mEnd;
};
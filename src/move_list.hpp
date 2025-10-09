#pragma once

#include "move.hpp"

class MoveList {
public:
    MoveList() { mEnd = mMoves; }

    void SetEnd(Move* end) { mEnd = end; }

    Move* begin() { return mMoves; }
    Move* end() { return mEnd; }
    const Move* begin() const { return mMoves; }
    const Move* end() const { return mEnd; }

private:
    static constexpr int MAX_NUM_MOVES = 256;

    Move mMoves[MAX_NUM_MOVES];
    Move* mEnd;
};
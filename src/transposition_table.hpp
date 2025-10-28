#pragma once

#include "zobrist_hash.hpp"
#include "move.hpp"

#include <vector>

// https://www.chessprogramming.org/Transposition_Table
class TranspositionTable {
public:
    struct Entry {
        enum class Type : uint8_t {
            PV,
            Fail_Low,
            Fail_High
        };

        ZobristHash hash;
        Move bestMove;
        Score score;
        uint16_t depth;
        Type type;
        uint8_t age;
    };

    explicit TranspositionTable(uint64_t size);
    void AddEntry(ZobristHash hash, Move bestMove, Score score, uint16_t depth, Entry::Type type);

private:
    std::vector<Entry> mTable;

};
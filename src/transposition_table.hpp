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

    explicit TranspositionTable(uint8_t log_2_size);
    void AddEntry(Entry entry);
    Entry GetEntry(ZobristHash hash);
    void NewSearch();

private:
    std::vector<Entry> mTable;
    ZobristHash::HashType mask;
    uint8_t currentAge;

    ZobristHash::HashType IndexOf(ZobristHash hash);
    bool ShouldOverwrite(Entry oldEntry, Entry newEntry);

};

inline void TranspositionTable::AddEntry(Entry entry) {
    Entry& stored = mTable[IndexOf(entry.hash)];
    if (ShouldOverwrite(stored, entry)) stored = entry;
}

inline TranspositionTable::Entry TranspositionTable::GetEntry(ZobristHash hash) {
    return mTable[IndexOf(hash)];
}

inline void TranspositionTable::NewSearch() {
    ++currentAge;
}

inline ZobristHash::HashType TranspositionTable::IndexOf(ZobristHash hash) {
    return hash & mask;
}

inline bool TranspositionTable::ShouldOverwrite(Entry oldEntry, Entry newEntry) {
    if (oldEntry.hash == 0) return true;
    if (oldEntry.age != currentAge) return true;
    return oldEntry.depth <= newEntry.depth;
}

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
    void AddEntry(Entry entry);
    Entry GetEntry(ZobristHash hash);
    void NewSearch();

private:
    std::vector<Entry> mTable;
    uint8_t currentAge;

    std::size_t IndexOf(ZobristHash hash);
    bool WriteEntry(Entry entry);

};

inline void TranspositionTable::AddEntry(Entry entry) {
    // TODO: slow because division?
    std::size_t index = IndexOf(entry.hash);
    Entry& stored = mTable[index];
    if (stored.hash != 0) {
        if (stored.age == currentAge) {
            if (stored.type != Entry::Type::PV && entry.type == Entry::Type::PV) 
        }
    }
    stored = entry;
}

inline TranspositionTable::Entry TranspositionTable::GetEntry(ZobristHash hash) {
    return mTable[IndexOf(hash)];
}

inline void TranspositionTable::NewSearch() {
    ++currentAge;
}

inline std::size_t TranspositionTable::IndexOf(ZobristHash hash) {
    // TODO: Slow?
    return hash % mTable.size();
}

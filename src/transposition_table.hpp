#pragma once

#include "zobrist_hash.hpp"
#include "move.hpp"

#include <vector>
#include <cassert>

// https://www.chessprogramming.org/Transposition_Table
class TranspositionTable {
public:
    class Entry {
    public:
        enum class Type : uint8_t {
            PV,
            Fail_Low,
            Fail_High
        };

        Entry() : mHash(0) {}
        Entry(ZobristHash hash, Move bestMove, Score score, uint16_t depth, Type type)
            : mHash(hash), mBestMove(bestMove), mScore(score), mDepth(depth), mType(type) {}
        
        bool IsValid() const        { return mHash != 0; }

        ZobristHash GetHash() const { return mHash; }
        Move GetBestMove() const    { assert(IsValid()); return mBestMove; }
        Score GetScore() const      { assert(IsValid()); return mScore; }
        uint16_t GetDepth() const   { assert(IsValid()); return mDepth; }
        Type GetType() const        { assert(IsValid()); return mType; }

    private:
        ZobristHash mHash;
        Move mBestMove;
        Score mScore;
        uint16_t mDepth;
        Type mType;
        uint8_t mAge;

        uint8_t GetAge() const { return mAge; }

        friend class TranspositionTable;

    };

    explicit TranspositionTable(uint8_t log_2_size);
    void SetEntry(Entry entry);
    Entry GetEntry(ZobristHash hash);
    void NewSearch();

private:
    std::vector<Entry> mTable;
    ZobristHash::HashType mask;
    uint8_t currentAge;

    ZobristHash::HashType IndexOf(ZobristHash hash);
    bool ShouldOverwrite(Entry oldEntry, Entry newEntry);

};

inline void TranspositionTable::SetEntry(Entry entry) {
    Entry& stored = mTable[IndexOf(entry.GetHash())];
    if (ShouldOverwrite(stored, entry)) {
        stored = entry;
        stored.mAge = currentAge;
    }
}

inline TranspositionTable::Entry TranspositionTable::GetEntry(ZobristHash hash) {
    Entry entry = mTable[IndexOf(hash)];
    if (entry.GetHash() == hash)    return entry;
    else                            return Entry();
}

inline void TranspositionTable::NewSearch() {
    ++currentAge;
}

inline ZobristHash::HashType TranspositionTable::IndexOf(ZobristHash hash) {
    return hash & mask;
}

inline bool TranspositionTable::ShouldOverwrite(Entry oldEntry, Entry newEntry) {
    if (oldEntry.GetHash() == 0) return true;
    if (oldEntry.GetAge() != currentAge) return true;
    return oldEntry.GetDepth() <= newEntry.GetDepth();
}

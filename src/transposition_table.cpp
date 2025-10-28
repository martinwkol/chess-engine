#include "transposition_table.hpp"

TranspositionTable::TranspositionTable(uint64_t size) {
    mTable.resize(size, {

    });
}
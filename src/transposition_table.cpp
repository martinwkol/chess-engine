#include "transposition_table.hpp"

TranspositionTable::TranspositionTable(uint8_t log_2_size) {
    mTable.resize(1 << log_2_size, Entry {
        ZobristHash()
    });
    mask = (1 << log_2_size) - 1;
    currentAge = 0;
}
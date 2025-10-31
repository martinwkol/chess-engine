#pragma once

#include "position.hpp"
#include "transposition_table.hpp"

void Search(Position& pos, TranspositionTable& table, int depth);
#include "position.hpp"
#include "move_list.hpp"
#include "move_generation.hpp"

#include <iostream>

struct PerftResult {
    uint64_t moves;
    uint64_t captures;
    uint64_t enPassant;
    uint64_t castles;
    uint64_t promotion;
    uint64_t checks;
    uint64_t doubleChecks;
};

uint64_t PerftSimple(Position& pos, int depth) {
    if (depth == 0) return 1;

    MoveList moveList(pos);
    if (depth == 1) return moveList.end() - moveList.begin();

    uint64_t numMoves = 0;
    for (Move move : moveList) {
        pos.DoMove(move);
        numMoves += PerftSimple(pos, depth - 1);
        pos.UndoMove();
    }
    return numMoves;
}

void TestPerftSimple(const char* fen, int depth, uint64_t correctNumMoves) {
    Position pos(fen);
    uint64_t numMoves = PerftSimple(pos, depth);
    if (numMoves == correctNumMoves) {
        std::cout   << "PASSED:\tSimple Perft [" << fen << ", depth=" << depth << "]" << std::endl;
    } else {
        std::cout   << "FAILED:\tSimple Perft [" << fen << ", depth=" << depth << "]: " 
                    << "expected=" << correctNumMoves << " result=" << numMoves << std::endl;
    }
}

int main() {
    BB::Init();
    TestPerftSimple("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, 3'894'594);
}
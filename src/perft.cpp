#include "position.hpp"
#include "move_list.hpp"
#include "move_generation.hpp"

#include <iostream>
#include <format>

struct PerftResult {
    uint64_t moves = 0;
    uint64_t captures = 0;
    uint64_t enPassant = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
    uint64_t doubleChecks = 0;

    bool operator==(const PerftResult& other) const {
        return
            moves           == other.moves      &&
            captures        == other.captures   &&
            enPassant       == other.enPassant  &&
            castles         == other.castles    && 
            promotions      == other.promotions &&
            checks          == other.checks     &&
            doubleChecks    == other.doubleChecks;
    }
    bool operator!=(const PerftResult& other) const {
        return !(*this == other);
    }
};

std::ostream& operator<<(std::ostream& stream, const PerftResult& result) {
    return stream << std::format(
        "[moves={}, captures={}, enPassant={}, castles={}, "
        "promotions={}, checks={}, doubleChecks={}]",
        result.moves, result.captures, result.enPassant, result.castles, 
        result.promotions, result.checks, result.doubleChecks
    );
}

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

void Perft(Position& pos, int depth, PerftResult& result) {
    MoveList moveList(pos);

    if (depth == 1) {
        for (Move move : moveList) {
            pos.DoMove(move);
            result.moves++;
            if (move.IsCapture())       result.captures++;
            if (move.IsEnPassant())     result.enPassant++;
            if (move.IsCastle())        result.castles++;
            if (move.IsPromotion())     result.promotions++;
            if (pos.IsCheck())          result.checks++;
            if (pos.IsDoubleCheck())    result.doubleChecks++;
            pos.UndoMove();
        }
    } else {
        for (Move move : moveList) {
            pos.DoMove(move);
            Perft(pos, depth - 1, result);
            pos.UndoMove();
        }
    }
}

PerftResult Perft(Position& pos, int depth) {
    PerftResult result;
    Perft(pos, depth, result);
    return result;
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

void TestPerft(const char* fen, int depth, const PerftResult& correctPerftResult) {
    Position pos(fen);
    PerftResult result = Perft(pos, depth);
    if (result == correctPerftResult) {
        std::cout   << "PASSED:\tPerft [" << fen << ", depth=" << depth << "]" << std::endl;
    } else {
        std::cout   << "FAILED:\tPerft [" << fen << ", depth=" << depth << "]: " << std::endl;
        std::cout   << "expected=" << correctPerftResult << std::endl;
        std::cout   << "result=" << result << std::endl;
    }
}

int main() {
    BB::Init();
    TestPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2, {
        2039,
        351,
        1 ,
        91,
        0,
        3,
        0 	
    });
    TestPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, {
        4085603,
        757163,
        1929,
        128013,
        15172,
        25523,
        6 	
    });
    // TestPerftSimple("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4, 3'894'594);
}
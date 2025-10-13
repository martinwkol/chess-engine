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

void PrintPerftPerMove(Position& pos, int depth) {
    MoveList list(pos);
    uint64_t total = 0;
    for (Move move : list) {
        pos.DoMove(move);
        uint64_t perftResult = PerftSimple(pos, depth - 1);
        total += perftResult;
        Square from = move.GetFrom();
        Square to = move.GetTo();
        std::cout 
            << char('a' + ToInt(FileOf(from))) << char('1' + ToInt(RankOf(from)))
            << char('a' + ToInt(FileOf(to))) << char('1' + ToInt(RankOf(to)))
            << ": " << perftResult
            << std::endl;
        pos.UndoMove();
    }
    std::cout << "Total: " << total << std::endl;
}

int main() {
    BB::Init();

    TestPerftSimple("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 7, 3195901860);
    TestPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, {
        119060324,
        2812008,
        5248,
        0,
        0,
        809099,
        46	
    });

    TestPerftSimple("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 6, 8031647685);
    TestPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, {
        193690690,
        35043416,
        73365,
        4993637,
        8392,
        3309887,
        2645	
    });

    TestPerftSimple("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 8, 3009794393);
    TestPerftSimple("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6, 706045033);
    TestPerftSimple("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, 89941194);
    TestPerftSimple("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 6, 6923051137);
}
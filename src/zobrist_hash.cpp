#include "zobrist_hash.hpp"

#include <random>

bool ZobristHash::initialized = false;

Array2D<ZobristHash::HashType, SQUARE_NUM, PIECE_NUM> ZobristHash::pieces;
ZobristHash::HashType                                 ZobristHash::sideToMove;
Array<ZobristHash::HashType, CASTLING_RIGHTS_NUM>     ZobristHash::castlingRights;
Array<ZobristHash::HashType, BOARD_FILE_NUM>          ZobristHash::enPassantFile;

void ZobristHash::Init(uint64_t seed) {
    std::mt19937_64 mt(seed);
    std::uniform_int_distribution<HashType> udist(0, std::numeric_limits<HashType>::max());

    for (auto& piecesOfSquare : pieces) {
        for (auto& piece : piecesOfSquare) {
            piece = udist(mt);
        }
    }

    sideToMove = udist(mt);

    for (auto& castlingRight : castlingRights) {
        castlingRight = udist(mt);
    }

    for (auto& enPassant : enPassantFile) {
        enPassant = udist(mt);
    }

    initialized = true;
}
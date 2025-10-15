#pragma once

#include "types.hpp"
#include "castling_rights.hpp"

#include <cstdint>
#include <cassert>
#include <array>

// See https://www.chessprogramming.org/Zobrist_Hashing
class ZobristHash {
public:
    using HashType = uint64_t;

    static void Init(uint64_t seed = 0);

    ZobristHash() { assert(initialized); }

    void SwitchPiece(Square square, Piece piece);
    void SwitchSideToMove();
    void SwitchCastlingRights(CastlingRights rights);
    void SwitchEnPassantFile(BoardFile file);
    operator HashType() const;

private:
    static bool initialized;

    static Array2D<HashType, SQUARE_NUM, PIECE_NUM> pieces;
    static HashType                                 sideToMove;
    static Array<HashType, CASTLING_RIGHTS_NUM>     castlingRights;
    static Array<HashType, BOARD_FILE_NUM>          enPassantFile;

    HashType mHash = 0;

};

inline void ZobristHash::SwitchPiece(Square square, Piece piece) {
    assert(IsValid(square));
    assert(IsValid(piece));
    mHash ^= pieces[ToInt(square)][ToInt(piece)];
}

inline void ZobristHash::SwitchSideToMove() {
    mHash ^= sideToMove;
}

inline void ZobristHash::SwitchCastlingRights(CastlingRights rights) {
    mHash ^= castlingRights[rights];
}

inline void ZobristHash::SwitchEnPassantFile(BoardFile file) {
    assert(IsValid(file));
    mHash ^= enPassantFile[ToInt(file)];
}

inline ZobristHash::operator HashType() const {
    return mHash;
}

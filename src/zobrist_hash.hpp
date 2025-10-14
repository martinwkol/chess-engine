#pragma once

#include "types.hpp"
#include "castling_rights.hpp"

#include <cstdint>
#include <cassert>
#include <array>

class ZobristHash {
public:
    using HashType = uint64_t;

    static void Init(uint64_t seed = 0);

    ZobristHash() { assert(initialized); }


private:
    static bool initialized;

    static std::array<std::array<HashType, PIECE_NUM>, SQUARE_NUM>  pieces;
    static HashType                                                 sideToMove;
    static std::array<HashType, CASTLING_RIGHTS_NUM>                castlingRights;
    static std::array<HashType, BOARD_FILE_NUM>                     enPassantFile;

    HashType mHash = 0;

};
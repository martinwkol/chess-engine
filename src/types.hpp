#pragma once

#include <cstdint>
#include <cassert>

/////////////////////////////////////////////////////////
///////////////////// BOARD SQUARES /////////////////////
/////////////////////////////////////////////////////////

enum class File : uint8_t {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6, 
    H = 7
};

enum class Rank : uint8_t {
    R1 = 0,
    R2 = 1,
    R3 = 2,
    R4 = 3,
    R5 = 4,
    R6 = 5,
    R7 = 6,
    R8 = 7
};

enum class Square : uint8_t {
    A1 = 0,  A2 = 1,  A3 = 2,  A4 = 3,  A5 = 4,  A6 = 5,  A7 = 6,  A8 = 7,  
    B1 = 8,  B2 = 9,  B3 = 10, B4 = 11, B5 = 12, B6 = 13, B7 = 14, B8 = 15,
    C1 = 16, C2 = 17, C3 = 18, C4 = 19, C5 = 20, C6 = 21, C7 = 22, C8 = 23, 
    D1 = 24, D2 = 25, D3 = 26, D4 = 27, D5 = 28, D6 = 29, D7 = 30, D8 = 31, 
    E1 = 32, E2 = 33, E3 = 34, E4 = 35, E5 = 36, E6 = 37, E7 = 38, E8 = 39, 
    F1 = 40, F2 = 41, F3 = 42, F4 = 43, F5 = 44, F6 = 45, F7 = 46, F8 = 47, 
    G1 = 48, G2 = 49, G3 = 50, G4 = 51, G5 = 52, G6 = 53, G7 = 54, G8 = 55, 
    H1 = 56, H2 = 57, H3 = 58, H4 = 59, H5 = 60, H6 = 61, H7 = 62, H8 = 63
};

constexpr bool IsValid(File file) {
    return File::A <= file && file <= File::H;
}

constexpr bool IsValid(Rank rank) {
    return Rank::R1 <= rank && rank <= Rank::R8;
}

constexpr bool IsValid(Square square) {
    return Square::A1 <= square && square <= Square::H8;
}

constexpr Square MakeSquare(File file, Rank rank) {
    assert(IsValid(file));
    assert(IsValid(rank));
    return static_cast<Square>(
        (static_cast<uint8_t>(rank) << 3) | static_cast<uint8_t>(file)
    );
}

constexpr File FileOf(Square square) {
    assert(IsValid(square));
    return static_cast<File>(
        static_cast<uint8_t>(square) & 7
    );
}

constexpr Rank RankOf(Square square) {
    assert(IsValid(square));
    return static_cast<Rank>(
        static_cast<uint8_t>(square) >> 3
    );
}




//////////////////////////////////////////////////////////
/////////////////////     PIECES     /////////////////////
//////////////////////////////////////////////////////////

enum class Color : uint8_t {
    White = 0,
    Black = 1
};

enum class PieceType : uint8_t {
    King = 0,
    Queen = 1,
    Rook = 2,
    Knight = 3,
    Bishop = 4,
    Pawn = 5
};

enum class Piece : uint8_t {
    WhiteKing = 0,   BlackKing = 1,  
    WhiteQueen = 2,  BlackQueen = 3, 
    WhiteRook = 4,   BlackRook = 5,  
    WhiteKnight = 6, BlackKnight = 7,
    WhiteBishop = 8, BlackBishop = 9,
    WhitePawn = 10,  BlackPawn = 11,  
};

constexpr bool IsValid(Color color) {
    return Color::White <= color && color <= Color::Black;
}

constexpr bool IsValid(PieceType type) {
    return PieceType::King <= type && type <= PieceType::Pawn;
}

constexpr bool IsValid(Piece piece) {
    return Piece::WhiteKing <= piece && piece <= Piece::BlackPawn;
}

constexpr Piece MakePiece(Color color, PieceType type) {
    assert(IsValid(color));
    assert(IsValid(type));
    return static_cast<Piece>(
        (static_cast<uint8_t>(type) << 1) | static_cast<uint8_t>(color)
    );
}

constexpr Color ColorOf(Piece piece) {
    assert(IsValid(piece));
    return static_cast<Color>(
        static_cast<uint8_t>(piece) & 1
    );
}

constexpr PieceType PieceTypeOf(Piece piece) {
    assert(IsValid(piece));
    return static_cast<PieceType>(
        static_cast<uint8_t>(piece) >> 1
    );
}


#pragma once

#include <cstdint>
#include <cassert>

/////////////////////////////////////////////////////////
/////////////////////   DIRECTION   /////////////////////
/////////////////////////////////////////////////////////

constexpr uint8_t DIRECTION_NUM = 8;
enum class Direction : int8_t {
    UP = 8,
    UP_RIGHT = 9,
    RIGHT = 1,
    DOWN_RIGHT = -7,
    DOWN = -8,
    DOWN_LEFT = -9,
    LEFT = -1,
    UP_LEFT = 7
};

constexpr int8_t ToInt(Direction direction) {
    return static_cast<int8_t>(direction);
}

constexpr Direction ToDirection(int8_t n) {
    return Direction(n);
}

constexpr Direction operator-(Direction direction) {
    return ToDirection(-ToInt(direction));
}





/////////////////////////////////////////////////////////
///////////////////// BOARD SQUARES /////////////////////
/////////////////////////////////////////////////////////

constexpr uint8_t BOARD_FILE_NUM = 8;
enum class BoardFile : int8_t {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6, 
    H = 7,  
    None = 127
};

constexpr uint8_t BOARD_RANK_NUM = 8;
enum class BoardRank : int8_t {
    R1 = 0,
    R2 = 1,
    R3 = 2,
    R4 = 3,
    R5 = 4,
    R6 = 5,
    R7 = 6,
    R8 = 7,  
    None = 127
};

constexpr uint8_t SQUARE_NUM = 64;
enum class Square : int8_t {
    A1 = 0,  A2 = 1,  A3 = 2,  A4 = 3,  A5 = 4,  A6 = 5,  A7 = 6,  A8 = 7,  
    B1 = 8,  B2 = 9,  B3 = 10, B4 = 11, B5 = 12, B6 = 13, B7 = 14, B8 = 15,
    C1 = 16, C2 = 17, C3 = 18, C4 = 19, C5 = 20, C6 = 21, C7 = 22, C8 = 23, 
    D1 = 24, D2 = 25, D3 = 26, D4 = 27, D5 = 28, D6 = 29, D7 = 30, D8 = 31, 
    E1 = 32, E2 = 33, E3 = 34, E4 = 35, E5 = 36, E6 = 37, E7 = 38, E8 = 39, 
    F1 = 40, F2 = 41, F3 = 42, F4 = 43, F5 = 44, F6 = 45, F7 = 46, F8 = 47, 
    G1 = 48, G2 = 49, G3 = 50, G4 = 51, G5 = 52, G6 = 53, G7 = 54, G8 = 55, 
    H1 = 56, H2 = 57, H3 = 58, H4 = 59, H5 = 60, H6 = 61, H7 = 62, H8 = 63,  
    None = 127
};

constexpr bool IsValid(BoardFile file) {
    return BoardFile::A <= file && file <= BoardFile::H;
}

constexpr bool IsValid(BoardRank rank) {
    return BoardRank::R1 <= rank && rank <= BoardRank::R8;
}

constexpr bool IsValid(Square square) {
    return Square::A1 <= square && square <= Square::H8;
}

constexpr BoardFile ToBoardFile(int8_t n) {
    BoardFile file = static_cast<BoardFile>(n);
    assert(IsValid(file));
    return file;
}

constexpr int8_t ToInt(BoardFile file) {
    assert(IsValid(file));
    return static_cast<int8_t>(file);
}

constexpr BoardRank ToBoardRank(int8_t n) {
    BoardRank rank = static_cast<BoardRank>(n);
    assert(IsValid(rank));
    return rank;
}

constexpr int8_t ToInt(BoardRank rank) {
    assert(IsValid(rank));
    return static_cast<int8_t>(rank);
}

constexpr Square ToSquare(int8_t n) {
    Square square = static_cast<Square>(n);
    assert(IsValid(square));
    return square;
}

constexpr int8_t ToInt(Square square) {
    assert(IsValid(square));
    return static_cast<int8_t>(square);
}

constexpr Square MakeSquare(BoardFile file, BoardRank rank) {
    assert(IsValid(file));
    assert(IsValid(rank));
    return static_cast<Square>(
        (static_cast<int8_t>(rank) << 3) | static_cast<int8_t>(file)
    );
}

constexpr BoardFile FileOf(Square square) {
    assert(IsValid(square));
    return static_cast<BoardFile>(
        static_cast<int8_t>(square) & 7
    );
}

constexpr BoardRank RankOf(Square square) {
    assert(IsValid(square));
    return static_cast<BoardRank>(
        static_cast<int8_t>(square) >> 3
    );
}

constexpr Square operator+(Square square, Direction direction) {
    return ToSquare(ToInt(square) + ToInt(direction));
}

constexpr Square& operator+=(Square& square, Direction direction) {
    square = square + direction;
    return square;
}

constexpr Square operator-(Square square, Direction direction) {
    return ToSquare(ToInt(square) - ToInt(direction));
}

constexpr Square& operator-=(Square& square, Direction direction) {
    square = square - direction;
    return square;
}






//////////////////////////////////////////////////////////
/////////////////////     PIECES     /////////////////////
//////////////////////////////////////////////////////////

constexpr uint8_t COLOR_NUM = 2;
enum class Color : int8_t {
    White = 0,
    Black = 1,  
    None = 127
};

constexpr uint8_t PIECE_TYPE_NUM = 6;
enum class PieceType : int8_t {
    Knight = 0,
    Bishop = 1,
    Rook = 2,
    Queen = 3,
    King = 4,
    Pawn = 5,  
    None = 127
};

constexpr uint8_t PIECE_NUM = 12;
enum class Piece : int8_t {
    WhiteKnight = 0,    BlackKnight = 1,  
    WhiteBishop = 2,    BlackBishop = 3, 
    WhiteRook = 4,      BlackRook = 5,  
    WhiteQueen = 6,     BlackQueen = 7,
    WhiteKing = 8,      BlackKing = 9,
    WhitePawn = 10,     BlackPawn = 11,  
    None = 127
};

constexpr bool IsValid(Color color) {
    return Color::White <= color && color <= Color::Black;
}

constexpr bool IsValid(PieceType type) {
    return PieceType::Knight <= type && type <= PieceType::Pawn;
}

constexpr bool IsValid(Piece piece) {
    return Piece::WhiteKnight <= piece && piece <= Piece::BlackPawn;
}

constexpr Color ToColor(int8_t n) {
    Color color = static_cast<Color>(n);
    assert(IsValid(color));
    return color;
}

constexpr int8_t ToInt(Color color) {
    assert(IsValid(color));
    return static_cast<int8_t>(color);
}

constexpr PieceType ToPieceType(int8_t n) {
    PieceType pieceType = static_cast<PieceType>(n);
    assert(IsValid(pieceType));
    return pieceType;
}

constexpr int8_t ToInt(PieceType pieceType) {
    assert(IsValid(pieceType));
    return static_cast<int8_t>(pieceType);
}

constexpr Piece ToPiece(int8_t n) {
    Piece piece = static_cast<Piece>(n);
    assert(IsValid(piece));
    return piece;
}

constexpr int8_t ToInt(Piece piece) {
    assert(IsValid(piece));
    return static_cast<int8_t>(piece);
}

constexpr Piece MakePiece(Color color, PieceType type) {
    assert(IsValid(color));
    assert(IsValid(type));
    return static_cast<Piece>(
        (static_cast<int8_t>(type) << 1) | static_cast<int8_t>(color)
    );
}

constexpr Color ColorOf(Piece piece) {
    assert(IsValid(piece));
    return static_cast<Color>(
        static_cast<int8_t>(piece) & 1
    );
}

constexpr PieceType PieceTypeOf(Piece piece) {
    assert(IsValid(piece));
    return static_cast<PieceType>(
        static_cast<int8_t>(piece) >> 1
    );
}


#define DEFINE_ADD_SUB_OPERATORS(T) \
    constexpr T& operator++(T& t)             { t = T(ToInt(t) + 1); return t; } \
    constexpr T& operator--(T& t)             { t = T(ToInt(t) - 1); return t; } \
    constexpr T  operator+(T t, int8_t s)     { return T(ToInt(t) + s); } \
    constexpr T& operator+=(T& t, int8_t s)   { t = T(ToInt(t) + s); return t; } \
    constexpr T  operator-(T t, int8_t s)     { return T(ToInt(t) - s); } \
    constexpr T& operator-=(T& t, int8_t s)   { t = T(ToInt(t) - s); return t; }


DEFINE_ADD_SUB_OPERATORS(BoardFile);
DEFINE_ADD_SUB_OPERATORS(BoardRank);
DEFINE_ADD_SUB_OPERATORS(Square);
DEFINE_ADD_SUB_OPERATORS(Color);
DEFINE_ADD_SUB_OPERATORS(PieceType);
DEFINE_ADD_SUB_OPERATORS(Piece);

#undef DEFINE_ADD_SUB_OPERATORS
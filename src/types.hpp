#pragma once

#include <cstdint>
#include <cassert>
#include <array>
#include <limits>


// Array classes

template <typename T, std::size_t num>
using Array = std::array<T, num>;

template <typename T, std::size_t num1, std::size_t num2>
using Array2D = std::array<std::array<T, num2>, num1>;



/////////////////////////////////////////////////////////
/////////////////////   DIRECTION   /////////////////////
/////////////////////////////////////////////////////////

constexpr uint8_t DIRECTION_NUM = 8;
enum class Direction : int8_t {
    Up = 8,
    UpRight = 9,
    Right = 1,
    DownRight = -7,
    Down = -8,
    DownLeft = -9,
    Left = -1,
    UpLeft = 7
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

constexpr Direction operator+(Direction dir1, Direction dir2) {
    return ToDirection(ToInt(dir1) + ToInt(dir2));
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
    A1 = 0,  B1 = 1,  C1 = 2,  D1 = 3,  E1 = 4,  F1 = 5,  G1 = 6,  H1 = 7,
    A2 = 8,  B2 = 9,  C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15,
    A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23,
    A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31,
    A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39,
    A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47,
    A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55,
    A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63,
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
    return static_cast<BoardFile>(n);
}

constexpr int8_t ToInt(BoardFile file) {
    return static_cast<int8_t>(file);
}

constexpr BoardRank ToBoardRank(int8_t n) {
    return static_cast<BoardRank>(n);
}

constexpr int8_t ToInt(BoardRank rank) {
    return static_cast<int8_t>(rank);
}

constexpr Square ToSquare(int8_t n) {
    return static_cast<Square>(n);
}

constexpr int8_t ToInt(Square square) {
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

constexpr Square MiddleOf(Square sq1, Square sq2) {
    assert(IsValid(sq1));
    assert(IsValid(sq2));
    return ToSquare((ToInt(sq1) + ToInt(sq2)) >> 1);
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

constexpr Color operator~(Color color) {
    return ToColor(1 - ToInt(color));
}



//////////////////////////////////////////////////////////
/////////////////////     SCORE     //////////////////////
//////////////////////////////////////////////////////////
using Score = int16_t;
constexpr Score SCORE_MIN = std::numeric_limits<Score>::min();
constexpr Score SCORE_MAX = std::numeric_limits<Score>::max();



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
#pragma once

#include "types.hpp"
#include "bitboard.hpp"
#include "castling_rights.hpp"

#include <string>

class Position {
public:
    Position() { InitFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); }
    explicit Position(const char* fen) { InitFromFEN(fen); }
    explicit Position(const std::string& fen) { InitFromFEN(fen.c_str()); }
    
    std::string GetFEN() const;

    Bitboard GetPiecesBB(Piece piece) const { return mPiecesBB[ToInt(ColorOf(piece))][ToInt(PieceTypeOf(piece))]; }
    Piece GetBoard(Square square) const { return mBoard[ToInt(square)]; }

    Bitboard GetOccupancy(Color color) const { return mOccupied[ToInt(color)]; }
    Bitboard GetOccupancy() const { return mOccupied[ToInt(Color::White)] | mOccupied[ToInt(Color::Black)]; }

    Bitboard GetKingAttackers() const { return mKingAttackers; }

    Color GetSideToMove() const { return mSideToMove; }
    

private:
    Bitboard mPiecesBB[COLOR_NUM][PIECE_TYPE_NUM]   = { BB::NONE };
    Piece mBoard[SQUARE_NUM]                        = { Piece::None };
    Color mSideToMove                               = Color::White;
    CastlingRights mCastlingRights                  = CastlingRights::ALL;
    Square mEnPassant                               = Square::None;
    uint32_t mReversableHalfMovesCnt                = 0;
    uint32_t mMoveNum                               = 1;

    Bitboard mOccupied[COLOR_NUM]                   = { BB::NONE };
    Bitboard mKingAttackers                         = BB::NONE;

    void InitFromFEN(const char* fen);
    const char* InitFromFEN_PiecePosition(const char* fen);
    const char* InitFromFEN_SideToMove(const char* fen);
    const char* InitFromFEN_CastlingRights(const char* fen);
    const char* InitFromFEN_ReversableHalfMoves(const char* fen);
    const char* InitFromFEN_MoveNum(const char* fen);
    const char* InitFromFEN_ExpectSpace(const char* fen);
    
    void AddPiece(Piece piece, Square square);

    Bitboard& PiecesBB(Piece piece) { return mPiecesBB[ToInt(ColorOf(piece))][ToInt(PieceTypeOf(piece))]; }
    Piece& Board(Square square) { return mBoard[ToInt(square)]; }
    Bitboard& Occupied(Color color) { return mOccupied[ToInt(color)]; }
    
};
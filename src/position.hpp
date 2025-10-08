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
    

private:
    Bitboard mPiecesBB[COLOR_NUM][PIECE_TYPE_NUM]   = { (Bitboard)0 };
    Piece mBoard[SQUARE_NUM]                        = { Piece::None };
    Color mSideToMove                               = Color::White;
    CastlingRights mCastlingRights                  = CastlingRights::ALL;
    Square mEnPassant                               = Square::None;
    uint32_t mReversableHalfMovesCnt                = 0;
    uint32_t mMoveNum                               = 1;

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
};
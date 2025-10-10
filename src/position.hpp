#pragma once

#include "types.hpp"
#include "bitboard.hpp"
#include "castling_rights.hpp"
#include "move.hpp"

#include <string>

class Position {
public:
    Position() { InitFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); }
    explicit Position(const char* fen) { InitFromFEN(fen); }
    explicit Position(const std::string& fen) { InitFromFEN(fen.c_str()); }

    void DoMove(Move move);

    Bitboard GetPiecesBB(Color color, PieceType type) const { return mPiecesBB[ToInt(color)][ToInt(type)]; }
    Bitboard GetPiecesBB(Piece piece) const                 { return GetPiecesBB(ColorOf(piece), PieceTypeOf(piece)); }
    Piece GetBoard(Square square) const                     { return mBoard[ToInt(square)]; }
    Color GetSideToMove() const                             { return mSideToMove; }
    CastlingRights GetCastlingRights() const                { return mCastlingRights; }
    Square GetEnPassant() const                             { return mEnPassant; }
    uint32_t GetReversableHalfMovesCnt() const              { return mReversableHalfMovesCnt; }
    uint32_t GetMoveNum() const                             { return mMoveNum; }
    
    Square GetKingPosition(Color color) const   { return BB::Lsb(GetPiecesBB(MakePiece(color, PieceType::King))); }

    Bitboard GetOccupancy(Color color) const    { return mOccupied[ToInt(color)]; }
    Bitboard GetOccupancy() const               { return mOccupied[ToInt(Color::White)] | mOccupied[ToInt(Color::Black)]; }
    Bitboard GetAttacks(Color color) const      { return mAttacks[ToInt(color)]; }
    Bitboard GetPinned(Color color) const       { return mPinned[ToInt(color)]; }
    Bitboard GetKingAttackers() const           { return mKingAttackers; }

    std::string GetFEN() const;

private:
    Bitboard mPiecesBB[COLOR_NUM][PIECE_TYPE_NUM]   = { BB::NONE };
    Piece mBoard[SQUARE_NUM]                        = { Piece::None };
    Color mSideToMove                               = Color::White;
    CastlingRights mCastlingRights                  = CastlingRights::ALL;
    Square mEnPassant                               = Square::None;
    uint32_t mReversableHalfMovesCnt                = 0;
    uint32_t mMoveNum                               = 1;

    Bitboard mOccupied[COLOR_NUM]                   = { BB::NONE };
    Bitboard mAttacks[COLOR_NUM]                    = { BB::NONE };
    Bitboard mPinned[COLOR_NUM]                     = { BB::NONE };
    Bitboard mKingAttackers                         = BB::NONE;

    Bitboard& PiecesBB(Color color, PieceType type) { return mPiecesBB[ToInt(color)][ToInt(type)]; }
    Bitboard& PiecesBB(Piece piece)                 { return PiecesBB(ColorOf(piece), PieceTypeOf(piece)); }
    Piece& Board(Square square)                     { return mBoard[ToInt(square)]; }
    Bitboard& Occupied(Color color)                 { return mOccupied[ToInt(color)]; }
    Bitboard& Attacks(Color color)                  { return mAttacks[ToInt(color)]; }
    Bitboard& Pinned(Color color)                   { return mPinned[ToInt(color)]; }

    void InitFromFEN(const char* fen);
    const char* InitFromFEN_PiecePosition(const char* fen);
    const char* InitFromFEN_SideToMove(const char* fen);
    const char* InitFromFEN_CastlingRights(const char* fen);
    const char* InitFromFEN_EnPassant(const char* fen);
    const char* InitFromFEN_ReversableHalfMoves(const char* fen);
    const char* InitFromFEN_MoveNum(const char* fen);
    const char* InitFromFEN_ExpectSpace(const char* fen);
    
    void AddPiece(Piece piece, Square square);
    void RemovePiece(Square square);
    void MovePiece(Square from, Square to);
    void CapturePiece(Square from, Square to);

    template <Color color>
    void UpdateAttacks();
    void UpdateAttacks();
    template <Color color>
    void UpdatePins();
    void UpdatePins();
    void UpdateKingAttacks();
    void UpdateAuxiliaryInfo();

};
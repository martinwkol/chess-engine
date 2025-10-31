#pragma once

#include "types.hpp"
#include "bitboard.hpp"
#include "castling_rights.hpp"
#include "move.hpp"
#include "zobrist_hash.hpp"

#include <string>

class Position {
public:
    Position() { InitFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); }
    explicit Position(const char* fen) { InitFromFEN(fen); }
    explicit Position(const std::string& fen) { InitFromFEN(fen.c_str()); }

    void DoMove(Move move);
    void UndoMove();

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
    Bitboard GetCheckSquares() const            { return mCheckSquares; }

    ZobristHash GetZobristHash() const          { return mZobristHash; }

    bool IsCheck() const                        { return mKingAttackers != BB::NONE; }
    bool IsDoubleCheck() const                  { return BB::AtLeast2(mKingAttackers); }

    std::string GetFEN() const;

private:
    static constexpr int MAX_HALF_MOVES = 4096;

    struct RestoreInfo {
        Move move;
        Piece capturedPiece;
        Square enPassant;
        CastlingRights castlingRights;
        uint32_t reversableHalfMovesCnt;
        Array<Bitboard, COLOR_NUM> attacks;
        Array<Bitboard, COLOR_NUM> pinned;
        Bitboard kingAttackers;
        Bitboard checkSquares;
        ZobristHash zobristHash;
    };

    Array2D<Bitboard, COLOR_NUM, PIECE_TYPE_NUM> mPiecesBB;
    Array<Piece, SQUARE_NUM> mBoard;
    Color mSideToMove                               = Color::White;
    CastlingRights mCastlingRights                  = CastlingRights::ALL;
    Square mEnPassant                               = Square::None;
    uint32_t mReversableHalfMovesCnt                = 0;
    uint32_t mMoveNum                               = 1;

    Array<Bitboard, COLOR_NUM> mOccupied;
    Array<Bitboard, COLOR_NUM> mAttacks;
    Array<Bitboard, COLOR_NUM> mPinned;
    Bitboard mKingAttackers                         = BB::NONE;
    Bitboard mCheckSquares                          = BB::NONE;

    ZobristHash mZobristHash;

    Array<RestoreInfo, MAX_HALF_MOVES> mHistory;
    uint32_t mHistoryNext                           = 0;

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

    void NullifyEnPassant();
    void SetEnPassant(Square square);
    void SwitchSideToMove();

    template <Color color>
    void UpdateCastlingRights(Square from, Square to);

    template <Color color>
    void UpdateAttacks();
    template <Color color>
    void UpdatePins();
    void UpdateKingAttackers();
    void UpdateAuxiliaryInfo();

    bool ZobristHashCorrect() const;

};
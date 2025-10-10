#include "move_generation.hpp"

template <Color This>
static Move* GenerateNormalKingMoves(Move* list, const Position& pos, Bitboard allowedTargets) {
    constexpr Color Other = This == Color::White ? Color::Black : Color::White;

    Square from = BB::Lsb(pos.GetPiecesBB(MakePiece(This, PieceType::King)));
    Bitboard movesBB = BB::Attacks<PieceType::King>(from);
    Bitboard attacked = pos.GetAttacks(Other);
    movesBB &= ~attacked; // King may not move into check
    movesBB &= allowedTargets;
    while (movesBB) {
        Square to = BB::PopLsb(movesBB);
        if (pos.GetBoard(to) == Piece::None) {
            *list++ = Move::NewQuiet(from, to);
        } else {
            *list++ = Move::NewCapture(from, to);
        }
    }
    return list;
}

template <Color This>
static Move* GenerateCastlingMoves(Move* list, const Position& pos, Bitboard allowedTargets) {
    constexpr Color Other = This == Color::White ? Color::Black : Color::White;

    constexpr BoardRank CastlingRank = This == Color::White ? BoardRank::R1 : BoardRank::R8;
    constexpr Bitboard KingsideGapBB = 
        BB::SquareBB(MakeSquare(BoardFile::F, CastlingRank)) |
        BB::SquareBB(MakeSquare(BoardFile::G, CastlingRank));
    constexpr Bitboard QueensideGapBB = 
        BB::SquareBB(MakeSquare(BoardFile::B, CastlingRank)) |
        BB::SquareBB(MakeSquare(BoardFile::C, CastlingRank)) |
        BB::SquareBB(MakeSquare(BoardFile::D, CastlingRank));
    constexpr Bitboard KingsideTravelBB = 
        BB::SquareBB(MakeSquare(BoardFile::F, CastlingRank)) |
        BB::SquareBB(MakeSquare(BoardFile::G, CastlingRank));
    constexpr Bitboard QueensideTravelBB = 
        BB::SquareBB(MakeSquare(BoardFile::C, CastlingRank)) |
        BB::SquareBB(MakeSquare(BoardFile::D, CastlingRank));

    CastlingRights castlingRights = pos.GetCastlingRights();
    Bitboard occupancy = pos.GetOccupancy();
    Bitboard attacked = pos.GetAttacks(Other);
    Square kingSquare = pos.GetKingPosition(This);
    if (castlingRights.CanCastleKingside<This>() && (KingsideGapBB & occupancy) == 0 && (KingsideTravelBB & attacked) == 0) {
        *list++ = Move::NewKingsideCastle(kingSquare, kingSquare + Direction::RIGHT + Direction::RIGHT);
    }
    if (castlingRights.CanCastleQueenside<This>() && (QueensideGapBB & occupancy) == 0 && (QueensideTravelBB & attacked) == 0) {
        *list++ = Move::NewQueensideCastle(kingSquare, kingSquare + Direction::LEFT + Direction::LEFT);
    }
    return list;
}

template <Color This, PieceType PType>
static Move* GenerateBigPieceMoves(Move* list, const Position& pos, Bitboard allowedTargets) {
    static_assert(PType != PieceType::King && PType != PieceType::Pawn);
    constexpr Color Other = This == Color::White ? Color::Black : Color::White;

    Bitboard piecesBB = pos.GetPiecesBB(MakePiece(This, PType));
    while (piecesBB) {
        Square from = BB::PopLsb(piecesBB);
        Bitboard movesBB = BB::Attacks<PType>(from, pos.GetOccupancy());
        movesBB &= allowedTargets;
        if (pos.GetPinned(This) & BB::SquareBB(from)) {
            movesBB &= BB::Line(pos.GetKingPosition(This), from);
        }
        while (movesBB) {
            Square to = BB::PopLsb(movesBB);
            if (pos.GetBoard(to) == Piece::None) {
                *list++ = Move::NewQuiet(from, to);
            } else {
                *list++ = Move::NewCapture(from, to);
            }
        }
    }
    return list;
}

static Move* GenerateMoves(Move* list, const Position& pos) {
    constexpr Color This = Color::White;

    Bitboard ThisOccupied = pos.GetOccupancy(This);
    Bitboard allowedTargets = ~ThisOccupied;
    Bitboard kingAttackers = pos.GetKingAttackers();

    list = GenerateNormalKingMoves<This>(list, pos, allowedTargets);

    if (BB::AtLeast2(kingAttackers)) {
        // Double check -> only king moves
        return list;
    }

    if (kingAttackers) {
        // King in check -> block or capture attacker
        Square kingSquare = BB::Lsb(pos.GetPiecesBB(MakePiece(This, PieceType::King)));
        Square attackerSquare = BB::Lsb(kingAttackers);
        allowedTargets &= BB::Between(kingSquare, attackerSquare) | kingAttackers;
    } else {
        list = GenerateCastlingMoves<This>(list, pos, allowedTargets);
    }

    list = GenerateBigPieceMoves<This, PieceType::Queen>(list, pos, allowedTargets);
    list = GenerateBigPieceMoves<This, PieceType::Rook>(list, pos, allowedTargets);
    list = GenerateBigPieceMoves<This, PieceType::Bishop>(list, pos, allowedTargets);
    list = GenerateBigPieceMoves<This, PieceType::Knight>(list, pos, allowedTargets);

    
}

void GenerateMoves(const Position& pos, MoveList& moveList) {
    /*
    if (pos.GetSideToMove() == Color::White) {
        moveList.SetEnd(GenerateMoves<Color::White>(moveList.begin(), pos));
    } else {
        moveList.SetEnd(GenerateMoves<Color::Black>(moveList.begin(), pos));
    }
    */
}
#include "move_generation.hpp"

template <Color This>
static Move* GenerateNormalKingMoves(Move* list, const Position& pos, Bitboard allowedTargets) {
    constexpr Color Other = This == Color::White ? Color::Black : Color::White;

    Square from = pos.GetKingPosition(This);
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
static Move* GenerateCastlingMoves(Move* list, const Position& pos) {
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

template <Direction Dir, Move (&NewMove)(Square, Square)>
static Move* AddNormalPawnMoves(Move* list, Bitboard bb) {
    while (bb) {
        Square to = BB::PopLsb(bb);
        Square from = to - Dir;
        *list++ = NewMove(from, to);
    }
    return list;
}

template <Direction Dir, Move (&NewMove)(Square, Square, PieceType)>
static Move* AddPromotions(Move* list, Bitboard bb) {
    while (bb) {
        Square to = BB::PopLsb(bb);
        Square from = to - Dir;
        *list++ = NewMove(from, to, PieceType::Knight);
        *list++ = NewMove(from, to, PieceType::Bishop);
        *list++ = NewMove(from, to, PieceType::Rook);
        *list++ = NewMove(from, to, PieceType::Queen);
    }
    return list;
}

template <Color This>
static Move* tryEnPassant(Move* list, const Position& pos, Square from, Square to, Square captured) {
    constexpr Color Other = This == Color::White ? Color::Black : Color::White;

    Bitboard occupancyAfterEnPassant = pos.GetOccupancy() ^ (BB::SquareBB(from) | BB::SquareBB(to) | BB::SquareBB(captured));
    Bitboard rooksOther = pos.GetPiecesBB(MakePiece(Other, PieceType::Rook));
    Bitboard queensOther = pos.GetPiecesBB(MakePiece(Other, PieceType::Queen));
    Bitboard kingRookAttack = BB::Attacks<PieceType::Rook>(pos.GetKingPosition(This), occupancyAfterEnPassant);
    if (!(kingRookAttack & (rooksOther | queensOther))) {
        *list++ = Move::NewEnPassant(from, to);
    }
    return list;
}

template <Color This>
static Move* GeneratePawnMoves(Move* list, const Position& pos, Bitboard allowedTargets) {
    constexpr Color Other = This == Color::White ? Color::Black : Color::White;

    constexpr Direction Forward             = This == Color::White ? Direction::UP : Direction::DOWN;
    constexpr BoardRank Rank3               = This == Color::White ? BoardRank::R3 : BoardRank::R6;
    constexpr BoardRank PrePromotionRank    = This == Color::White ? BoardRank::R7 : BoardRank::R2;
    constexpr BoardRank PromotionRank       = This == Color::White ? BoardRank::R8 : BoardRank::R1;
    constexpr Bitboard Rank3BB              = BB::RankBB(Rank3);
    constexpr Bitboard PrePromotionRankBB   = BB::RankBB(PrePromotionRank);
    constexpr Bitboard PromotionRankBB      = BB::RankBB(PromotionRank);
    
    Bitboard pawns = pos.GetPiecesBB(MakePiece(This, PieceType::Pawn));
    if (!pawns) return list;

    Move* listStart = list;

    Bitboard free           = ~pos.GetOccupancy() & allowedTargets;
    Bitboard occupancyOther = pos.GetOccupancy(Other) & allowedTargets;
    
    Bitboard forwardSingle  = BB::Shift<Forward>(pawns) & free;
    Bitboard forwardDouble  = BB::Shift<Forward>(forwardSingle & Rank3BB) & free;
    Bitboard captureLeft    = BB::Shift<Forward + Direction::LEFT>(pawns) & occupancyOther;
    Bitboard captureRight   = BB::Shift<Forward + Direction::RIGHT>(pawns) & occupancyOther;
    
    if (pawns & PrePromotionRankBB) {
        Bitboard forwardPromotion       = forwardSingle & PromotionRankBB;
        Bitboard captureLeftPromotion   = captureLeft & PromotionRankBB;
        Bitboard captureRightPromotion  = captureRight & PromotionRankBB;
        list = AddPromotions<Forward, Move::NewPromotionNormal>(list, forwardPromotion);
        list = AddPromotions<Forward + Direction::LEFT, Move::NewPromotionCapture>(list, captureLeftPromotion);
        list = AddPromotions<Forward + Direction::RIGHT, Move::NewPromotionCapture>(list, captureRightPromotion);
        
        forwardSingle   &= ~PromotionRankBB;
        captureLeft     &= ~PromotionRankBB;
        captureRight    &= ~PromotionRankBB;
    }

    list = AddNormalPawnMoves<Forward, Move::NewQuiet>(list, forwardSingle);
    list = AddNormalPawnMoves<Forward + Forward, Move::NewDoublePawnPush>(list, forwardDouble);
    list = AddNormalPawnMoves<Forward + Direction::LEFT, Move::NewCapture>(list, captureLeft);
    list = AddNormalPawnMoves<Forward + Direction::RIGHT, Move::NewCapture>(list, captureRight);

    Square enPassant = pos.GetEnPassant();
    if (enPassant != Square::None && (BB::SquareBB(enPassant) & allowedTargets)) {
        Square captured = enPassant - Forward;
        Square leftFrom = enPassant - (Forward + Direction::LEFT);
        if (pos.GetBoard(leftFrom) == MakePiece(This, PieceType::Pawn)) {
            list = tryEnPassant<This>(list, pos, leftFrom, enPassant, captured);
        }
        Square rightFrom = enPassant - (Forward + Direction::RIGHT);
        if (pos.GetBoard(rightFrom) == MakePiece(This, PieceType::Pawn)) {
            list = tryEnPassant<This>(list, pos, rightFrom, enPassant, captured);
        }
    }

    // Remove pawn moves that ignore a pin
    Bitboard pinnedPawns = pos.GetPinned(This) & pawns;
    if (pinnedPawns) {
        Bitboard kingBB = pos.GetPiecesBB(MakePiece(This, PieceType::King));
        for (Move* pawnMoves = listStart; pawnMoves < list; ++pawnMoves) {
            Square from = pawnMoves->GetFrom();
            if ((BB::SquareBB(from) & pinnedPawns) && !(BB::Line(from, pawnMoves->GetTo()) & kingBB)) 
                *pawnMoves = *(--list);
        }
    }
    
    return list;
}

template <Color This>
static Move* GenerateMoves(Move* list, const Position& pos) {
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
        Square kingSquare = pos.GetKingPosition(This);
        Square attackerSquare = BB::Lsb(kingAttackers);
        allowedTargets &= BB::Between(kingSquare, attackerSquare) | kingAttackers;
    } else {
        list = GenerateCastlingMoves<This>(list, pos);
    }

    list = GenerateBigPieceMoves<This, PieceType::Queen>(list, pos, allowedTargets);
    list = GenerateBigPieceMoves<This, PieceType::Rook>(list, pos, allowedTargets);
    list = GenerateBigPieceMoves<This, PieceType::Bishop>(list, pos, allowedTargets);
    list = GenerateBigPieceMoves<This, PieceType::Knight>(list, pos, allowedTargets);
    list = GeneratePawnMoves<This>(list, pos, allowedTargets);
    return list;
}

void GenerateMoves(const Position& pos, MoveList& moveList) {
    if (pos.GetSideToMove() == Color::White) {
        moveList.SetEnd(GenerateMoves<Color::White>(moveList.begin(), pos));
    } else {
        moveList.SetEnd(GenerateMoves<Color::Black>(moveList.begin(), pos));
    }
}
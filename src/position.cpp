#include "position.hpp"

#include <stdexcept>
#include <cctype>
#include <string>
#include <sstream>

void Position::DoMove(Move move) {
    Square from = move.GetFrom();
    Square to = move.GetTo();

    RestoreInfo& restoreInfo            = mHistory[mHistoryNext++];
    restoreInfo.move                    = move;
    restoreInfo.capturedPiece           = move.IsCapture() ? GetBoard(to) : Piece::None;
    restoreInfo.enPassant               = mEnPassant;
    restoreInfo.castlingRights          = mCastlingRights;
    restoreInfo.reversableHalfMovesCnt  = mReversableHalfMovesCnt;
    restoreInfo.attacks                 = mAttacks;
    restoreInfo.pinned                  = mPinned;
    restoreInfo.kingAttackers           = mKingAttackers;
    restoreInfo.checkSquares            = mCheckSquares;
    restoreInfo.zobristHash             = mZobristHash;

    if (mEnPassant != Square::None) NullifyEnPassant();

    if (move.IsQuiet()) {
        MovePiece(from, to);
        if (move.IsDoublePawnPush()) {
            SetEnPassant(MiddleOf(from, to));
        }
    }
    else if (move.IsNormalCapture()) {
        CapturePiece(from, to);
    }
    else if (move.IsEnPassant()) {
        Square captured = MakeSquare(FileOf(to), RankOf(from));
        RemovePiece(captured);
        MovePiece(from, to);
    }
    else if (move.IsCastle()) {
        BoardFile rookFile = move.IsQueensideCastle() ? BoardFile::A : BoardFile::H;
        Square rook = MakeSquare(rookFile, RankOf(from));
        MovePiece(from, to);
        MovePiece(rook, MiddleOf(from, to));
    }
    else {
        assert(move.IsPromotion());
        if (move.IsCapture()) RemovePiece(to);
        RemovePiece(from);
        AddPiece(MakePiece(GetSideToMove(), move.GetPromotionType()), to);
    }

    if (mSideToMove == Color::White)    UpdateCastlingRights<Color::White>(from, to);
    else                                UpdateCastlingRights<Color::Black>(from, to);

    if (move.IsQuiet()) {
        ++mReversableHalfMovesCnt;
    }
    else {
        mReversableHalfMovesCnt = 0;
    }
    if (GetSideToMove() == Color::Black) ++mMoveNum;

    SwitchSideToMove();
    UpdateAuxiliaryInfo();

    assert(ZobristHashCorrect());
}

void Position::UndoMove() {
    RestoreInfo& restoreInfo = mHistory[--mHistoryNext];

    Square from = restoreInfo.move.GetFrom();
    Square to = restoreInfo.move.GetTo();
    if (restoreInfo.move.IsQuiet() || restoreInfo.move.IsNormalCapture()) {
        MovePiece(to, from);
    }
    else if (restoreInfo.move.IsEnPassant()) {
        Square captured = MakeSquare(FileOf(to), RankOf(from));
        MovePiece(to, from);
        AddPiece(MakePiece(mSideToMove, PieceType::Pawn), captured);
    }
    else if (restoreInfo.move.IsCastle()) {
        BoardFile rookFile = restoreInfo.move.IsQueensideCastle() ? BoardFile::A : BoardFile::H;
        Square rook = MakeSquare(rookFile, RankOf(from));
        MovePiece(to, from);
        MovePiece(MiddleOf(from, to), rook);
    }
    else {
        assert(restoreInfo.move.IsPromotion());
        RemovePiece(to);
        AddPiece(MakePiece(~mSideToMove, PieceType::Pawn), from);
    }

    if (restoreInfo.capturedPiece != Piece::None) AddPiece(restoreInfo.capturedPiece, to);
    if (mSideToMove == Color::White) mMoveNum--;
    SwitchSideToMove();

    mEnPassant              = restoreInfo.enPassant;
    mCastlingRights         = restoreInfo.castlingRights;
    mReversableHalfMovesCnt = restoreInfo.reversableHalfMovesCnt;
    mAttacks                = restoreInfo.attacks;
    mPinned                 = restoreInfo.pinned;
    mKingAttackers          = restoreInfo.kingAttackers;
    mCheckSquares           = restoreInfo.checkSquares;
    mZobristHash            = restoreInfo.zobristHash;

    assert(ZobristHashCorrect());
}

std::string Position::GetFEN() const {
    std::stringstream s;
    for (BoardRank rank = BoardRank::R8; rank >= BoardRank::R1; --rank) {
        int emptyCounter = 0;
        for (BoardFile file = BoardFile::A; file <= BoardFile::H; ++file) {
            Piece piece = GetBoard(MakeSquare(file, rank));
            if (piece != Piece::None) {
                char pieceChar;
                switch (PieceTypeOf(piece)) {
                case PieceType::King :      pieceChar = 'K'; break;
                case PieceType::Queen :     pieceChar = 'Q'; break;
                case PieceType::Rook :      pieceChar = 'R'; break;
                case PieceType::Knight :    pieceChar = 'N'; break;
                case PieceType::Bishop :    pieceChar = 'B'; break;
                case PieceType::Pawn :      pieceChar = 'P'; break;
                default:                    pieceChar = 'E'; break;
                }
                if (ColorOf(piece) == Color::Black) pieceChar = std::tolower(pieceChar);
                if (emptyCounter > 0) s << emptyCounter;
                emptyCounter = 0;
                s << pieceChar;
            } 
            else {
                ++emptyCounter;
            }
        }
        if (emptyCounter > 0) s << emptyCounter;
        if (rank != BoardRank::R1) s << '/';
    }

    s << ' ' << (mSideToMove == Color::White ? 'w' : 'b') << ' ';

    if (mCastlingRights.AnyCastlingAllowed()) {
        s   << (mCastlingRights.CanCastleKingside<Color::White>() ? "K" : "")
            << (mCastlingRights.CanCastleQueenside<Color::White>() ? "Q" : "")
            << (mCastlingRights.CanCastleKingside<Color::Black>() ? "k" : "")
            << (mCastlingRights.CanCastleQueenside<Color::Black>() ? "q" : "");
    } 
    else {
        s << '-';
    }

    if (mEnPassant != Square::None) {
        s << ' ' << ('a' + ToInt(FileOf(mEnPassant))) << ('1' + ToInt(RankOf(mEnPassant)));
    } else {
        s << ' ' << '-';
    }

    s << ' ' << mReversableHalfMovesCnt << ' ' << mMoveNum;

    return s.str();
}

void Position::InitFromFEN(const char *fen) {
    mPiecesBB[ToInt(Color::White)].fill(BB::NONE);
    mPiecesBB[ToInt(Color::Black)].fill(BB::NONE);
    mBoard.fill(Piece::None);
    mOccupied.fill(BB::NONE);
    mAttacks.fill(BB::NONE);
    mPinned.fill(BB::NONE);
    

    fen = InitFromFEN_PiecePosition(fen);
    fen = InitFromFEN_ExpectSpace(fen);

    fen = InitFromFEN_SideToMove(fen);
    fen = InitFromFEN_ExpectSpace(fen);

    fen = InitFromFEN_CastlingRights(fen);
    fen = InitFromFEN_ExpectSpace(fen);

    fen = InitFromFEN_EnPassant(fen);
    fen = InitFromFEN_ExpectSpace(fen);

    fen = InitFromFEN_ReversableHalfMoves(fen);
    fen = InitFromFEN_ExpectSpace(fen);

    fen = InitFromFEN_MoveNum(fen);

    UpdateAuxiliaryInfo();

    assert(ZobristHashCorrect());
}

const char* Position::InitFromFEN_PiecePosition(const char* fen) {
    BoardRank rank = BoardRank::R8;
    BoardFile file = BoardFile::A;
    for (; *fen && *fen != ' '; ++fen) {
        if (*fen == '/') {
            if (file <= BoardFile::H) 
                throw std::invalid_argument("Illegal fen: '/' before end of rank");
            if (rank == BoardRank::R1)
                throw std::invalid_argument("Illegal fen: '/' after last rank");
            --rank; file = BoardFile::A;
        } 
        else if (file > BoardFile::H) {
            throw std::invalid_argument("Illegal fen: rank already full, '/' expected");
        }
        else if ('1' <= *fen && *fen <= '8') {
            file += (*fen - '0');
            if (file > BoardFile::H + 1)
                throw std::invalid_argument("Illegal fen: skipped past end of rank");
        }
        else { // piece
            Color color = std::isupper(*fen) ? Color::White : Color::Black;
            PieceType type = PieceType::None;
            switch (std::toupper(*fen)) {
                case 'K': type = PieceType::King; break;
                case 'Q': type = PieceType::Queen; break;
                case 'R': type = PieceType::Rook; break;
                case 'N': type = PieceType::Knight; break;
                case 'B': type = PieceType::Bishop; break;
                case 'P': type = PieceType::Pawn; break;
                default: throw std::invalid_argument("Illegal fen: illegal piece type");
            }
            AddPiece(MakePiece(color, type), MakeSquare(file, rank));
            ++file;
        }
    }
    return fen;
}

const char* Position::InitFromFEN_SideToMove(const char* fen) {
    if (!(*fen == 'w' || *fen == 'b')) throw std::invalid_argument("Illegal fen: expected 'w' or 'b' as active color");
    mSideToMove = Color::White;
    if (*fen == 'b') {
        SwitchSideToMove();
    }
    return fen + 1;
}

const char* Position::InitFromFEN_CastlingRights(const char* fen) {
    mCastlingRights = CastlingRights::NONE;
    if (*fen == '-') return fen + 1;
    if (*fen == 'K') {
        mCastlingRights.AllowCastlingKingside<Color::White>();
        ++fen;
    }
    if (*fen == 'Q') {
        mCastlingRights.AllowCastlingQueenside<Color::White>();
        ++fen;
    }
    if (*fen == 'k') {
        mCastlingRights.AllowCastlingKingside<Color::Black>();
        ++fen;
    }
    if (*fen == 'q') {
        mCastlingRights.AllowCastlingQueenside<Color::Black>();
        ++fen;
    }
    if (mCastlingRights == CastlingRights::NONE) throw std::invalid_argument("Illegal fen: invalid castling rights");
    mZobristHash.SwitchCastlingRights(mCastlingRights);
    return fen;
}

const char *Position::InitFromFEN_EnPassant(const char *fen) {
    if (*fen == '-') {
        mEnPassant = Square::None;
        return fen + 1;
    }

    if (*fen < 'a' || 'h' < *fen) throw std::invalid_argument("Illegal fen: invalid en passant");
    BoardFile file = ToBoardFile(*fen - 'a');
    ++fen;

    if (*fen < '1' || '8' < *fen) throw std::invalid_argument("Illegal fen: invalid en passant");
    BoardRank rank = ToBoardRank(*fen - '1');
    mEnPassant = MakeSquare(file, rank);
    mZobristHash.SwitchEnPassantFile(file);
    return fen + 1;
}

const char* Position::InitFromFEN_ReversableHalfMoves(const char* fen) {
    mReversableHalfMovesCnt = std::stoul(fen);
    while (*fen && *fen != ' ') ++fen;
    return fen;
}

const char* Position::InitFromFEN_MoveNum(const char* fen) {
    mMoveNum = std::stoul(fen);
    while (*fen && *fen != ' ') ++fen;
    return fen;
}

const char* Position::InitFromFEN_ExpectSpace(const char* fen) {
    if (*fen != ' ') throw std::invalid_argument("Illegal fen: fen ends before castling rights color");
    return fen + 1;
}

void Position::AddPiece(Piece piece, Square square) {
    assert(Board(square) == Piece::None);

    Bitboard squareBB            = BB::SquareBB(square);
    Board(square)                = piece;
    PiecesBB(piece)             |= squareBB;
    Occupied(ColorOf(piece))    |= squareBB;

    mZobristHash.SwitchPiece(square, piece);
}

void Position::RemovePiece(Square square) {
    assert(Board(square) != Piece::None);

    Bitboard squareBB        = BB::SquareBB(square);
    Piece piece                = Board(square);
    Board(square)            = Piece::None;
    PiecesBB(piece)           ^= squareBB;
    Occupied(ColorOf(piece))  ^= squareBB;

    mZobristHash.SwitchPiece(square, piece);
}

void Position::MovePiece(Square from, Square to) {
    assert(Board(from) != Piece::None);
    assert(Board(to) == Piece::None);

    Bitboard fromBB              = BB::SquareBB(from);
    Bitboard toBB                = BB::SquareBB(to);
    Bitboard bothBB              = fromBB | toBB;
    Piece piece                  = Board(from);
    Board(from)                  = Piece::None;
    Board(to)                    = piece;
    PiecesBB(piece)             ^= bothBB;
    Occupied(ColorOf(piece))    ^= bothBB;

    mZobristHash.SwitchPiece(from, piece);
    mZobristHash.SwitchPiece(to, piece);
}

void Position::CapturePiece(Square from, Square to) {
    assert(Board(from) != Piece::None);
    assert(Board(to) != Piece::None);
    assert(PieceTypeOf(Board(to)) != PieceType::King);

    Bitboard fromBB                      = BB::SquareBB(from);
    Bitboard toBB                        = BB::SquareBB(to);
    Bitboard bothBB                      = fromBB | toBB;
    Piece movingPiece                    = Board(from);
    Piece capturedPiece                  = Board(to);
    Board(from)                          = Piece::None;
    Board(to)                            = movingPiece;
    PiecesBB(movingPiece)               ^= bothBB;
    PiecesBB(capturedPiece)             ^= toBB;
    Occupied(ColorOf(movingPiece))      ^= bothBB;
    Occupied(ColorOf(capturedPiece))    ^= toBB;

    mZobristHash.SwitchPiece(from, movingPiece);
    mZobristHash.SwitchPiece(to, movingPiece);
    mZobristHash.SwitchPiece(to, capturedPiece);
}

void Position::NullifyEnPassant() {
    assert(mEnPassant != Square::None);
    mZobristHash.SwitchEnPassantFile(FileOf(mEnPassant));
    mEnPassant = Square::None;
}

void Position::SetEnPassant(Square square) {
    assert(mEnPassant == Square::None);
    mEnPassant = square;
    mZobristHash.SwitchEnPassantFile(FileOf(square));
}

void Position::SwitchSideToMove() {
    mSideToMove = ~mSideToMove;
    mZobristHash.SwitchSideToMove();
}

template <Color color>
void Position::UpdateCastlingRights(Square from, Square to) {
    constexpr Color other           = ~color;
    constexpr BoardRank MySide      = color == Color::White ? BoardRank::R1 : BoardRank::R8;
    constexpr BoardRank TheirSide   = other == Color::White ? BoardRank::R1 : BoardRank::R8;

    constexpr Square MyQueenRook    = MakeSquare(BoardFile::A, MySide);
    constexpr Square MyKingRook     = MakeSquare(BoardFile::H, MySide);
    constexpr Square MyKing         = MakeSquare(BoardFile::E, MySide);
    constexpr Square TheirQueenRook = MakeSquare(BoardFile::A, TheirSide);
    constexpr Square TheirKingRook  = MakeSquare(BoardFile::H, TheirSide);
    constexpr Square TheirKing      = MakeSquare(BoardFile::E, TheirSide);
    
    constexpr Bitboard MySideBB     = BB::SquareBB(MyQueenRook) | BB::SquareBB(MyKingRook) | BB::SquareBB(MyKing);
    constexpr Bitboard TheirSideBB  = BB::SquareBB(TheirQueenRook) | BB::SquareBB(TheirKingRook) | BB::SquareBB(TheirKing);
    constexpr Bitboard BothBB       = MySideBB | TheirSideBB;

    Bitboard fromBB = BB::SquareBB(from);
    Bitboard toBB   = BB::SquareBB(to);

    if (BothBB & (fromBB | toBB)) {
        mZobristHash.SwitchCastlingRights(mCastlingRights);
        if (MySideBB & fromBB) {
            switch (from) {
            case MyQueenRook:       mCastlingRights.ForbidCastlingQueenside<color>(); break;
            case MyKingRook:        mCastlingRights.ForbidCastlingKingside<color>(); break;
            case MyKing:            mCastlingRights.ForbidCastling<color>(); break;
            default:                break;
            }
        }
        if (TheirSideBB & toBB) {
            switch (to) {
            case TheirQueenRook:    mCastlingRights.ForbidCastlingQueenside<other>(); break;
            case TheirKingRook:     mCastlingRights.ForbidCastlingKingside<other>(); break;
            case TheirKing:         mCastlingRights.ForbidCastling<other>(); break;
            default:                break;
            }
        }
        mZobristHash.SwitchCastlingRights(mCastlingRights);
    }
}

template <PieceType PType>
static Bitboard BigPieceAttacks(Bitboard piecesBB, Bitboard occupancy) {
    Bitboard attacks = BB::NONE;
    while (piecesBB) {
        Square square = BB::PopLsb(piecesBB);
        attacks |= BB::Attacks<PType>(square, occupancy);
    }
    return attacks;
}

template <Color color>
void Position::UpdateAttacks() {
    Bitboard occupancy = GetOccupancy();
    Bitboard attacks = BB::NONE;
    attacks |= BigPieceAttacks<PieceType::King>(GetPiecesBB(color, PieceType::King), occupancy);
    attacks |= BigPieceAttacks<PieceType::Queen>(GetPiecesBB(color, PieceType::Queen), occupancy);
    attacks |= BigPieceAttacks<PieceType::Rook>(GetPiecesBB(color, PieceType::Rook), occupancy);
    attacks |= BigPieceAttacks<PieceType::Bishop>(GetPiecesBB(color, PieceType::Bishop), occupancy);
    attacks |= BigPieceAttacks<PieceType::Knight>(GetPiecesBB(color, PieceType::Knight), occupancy);
    attacks |= BB::PawnAttacks<color>(GetPiecesBB(color, PieceType::Pawn));
    Attacks(color) = attacks;
}

template <Color color>
void Position::UpdatePins() {
    constexpr Color other = ~color;
    Bitboard queensBB   = GetPiecesBB(other, PieceType::Queen);
    Bitboard rooksBB    = GetPiecesBB(other, PieceType::Rook);
    Bitboard bishopsBB  = GetPiecesBB(other, PieceType::Bishop);
    Bitboard straightSlidersBB = queensBB | rooksBB;
    Bitboard diagonalSlidersBB = queensBB | bishopsBB;

    Square kingSquare = GetKingPosition(color);
    Bitboard occupancyOther = GetOccupancy(other);
    Bitboard straightAttack = BB::Attacks<PieceType::Rook>(kingSquare, occupancyOther);
    Bitboard diagonalAttack = BB::Attacks<PieceType::Bishop>(kingSquare, occupancyOther);
    
    Bitboard straightAttackersBB = straightAttack & straightSlidersBB;
    Bitboard diagonalAttackersBB = diagonalAttack & diagonalSlidersBB;

    Bitboard slideAttackersBB   = straightAttackersBB | diagonalAttackersBB;
    Bitboard pinnedBB           = BB::NONE;
    while (slideAttackersBB) {
        Square attackerSq       = BB::PopLsb(slideAttackersBB);
        Bitboard between        = BB::Between(kingSquare, attackerSq);
        Bitboard blockedByBB    = between & GetOccupancy(color);
        if (blockedByBB && !BB::AtLeast2(blockedByBB)) pinnedBB |= blockedByBB;
    }
    Pinned(color) = pinnedBB;
}

void Position::UpdateKingAttackers() {
    Color thisSide      = GetSideToMove();
    Color opposingSide  = ~thisSide;

    mKingAttackers  = BB::NONE;
    mCheckSquares   = Attacks(opposingSide);

    if (!(GetAttacks(opposingSide) & GetPiecesBB(thisSide, PieceType::King))) return;
    
    Bitboard opposingQueens     = GetPiecesBB(opposingSide, PieceType::Queen);
    Bitboard opposingRooks      = GetPiecesBB(opposingSide, PieceType::Rook);
    Bitboard opposingBishops    = GetPiecesBB(opposingSide, PieceType::Bishop);
    Bitboard sliders            = opposingQueens | opposingRooks | opposingBishops;
    Bitboard straightSliders    = opposingQueens | opposingRooks;
    Bitboard diagonalSliders    = opposingQueens | opposingBishops;

    Square kingSq       = GetKingPosition(thisSide);
    Bitboard occupancy  = GetOccupancy();
    mKingAttackers |= BB::Attacks<PieceType::Rook>(kingSq, occupancy) & straightSliders;
    mKingAttackers |= BB::Attacks<PieceType::Bishop>(kingSq, occupancy) & diagonalSliders;
    mKingAttackers |= BB::Attacks<PieceType::Knight>(kingSq) & GetPiecesBB(opposingSide, PieceType::Knight);
    mKingAttackers |= BB::PawnAttacks(thisSide, kingSq) & GetPiecesBB(opposingSide, PieceType::Pawn);

    Bitboard slidingAttackers = mKingAttackers & sliders;
    while (slidingAttackers) {
        Square attacker          = BB::PopLsb(slidingAttackers);
        Piece piece              = Board(attacker);
        Bitboard occupancyNoKing = GetOccupancy() ^ PiecesBB(thisSide, PieceType::King);
        mCheckSquares           |= BB::Attacks(PieceTypeOf(piece), attacker, occupancyNoKing);
    }
}

void Position::UpdateAuxiliaryInfo() {
    UpdateAttacks<Color::White>();
    UpdateAttacks<Color::Black>();
    UpdatePins<Color::White>();
    UpdatePins<Color::Black>();
    UpdateKingAttackers();
}

bool Position::ZobristHashCorrect() const {
    ZobristHash hash;
    for (Square square = Square::A1; square <= Square::H8; ++square) {
        Piece piece = GetBoard(square);
        if (piece != Piece::None) {
            hash.SwitchPiece(square, piece);
        }
    }
    if (mSideToMove == Color::Black) hash.SwitchSideToMove();
    hash.SwitchCastlingRights(mCastlingRights);
    if (mEnPassant != Square::None) hash.SwitchEnPassantFile(FileOf(mEnPassant));
    return mZobristHash == hash;
}

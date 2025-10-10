#include "position.hpp"

#include <stdexcept>
#include <cctype>
#include <string>
#include <sstream>

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
        }
    }
    return fen;
}

const char* Position::InitFromFEN_SideToMove(const char* fen) {
    if (!(*fen == 'w' || *fen == 'b')) throw std::invalid_argument("Illegal fen: expected 'w' or 'b' as active color");
    if (*fen == 'w') mSideToMove = Color::White;
    else mSideToMove = Color::Black;
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
    assert(mBoard[ToInt(square)] == Piece::None);
    Board(square) = piece;
    PiecesBB(piece) |= BB::SquareBB(square);
    Occupied(ColorOf(piece)) |= BB::SquareBB(square);
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

template <PieceType PType>
static Bitboard PawnAttacks(Bitboard piecesBB, Bitboard occupancy) {
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
    attacks |= BB::PawnAttacks(color, GetPiecesBB(color, PieceType::Pawn));
    Attacks(color) = attacks;
}

void Position::UpdateAttacks() {
    UpdateAttacks<Color::White>();
    UpdateAttacks<Color::Black>();
}


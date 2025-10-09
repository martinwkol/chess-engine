#include "bitboard.hpp"

#include <initializer_list>

namespace {
    Bitboard attacksTable[(1 << 12) * SQUARE_NUM + (1 << 9) * SQUARE_NUM];
}

bool BB::initialized = false;
Bitboard BB::pseudoAttacks[PIECE_TYPE_NUM][SQUARE_NUM];

// Precomputed magic numbers
// Implementation on branch find-magic-numbers
BB::Magic BB::rookAttacks[SQUARE_NUM] = {
    { nullptr, 0x000101010101017eULL, 0x0a8002c000108020ULL, 52 },
    { nullptr, 0x000202020202027cULL, 0x4440200140003000ULL, 53 },
    { nullptr, 0x000404040404047aULL, 0x8080200010011880ULL, 53 },
    { nullptr, 0x0008080808080876ULL, 0x0380180080141000ULL, 53 },
    { nullptr, 0x001010101010106eULL, 0x1a00060008211044ULL, 53 },
    { nullptr, 0x002020202020205eULL, 0x410001000a0c0008ULL, 53 },
    { nullptr, 0x004040404040403eULL, 0x9500060004008100ULL, 53 },
    { nullptr, 0x008080808080807eULL, 0x0100024284a20700ULL, 52 },
    { nullptr, 0x0001010101017e00ULL, 0x0000802140008000ULL, 53 },
    { nullptr, 0x0002020202027c00ULL, 0x0080c01002a00840ULL, 54 },
    { nullptr, 0x0004040404047a00ULL, 0x0402004282011020ULL, 54 },
    { nullptr, 0x0008080808087600ULL, 0x9862000820420050ULL, 54 },
    { nullptr, 0x0010101010106e00ULL, 0x0001001448011100ULL, 54 },
    { nullptr, 0x0020202020205e00ULL, 0x6432800200800400ULL, 54 },
    { nullptr, 0x0040404040403e00ULL, 0x040100010002000cULL, 54 },
    { nullptr, 0x0080808080807e00ULL, 0x0002800d0010c080ULL, 53 },
    { nullptr, 0x00010101017e0100ULL, 0x90c0008000803042ULL, 53 },
    { nullptr, 0x00020202027c0200ULL, 0x4010004000200041ULL, 54 },
    { nullptr, 0x00040404047a0400ULL, 0x0003010010200040ULL, 54 },
    { nullptr, 0x0008080808760800ULL, 0x0a40828028001000ULL, 54 },
    { nullptr, 0x00101010106e1000ULL, 0x0123010008000430ULL, 54 },
    { nullptr, 0x00202020205e2000ULL, 0x0024008004020080ULL, 54 },
    { nullptr, 0x00404040403e4000ULL, 0x0060040001104802ULL, 54 },
    { nullptr, 0x00808080807e8000ULL, 0x00582200028400d1ULL, 53 },
    { nullptr, 0x000101017e010100ULL, 0x4000802080044000ULL, 53 },
    { nullptr, 0x000202027c020200ULL, 0x0408208200420308ULL, 54 },
    { nullptr, 0x000404047a040400ULL, 0x0610038080102000ULL, 54 },
    { nullptr, 0x0008080876080800ULL, 0x3601000900100020ULL, 54 },
    { nullptr, 0x001010106e101000ULL, 0x0000080080040180ULL, 54 },
    { nullptr, 0x002020205e202000ULL, 0x00c2020080040080ULL, 54 },
    { nullptr, 0x004040403e404000ULL, 0x0080084400100102ULL, 54 },
    { nullptr, 0x008080807e808000ULL, 0x4022408200014401ULL, 53 },
    { nullptr, 0x0001017e01010100ULL, 0x0040052040800082ULL, 53 },
    { nullptr, 0x0002027c02020200ULL, 0x0b08200280804000ULL, 54 },
    { nullptr, 0x0004047a04040400ULL, 0x008a80a008801000ULL, 54 },
    { nullptr, 0x0008087608080800ULL, 0x4000480080801000ULL, 54 },
    { nullptr, 0x0010106e10101000ULL, 0x0911808800801401ULL, 54 },
    { nullptr, 0x0020205e20202000ULL, 0x822a003002001894ULL, 54 },
    { nullptr, 0x0040403e40404000ULL, 0x401068091400108aULL, 54 },
    { nullptr, 0x0080807e80808000ULL, 0x000004a10a00004cULL, 53 },
    { nullptr, 0x00017e0101010100ULL, 0x2000800640008024ULL, 53 },
    { nullptr, 0x00027c0202020200ULL, 0x1486408102020020ULL, 54 },
    { nullptr, 0x00047a0404040400ULL, 0x000100a000d50041ULL, 54 },
    { nullptr, 0x0008760808080800ULL, 0x00810050020b0020ULL, 54 },
    { nullptr, 0x00106e1010101000ULL, 0x0204000800808004ULL, 54 },
    { nullptr, 0x00205e2020202000ULL, 0x00020048100a000cULL, 54 },
    { nullptr, 0x00403e4040404000ULL, 0x0112000831020004ULL, 54 },
    { nullptr, 0x00807e8080808000ULL, 0x0009000040810002ULL, 53 },
    { nullptr, 0x007e010101010100ULL, 0x0440490200208200ULL, 53 },
    { nullptr, 0x007c020202020200ULL, 0x8910401000200040ULL, 54 },
    { nullptr, 0x007a040404040400ULL, 0x6404200050008480ULL, 54 },
    { nullptr, 0x0076080808080800ULL, 0x4b824a2010010100ULL, 54 },
    { nullptr, 0x006e101010101000ULL, 0x04080801810c0080ULL, 54 },
    { nullptr, 0x005e202020202000ULL, 0x00000400802a0080ULL, 54 },
    { nullptr, 0x003e404040404000ULL, 0x8224080110026400ULL, 54 },
    { nullptr, 0x007e808080808000ULL, 0x40002c4104088200ULL, 53 },
    { nullptr, 0x7e01010101010100ULL, 0x01002100104a0282ULL, 52 },
    { nullptr, 0x7c02020202020200ULL, 0x1208400811048021ULL, 53 },
    { nullptr, 0x7a04040404040400ULL, 0x3201014a40d02001ULL, 53 },
    { nullptr, 0x7608080808080800ULL, 0x0005100019200501ULL, 53 },
    { nullptr, 0x6e10101010101000ULL, 0x0101000208001005ULL, 53 },
    { nullptr, 0x5e20202020202000ULL, 0x0002008450080702ULL, 53 },
    { nullptr, 0x3e40404040404000ULL, 0x001002080301d00cULL, 53 },
    { nullptr, 0x7e80808080808000ULL, 0x410201ce5c030092ULL, 52 }
};

BB::Magic BB::bishopAttacks[SQUARE_NUM] = {
    { nullptr, 0x0040201008040200ULL, 0x0040210414004040ULL, 58 },
    { nullptr, 0x0000402010080400ULL, 0x2290100115012200ULL, 59 },
    { nullptr, 0x0000004020100a00ULL, 0x0a240400a6004201ULL, 59 },
    { nullptr, 0x0000000040221400ULL, 0x00080a0420800480ULL, 59 },
    { nullptr, 0x0000000002442800ULL, 0x4022021000000061ULL, 59 },
    { nullptr, 0x0000000204085000ULL, 0x0031012010200000ULL, 59 },
    { nullptr, 0x0000020408102000ULL, 0x4404421051080068ULL, 59 },
    { nullptr, 0x0002040810204000ULL, 0x0001040882015000ULL, 58 },
    { nullptr, 0x0020100804020000ULL, 0x8048c01206021210ULL, 59 },
    { nullptr, 0x0040201008040000ULL, 0x0222091024088820ULL, 59 },
    { nullptr, 0x00004020100a0000ULL, 0x4328110102020200ULL, 59 },
    { nullptr, 0x0000004022140000ULL, 0x0901cc41052000d0ULL, 59 },
    { nullptr, 0x0000000244280000ULL, 0xa828c20210000200ULL, 59 },
    { nullptr, 0x0000020408500000ULL, 0x0308419004a004e0ULL, 59 },
    { nullptr, 0x0002040810200000ULL, 0x4000840404860881ULL, 59 },
    { nullptr, 0x0004081020400000ULL, 0x0800008424020680ULL, 59 },
    { nullptr, 0x0010080402000200ULL, 0x28100040100204a1ULL, 59 },
    { nullptr, 0x0020100804000400ULL, 0x0082001002080510ULL, 59 },
    { nullptr, 0x004020100a000a00ULL, 0x9008103000204010ULL, 57 },
    { nullptr, 0x0000402214001400ULL, 0x141820040c00b000ULL, 57 },
    { nullptr, 0x0000024428002800ULL, 0x0081010090402022ULL, 57 },
    { nullptr, 0x0002040850005000ULL, 0x0014400480602000ULL, 57 },
    { nullptr, 0x0004081020002000ULL, 0x008a008048443c00ULL, 59 },
    { nullptr, 0x0008102040004000ULL, 0x0000280202060220ULL, 59 },
    { nullptr, 0x0008040200020400ULL, 0x3520100860841100ULL, 59 },
    { nullptr, 0x0010080400040800ULL, 0x9810083c02080100ULL, 59 },
    { nullptr, 0x0020100a000a1000ULL, 0x41003000620c0140ULL, 57 },
    { nullptr, 0x0040221400142200ULL, 0x06100400104010a0ULL, 55 },
    { nullptr, 0x0002442800284400ULL, 0x0020840000802008ULL, 55 },
    { nullptr, 0x0004085000500800ULL, 0x40050a010900a080ULL, 57 },
    { nullptr, 0x0008102000201000ULL, 0x0818404001041602ULL, 59 },
    { nullptr, 0x0010204000402000ULL, 0x8040604006010400ULL, 59 },
    { nullptr, 0x0004020002040800ULL, 0x1028044001041800ULL, 59 },
    { nullptr, 0x0008040004081000ULL, 0x0080b00828108200ULL, 59 },
    { nullptr, 0x00100a000a102000ULL, 0xc000280c04080220ULL, 57 },
    { nullptr, 0x0022140014224000ULL, 0x3010020080880081ULL, 55 },
    { nullptr, 0x0044280028440200ULL, 0x10004c0400004100ULL, 55 },
    { nullptr, 0x0008500050080400ULL, 0x3010020200002080ULL, 57 },
    { nullptr, 0x0010200020100800ULL, 0x202304019004020aULL, 59 },
    { nullptr, 0x0020400040201000ULL, 0x0004208a0000e110ULL, 59 },
    { nullptr, 0x0002000204081000ULL, 0x0108018410006000ULL, 59 },
    { nullptr, 0x0004000408102000ULL, 0x0202210120440800ULL, 59 },
    { nullptr, 0x000a000a10204000ULL, 0x100850c828001000ULL, 57 },
    { nullptr, 0x0014001422400000ULL, 0x1401024204800800ULL, 57 },
    { nullptr, 0x0028002844020000ULL, 0x0000041028800402ULL, 57 },
    { nullptr, 0x0050005008040200ULL, 0x0020642300480600ULL, 57 },
    { nullptr, 0x0020002010080400ULL, 0x0020410200800202ULL, 59 },
    { nullptr, 0x0040004020100800ULL, 0xca02480845000080ULL, 59 },
    { nullptr, 0x0000020408102000ULL, 0x0140c404a0080410ULL, 59 },
    { nullptr, 0x0000040810204000ULL, 0x2180a40108884441ULL, 59 },
    { nullptr, 0x00000a1020400000ULL, 0x4410420104980302ULL, 59 },
    { nullptr, 0x0000142240000000ULL, 0x1108040046080000ULL, 59 },
    { nullptr, 0x0000284402000000ULL, 0x8141029012020008ULL, 59 },
    { nullptr, 0x0000500804020000ULL, 0x0894081818082800ULL, 59 },
    { nullptr, 0x0000201008040200ULL, 0x0040020404628000ULL, 59 },
    { nullptr, 0x0000402010080400ULL, 0x0804100c010c2122ULL, 59 },
    { nullptr, 0x0002040810204000ULL, 0x8168210510101200ULL, 58 },
    { nullptr, 0x0004081020400000ULL, 0x0001088148121080ULL, 59 },
    { nullptr, 0x000a102040000000ULL, 0x0204010100c11010ULL, 59 },
    { nullptr, 0x0014224000000000ULL, 0x1814102013841400ULL, 59 },
    { nullptr, 0x0028440200000000ULL, 0x0000c00010020602ULL, 59 },
    { nullptr, 0x0050080402000000ULL, 0x001045220c040820ULL, 59 },
    { nullptr, 0x0020100804020000ULL, 0x0012400808070840ULL, 59 },
    { nullptr, 0x0040201008040200ULL, 0x002004012a040132ULL, 58 }
};

Bitboard BB::between[SQUARE_NUM][SQUARE_NUM] = { BB::NONE };
Bitboard BB::line[SQUARE_NUM][SQUARE_NUM] = { BB::NONE };


template <Direction dir>
static Bitboard SlideAttack(Square square, Bitboard occupancy) {
    Bitboard bb = 0;
    Bitboard squareBB = BB::SquareBB(square);
    while (true) {
        squareBB = BB::Shift<dir>(squareBB);
        bb |= squareBB;
        if (!squareBB || squareBB & occupancy) return bb;
    }
}

static Bitboard SlideAttack(Square square, Direction dir, Bitboard occupancy) {
    switch (dir) {
    case Direction::UP:         return SlideAttack<Direction::UP>(square, occupancy);
    case Direction::UP_RIGHT:   return SlideAttack<Direction::UP_RIGHT>(square, occupancy);
    case Direction::RIGHT:      return SlideAttack<Direction::RIGHT>(square, occupancy);
    case Direction::DOWN_RIGHT: return SlideAttack<Direction::DOWN_RIGHT>(square, occupancy);
    case Direction::DOWN:       return SlideAttack<Direction::DOWN>(square, occupancy);
    case Direction::DOWN_LEFT:  return SlideAttack<Direction::DOWN_LEFT>(square, occupancy);
    case Direction::LEFT:       return SlideAttack<Direction::LEFT>(square, occupancy);
    case Direction::UP_LEFT:    return SlideAttack<Direction::UP_LEFT>(square, occupancy);
    default:                    return 0;
    }
}

static Bitboard ComputeKingAttack(Square square) {
    Bitboard squareBB = BB::SquareBB(square);
    return
        BB::Shift<Direction::UP>(squareBB)        |
        BB::Shift<Direction::DOWN>(squareBB)      |
        BB::Shift<Direction::LEFT>(squareBB)      |
        BB::Shift<Direction::RIGHT>(squareBB)     |
        BB::Shift<Direction::UP_LEFT>(squareBB)   |
        BB::Shift<Direction::UP_RIGHT>(squareBB)  |
        BB::Shift<Direction::DOWN_LEFT>(squareBB) |
        BB::Shift<Direction::DOWN_RIGHT>(squareBB);
}

static Bitboard ComputeRookAttack(Square square, Bitboard occupancy) {
    return
        SlideAttack<Direction::UP>(square, occupancy)   |
        SlideAttack<Direction::DOWN>(square, occupancy) |
        SlideAttack<Direction::LEFT>(square, occupancy) |
        SlideAttack<Direction::RIGHT>(square, occupancy);
}

static Bitboard ComputeBishopAttack(Square square, Bitboard occupancy) {
    return
        SlideAttack<Direction::UP_LEFT>(square, occupancy)  |
        SlideAttack<Direction::UP_RIGHT>(square, occupancy) |
        SlideAttack<Direction::DOWN_LEFT>(square, occupancy)|
        SlideAttack<Direction::DOWN_RIGHT>(square, occupancy);
}

static Bitboard ComputeKightAttack(Square square) {
    Bitboard squareBB = BB::SquareBB(square);
    Bitboard ul = BB::Shift<Direction::UP_LEFT>(squareBB);
    Bitboard ur = BB::Shift<Direction::UP_RIGHT>(squareBB);
    Bitboard dl = BB::Shift<Direction::DOWN_LEFT>(squareBB);
    Bitboard dr = BB::Shift<Direction::DOWN_RIGHT>(squareBB);
    return
        BB::Shift<Direction::UP>(ul)      |
        BB::Shift<Direction::LEFT>(ul)    |
        BB::Shift<Direction::UP>(ur)      |
        BB::Shift<Direction::RIGHT>(ur)   |
        BB::Shift<Direction::DOWN>(dl)    |
        BB::Shift<Direction::LEFT>(dl)    |
        BB::Shift<Direction::DOWN>(dr)    |
        BB::Shift<Direction::RIGHT>(dr);
}

void BB::InitPseudoAttacks() {
    for (Square square = Square::A1; square <= Square::H8; ++square) {
        Bitboard kingAttack = ComputeKingAttack(square);
        Bitboard rookAttack = ComputeRookAttack(square, 0);
        Bitboard bishopAttack = ComputeBishopAttack(square, 0);
        Bitboard knightAttack = ComputeKightAttack(square);

        pseudoAttacks[ToInt(PieceType::King)][ToInt(square)]    = kingAttack;
        pseudoAttacks[ToInt(PieceType::Queen)][ToInt(square)]   = rookAttack | bishopAttack;
        pseudoAttacks[ToInt(PieceType::Rook)][ToInt(square)]    = rookAttack;
        pseudoAttacks[ToInt(PieceType::Bishop)][ToInt(square)]  = bishopAttack;
        pseudoAttacks[ToInt(PieceType::Knight)][ToInt(square)]  = knightAttack;
    }
}

void BB::InitMagicBitboards() {
    Bitboard* table = attacksTable;
    for (Square square = Square::A1; square <= Square::H8; ++square) {
        table = InitMagicBitboards(PieceType::Rook, square, table);
    }
    for (Square square = Square::A1; square <= Square::H8; ++square) {
        table = InitMagicBitboards(PieceType::Bishop, square, table);
    }
}

Bitboard* BB::InitMagicBitboards(PieceType pieceType, Square square, Bitboard* tableStart) {
    const auto squareInt = ToInt(square);
    const auto computeAttack = pieceType == PieceType::Rook ? ComputeRookAttack : ComputeBishopAttack;
   
    Magic& attacks = pieceType == PieceType::Rook ? rookAttacks[squareInt] : bishopAttacks[squareInt];
    attacks.table = tableStart;
    for (Bitboard occupancy = attacks.mask; ; occupancy = (occupancy - 1) & attacks.mask) {
        uint64_t index = attacks.TableIndex(occupancy);
        attacks.table[index] = computeAttack(square, occupancy);
        if (!occupancy) break; // finished
    }

    assert(Count1s(attacks.mask) == 64u - attacks.shift);
    return tableStart + (1 << (64 - attacks.shift));
}

void BB::InitBetween() {
    for (Square sq1 = Square::A1; sq1 <= Square::H8; ++sq1) {
        for (Direction dir : { 
            Direction::UP, Direction::UP_RIGHT, Direction::RIGHT, 
            Direction::DOWN_RIGHT, Direction::DOWN, Direction::DOWN_LEFT, 
            Direction::LEFT, Direction::UP_LEFT 
        }) {
            Bitboard bb = SquareBB(sq1 + dir);
            for (Square sq2 = sq1 + dir + dir; IsValid(sq2); sq2 += dir) {
                between[ToInt(sq1)][ToInt(sq2)] = bb;
                bb |= SquareBB(sq2);
            }
        }
    }
}

void BB::InitLine() {
    for (Square sq1 = Square::A1; sq1 <= Square::H8; ++sq1) {
        for (Direction dir : { 
            Direction::UP, Direction::UP_RIGHT, Direction::RIGHT, 
            Direction::DOWN_RIGHT, Direction::DOWN, Direction::DOWN_LEFT, 
            Direction::LEFT, Direction::UP_LEFT 
        }) {
            Bitboard bb = SlideAttack(sq1, dir, 0) | SlideAttack(sq1, -dir, 0) | SquareBB(sq1);
            for (Square sq2 = sq1 + dir; IsValid(sq2); sq2 += dir) {
                between[ToInt(sq1)][ToInt(sq2)] = bb;
            }
        }
    }
}

void BB::Init() {
    InitPseudoAttacks();
    InitMagicBitboards();
    InitBetween();
    InitLine();
    initialized = true;
}
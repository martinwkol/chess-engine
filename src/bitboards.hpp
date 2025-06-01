#pragma once

#include <cstdint>

using Bitboard = uint64_t;

constexpr Bitboard BB_FILE_A = (1 << 8) - 1;
constexpr Bitboard BB_FILE_B = BB_FILE_A << (8 * 1);
constexpr Bitboard BB_FILE_C = BB_FILE_A << (8 * 2);
constexpr Bitboard BB_FILE_D = BB_FILE_A << (8 * 3);
constexpr Bitboard BB_FILE_E = BB_FILE_A << (8 * 4);
constexpr Bitboard BB_FILE_F = BB_FILE_A << (8 * 5);
constexpr Bitboard BB_FILE_G = BB_FILE_A << (8 * 6);
constexpr Bitboard BB_FILE_H = BB_FILE_A << (8 * 7);

constexpr Bitboard BB_RANK_1 = 0x0101010101010101;
constexpr Bitboard BB_RANK_2 = BB_RANK_1 << 1;
constexpr Bitboard BB_RANK_3 = BB_RANK_1 << 2;
constexpr Bitboard BB_RANK_4 = BB_RANK_1 << 3;
constexpr Bitboard BB_RANK_5 = BB_RANK_1 << 4;
constexpr Bitboard BB_RANK_6 = BB_RANK_1 << 5;
constexpr Bitboard BB_RANK_7 = BB_RANK_1 << 6;
constexpr Bitboard BB_RANK_8 = BB_RANK_1 << 7;


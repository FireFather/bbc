#pragma once
#include <cstdint>
#include <intrin.h>

#include "pragma.h"

#define SET_BIT_ON(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define GET_BIT(bitboard, square) ((bitboard) & (1ULL << (square)))
#define SET_BIT_OFF(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

inline int popcount(const uint64_t b)
{
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
	return static_cast<int>(_mm_popcnt_u64(b));
#elif defined(__GNUC__)
	return __builtin_popcountll(b);
#endif
}

static int lsb(const uint64_t b)
{
#if defined(_WIN64) && defined(_MSC_VER)
	unsigned long idx;
	_BitScanForward64(&idx, b);
	return static_cast<int>(idx);
#elif defined(__GNUC__)
	return static_cast<int>(__builtin_ctzll(b));
#endif
}

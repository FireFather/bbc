#include "attack.h"

#include "board.h"
#include "global.h"

const int bishop_relevant_bits[64] = {
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};

const int rook_relevant_bits[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

uint64_t pawn_attacks[2][64];
uint64_t knight_attacks[64];
uint64_t king_attacks[64];
uint64_t bishop_masks[64];
uint64_t rook_masks[64];
uint64_t bishop_attacks[64][512];
uint64_t rook_attacks[64][4096];

uint64_t mask_pawn_attacks(const int side, const int square)
{
	uint64_t attacks = 0ULL;
	uint64_t bitboard = 0ULL;
	SET_BIT_ON(bitboard, square);

	if (!side)
	{
		if ((bitboard >> 7) & not_a_file)
			attacks |= (bitboard >> 7);
		if ((bitboard >> 9) & not_h_file)
			attacks |= (bitboard >> 9);
	}
	else
	{
		if ((bitboard << 7) & not_h_file)
			attacks |= (bitboard << 7);
		if ((bitboard << 9) & not_a_file)
			attacks |= (bitboard << 9);
	}

	return attacks;
}

uint64_t mask_knight_attacks(const int square)
{
	uint64_t attacks = 0ULL;
	uint64_t bitboard = 0ULL;

	SET_BIT_ON(bitboard, square);

	if ((bitboard >> 17) & not_h_file)
	{
		attacks |= (bitboard >> 17);
	}

	if ((bitboard >> 15) & not_a_file)
	{
		attacks |= (bitboard >> 15);
	}

	if ((bitboard >> 10) & not_hg_file)
	{
		attacks |= (bitboard >> 10);
	}

	if ((bitboard >> 6) & not_ab_file)
	{
		attacks |= (bitboard >> 6);
	}

	if ((bitboard << 17) & not_a_file)
	{
		attacks |= (bitboard << 17);
	}

	if ((bitboard << 15) & not_h_file)
	{
		attacks |= (bitboard << 15);
	}

	if ((bitboard << 10) & not_ab_file)
	{
		attacks |= (bitboard << 10);
	}

	if ((bitboard << 6) & not_hg_file)
	{
		attacks |= (bitboard << 6);
	}
	return attacks;
}

uint64_t mask_king_attacks(const int square)
{
	uint64_t attacks = 0ULL;
	uint64_t bitboard = 0ULL;

	SET_BIT_ON(bitboard, square);

	if (bitboard >> 8)
	{
		attacks |= (bitboard >> 8);
	}

	if ((bitboard >> 9) & not_h_file)
	{
		attacks |= (bitboard >> 9);
	}

	if ((bitboard >> 7) & not_a_file)
	{
		attacks |= (bitboard >> 7);
	}

	if ((bitboard >> 1) & not_h_file)
	{
		attacks |= (bitboard >> 1);
	}

	if (bitboard << 8)
	{
		attacks |= (bitboard << 8);
	}

	if ((bitboard << 9) & not_a_file)
	{
		attacks |= (bitboard << 9);
	}

	if ((bitboard << 7) & not_h_file)
	{
		attacks |= (bitboard << 7);
	}

	if ((bitboard << 1) & not_a_file)
	{
		attacks |= (bitboard << 1);
	}
	return attacks;
}

uint64_t mask_bishop_attacks(const int square)
{
	uint64_t attacks = 0ULL;
	int rank, file;
	const int target_file = square % 8;
	const int target_rank = square / 8;

	for (rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; rank++, file++)
	{
		attacks |= (1ULL << (rank * 8 + file));
	}

	for (rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; rank--, file++)
	{
		attacks |= (1ULL << (rank * 8 + file));
	}

	for (rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; rank++, file--)
	{
		attacks |= (1ULL << (rank * 8 + file));
	}

	for (rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; rank--, file--)
	{
		attacks |= (1ULL << (rank * 8 + file));
	}

	return attacks;
}

uint64_t mask_rook_attacks(const int square)
{
	uint64_t attacks = 0ULL;
	int rank, file;
	const int target_file = square % 8;
	const int target_rank = square / 8;

	for (rank = target_rank + 1; rank <= 6; rank++)
	{
		attacks |= (1ULL << (rank * 8 + target_file));
	}

	for (rank = target_rank - 1; rank >= 1; rank--)
	{
		attacks |= (1ULL << (rank * 8 + target_file));
	}

	for (file = target_file + 1; file <= 6; file++)
	{
		attacks |= (1ULL << (target_rank * 8 + file));
	}

	for (file = target_file - 1; file >= 1; file--)
	{
		attacks |= (1ULL << (target_rank * 8 + file));
	}
	return attacks;
}

uint64_t bishop_attacks_on_the_fly(const int square, const uint64_t block)
{
	uint64_t attacks = 0ULL;

	int rank, file;
	const int target_file = square % 8;
	const int target_rank = square / 8;

	for (rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; rank++, file++)
	{
		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & block)
		{
			break;
		}
	}

	for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; rank--, file++)
	{
		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & block)
		{
			break;
		}
	}

	for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; rank++, file--)
	{
		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & block)
		{
			break;
		}
	}
	for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; rank--, file--)
	{
		attacks |= (1ULL << (rank * 8 + file));
		if ((1ULL << (rank * 8 + file)) & block)
		{
			break;
		}
	}
	return attacks;
}

uint64_t rook_attacks_on_the_fly(const int square, const uint64_t block)
{
	uint64_t attacks = 0ULL;

	int rank, file;
	const int target_rank = square / 8;
	const int target_file = square % 8;

	for (rank = target_rank + 1; rank <= 7; rank++)
	{
		attacks |= (1ULL << (rank * 8 + target_file));
		if ((1ULL << (rank * 8 + target_file)) & block)
		{
			break;
		}
	}

	for (rank = target_rank - 1; rank >= 0; rank--)
	{
		attacks |= (1ULL << (rank * 8 + target_file));
		if ((1ULL << (rank * 8 + target_file)) & block)
		{
			break;
		}
	}

	for (file = target_file + 1; file <= 7; file++)
	{
		attacks |= (1ULL << (target_rank * 8 + file));
		if ((1ULL << (target_rank * 8 + file)) & block)
		{
			break;
		}
	}

	for (file = target_file - 1; file >= 0; file--)
	{
		attacks |= (1ULL << (target_rank * 8 + file));
		if ((1ULL << (target_rank * 8 + file)) & block)
		{
			break;
		}
	}
	return attacks;
}

void init_jump_attacks()
{
	for (int square = 0; square < 64; square++)
	{
		pawn_attacks[white][square] = mask_pawn_attacks(white, square);
		pawn_attacks[black][square] = mask_pawn_attacks(black, square);

		knight_attacks[square] = mask_knight_attacks(square);

		king_attacks[square] = mask_king_attacks(square);
	}
}

uint64_t set_occupancy(const int index, const int bits_in_mask, uint64_t attack_mask)
{
	uint64_t occupancy = 0ULL;

	for (int count = 0; count < bits_in_mask; count++)
	{
		const int square = lsb(attack_mask);

		SET_BIT_OFF(attack_mask, square);

		if (index & (1 << count))
		{
			occupancy |= (1ULL << square);
		}
	}

	return occupancy;
}

void init_slider_attacks(const int bishop)
{
	for (int square = 0; square < 64; square++)
	{
		bishop_masks[square] = mask_bishop_attacks(square);
		rook_masks[square] = mask_rook_attacks(square);

		const uint64_t attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

		const int relevant_bits_count = popcount(attack_mask);

		const int occupancy_indicies = (1 << relevant_bits_count);

		for (int index = 0; index < occupancy_indicies; index++)
		{
			if (bishop)
			{
				const uint64_t occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
				const int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);
				bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
			}

			else
			{
				const uint64_t occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
				const int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);
				rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
			}
		}
	}
}

uint64_t get_bishop_attacks(const int square, uint64_t occupancy)
{
	occupancy &= bishop_masks[square];
	occupancy *= bishop_magic_numbers[square];
	occupancy >>= 64 - bishop_relevant_bits[square];
	return bishop_attacks[square][occupancy];
}

uint64_t get_rook_attacks(const int square, uint64_t occupancy)
{
	occupancy &= rook_masks[square];
	occupancy *= rook_magic_numbers[square];
	occupancy >>= 64 - rook_relevant_bits[square];
	return rook_attacks[square][occupancy];
}

uint64_t get_queen_attacks(const int square, const unsigned long long occupancy)
{
	return (get_bishop_attacks(square, occupancy) | get_rook_attacks(square, occupancy));
}

int is_square_attacked(const int square, const int side)
{
	if ((side == white) && (pawn_attacks[black][square] & bitboards[P]))
	{
		return 1;
	}
	if ((side == black) && (pawn_attacks[white][square] & bitboards[p]))
	{
		return 1;
	}
	if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n]))
	{
		return 1;
	}
	if (get_bishop_attacks(square, occupied[both]) & ((side == white) ? bitboards[B] : bitboards[b]))
	{
		return 1;
	}
	if (get_rook_attacks(square, occupied[both]) & ((side == white) ? bitboards[R] : bitboards[r]))
	{
		return 1;
	}
	if (get_queen_attacks(square, occupied[both]) & ((side == white) ? bitboards[Q] : bitboards[q]))
	{
		return 1;
	}

	if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k]))
	{
		return 1;
	}
	return 0;
}

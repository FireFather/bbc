#include "zobrist.h"
#include "board.h"
#include "global.h"

uint64_t piece_keys[12][64];
uint64_t enpassant_keys[64];
uint64_t castle_keys[16];
uint64_t side_key;

void init_hash_keys()
{
	random_state = 1804289383;

	for (int piece = P; piece <= k; piece++)
	{
		for (int square = 0; square < 64; square++)
			piece_keys[piece][square] = get_random_u64_number();
	}

	for (unsigned long long& enpassant_key : enpassant_keys)
		enpassant_key = get_random_u64_number();

	for (unsigned long long& castle_key : castle_keys)
		castle_key = get_random_u64_number();

	side_key = get_random_u64_number();
}

uint64_t generate_hash_key()
{
	uint64_t output_key = 0ULL;

	for (int piece = P; piece <= k; piece++)
	{
		uint64_t bitboard = bitboards[piece];

		while (bitboard)
		{
			const int square = lsb(bitboard);

			output_key ^= piece_keys[piece][square];

			SET_BIT_OFF(bitboard, square);
		}
	}

	if (enpassant != NO_SQUARE)
	{
		output_key ^= enpassant_keys[enpassant];
	}

	output_key ^= castle_keys[castle];

	if (color == black)
	{
		output_key ^= side_key;
	}
	return output_key;
}

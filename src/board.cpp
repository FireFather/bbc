#include <cstdio>

#include "board.h"

#include <cstring>

#include "attack.h"
#include "global.h"
#include "search.h"
#include "zobrist.h"

uint64_t bitboards[12];
uint64_t occupied[3];
int color;
int enpassant;
int castle;
uint64_t hashKey;
uint64_t repetition_table[1000];
int repetition_index;
int ply;
int fifty_move_counter;

const int castle_rights[64] = {
	7, 15, 15, 15, 3, 15, 15, 11,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	13, 15, 15, 15, 12, 15, 15, 14
};

int convert_piece(const char c)
{
	switch (c)
	{
	case 'P':
		return P;
	case 'N':
		return N;
	case 'B':
		return B;
	case 'R':
		return R;
	case 'Q':
		return Q;
	case 'K':
		return K;
	case 'p':
		return p;
	case 'n':
		return n;
	case 'b':
		return b;
	case 'r':
		return r;
	case 'q':
		return q;
	case 'k':
		return k;
	default:
		return -1;
	}
}

char get_promoted_piece(const int piece)
{
	switch (piece)
	{
	case Q:
	case q:
		return 'q';
	case R:
	case r:
		return 'r';
	case B:
	case b:
		return 'b';
	case N:
	case n:
		return 'n';
	default:
		return ' ';
	}
}

void print_board()
{
	printf("\n");
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 8; file++)
		{
			const int square = rank * 8 + file;

			int piece = -1;
			for (int bitboardPiece = P; bitboardPiece <= k; bitboardPiece++)
			{
				if (GET_BIT(bitboards[bitboardPiece], square))
				{
					piece = bitboardPiece;
				}
			}
			printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
		}
		printf("\n");
	}
	printf("\n");
	printf(" side: %s\n", !color ? "white" : "black");
	printf(" enpassant: %s\n", (enpassant != NO_SQUARE) ? square_to_coordinates[enpassant] : "no");
	printf(" castling: %c%c%c%c\n\n", (castle & wk) ? 'K' : '-',
	       (castle & wq) ? 'Q' : '-',
	       (castle & bk) ? 'k' : '-',
	       (castle & bq) ? 'q' : '-');
	printf(" hash key:\n");
	printf(" %llx\n\n", hashKey);
}

void reset_board()
{
	memset(bitboards, 0ULL, sizeof(bitboards));
	memset(occupied, 0ULL, sizeof(occupied));
	color = 0;
	fifty_move_counter = 0;
	enpassant = NO_SQUARE;
	castle = 0;
	repetition_index = 0;
	memset(repetition_table, 0ULL, sizeof(repetition_table));
}

void parse_fen(char* fen)
{
	reset_board();
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 8; file++)
		{
			const int square = rank * 8 + file;
			if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z'))
			{
				if (const int piece = convert_piece(*fen); piece != -1)
				{
					SET_BIT_ON(bitboards[piece], square);
				}
				fen++;
			}

			if (*fen >= '0' && *fen <= '9')
			{
				const int offset = *fen - '0';
				int piece = -1;
				for (int bb_piece = P; bb_piece <= k; bb_piece++)
				{
					if (GET_BIT(bitboards[bb_piece], square))
						piece = bb_piece;
				}
				if (piece == -1)
				{
					file--;
				}
				file += offset;
				fen++;
			}

			if (*fen == '/')
			{
				fen++;
			}
		}
	}
	fen++;
	(*fen == 'w') ? (color = white) : (color = black);

	fen += 2;

	while (*fen != ' ')
	{
		switch (*fen)
		{
		case 'K':
			castle |= wk;
			break;
		case 'Q':
			castle |= wq;
			break;
		case 'k':
			castle |= bk;
			break;
		case 'q':
			castle |= bq;
			break;
		case '-':
			break;
		}
		fen++;
	}
	fen++;

	if (*fen != '-')
	{
		const int file = fen[0] - 'a';
		const int rank = 8 - (fen[1] - '0');
		enpassant = rank * 8 + file;
	}
	else
	{
		enpassant = NO_SQUARE;
	}
	for (int piece = P; piece <= K; piece++)
	{
		occupied[white] |= bitboards[piece];
	}

	for (int piece = p; piece <= k; piece++)
	{
		occupied[black] |= bitboards[piece];
	}
	occupied[both] |= occupied[white];
	occupied[both] |= occupied[black];
	hashKey = generate_hash_key();
}

void print_move(const int move)
{
	if (GET_MOVE_PROMOTED(move))
		printf("%s%s%c", square_to_coordinates[GET_MOVE_SOURCE(move)],
		       square_to_coordinates[GET_MOVE_TARGET(move)],
		       get_promoted_piece(GET_MOVE_PROMOTED(move)));
	else
		printf("%s%s", square_to_coordinates[GET_MOVE_SOURCE(move)],
		       square_to_coordinates[GET_MOVE_TARGET(move)]);
}

int is_repetition()
{
	for (int index = 0; index < repetition_index; index++)
	{
		if (repetition_table[index] == hashKey)
		{
			return 1;
		}
	}
	return 0;
}

int make_move(const int move, const int move_flag)
{
	if (move_flag == all_moves)
	{
		COPY_BOARD()

		const int source_square = GET_MOVE_SOURCE(move);
		const int target_square = GET_MOVE_TARGET(move);
		const int piece = GET_MOVE_PIECE(move);
		const int promoted_piece = GET_MOVE_PROMOTED(move);
		const int capture = GET_MOVE_CAPTURE(move);
		const int double_push = GET_MOVE_DOUBLE(move);
		const int enpass = GET_MOVE_ENPASSANT(move);
		const int castling = GET_MOVE_CASTLING(move);

		SET_BIT_OFF(bitboards[piece], source_square);
		SET_BIT_ON(bitboards[piece], target_square);

		hashKey ^= piece_keys[piece][source_square];
		hashKey ^= piece_keys[piece][target_square];

		fifty_move_counter++;
		if (piece == P || piece == p)
		{
			fifty_move_counter = 0;
		}

		if (capture)
		{
			fifty_move_counter = 0;
			int start_piece, endPiece;
			if (color == white)
			{
				start_piece = p;
				endPiece = k;
			}
			else
			{
				start_piece = P;
				endPiece = K;
			}

			for (int current_piece = start_piece; current_piece <= endPiece; current_piece++)
			{
				if (GET_BIT(bitboards[current_piece], target_square))
				{
					SET_BIT_OFF(bitboards[current_piece], target_square);
					hashKey ^= piece_keys[current_piece][target_square];
					break;
				}
			}
		}

		if (promoted_piece)
		{
			if (color == white)
			{
				SET_BIT_OFF(bitboards[P], target_square);
				hashKey ^= piece_keys[P][target_square];
			}
			else
			{
				SET_BIT_OFF(bitboards[p], target_square);
				hashKey ^= piece_keys[p][target_square];
			}

			SET_BIT_ON(bitboards[promoted_piece], target_square);
			hashKey ^= piece_keys[promoted_piece][target_square];
		}

		if (enpass)
		{
			(color == white) ? SET_BIT_OFF(bitboards[p], target_square + 8) : SET_BIT_OFF(bitboards[P], target_square - 8);
			if (color == white)
			{
				SET_BIT_OFF(bitboards[p], target_square + 8);
				hashKey ^= piece_keys[p][target_square + 8];
			}
			else
			{
				SET_BIT_OFF(bitboards[P], target_square - 8);
				hashKey ^= piece_keys[P][target_square - 8];
			}
		}

		if (enpassant != NO_SQUARE)
		{
			hashKey ^= enpassant_keys[enpassant];
		}

		enpassant = NO_SQUARE;
		if (double_push)
		{
			if (color == white)
			{
				enpassant = target_square + 8;
				hashKey ^= enpassant_keys[target_square + 8];
			}
			else
			{
				enpassant = target_square - 8;
				hashKey ^= enpassant_keys[target_square - 8];
			}
		}

		if (castling)
		{
			switch (target_square)
			{
			case (G1):
				SET_BIT_OFF(bitboards[R], H1);
				SET_BIT_ON(bitboards[R], F1);
				hashKey ^= piece_keys[R][H1];
				hashKey ^= piece_keys[R][F1];
				break;
			case (C1):
				SET_BIT_OFF(bitboards[R], A1);
				SET_BIT_ON(bitboards[R], D1);

				hashKey ^= piece_keys[R][A1];
				hashKey ^= piece_keys[R][D1];
				break;

			case (G8):
				SET_BIT_OFF(bitboards[r], H8);
				SET_BIT_ON(bitboards[r], F8);

				hashKey ^= piece_keys[r][H8];
				hashKey ^= piece_keys[r][F8];
				break;

			case (C8):
				SET_BIT_OFF(bitboards[r], A8);
				SET_BIT_ON(bitboards[r], D8);

				hashKey ^= piece_keys[r][A8];
				hashKey ^= piece_keys[r][D8];
				break;
			default: ;
			}
		}

		hashKey ^= castle_keys[castle];
		castle &= castle_rights[source_square];
		castle &= castle_rights[target_square];
		hashKey ^= castle_keys[castle];
		memset(occupied, 0ULL, 24);

		for (int current_white_piece = P; current_white_piece <= K; current_white_piece++)
		{
			occupied[white] |= bitboards[current_white_piece];
		}

		for (int current_black_piece = p; current_black_piece <= k; current_black_piece++)
		{
			occupied[black] |= bitboards[current_black_piece];
		}

		occupied[both] |= occupied[white];
		occupied[both] |= occupied[black];
		color ^= 1;
		hashKey ^= side_key;

		if (is_square_attacked((color == white) ? lsb(bitboards[k]) : lsb(bitboards[K]), color))
		{
			RESTORE_BOARD()
			return 0;
		}
		return 1;
	}
	if (GET_MOVE_CAPTURE(move))
	{
		return make_move(move, all_moves);
	}
	return 0;
}

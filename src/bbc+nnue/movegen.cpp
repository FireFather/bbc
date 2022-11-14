#include "attack.h"
#include "bit.h"
#include "board.h"
#include "global.h"

void add_move(moves* moveList, const int move) {
	moveList->moves[moveList->count] =
		move;
	moveList->count++;
}

void generate_moves(moves* move_list) {
	move_list->count = 0;

	int from_sq, to_sq;
	uint64_t attacks;

	for (int piece = P; piece <= k; piece++) {
		uint64_t bitboard = bitboards[piece];

		if (color == white) {
			if (piece == P) {
				while (bitboard) {
					from_sq = lsb(bitboard);

					to_sq = from_sq - 8;

					if (!(to_sq < A8) && !GET_BIT(occupied[both], to_sq)) {
						if (from_sq >= A7 && from_sq <= H7) {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, Q, 0, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, R, 0, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, B, 0, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, N, 0, 0, 0, 0));
						}
						else {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 0, 0, 0, 0));

							if ((from_sq >= A2 && from_sq <= H2) &&
								!GET_BIT(occupied[both], to_sq - 8)) {
								add_move(move_list,
									ENCODE_MOVE(from_sq, (to_sq - 8), piece, 0, 0, 1, 0, 0));
							}

						}
					}
					attacks = pawn_attacks[color][from_sq] & occupied[black];

					while (attacks) {

						to_sq = lsb(attacks);

						if (from_sq >= A7 && from_sq <= H7) {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, Q, 1, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, R, 1, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, B, 1, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, N, 1, 0, 0, 0));
						}
						else {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 1, 0, 0, 0));
						}

						SET_BIT_OFF(attacks, to_sq);
					}

					if (enpassant != NO_SQUARE) {
						if (const uint64_t enpassant_attacks = pawn_attacks[color][from_sq] & (1ULL << enpassant)) {
							const int target_enpassant = lsb(enpassant_attacks);
							add_move(move_list, ENCODE_MOVE(from_sq, target_enpassant, piece, 0, 1, 0, 1, 0));
						}
					}

					SET_BIT_OFF(bitboard, from_sq);
				}
			}

			if (piece == K) {
				if (castle & wk) {
					if (!GET_BIT(occupied[both], F1) && !GET_BIT(occupied[both], G1)) {
						if (!is_square_attacked(E1, black) && !is_square_attacked(F1, black))
							add_move(move_list, ENCODE_MOVE(E1, G1, piece, 0, 0, 0, 0, 1));
					}
				}

				if (castle & wq) {
					if (!GET_BIT(occupied[both], D1) && !GET_BIT(occupied[both], C1) &&
						!GET_BIT(occupied[both], B1)) {
						if (!is_square_attacked(E1, black) && !is_square_attacked(D1, black))
							add_move(move_list, ENCODE_MOVE(E1, C1, piece, 0, 0, 0, 0, 1));
					}
				}
			}
		}
		else {

			if (piece == p) {
				while (bitboard) {
					from_sq = lsb(bitboard);
					to_sq = from_sq + 8;

					if (!(to_sq > H1) && !GET_BIT(occupied[both], to_sq)) {
						if (from_sq >= A2 && from_sq <= H2) {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, q, 0, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, r, 0, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, b, 0, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, n, 0, 0, 0, 0));
						}
						else {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 0, 0, 0, 0));

							if ((from_sq >= A7 && from_sq <= H7) &&
								!GET_BIT(occupied[both], to_sq + 8)) {
								add_move(move_list,
									ENCODE_MOVE(from_sq, (to_sq + 8), piece, 0, 0, 1, 0, 0));
							}

						}
					}

					attacks = pawn_attacks[color][from_sq] & occupied[white];

					while (attacks) {

						to_sq = lsb(attacks);

						if (from_sq >= A2 && from_sq <= H2) {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, q, 1, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, r, 1, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, b, 1, 0, 0, 0));
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, n, 1, 0, 0, 0));
						}
						else {
							add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 1, 0, 0, 0));
						}
						SET_BIT_OFF(attacks, to_sq);
					}

					if (enpassant != NO_SQUARE) {
						if (const uint64_t enpassantAttacks = pawn_attacks[color][from_sq] & (1ULL << enpassant)) {
							const int targetEnpassant = lsb(enpassantAttacks);
							add_move(move_list, ENCODE_MOVE(from_sq, targetEnpassant, piece, 0, 1, 0, 1, 0));
						}
					}
					SET_BIT_OFF(bitboard, from_sq);
				}
			}

			if (piece == k) {
				if (castle & bk) {
					if (!GET_BIT(occupied[both], F8) && !GET_BIT(occupied[both], G8)) {
						if (!is_square_attacked(E8, white) && !is_square_attacked(F8, white)) {
							add_move(move_list, ENCODE_MOVE(E8, G8, piece, 0, 0, 0, 0, 1));
						}
					}
				}

				if (castle & bq) {
					if (!GET_BIT(occupied[both], D8) && !GET_BIT(occupied[both], C8) &&
						!GET_BIT(occupied[both], B8)) {
						if (!is_square_attacked(E8, white) && !is_square_attacked(D8, white)) {
							add_move(move_list, ENCODE_MOVE(E8, C8, piece, 0, 0, 0, 0, 1));
						}
					}
				}
			}
		}

		if ((color == white) ? piece == N : piece == n) {
			while (bitboard) {
				from_sq = lsb(bitboard);
				attacks = knight_attacks[from_sq] & ((color == white) ? ~occupied[white] : ~occupied[black]);
				while (attacks) {
					to_sq = lsb(attacks);
					if (!GET_BIT(((color == white) ? occupied[black] : occupied[white]), to_sq)) {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 0, 0, 0, 0));
					}
					else {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 1, 0, 0, 0));
					}
					SET_BIT_OFF(attacks, to_sq);
				}
				SET_BIT_OFF(bitboard, from_sq);
			}
		}

		if ((color == white) ? piece == B : piece == b) {

			while (bitboard) {
				from_sq = lsb(bitboard);
				attacks = get_bishop_attacks(from_sq, occupied[both]) &
					((color == white) ? ~occupied[white] : ~occupied[black]);

				while (attacks) {
					to_sq = lsb(attacks);

					if (!GET_BIT(((color == white) ? occupied[black] : occupied[white]), to_sq)) {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 0, 0, 0, 0));
					}
					else {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 1, 0, 0, 0));
					}
					SET_BIT_OFF(attacks, to_sq);
				}

				SET_BIT_OFF(bitboard, from_sq);
			}
		}
		if ((color == white) ? piece == R : piece == r) {

			while (bitboard) {
				from_sq = lsb(bitboard);

				attacks = get_rook_attacks(from_sq, occupied[both]) &
					((color == white) ? ~occupied[white] : ~occupied[black]);

				while (attacks) {
					to_sq = lsb(attacks);

					if (!GET_BIT(((color == white) ? occupied[black] : occupied[white]), to_sq)) {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 0, 0, 0, 0));
					}
					else {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 1, 0, 0, 0));
					}
					SET_BIT_OFF(attacks, to_sq);
				}
				SET_BIT_OFF(bitboard, from_sq);
			}
		}

		if ((color == white) ? piece == Q : piece == q) {
			while (bitboard) {
				from_sq = lsb(bitboard);

				attacks = get_queen_attacks(from_sq, occupied[both]) &
					((color == white) ? ~occupied[white] : ~occupied[black]);

				while (attacks) {
					to_sq = lsb(attacks);

					if (!GET_BIT(((color == white) ? occupied[black] : occupied[white]), to_sq)) {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 0, 0, 0, 0));
					}
					else {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 1, 0, 0, 0));
					}
					SET_BIT_OFF(attacks, to_sq);
				}
				SET_BIT_OFF(bitboard, from_sq);
			}
		}

		if ((color == white) ? piece == K : piece == k) {
			while (bitboard) {
				from_sq = lsb(bitboard);

				attacks = king_attacks[from_sq] & ((color == white) ? ~occupied[white] : ~occupied[black]);
				while (attacks) {
					to_sq = lsb(attacks);
					if (!GET_BIT(((color == white) ? occupied[black] : occupied[white]), to_sq)) {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 0, 0, 0, 0));
					}
					else {
						add_move(move_list, ENCODE_MOVE(from_sq, to_sq, piece, 0, 1, 0, 0, 0));
					}

					SET_BIT_OFF(attacks, to_sq);
				}
				SET_BIT_OFF(bitboard, from_sq);
			}
		}
	}
}

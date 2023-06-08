#include "search.h"

#include "attack.h"
#include "board.h"
#include "hash.h"
#include "zobrist.h"
#include "chrono.h"
#include "eval.h"
#include "global.h"
#include "movegen.h"

static int score_move(const int move)
{
	if (scorePV)
	{
		if (pvTable[0][ply] == move)
		{
			scorePV = 0;
			return 20000;
		}
	}

	if (GET_MOVE_CAPTURE(move))
	{
		int target_piece = P;

		int start_piece, end_piece;

		if (color == white)
		{
			start_piece = p;
			end_piece = k;
		}
		else
		{
			start_piece = P;
			end_piece = K;
		}

		for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
		{
			if (GET_BIT(bitboards[bb_piece], GET_MOVE_TARGET(move)))
			{
				target_piece = bb_piece;
				break;
			}
		}

		return MVV_LVA[GET_MOVE_PIECE(move)][target_piece] + 10000;
	}
	if (killerMoves[0][ply] == move)
	{
		return 9000;
	}
	if (killerMoves[1][ply] == move)
	{
		return 8000;
	}
	return historyMoves[GET_MOVE_PIECE(move)][GET_MOVE_TARGET(move)];
}

static int sort_moves(moves* move_list, const int best_move)
{
	int move_scores[1024]{};

	for (int i = 0; i < move_list->count; i++)
	{
		if (best_move == move_list->moves[i])
		{
			move_scores[i] = 30000;
		}
		else
		{
			move_scores[i] = score_move(move_list->moves[i]);
		}
	}

	for (int current_move = 0; current_move < move_list->count; current_move++)
	{
		for (int next_move = current_move + 1; next_move < move_list->count; next_move++)
		{
			if (move_scores[current_move] < move_scores[next_move])
			{
				const int tempScore = move_scores[current_move];
				move_scores[current_move] = move_scores[next_move];
				move_scores[next_move] = tempScore;

				const int tempMove = move_list->moves[current_move];
				move_list->moves[current_move] = move_list->moves[next_move];
				move_list->moves[next_move] = tempMove;
			}
		}
	}
	return 1;
}

static int q_search(int alpha, const int beta)
{
	if ((nodes & 2047) == 0)
	{
		communicate();
	}

	nodes++;

	if (ply > MAX_PLY - 1)
	{
		return evaluate();
	}

	const int evaluation = evaluate();

	if (evaluation >= beta)
	{
		return beta;
	}

	if (evaluation > alpha)
	{
		alpha = evaluation;
	}

	moves moveList[1];
	generate_moves(moveList);
	sort_moves(moveList, 0);

	for (int count = 0; count < moveList->count; count++)
	{
		COPY_BOARD();

		ply++;

		repetition_index++;
		repetition_table[repetition_index] = hashKey;

		if (make_move(moveList->moves[count], captures_only) == 0)
		{
			ply--;
			repetition_index--;
			continue;
		}

		const int score = -q_search(-beta, -alpha);
		ply--;

		repetition_index--;

		RESTORE_BOARD();

		if (is_stopped == 1)
		{
			return 0;
		}

		if (score > alpha)
		{
			alpha = score;

			if (score >= beta)
			{
				return beta;
			}
		}
	}

	return alpha;
}

static void enable_pv_scoring(const moves* move_list)
{
	followPV = 0;
	for (int count = 0; count < move_list->count; count++)
	{
		if (pvTable[0][ply] == move_list->moves[count])
		{
			scorePV = 1;
			followPV = 1;
		}
	}
}

static int negamax(int alpha, const int beta, int depth)
{
	pvLength[ply] = ply;

	int score;
	int best_move = 0;
	int hash_flag = alpha;

	if ((ply && is_repetition()) || fifty_move_counter >= 100)
	{
		return 0;
	}

	const int pv_node = beta - alpha > 1;

	if (ply && (score = probe_hash(alpha, beta, &best_move, depth)) != no_hash_found && pv_node == 0)
	{
		return score;
	}

	if ((nodes & 2047) == 0)
	{
		communicate();
	}

	if (depth == 0)
	{
		return q_search(alpha, beta);
	}

	if (ply > MAX_PLY - 1)
	{
		return evaluate();
	}

	nodes++;

	const int is_in_check = is_square_attacked((color == white) ? lsb(bitboards[K]) : lsb(bitboards[k]), color ^ 1);

	if (is_in_check)
	{
		depth++;
	}

	int legal_moves = 0;

	const int static_eval = evaluate();
	if (depth < 3 && !pv_node && !is_in_check && abs(beta - 1) > -INFIN + 100)
	{
		if (const int eval_margin = 120 * depth; static_eval - eval_margin >= beta)
		{
			return static_eval - eval_margin;
		}
	}

	if (depth >= 3 && is_in_check == 0 && ply)
	{
		COPY_BOARD()
		ply++;

		repetition_index++;
		repetition_table[repetition_index] = hashKey;

		if (enpassant != NO_SQUARE)
			hashKey ^= enpassant_keys[enpassant];
		enpassant = NO_SQUARE;

		color ^= 1;
		hashKey ^= side_key;

		score = -negamax(-beta, -beta + 1, depth - 1 - 2);

		ply--;
		repetition_index--;
		RESTORE_BOARD()

		if (is_stopped == 1)
		{
			return 0;
		}

		if (score >= beta)
		{
			return beta;
		}
	}

	if (!pv_node && !is_in_check && depth <= 3)
	{
		score = evaluate() + 125;

		if (score < beta)
		{
			int new_score;
			if (depth == 1)
			{
				new_score = q_search(alpha, beta);

				return (new_score > score) ? new_score : score;
			}

			score += 175;

			if (score < beta && depth <= 2)
			{
				new_score = q_search(alpha, beta);

				if (new_score < beta)
					return (new_score > score) ? new_score : score;
			}
		}
	}

	moves move_list[1];
	generate_moves(move_list);

	if (followPV)
	{
		enable_pv_scoring(move_list);
	}

	sort_moves(move_list, best_move);
	int moves_searched = 0;

	for (int count = 0; count < move_list->count; count++)
	{
		COPY_BOARD()
		ply++;

		repetition_index++;
		repetition_table[repetition_index] = hashKey;

		if (make_move(move_list->moves[count], all_moves) == 0)
		{
			ply--;
			repetition_index--;
			continue;
		}

		legal_moves++;

		if (moves_searched == 0)
		{
			score = -negamax(-beta, -alpha, depth - 1);
		}
		else
		{
			if (
				moves_searched >= fullDepthMoves &&
				depth >= reductionLimit &&
				is_in_check == 0 &&
				GET_MOVE_CAPTURE(move_list->moves[count]) == 0 &&
				GET_MOVE_PROMOTED(move_list->moves[count]) == 0)
				score = -negamax(-alpha - 1, -alpha, depth - 2);

			else
				score = alpha + 1;

			if (score > alpha)
			{
				score = -negamax(-alpha - 1, -alpha, depth - 1);

				if ((score > alpha) && (score < beta))
				{
					score = -negamax(-beta, -alpha, depth - 1);
				}
			}
		}

		ply--;
		repetition_index--;
		RESTORE_BOARD()

		if (is_stopped == 1)
		{
			return 0;
		}

		moves_searched++;

		if (score > alpha)
		{
			hash_flag = pv;

			best_move = move_list->moves[count];

			if (GET_MOVE_CAPTURE(move_list->moves[count]) == 0)
			{
				historyMoves[GET_MOVE_PIECE(move_list->moves[count])][GET_MOVE_TARGET(
					move_list->moves[count])] += depth;
			}

			alpha = score;
			pvTable[ply][ply] = move_list->moves[count];

			for (int next_ply = ply + 1; next_ply < pvLength[ply + 1]; next_ply++)
			{
				pvTable[ply][next_ply] = pvTable[ply + 1][next_ply];
			}
			pvLength[ply] = pvLength[ply + 1];

			if (score >= beta)
			{
				write_hash(beta, best_move, depth, beta);

				if (GET_MOVE_CAPTURE(move_list->moves[count]) == 0)
				{
					killerMoves[1][ply] = killerMoves[0][ply];
					killerMoves[0][ply] = move_list->moves[count];
				}

				return beta;
			}
		}
	}

	if (legal_moves == 0)
	{
		if (is_in_check)
		{
			return -MATE_VALUE + ply;
		}
		return 0;
	}

	write_hash(alpha, best_move, depth, hash_flag);
	return alpha;
}

void search_position(const int depth)
{
	const long start = get_time_ms();
	nodes = 0;
	is_stopped = 0;
	followPV = 0;
	scorePV = 0;

	memset(killerMoves, 0, sizeof(killerMoves));
	memset(historyMoves, 0, sizeof(historyMoves));
	memset(pvTable, 0, sizeof(pvTable));
	memset(pvLength, 0, sizeof(pvLength));

	int alpha = -INFIN;
	int beta = INFIN;

	for (int currentDepth = 1; currentDepth <= depth; currentDepth++)
	{
		if (is_stopped == 1)
		{
			break;
		}

		followPV = 1;
		const int score = negamax(alpha, beta, currentDepth);

		if ((score <= alpha) || (score >= beta))
		{
			alpha = -INFIN;
			beta = INFIN;
			continue;
		}

		alpha = score - 50;
		beta = score + 50;

		if (pvLength[0] && !is_stopped)
		{
			if (score > -MATE_VALUE && score < -MATE_SCORE)
			{
				printf("info score mate %d depth %d nodes %lld time %ld pv ", -(score + MATE_VALUE) / 2 - 1,
					currentDepth, nodes,
					(get_time_ms() - start));
			}
			else if (score > MATE_SCORE && score < MATE_VALUE)
			{
				printf("info score mate %d depth %d nodes %lld time %ld pv ", (MATE_VALUE - score) / 2 + 1,
					currentDepth, nodes,
					(get_time_ms() - start));
			}
			else
			{
				printf("info score cp %d depth %d nodes %lld time %ld pv ", score, currentDepth, nodes,
					(get_time_ms() - start));
			}

			for (int count = 0; count < pvLength[0]; count++)
			{
				print_move(pvTable[0][count]);
				printf(" ");
			}
			printf("\n");
		}
	}

	printf("bestmove ");
	if (pvTable[0][0])
	{
		print_move(pvTable[0][0]);
	}
	else
	{
		printf("(none)");
	}

	printf("\n");
}

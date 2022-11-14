#include "perft.h"

#include <cstdint>

#include "board.h"
#include "chrono.h"
#include "movegen.h"
#include "search.h"

static void perftDriver(const int depth)
{
	if (depth == 0)
	{
		nodes++;
		return;
	}

	moves moveList[1];
	generate_moves(moveList);

	for (int move_count = 0; move_count < moveList->count; move_count++)
	{
		COPY_BOARD()

		if (!make_move(moveList->moves[move_count], all_moves))
		{
			continue;
		}

		perftDriver(depth - 1);

		RESTORE_BOARD()
	}
}

void perft_test(const int depth)
{
	moves move_list[1];
	generate_moves(move_list);
	const long start = get_time_ms();

	nodes = 0;
	for (int move_count = 0; move_count < move_list->count; move_count++)
	{
		COPY_BOARD()

		if (!make_move(move_list->moves[move_count], all_moves))
		{
			continue;
		}

		const uint64_t cummulative_nodes = nodes;
		perftDriver(depth - 1);
		const long old_nodes = static_cast<long>(nodes - cummulative_nodes);

		RESTORE_BOARD()

		printf(" move: %s%s%c nodes: %ld\n", square_to_coordinates[GET_MOVE_SOURCE(move_list->moves[move_count])],
		       square_to_coordinates[GET_MOVE_TARGET(move_list->moves[move_count])],
		       GET_MOVE_PROMOTED(move_list->moves[move_count])
			       ? get_promoted_piece(
				       GET_MOVE_PROMOTED(move_list->moves[move_count]))
			       : ' ',
		       old_nodes);
	}
	const uint64_t nps = nodes / (get_time_ms() - (start));
	printf("\n");
	printf(" depth: %d\n", depth);
	printf(" nodes: %lld\n", nodes);
	printf(" time : %ld\n", get_time_ms() - start);
	printf(" nps  : %lld\n\n", nps * 1000);
	nodes = 0;
}

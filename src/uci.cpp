#include "uci.h"

#include "board.h"
#include "chrono.h"
#include "global.h"
#include "hash.h"
#include "movegen.h"
#include "perft.h"
#include "pragma.h"
#include "search.h"

int parse_move(const char* move_as_string)
{
	moves move_list[1];
	generate_moves(move_list);

	const int source_square = (move_as_string[0] - 'a') + (8 - (move_as_string[1] - '0')) * 8;
	const int target_square = (move_as_string[2] - 'a') + (8 - (move_as_string[3] - '0')) * 8;

	for (int i = 0; i < move_list->count; i++)
	{
		if (const int move = move_list->moves[i]; source_square == GET_MOVE_SOURCE(move) && target_square == GET_MOVE_TARGET(move))
		{
			if (const int promoted_piece = GET_MOVE_PROMOTED(move))
			{
				if ((promoted_piece == Q || promoted_piece == q) && move_as_string[4] == 'q')
				{
					return move;
				}
				if ((promoted_piece == R || promoted_piece == r) && move_as_string[4] == 'r')
				{
					return move;
				}
				if ((promoted_piece == B || promoted_piece == b) && move_as_string[4] == 'b')
				{
					return move;
				}
				if ((promoted_piece == N || promoted_piece == n) && move_as_string[4] == 'n')
				{
					return move;
				}

				continue;
			}

			return move;
		}
	}

	return 0;
}

void parse_position(char* command)
{
	command += 9;

	char* current_char;

	if (strncmp(command, "startpos", 8) == 0)
	{
		parse_fen(START_POSITION);
	}

	else
	{
		current_char = strstr(command, "fen");

		if (current_char == nullptr)
			parse_fen(START_POSITION);
		else
		{
			current_char += 4;

			parse_fen(current_char);
		}
	}

	current_char = strstr(command, "moves");

	if (current_char != nullptr)
	{
		current_char += 6;

		while (*current_char)
		{
			const int move = parse_move(current_char);

			if (move == 0)
			{
				break;
			}

			repetition_index++;
			repetition_table[repetition_index] = hashKey;

			make_move(move, all_moves);

			while (*current_char && *current_char != ' ')
			{
				current_char++;
			}
			current_char++;
		}
	}
}

void parse_go(char* command)
{
	reset_time_control();

	int depth = -1;
	const char* argument;

	if ((argument = strstr(command, "infinite")))
	{
	}

	if ((argument = strstr(command, "binc")) && color == black)
	{
		increment = atoi(argument + 5);
	}
	if ((argument = strstr(command, "winc")) && color == white)
	{
		increment = atoi(argument + 5);
	}

	if ((argument = strstr(command, "wtime")) && color == white)
	{
		base_time = atoi(argument + 6);
	}

	if ((argument = strstr(command, "btime")) && color == black)
	{
		base_time = atoi(argument + 6);
	}

	if ((argument = strstr(command, "movestogo")))
	{
		moves_to_go = atoi(argument + 10);
	}

	if ((argument = strstr(command, "movetime")))
	{
		move_time = atoi(argument + 9);
	}

	if ((argument = strstr(command, "depth")))
	{
		depth = atoi(argument + 6);
	}

	if (move_time != -1)
	{
		base_time = move_time;
		moves_to_go = 1;
	}

	start_time = get_time_ms();

	if (base_time != -1)
	{
		is_time_set = 1;

		base_time /= moves_to_go;
		base_time -= 450;

		if (base_time < 0)
		{
			base_time = 0;
			increment -= 450;
			if (increment < 0)
			{
				increment = 1;
			}
		}

		stop_time = start_time + base_time + increment;
	}

	if (depth == -1)
	{
		depth = 64;
	}

	search_position(depth);
}

void print_info(const int with_hash)
{
	printf("id name %s %s %s\n", engine, version, platform);
	printf("id author %s\n", author);
	printf("option name Hash type spin default %d min %d max %d\n", default_hash_size, min_hash_size, max_hash_size);

	printf("uciok\n");
}

void uci_loop()
{
	int mb = default_hash_size;

	setbuf(stdin, nullptr);
	setbuf(stdout, nullptr);

	parse_fen(START_POSITION);

	while (true)
	{
		char input[2000] = {};
		fflush(stdout);

		if (!fgets(input, 2000, stdin) || (input[0] == '\n'))
		{
			continue;
		}

		if (strncmp(input, "isready", 7) == 0)
		{
			printf("readyok\n");
			continue;
		}

		if (strncmp(input, "position", 8) == 0)
		{
			parse_position(input);
			clear_hash();
		}
		else if (strncmp(input, "ucinewgame", 10) == 0)
		{
			parse_position("position startpos");
			clear_hash();
		}
		else if (strncmp(input, "go", 2) == 0)
		{
			parse_go(input);
		}
		else if (strncmp(input, "quit", 4) == 0)
		{
			break;
		}
		else if (strncmp(input, "uci", 3) == 0)
		{
			print_info(0);
		}
		else if (!strncmp(input, "setoption name Hash value ", 26))
		{
			sscanf(input, "%*s %*s %*s %*s %d", &mb);

			if (mb < min_hash_size)
			{
				mb = min_hash_size;
			}
			if (mb > max_hash_size)
			{
				mb = max_hash_size;
			}
			printf(" Set hash table size to %dMB\n", mb);
			init_hash_table(mb);
		}
		else if (strncmp(input, "d", 1) == 0)
		{
			print_board();
		}
		else if (strncmp(input, "perft", 5) == 0)
		{
			perft_test(6);
		}
	}
}

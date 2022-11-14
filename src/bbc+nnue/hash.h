#pragma once
#include <cstdint>

constexpr auto no_hash_found = 100000;
constexpr auto pv = 0;
constexpr auto lower_bound = 1;
constexpr auto upper_bound = 2;
static int hash_entries = 0;

using hash_entry = struct
{
	uint64_t hash_key;
	int depth;
	int flag;
	int score;
	int best_move;
};

static hash_entry* hash_table = nullptr;
void init_hash_table(int mb);
int probe_hash(int alpha, int beta, int* best_move, int depth);
void write_hash(int score, int best_move, int depth, int hash_flag);
void clear_hash();

#include <cstdio>
#include <cstdlib>

#include "hash.h"
#include "board.h"
#include "pragma.h"
#include "search.h"

void init_hash_table(const int mb) {
	const int hash_size = 0x100000 * mb;

    hash_entries = hash_size / sizeof(hash_entry);

    if (hash_table != nullptr) {
        free(hash_table);
    }

    hash_table = static_cast<hash_entry*>(malloc(hash_entries * sizeof(hash_entry)));

    if (hash_table == nullptr) {
        printf("    Couldn't allocate memory for hash table, trying %dMB...", mb / 2);
        init_hash_table(mb / 2);
    }
}

int probe_hash(const int alpha, const int beta, int* best_move, const int depth) {
	if (const hash_entry* hash_entry = &hash_table[hashKey % hash_entries]; hash_entry->hash_key == hashKey) {
        if (hash_entry->depth >= depth) {
            int score = hash_entry->score;

            if (score < -MATE_SCORE) {
                score += ply;
            }

            if (score > MATE_SCORE) {
                score -= ply;
            }

            if (hash_entry->flag == pv) {
                return score;
            }

            if ((hash_entry->flag == alpha) && (score <= alpha)) {
                return alpha;
            }

            if ((hash_entry->flag == beta) && (score >= beta)) {
                return beta;
            }

        }
        *best_move = hash_entry->best_move;
    }

    return no_hash_found;
}

void write_hash(int score, const int best_move, const int depth, const int hash_flag) {
    hash_entry* hash_entry = &hash_table[hashKey % hash_entries];

    if (score < -MATE_SCORE) {
        score -= ply;
    }

    if (score > MATE_SCORE) {
        score += ply;
    }

    hash_entry->hash_key = hashKey;
    hash_entry->score = score;
    hash_entry->flag = hash_flag;
    hash_entry->depth = depth;
    hash_entry->best_move = best_move;
}

void clear_hash() {
    for (hash_entry* hash_entry = hash_table; hash_entry < hash_table + hash_entries; hash_entry++) {
        hash_entry->hash_key = 0;
        hash_entry->depth = 0;
        hash_entry->flag = 0;
        hash_entry->score = 0;
    }
}
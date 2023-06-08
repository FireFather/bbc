#pragma once

#include "random.h"

extern uint64_t piece_keys[12][64];
extern uint64_t enpassant_keys[64];
extern uint64_t castle_keys[16];
extern uint64_t side_key;

void init_hash_keys();
uint64_t generate_hash_key();

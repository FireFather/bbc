#include "attack.h"
#include "eval.h"
#include "global.h"
#include "hash.h"
#include "uci.h"
#include "zobrist.h"

int main()
{
	show_version();
	init_jump_attacks();
	init_slider_attacks(bishop);
	init_slider_attacks(rook);
	init_hash_keys();
	init_hash_table(default_hash_size);
	init_nnue(NNUE_FILE);
	uci_loop();
	free(hash_table);
	return 0;
}

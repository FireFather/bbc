#pragma once

#include "bit.h"

enum {
	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1,

	NO_SQUARE
};

static const char* square_to_coordinates[] = {
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

static char ascii_pieces[12] = { 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k' };

enum Piece {
	P,
	N,
	B,
	R,
	Q,
	K,
	p,
	n,
	b,
	r,
	q,
	k
};

enum {
	all_moves,
	captures_only
};

enum castling_rights {
	wk = 1,
	wq = 2,
	bk = 4,
	bq = 8
};

typedef struct {
	int moves[256];
	int count;
} moves;

#define COPY_BOARD()                                                    \
    uint64_t bitboardsCopy[12], occupanciesCopy[3];                        \
    int sideCopy, enpassantCopy, castleCopy,fiftyRuleCounterCopy;     \
    memcpy(bitboardsCopy, bitboards, 96);                              \
    memcpy(occupanciesCopy, occupied, 24);                          \
    sideCopy = color, enpassantCopy = enpassant, castleCopy = castle;   \
    fiftyRuleCounterCopy = fifty_move_counter;                            \
    uint64_t hashKeyCopy = hashKey;

#define RESTORE_BOARD()                                                     \
    memcpy(bitboards, bitboardsCopy, 96);                              \
    memcpy(occupied, occupanciesCopy, 24);                              \
    color = sideCopy, enpassant = enpassantCopy, castle = castleCopy;       \
    fifty_move_counter= fiftyRuleCounterCopy;                               \
    hashKey = hashKeyCopy;

#define ENCODE_MOVE(source, target, piece, promoted, capture, doublePush, enpassant, castling) \
    ((source) |                                                                             \
        ((target) << 6) |                                                                    \
        ((piece) << 12) |                                                                    \
        ((promoted) << 16) |                                                                 \
        ((capture) << 20) |                                                                  \
        ((doublePush) << 21) |                                                               \
        ((enpassant) << 22) |                                                                \
        ((castling) << 23))

#define GET_MOVE_SOURCE(move) ((move) & 0x3f)
#define GET_MOVE_TARGET(move) (((move) & 0xfc0) >> 6)
#define GET_MOVE_PIECE(move) (((move) & 0xf000) >> 12)
#define GET_MOVE_PROMOTED(move) (((move) & 0xf0000) >> 16)
#define GET_MOVE_CAPTURE(move) ((move) & 0x100000)
#define GET_MOVE_DOUBLE(move) ((move) & 0x200000)
#define GET_MOVE_ENPASSANT(move) ((move) & 0x400000)
#define GET_MOVE_CASTLING(move) ((move) & 0x800000)

extern uint64_t bitboards[12];
extern uint64_t occupied[3];
extern int color;
extern int enpassant;
extern int castle;
extern uint64_t hashKey;
extern uint64_t repetition_table[1000];
extern int repetition_index;
extern const int castle_rights[64];
extern int ply;
extern int fifty_move_counter;

void print_move(int move);
int make_move(int move, int move_flag);
char get_promoted_piece(int piece);
void print_board();
void reset_board();
void parse_fen(char* fen);
int is_repetition();
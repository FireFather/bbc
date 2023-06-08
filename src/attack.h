#pragma once

#include "bit.h"

constexpr uint64_t not_a_file = 18374403900871474942ULL;
constexpr uint64_t not_h_file = 9187201950435737471ULL;
constexpr uint64_t not_hg_file = 4557430888798830399ULL;
constexpr uint64_t not_ab_file = 18229723555195321596ULL;

extern const int bishop_relevant_bits[64];
extern const int rook_relevant_bits[64];
extern uint64_t rook_magic_numbers[64];
extern uint64_t bishop_magic_numbers[64];
extern uint64_t pawn_attacks[2][64];
extern uint64_t knight_attacks[64];
extern uint64_t king_attacks[64];
extern uint64_t bishop_masks[64];
extern uint64_t rook_masks[64];
extern uint64_t bishop_attacks[64][512];
extern uint64_t rook_attacks[64][4096];
extern uint64_t mask_pawn_attacks(int side, int square);
extern uint64_t mask_knight_attacks(int square);
extern uint64_t mask_king_attacks(int square);

uint64_t mask_bishop_attacks(int square);
uint64_t mask_rook_attacks(int square);
void init_jump_attacks();

uint64_t bishop_attacks_on_the_fly(int square, uint64_t block);
uint64_t rook_attacks_on_the_fly(int square, uint64_t block);

uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask);
void init_slider_attacks(int bishop);
int is_square_attacked(int square, int side);

uint64_t get_bishop_attacks(int square, uint64_t occupancy);
uint64_t get_rook_attacks(int square, uint64_t occupancy);
uint64_t get_queen_attacks(int square, unsigned long long occupancy);

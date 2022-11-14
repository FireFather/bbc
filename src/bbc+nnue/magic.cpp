#include "magic.h"
#include <cstring>

#include "attack.h"
#include "global.h"

uint64_t find_magic_number(const int square, const int relevant_bits, const int bishop) {
    uint64_t occupied[4096];
    uint64_t attacks[4096];
    uint64_t used_attacks[4096];
    const uint64_t attackMask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    const int occupancy_indicies = 1 << relevant_bits;

    for (int index = 0; index < occupancy_indicies; index++) {
        occupied[index] = set_occupancy(index, relevant_bits, attackMask);
        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupied[index]) : rook_attacks_on_the_fly(
                square, occupied[index]);
    }

    for (int randomCount = 0; randomCount < 100000000; randomCount++) {
        const uint64_t magicNumber = generate_magic_number();

        if (popcount((attackMask * magicNumber) & 0xFF00000000000000) < 6) {
            continue;
        }

        memset(used_attacks, 0ULL, sizeof(used_attacks));

        int index, fail;

        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++) {
	        if (const int magic_index = static_cast<int>((occupied[index] * magicNumber) >> (64 - relevant_bits)); used_attacks[magic_index] == 0ULL) {
                used_attacks[magic_index] = attacks[index];
            } else if (used_attacks[magic_index] != attacks[index]) {
                fail = 1;
            }

        }

        if (!fail) {
            return magicNumber;
        }

    }

    return 0ULL;
}

void init_magic_numbers() {
    for (int square = 0; square < 64; square++) {
        rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], rook);
    }
    for (int square = 0; square < 64; square++) {
        bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
    }

}

uint64_t generate_magic_number() {
    return get_random_u64_number() & get_random_u64_number() & get_random_u64_number();
}

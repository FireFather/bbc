
#include "random.h"

unsigned int get_random_u32_number() {

    unsigned int number = random_state;

    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    random_state = number;
    return number;
}

uint64_t get_random_u64_number() {
	const uint64_t n1 = static_cast<uint64_t>(get_random_u32_number()) & 0xFFFF;
	const uint64_t n2 = static_cast<uint64_t>(get_random_u32_number()) & 0xFFFF;
	const uint64_t n3 = static_cast<uint64_t>(get_random_u32_number()) & 0xFFFF;
	const uint64_t n4 = static_cast<uint64_t>(get_random_u32_number()) & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}


#pragma once
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN64
constexpr auto platform = "x64";
#else
constexpr auto platform = "w32";
#endif

constexpr auto engine = "BBC";
constexpr auto author = "Code Monkey King";
constexpr auto version = "1.5 + NNUE";

inline void show_version()
{
	std::stringstream ei;
	ei << engine << " ";
	ei << version << " ";
	ei << platform << std::endl;
	std::cout << ei.str();
}

constexpr auto max_hash_size = 16384;
constexpr auto min_hash_size = 4;
constexpr auto default_hash_size = 64;

#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

int parse_move(const char* move_as_string);
void parse_position(char* command);
void parse_go(char* command);
void uci_loop();

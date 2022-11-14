#include "eval.h"
#include "nnue.h"

void init_nnue(const char* file_name) {
    nnue_init(file_name);
}

int evaluate_nnue(const int player, int* pieces, int* squares) {
    return nnue_evaluate(player, pieces, squares);
}

int evaluate_nnue() {

    int pieces[33]{};
    int squares[33]{};
    int index = 2;

    for (int piece_type = P; piece_type <= k; piece_type++) {
        uint64_t bitboard = bitboards[piece_type];
        while (bitboard) {
            const int piece = piece_type;
            const int square = lsb(bitboard);

            if (piece == K) {
                pieces[white_king] = nnue_pieces[piece];
                squares[white_king] = board_square[square];
            }
            else if (piece == k) {
                pieces[black_king] = nnue_pieces[piece];
                squares[black_king] = board_square[square];
            }
            else {
                pieces[index] = nnue_pieces[piece];
                squares[index] = board_square[square];
                index++;
            }
            SET_BIT_OFF(bitboard, square);
        }
    }

    pieces[index] = 0;
    squares[index] = 0;

    return (evaluate_nnue(color, pieces, squares) * (100 - fifty_move_counter) / 100);
}

int evaluate()
{
	return evaluate_nnue();
}
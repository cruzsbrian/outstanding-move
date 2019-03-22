#include "negamax.h"

#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include "../board/board.h"
#include "../eval/simple_eval.h"


/**
 * Negamax:
 * Finds best move through minimax search. If no move is found (in other words
 * pass this round), then -1 is returned.
 */
move_score_t negamax(board_t *board, int c, int depth) {
    uint64_t moves, old_b, old_w;
    int best_move, move;
    float max_score, score;
    move_score_t result;

    /* If depth reached, evaluate and return. */
    if (depth == 0) {
        result.pos = -1;
        result.score = simple_eval(board, c);
        return result;
    }

    moves = get_moves(board, c);

    /* If game over (neither player can move), evaluate and return. */
    if (moves == 0 && get_moves(board, !c) == 0) {
        result.pos = -1;
        result.score = simple_eval(board, c);
        return result;
    }

    max_score = FLT_MIN;
    best_move = -1;

    while (moves) {
        /* Get index of least significant bit. */
        move = __builtin_ctzll(moves);

        /* Zero out least significant bit. */
        moves &= moves - 1;

        /* Store old board for undoing move. */
        old_b = board->b;
        old_w = board->w;

        /* Make move and get its score. */
        do_move(board, 1L << move, c);
        score = -negamax(board, !c, depth - 1).score;

        /* Undo move. */
        board->b = old_b;
        board->w = old_w;

        if (score > max_score) {
            best_move = move;
            max_score = score;
        }
    }

    result.pos = best_move;
    result.score = -max_score;

    return result;
}

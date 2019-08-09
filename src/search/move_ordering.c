#include "move_ordering.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include "../board/board.h"
#include "../eval/table_eval.h"
#include "alphabeta.h"
#include "trans_table.h"


int compare_moves(const void *m1, const void *m2);


/**
 * Get scored moves:
 * Gets all the moves available to color c for board b, puts them in an array,
 * and sorts them. The resulting array is at *move_arr. The number of moves is
 * put into *n. *move_arr must be freed later.
 */
void get_scored_moves(move_score_t **move_arr, size_t *n, board_t *board, int c, node_t *tt, long *n_nodes) {
    int ii, move;
    board_t old;
    uint64_t moves_mask = get_moves(board, c);

    /* Get count of moves. */
    *n = popcount(moves_mask);

    /* Allocate space for array of moves. */
    *move_arr = malloc(*n * sizeof(move_score_t));
    if (*move_arr == NULL) {
        fprintf(stderr, "Error: unable to allocate memory for move array.");
        exit(1);
    }

    /* Loop through all moves. */
    for (ii = 0; ii < *n; ++ii) {
        int found;
        char prev_depth, type;
        int16_t score;

        /*
         * Find next move by counting trailing zeros in the mask, then zero out
         * the LSB to remove it from the mask.
         */
        move = __builtin_ctzll(moves_mask);
        moves_mask &= moves_mask - 1;

        /* Add it to the array. */
        (*move_arr)[ii].pos = move;

        /* Store old board. */
        old = *board;

        /* Apply the move, score it, and put that in the array element. */
        do_move(board, move, c);

        /* fprintf(stderr, "\n\tLooking up score for move %d for color %d\n", move, c); */
        found = lookup_score(tt, *board, !c, &score, &prev_depth, &type);
        if (!found) {
            score = alphabeta(board, !c, -INT16_MAX, INT16_MAX, 0, 5, tt, n_nodes, 0).score;
        }
        (*move_arr)[ii].score = -score;

        /* Restore the old board. */
        *board = old;
    }

    /* Finally sort the array using quicksort. */
    qsort(*move_arr, *n, sizeof(move_score_t), compare_moves);
}

/* Comparison function for sorting scored moves. */
int compare_moves(const void *m1, const void *m2) {
    int16_t s1 = ((move_score_t *)m1)->score;
    int16_t s2 = ((move_score_t *)m2)->score;
    return s2 - s1;
}

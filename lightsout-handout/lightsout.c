#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"
#include "board-ht.h"
#include "lib/queue.h"
#include "lib/hdict.h"
#include "lib/boardutil.h"

// Abbreviation for lazyness
typedef struct board_data bd;


void free_value(void* v)
{
    bd *free_me = (bd*) v;
    free(free_me);
    return;
}

bitvector make_move(int row, int col, uint8_t width_out,
                    uint8_t height_out, bitvector B) {

    uint8_t i = get_index(row, col, width_out, height_out);
    bitvector newboard = bitvector_flip(B, i);
    if (row+1 < height_out) {
        i = get_index(row+1, col, width_out, height_out);
        newboard = bitvector_flip(newboard, i);
    }
    if (row-1 >= 0) {
        i = get_index(row-1, col, width_out, height_out);
        newboard = bitvector_flip(newboard, i);
    }
    if (col+1 < width_out) {
        i = get_index(row, col+1, width_out, height_out);
        newboard = bitvector_flip(newboard, i);
    }
    if (col-1 >= 0) {
        i = get_index(row, col-1, width_out, height_out);
        newboard = bitvector_flip(newboard, i);
    }
    return newboard;
}

void get_moves(bitvector solution, uint8_t width_out){
    for (uint8_t i = 0; i < BITVECTOR_LIMIT; i++) {
        bool tmp = bitvector_get(solution, i);
        if (tmp){
            uint8_t row = i / width_out;
            uint8_t col = i % width_out;
            printf("%d:%d\n", row, col);
        }

    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: lightsout <board name>\n");
        return 1;
    }

    //Initialize variables
    char *board_filename = argv[1];
    queue_t Q = queue_new();
    hdict_t H = ht_new((size_t)1000);
    bitvector newboard = bitvector_new();
    uint8_t width_out, height_out = 0;

    //Check if board can be read
    if (!file_read(board_filename, &newboard, &width_out, &height_out)) {
        fprintf(stderr, "Error loading file\n");
        queue_free(Q, NULL);
        hdict_free(H);
        return 1;
    }

    //Enqueue initial board
    bitvector solution = bitvector_new();
    bd *A = xmalloc(sizeof(bd));
    A->board = newboard;
    A->solution = solution;
    ht_insert(H, A);
    enq(Q, (void*) A);

    //Breath First Search
    while (!queue_empty(Q)) {
        bd* M = (bd*) deq(Q);
        newboard = M->board;
        solution = M->solution;
        for (int row = 0; row < height_out; row++) {
            for (int col = 0; col < width_out; col++) {

                //Make move
                newboard = make_move(row, col, width_out,
                                               height_out, newboard);

                solution = bitvector_flip(solution, get_index(row,
                            col, width_out, height_out));

                //if board is solved free all data structures
                if (bitvector_equal(newboard, bitvector_new())) {
                    get_moves(solution, width_out);
                    queue_free(Q, NULL);
                    hdict_free(H);
                    return 0;
                }

                // Else add element to hash table and enq
                if (ht_lookup(H, newboard) == NULL) {
                    bd *N = xmalloc(sizeof(bd));
                    N->board = newboard;
                    N->solution = solution;
                    ht_insert(H, N);
                    enq(Q, (void*) N);
                }
            }
        }
    }
    // Free memory
    queue_free(Q, NULL);
    hdict_free(H);
    fprintf(stderr, "No solution\n");
    return 1;
}

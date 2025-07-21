#include <stdio.h>
#include <string.h>
#include "sudoku.h"

// Functions for loading a sudoku puzzle into a datastructure,
// solving it, and printing the result.

const int MAX_EAGER = 91;

static inline short field_to_short(short);
static inline short short_to_field(short);
static inline int is_popcount_one(short);
static inline int popcount(short);
static inline void get_indices(int location, int *block, int *col, int *row);
static inline void set_value(sudoku *puzzle, short val, int location);
static inline void clear_value(sudoku *puzzle, int location);
static inline void backout_moves(sudoku *puzzle, int eager_count, int eager_moves[MAX_EAGER]);



// Optimized solve using constraint propagation and backtracking
int solve_sudoku(sudoku * const puzzle) {
    // Keep track of the moves we make that might be provisional and need to be backed out.
    int eager_moves[MAX_EAGER];
    int eager_count = 0;

    while (1) {
        int solved_count = 0;
        int lowest_space = -1;
        int lowest_count = 10;
        short lowest_possibles = 0;

        // Check all empty cells
        for (int i = 0; i < 81; i++) {
            if (puzzle->cells[i] != 0) {
                continue; // already solved
            }

            int block, col, row;
            get_indices(i, &block, &col, &row);
            short possibles = puzzle->blocks[block] & puzzle->cols[col] & puzzle->rows[row];
            int count = popcount(possibles);

            if (count == 1) {
                // Only one possibility, set it
                set_value(puzzle, possibles, i);
                solved_count++;
                eager_moves[eager_count] = i;
                eager_count++;
            } else if (count == 0) {
                // No possibilities, invalid state
                backout_moves(puzzle, eager_count, eager_moves);
                return -1;
            } else if (count < lowest_count) {
                lowest_count = count;
                lowest_space = i;
                lowest_possibles = possibles;
            }
        }

        if (solved_count > 0) {
            // Made progress with constraint propagation, continue
            continue;
        }

        if (puzzle->remaining == 0) {
            // Solved!
            return 0;
        }

        // Need to guess - try each possibility for the most constrained cell
        for (int val = 1; val <= 9; val++) {
            short field_val = short_to_field(val);
            if (lowest_possibles & field_val) {
                set_value(puzzle, field_val, lowest_space);
                if (solve_sudoku(puzzle) == 0) {
                    return 0;
                }
                clear_value(puzzle, lowest_space);
            }
        }

        backout_moves(puzzle, eager_count, eager_moves);

        // No guess worked
        return -1;
    }
}


void get_indices(int location, int *block, int *col, int *row) {
    *row = location / 9;
    *col = location % 9;
    *block = (*row / 3) * 3 + (*col / 3);
}

void set_value(sudoku *puzzle, short val, int location) {
    puzzle->cells[location] = val;

    int block, col, row;
    get_indices(location, &block, &col, &row);
    puzzle->blocks[block] &= ~val;
    puzzle->cols[col] &= ~val;
    puzzle->rows[row] &= ~val;
    puzzle->remaining--;
}

void clear_value(sudoku *puzzle, int location) {
    short val = puzzle->cells[location];
    puzzle->cells[location] = 0;

    int block, col, row;
    get_indices(location, &block, &col, &row);
    puzzle->blocks[block] |= val;
    puzzle->cols[col] |= val;
    puzzle->rows[row] |= val;
    puzzle->remaining++;
}

void backout_moves(sudoku *puzzle, int eager_count, int eager_moves[MAX_EAGER]) {
    for (int i = 0; i < eager_count; i++) {
        clear_value(puzzle, eager_moves[i]);
    }
}

void print_sudoku(const sudoku * const puzzle) {
    for (int i = 0; i < 81; i++) {
        int row = i / 9;
        int col = i % 9;

        if(row % 3 == 0 && row != 0 && col == 0) printf("\n");
        if(col % 3 == 0 && col != 0) printf(" ");
        printf("%d ", field_to_short(puzzle->cells[i]));
        if(col == 8) printf("\n");
    }
    printf("\n");
}


int load_sudoku(sudoku * const puzzle, const char * const filename) {
    FILE *fp;
    short value;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Invalid file\n");
        return -1;
    }

    // Initialize all constraint arrays with all possibilities (bits 0-8 set)
    short start_possibilities = 0x1FF;
    for (int i = 0; i < 9; i++) {
        puzzle->blocks[i] = start_possibilities;
        puzzle->cols[i] = start_possibilities;
        puzzle->rows[i] = start_possibilities;
    }

    // Initialize all cells as empty and count
    memset(puzzle->cells, 0, sizeof(puzzle->cells));
    puzzle->remaining = 81;

    for (int i = 0; i < 81; i++) {
        if (fscanf(fp, "%hd", &value)) {
            if (value != 0) {
                set_value(puzzle, short_to_field(value), i);
            }
        } else {
            printf("Invalid input\n");
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}

void copy_sudoku(sudoku * const destination, const sudoku * const source) {
    memcpy(destination, source, sizeof(sudoku));
}

short field_to_short(short input) {
    if (input == 0) return 0;
    if (!is_popcount_one(input)) return 0;
    return __builtin_ctz(input) + 1;
}

short short_to_field(short x) {
    if (x) {
        return 1 << (x-1);
    } else {
        return 0;
    }
}

int is_popcount_one(short x) {
    return popcount(x) == 1;
}

int popcount(short x) {
    return __builtin_popcount(x);
}

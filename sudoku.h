#pragma once

typedef struct {
    short cells[81];    // Final values for each cell (bitfield with single bit set, or 0)
    short blocks[9];    // Remaining possibilities for each 3x3 block
    short cols[9];      // Remaining possibilities for each column
    short rows[9];      // Remaining possibilities for each row
    int remaining;      // Number of unsolved cells
} sudoku;

// Takes a name of a file containing a sudoku represented as SUDOKU_SIZE
// numbers seperated by whitespace, and loads them into the provided sudoku
// structure
int load_sudoku( sudoku*, const char*);

// Copy the state of a sudoku problem
void copy_sudoku(sudoku * destination, const sudoku * source);

// Takes the first sudoku structure passed in and iterates solving it.  It
// returns 0 if a solution is found, and -1 if it can no longer make progress.
int solve_sudoku( sudoku*);


// Takes a sudoku and prints it to the screen
void print_sudoku( const sudoku *);

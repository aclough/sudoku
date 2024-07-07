#pragma once

typedef struct {
    short value;   // A bit field that says what the final value of that space is.
                   //  Only 1 bit should be set!
    short possibles; // Encodes a bit field, where bit X says whether the value
                     // at that location could be X
                     // When the value is know, becomes 0
} element;

typedef struct {
    element space[9][9];
} sudoku;

// Takes a name of a file containing a sudoku represented as SUDOKU_SIZE
// numbers seperated by whitespace, and loads them into the provided sudoku
// structure
int load_sudoku( sudoku*, const char*);

// Takes the first sudoku structure passed in and iterates solving it.  It
// returns 0 if a solution is found, and -1 if it can no longer make progress.
int solve_sudoku( sudoku*);


// Takes a sudoku and prints it to the screen
void print_sudoku( const sudoku*);

#define SUDOKU_SIZE 9   //This is the number of possible different numbers, and
                        // thus both the length and width of the grid, and the
                        // number of items in a box
                        // Must be a sqaure for everything to work out
#define SUDOKU_BOX_SIZE 3 // The square root of the SUDOKU_SIZE

typedef struct {
    int value[SUDOKU_SIZE][SUDOKU_SIZE];   // A bit field that says what the 
                                           // Final value of that space is.
                                           // Only 1 bit should be set!
    int possibles[SUDOKU_SIZE][SUDOKU_SIZE]; // Encodes a bit field, where bit X
                                             // says whether the value at that
                                             // location could be X
} sudoku;


int load_sudoku( sudoku*, char*);
void print_sudoku( sudoku*);

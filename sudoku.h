#define SUDOKU_BOX_LENGTH 3 // The square root of the SUDOKU_SIZE
#define SUDOKU_LENGTH (SUDOKU_BOX_LENGTH*SUDOKU_BOX_LENGTH) 
                        // This is the number of possible different numbers, and
                        // thus both the length and width of the grid, and the
                        // number of items in a box
                        // Must be a sqaure for everything to work out
#define SUDOKU_SIZE (SUDOKU_LENGTH * SUDOKU_LENGTH)
                        // The total number of elements in the SUDOKU


typedef struct {
    int value;   // A bit field that says what the final value of that space is.
                 //  Only 1 bit should be set!
    int possibles; // Encodes a bit field, where bit X says whether the value 
                   // at that  location could be X
} element;

typedef struct{
    element spot[SUDOKU_SIZE];  // All the elements in the SUDOKU
} sudoku;

// These are views of rows, columns, or boxes of a sudoku that
// can be passed to a generic solving function
typedef struct {
    element *spot[SUDOKU_LENGTH];  // A pointer to a element in a sudoku
} block;


int load_sudoku( sudoku*, char*);
int solve_sudoku( sudoku*, sudoku*);
int check_sudoku( sudoku*, sudoku*);
int count_sudoku( sudoku*);
void do_sudoku( block[]);
void do_block( element *[]);
void print_sudoku( sudoku*);
int do_test();

// Takes a bitfield and, if it corresponds to an int, will return that int
// If it corresponds to 0 or more than 1 int, it will return 0
int field_to_int(int);
int int_to_field(int);

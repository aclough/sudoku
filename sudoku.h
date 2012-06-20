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


// Takes a name of a file containing a sudoku represented as SUDOKU_SIZE 
// numbers seperated by whitespace, and loads them into the provided sudoku
// structure
int load_sudoku( sudoku*, char*);

// Takes the first sudoku structure passed in and iterates solving it.  It
// returns 0 if a solution is found, and -1 if it can no longer make progress.
// The second argument is a check against which the solution is measured.  If
// the forming solution deviates from the check the function also returns -1
int solve_sudoku( sudoku*, sudoku*);

// Compares the two solutions.  If they deviate in any solved space it returns
// -1, else it returns 0
int check_sudoku( sudoku*, sudoku*);

// Returns the number of unsolved spaces in a sudoku
int count_sudoku( sudoku*);

// Takes a list of SUDOKU_LENGTH blocks and applies do_block() to each of them
void do_sudoku( block[]);

// Takes a row, colum, or box from a sudoku (a "block") and applies a solving 
// iteration, asking both if there are any spaces that can have only one value,
// and if there are any values that can have only one space.
void do_block( element *[]);

// Takes a sudoku and prints it to the screen
void print_sudoku( sudoku*);

// Does a diagnostic test to ensure that field_to_int() and int_to_field()
// are working properly.
int do_test();

// Takes a bitfield and, if it corresponds to an int, will return that int
// If it corresponds to 0 or more than 1 int, it will return 0
int field_to_int(int);

// Takes an integer and returns a bitfield that represents that integer.
int int_to_field(int);

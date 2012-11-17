#include <stdio.h>
#include "sudoku.h"

/* An implementation of the sudoku solver */

// These are views of rows, columns, or boxes of a sudoku that
// can be passed to a generic solving function
typedef struct {
    element *space[9];  // A pointer to a element in a sudoku
} block;

static short field_to_short(short);
static short short_to_field(short);
static int check_sudoku(sudoku *, sudoku *);
static int check_remaining(sudoku *);
static void solve_block(block *, int);
static int is_popcount_one(short);
static int popcount(unsigned int x);
static void print_field(short x);


// Takes in a sudoku puzzle and optionally it's solution and
// returns the puzzle, solved.
// If the solution is provided, it compares it's work to the solution 
// every round of solving and aborts if a discrepency emerges.
int solve_sudoku(sudoku* puzzle, sudoku* expected){
    int x,y;
    int a,b;
    int remaining, last_remaining;
    int count=0;
    block rows[9], colums[9], squares[9];
    for( x = 0; x < 9; x++){
        for( y = 0; y < 9; y++){
            rows[x].space[y] = &(puzzle->space[x][y]);
            colums[x].space[y] = &(puzzle->space[y][x]);

            a = y/3 + 3*(x/3);
            b = y%3 + 3*(x%3);
            squares[a].space[b] = &(puzzle->space[x][y]);
        }
    }
    // Actual solving
    remaining = check_remaining( puzzle);
    while(remaining){
        count++;
        for( x = 0; x < 9; x++){
            solve_block(&rows[x], x==2);
            solve_block(&colums[x], 0);
            solve_block(&squares[x], 0);
        }

        if(check_sudoku(puzzle, expected)){
            return -1;
        }

        last_remaining = remaining;
        remaining = check_remaining(puzzle);
        if(remaining == last_remaining){
            printf("Failing to make progress on round %d\n", count);
            return -1;
        }
    }
    return 0;
}


void solve_block(block* myblock, int verbose){
    int x;
    short value_screen=0, once_screen=0, multi_screen=0;
    element *myspace;
    // Find the bit fields of the actual values
    for(x = 0; x < 9; x++){
        value_screen |= myblock->space[x]->value;
    }
    for(x = 0; x < 9; x++){
        myspace = myblock->space[x];
        // If something is a value, it can't be a possiblity
        myspace->possibles &= ~value_screen;
        if(is_popcount_one(myspace->possibles) && !myspace->value){
            myspace->value = myspace->possibles;
        }
        // Add those possiblities that have already been spotted in
        // the block to the multi_screen
        multi_screen |= once_screen & myspace->possibles;
        // And every possiblity to the once screen
        once_screen |= myspace->possibles;
    }
    // remove values spotted multiple times from the once_screen
    once_screen &= ~multi_screen;
    // And if we got solutions from that
    if(once_screen){
        for(x = 0; x < 9; x++){
            myspace = myblock->space[x];
            // Set each space's value to those of it's possiblities for which 
            // it's the only one in this block that can be a match for the value
            // If this isn't a solution, it'll just be oring the value with 0
            if(myspace->possibles && once_screen)
            myspace->value |= myspace->possibles & once_screen;
            // I'm assuming here that I only get a single value bit from this.  If not
            // I'll get an erorr message fairly quickly
        }
    }
}

int check_remaining(sudoku *puzzle){
    int x,y;
    int remaining = 9*9;
    for( x = 0; x < 9; x++){
        for( y = 0; y < 9; y++){
            if(puzzle->space[x][y].value){
                remaining--;
            }
        }
    }
    return remaining;
}

int check_sudoku(sudoku *puzzle, sudoku *expected){
    int x, y;
    short value;
    if(NULL == expected){
        return 0;
    }
    for( x = 0; x < 9; x++){
        for( y = 0; y < 9; y++){
            value = puzzle->space[x][y].value;
            if(value){
                if(expected->space[x][y].value != value){
                    printf("Mismatch at %d,%d\n\n", x, y);
                    printf("Observed:\n");
                    print_sudoku(puzzle);
                    printf("\nExpected:\n");
                    print_sudoku(expected);
                    return -1;
                }
            }
        }
    }
    return 0;
}


void print_sudoku(sudoku *puzzle){
    int x,y;
    for( x = 0; x < 9; x++){
        if(0 == x%3 && 0 != x) printf("\n");
        for( y = 0; y < 9; y++){
            if(0 == y%3 && 0 != y) printf(" ");
            printf("%d ", field_to_short(puzzle->space[x][y].value));
        }
        printf("\n");
    }
}


int load_sudoku(sudoku *puzzle, char *filename){
    FILE *fp;
    short value;
    int x,y;
    printf("Loading file: %s\n", filename);
    fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Invalid file\n");
        return -1;
    }
    for( x = 0; x < 9; x++){
        for( y = 0; y < 9; y++){
            if(fscanf(fp, "%hd", &value)){
                if(0 == value){
                    puzzle->space[x][y].value = 0;
                    puzzle->space[x][y].possibles = (1<<9)-1;
                } else {
                    puzzle->space[x][y].value = short_to_field(value);
                    puzzle->space[x][y].possibles = 0;
                }

            } else {
                printf("Invalid input\n");
                return -1;
            }
        }
    }
    fclose(fp);
    printf("Finished\n");
    return 0;
}

short field_to_short(short input){
    short index = 1;
    if( !is_popcount_one(input)) return 0;
    while(input > 0){
        if(1 == (input % 2)){
            return index;
        } else {
            index++;
            input >>= 1;
        }
    }
    printf("field_to_short should never get here\n");
    return 0;
}

short short_to_field(short x){
    if(x){
        return 1 << (x-1);
    } else {
        return 0;
    }
}

// returns 1 if popcount of input is 1, else 0
int is_popcount_one(short x){
    if( !x) return 0; // popcount == 0
    if(x & (x-1)) return 0; // popcount >1
    return 1;  // popcount = 1
}

int popcount(unsigned int x)
{
    int c = 0;
    for (; x > 0; x &= x -1) c++;
    return c;
}

void print_field(short x){
    int i;
    for(i = 1; i < 10; i++){
        printf("%d",i);
    }
    printf("\n");
    for(i = 1; i < 10; i++){
        if(x & short_to_field(i)){
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
}

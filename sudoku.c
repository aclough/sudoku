#include <stdio.h>
#include "sudoku.h"

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Please specify a sudoku to solve\n");
        return -1;
    }
    sudoku *puzzle = (sudoku*)malloc(sizeof(sudoku));
    if(load_sudoku( puzzle, argv[1])){
        return -1;
    }
    print_sudoku(puzzle);
    if(solve_sudoku(puzzle)){
        return -1;
    }
    print_sudoku(puzzle);
    return 0;
}

int load_sudoku(sudoku *puzzle, char *filename){
    FILE *fp;
    int value;
    printf("Loading file: %s\n", filename);
    fp = fopen(filename, "r");
    for(int i = 0; i < SUDOKU_SIZE; i++){
        if(fscanf(fp, "%d", &value)){
            puzzle->value[i] = 1 << (value -1); // It's stored as a bit field
        } else {
            printf("Invalid input or not enough values\n");
            return -1;
        }
    }
    if(
    printf("Finished\n");
    return 0;
}

int solve_sudoku(sudoku *puzzle){
    int old_solved = SUDOKU_SIZE;
    int new_solved = do_sudoku(puzzle);
    while(new_solved){
        if(old_solved == new_solved){
            printf("Could not solve sudoku\n");
            return old_solved;
        } 
        old_solved = new_solved;
        new_solved = do_sudoku(puzzle);
    }
    printf("solved sudoku!\n");
    return 0;
}

void print_sudoku(sudoku *puzzle){
    int x;
    printf("\n");
    for(int i = 0; i < SUDOKU_SIZE; i++){
        x = field_to_int(puzzle->value[i]);
        printf("%2d ", x);
        if( 0 == (i+1) % SUDOKU_BOX_LENGTH){
            printf(" ");
        }
        if( 0 == (i+1) % SUDOKU_LENGTH){
            printf("\n");
        }
        if( 0 == (i+1) % (SUDOKU_LENGTH * SUDOKU_BOX_LENGTH)){
            printf("\n");
        }
    }
    printf("\n");
}

int field_to_int(int x){
    int y = 1;
    while(x > 0){
        if(1 == x){
            return y;
        } else {
            y++;
            x >>= 1;
        }
    }
    return 0;
}


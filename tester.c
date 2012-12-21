#include <stdio.h>
#include "sudoku.h"



int main(int argc, char *argv[]){
    sudoku puzzle, expected;
    if(argc < 2){
        printf("Please give a sudoku to solve\n");
        return -1;
    }
    if (load_sudoku(&puzzle, argv[1] )){
        return -1;
    }
    if(argc < 3){
        if(solve_sudoku(&puzzle, NULL)){
            return -1;
        }
    } else {
        if(load_sudoku(&expected,argv[2] )){
            return -1;
        }
        if(solve_sudoku(&puzzle, &expected)){
            return -1;
        }
    }
    printf("\nResult:\n");
    print_sudoku(&puzzle);
    return 0;
}


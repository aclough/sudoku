#include <stdio.h>
#include "sudoku.h"



int main(int argc, char *argv[]){
    sudoku puzzle;
    if(argc < 2){
        printf("Please give a sudoku to solve\n");
        return -1;
    }
    if (load_sudoku(&puzzle, argv[1] )){
        return -1;
    }
    if(solve_sudoku(&puzzle)){
        return -1;
    }
    printf("\nResult:\n");
    print_sudoku(&puzzle);
    return 0;
}


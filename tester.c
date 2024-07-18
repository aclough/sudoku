#include <stdio.h>
#include <stdlib.h>
#include "sudoku.h"



int main(int argc, char *argv[]){
    int target_iterations = 1;
    sudoku puzzle;
    if(argc < 2){
        printf("Please give a sudoku to solve\n");
        return -1;
    }
    if (argc > 2){
        target_iterations = atoi(argv[2]);
    }
    for (int i = 0; i < target_iterations; i++) {
        if (load_sudoku(&puzzle, argv[1] )){
            return -1;
        }
        if(solve_sudoku(&puzzle)){
            return -1;
        }
    }
    printf("\nResult:\n");
    print_sudoku(&puzzle);
    return 0;
}


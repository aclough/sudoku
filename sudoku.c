#include <stdio.h>
#include "sudoku.h"

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Please specify a sudoku to solve\n");
        return 0;
    }
    sudoku *puzzle = (sudoku*)malloc(sizeof(sudoku));
    load_sudoku( puzzle, argv[1]);
    return 0;
}

int load_sudoku(sudoku *puzzle, char *filename){
    FILE *fp;
    printf("Loading file: %s\n", filename);
    fp = fopen(filename, "r");

    printf("Finished\n");
    return 0;
}

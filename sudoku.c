#include <stdio.h>
#include "sudoku.h"

int main(int argc, char *argv[]){
    if( do_tests()){
        printf("Failed tests\n");
        return -1;
    }
    if(argc < 2){
        printf("Please specify a sudoku to solve\n");
        return -1;
    }
    sudoku *puzzle = (sudoku*)malloc(sizeof(sudoku));
    sudoku *check = NULL;
    if(load_sudoku( puzzle, argv[1])){
        return -1;
    }
    print_sudoku(puzzle);
    if(argc > 2){
        check = (sudoku*)malloc(sizeof(sudoku));
        if(load_sudoku( check, argv[2])){
            return -1;
        }
    }
    if(solve_sudoku(puzzle, check)){
        return -1;
    }
    print_sudoku(puzzle);
    return 0;
}

int do_tests(){
    for(int i = 0; i < SUDOKU_LENGTH; i++){
        if(i == field_to_int(int_to_field(i))){
            continue;
        } else {
            printf("Field/int transformation not symmetric at %d\n", i);
            return -1;
        }
    }
    if(field_to_int(0x1ab) != 0){
        printf("field_to_int failed to regect 0x1ab");
        return -1;
    }
    return 0;
}

int load_sudoku(sudoku *puzzle, char *filename){
    FILE *fp;
    int value;
    printf("Loading file: %s\n", filename);
    fp = fopen(filename, "r");
    for(int i = 0; i < SUDOKU_SIZE; i++){
        if(fscanf(fp, "%d", &value)){
            if(0 == value){
                puzzle->spot[i].value = 0;
                puzzle->spot[i].possibles = (1 << SUDOKU_LENGTH) -1;
            } else {
                puzzle->spot[i].value = int_to_field(value); 
                puzzle->spot[i].possibles = 0;
            }

        } else {
            printf("Invalid input or not enough values\n");
            return -1;
        }
    }
    printf("Finished\n");
    return 0;
}

int solve_sudoku(sudoku *puzzle, sudoku *check){
    block rows[SUDOKU_LENGTH];
    block colums[SUDOKU_LENGTH];
    block boxes[SUDOKU_LENGTH];
    element foo;

    for(int x = 0; x < SUDOKU_LENGTH; x++){
        for(int y = 0; y < SUDOKU_LENGTH; y++){
            int index = x * SUDOKU_LENGTH + y;
            rows[x].spot[y] = &(puzzle->spot[index]);
            colums[y].spot[x] = &(puzzle->spot[index]);
        }
    }
    printf("\n");
    for(int b = 0; b < SUDOKU_LENGTH; b++){ 
        int first_row = (b / SUDOKU_BOX_LENGTH) * SUDOKU_BOX_LENGTH;
        int colum_mod = SUDOKU_BOX_LENGTH;
        int first_colum = (b % colum_mod) * SUDOKU_BOX_LENGTH;
        for(int x = 0; x < SUDOKU_BOX_LENGTH; x++){
            for(int y = 0; y < SUDOKU_BOX_LENGTH; y++){
                boxes[b].spot[x + y * SUDOKU_BOX_LENGTH] =
                    rows[first_row + x].spot[first_colum + y];
            }
        }
    }

    int old_remaining = SUDOKU_SIZE;
    int new_remaining = count_sudoku(puzzle);
    int index = 0;
    while(new_remaining){
        printf("Itteration %d with %d remaining\n", index, new_remaining); fflush(stdout);
        if(old_remaining == new_remaining){
            printf("Could not solve sudoku\n");
            print_sudoku(puzzle);
            return old_remaining;
        } 
        old_remaining = new_remaining;
        do_sudoku(rows);
        if(check){
            if(check_sudoku(puzzle, check)){
                printf("failed check\n");
                print_sudoku(puzzle);
                printf("\n");
                print_sudoku(check);
                return -1;
            }
        }
        do_sudoku(colums);
        if(check){
            if(check_sudoku(puzzle, check)){
                printf("failed check\n");
                print_sudoku(puzzle);
                printf("\n");
                print_sudoku(check);
                return -1;
            }
        }
        do_sudoku(boxes);
        if(check){
            if(check_sudoku(puzzle, check)){
                printf("failed check\n");
                print_sudoku(puzzle);
                printf("\n");
                print_sudoku(check);
                return -1;
            }
        }
        new_remaining = count_sudoku(puzzle);
        index++;
    }
    printf("The sudoku is solved!\n");
    return 0;
}

int check_sudoku(sudoku *puzzle, sudoku *check){
    for(int i = 0; i < SUDOKU_SIZE; i++){
        int val = puzzle->spot[i].value;
        if(val != check->spot[i].value && val){
            printf("Check fails with %d vs %d at %d\n", field_to_int(val),
                    field_to_int(check->spot[i].value), i);
            return -1;
        }
    }
    return 0;
}

int count_sudoku(sudoku * puzzle){
    int unsolved = 0;
    for(int i = 0; i < SUDOKU_SIZE; i++){
        if(0 == puzzle->spot[i].value){
            unsolved++;
        }
    }
    return unsolved;
}

void do_sudoku( block blocks[]){
    for(int b = 0; b < SUDOKU_LENGTH; b++){
        do_block(blocks[b].spot);
    }
}

void do_block(element * spots[]){
    int block_not_possibles = 0;
    for(int i = 0; i < SUDOKU_LENGTH; i++){
       block_not_possibles += spots[i]->value;
    }
    if( block_not_possibles >= (1 << SUDOKU_LENGTH)){
        printf("Invalid state detected!\n");
    }
    for(int i = 0; i < SUDOKU_LENGTH; i++){
        spots[i]->possibles &= ~block_not_possibles;
        int val = field_to_int(spots[i]->possibles);
        if(val){  // There's a single possible value for the spot
            spots[i]->value = int_to_field(val);
            spots[i]->possibles = 0;
        }
    }
    for(int v = 0; v < SUDOKU_LENGTH; v++){
        int count = 0;
        int index = 0;
        for(int i = 0; i < SUDOKU_LENGTH; i++){
            if(spots[i]->possibles | (1 << (v -1))){
                index = i;
                count++;
            }
        }
        if(1 == count){ // We found one spot that could have a value of v
            spots[index]->value = v;
            spots[index]->possibles = 0;
        }
    }
}

void print_sudoku(sudoku *puzzle){
    int x;
    printf("\n");
    for(int i = 0; i < SUDOKU_SIZE; i++){
        x = field_to_int(puzzle->spot[i].value);
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
        if(1 == (x % 2)){
            if(1 == x){
                return y;
            } else {
                return 0;
            }
        } else {
            y++;
            x >>= 1;
        }
    }
    return 0;
}

int int_to_field(int x){
    if(x){
        return 1 << (x-1);
    } else {
        return 0;
    }
}


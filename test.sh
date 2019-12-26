#!/bin/bash
clang tester.c sudoku.c
nim c sudoku.nim
echo "C speed 1000 times"
time for i in {1..1000}; do ./a.out hard-sudoku.txt > /dev/null; done

echo "Nim speed 1000 times"
time for i in {1..1000}; do ./sudoku hard-sudoku.txt > /dev/null; done

echo "Python speed 1000 times"
time for i in {1..1000}; do python sudoku.py hard-sudoku.txt > /dev/null; done

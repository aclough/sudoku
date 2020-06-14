#!/bin/bash -e
clang -O2 tester.c sudoku.c
clang -O2 -shared -o sudoku.so -fPIC sudoku.c
nim c -d:release sudoku.nim
echo "C speed 1000 times"
time for i in {1..1000}; do ./a.out hard-sudoku.txt > /dev/null; done

echo "Nim speed 1000 times"
time for i in {1..1000}; do ./sudoku hard-sudoku.txt > /dev/null; done

echo "Python speed 1000 times"
time for i in {1..1000}; do python3 sudoku.py hard-sudoku.txt > /dev/null; done

echo "Python calling c speed 1000 times"
time for i in {1..1000}; do python3 sudoku-with-c.py hard-sudoku.txt > /dev/null; done

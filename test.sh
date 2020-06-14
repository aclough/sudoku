#!/bin/bash -e

echo ""
echo "Compiling C"
clang -O2 tester.c sudoku.c
echo ""
echo "Compiling C shared library"
clang -O2 -shared -o sudoku.so -fPIC sudoku.c
echo ""
echo "Compiling Nim"
nim c -d:release --verbosity:0 sudoku.nim

echo ""
echo "C speed 1000 times"
time for i in {1..1000}; do ./a.out hard-sudoku.txt > /dev/null; done

echo ""
echo "Nim speed 1000 times"
time for i in {1..1000}; do ./sudoku hard-sudoku.txt > /dev/null; done

echo ""
echo "Python speed 1000 times"
time for i in {1..1000}; do python3 sudoku.py hard-sudoku.txt > /dev/null; done

echo ""
echo "Python calling c speed 1000 times"
time for i in {1..1000}; do python3 sudoku-with-c.py hard-sudoku.txt > /dev/null; done

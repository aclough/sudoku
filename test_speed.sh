#!/bin/bash -e

echo ""
echo "Compiling C"
gcc -O2 tester.c sudoku.c -o sudoku_c
echo ""
echo "Compiling C++"
g++ -O2 sudoku.cpp -lboost_program_options -o sudoku_cpp
echo ""
echo "Compiling C shared library"
gcc -O2 -shared -o sudoku.so -fPIC sudoku.c
echo ""
echo "Compiling Nim"
nim c -d:release --verbosity:0 sudoku.nim
mv sudoku sudoku_nim
echo ""
echo "Compiling Rust"
RUSTFLAGS="-C target-cpu=native" cargo build --release
cp ./target/release/sudoku_rust sudoku_rust

echo ""
echo "C speed 10000 times"
time for i in {1..10000}; do ./sudoku_c hard-sudoku.txt > /dev/null; done

echo ""
echo "C++ speed 1,000,000 times"
time ./sudoku_cpp --iterations 1000000 hard-sudoku.txt > /dev/null

echo ""
echo "Nim speed 10000 times"
time for i in {1..10000}; do ./sudoku_nim hard-sudoku.txt > /dev/null; done

echo ""
echo "Rust speed 10000 times"
time for i in {1..10000}; do ./sudoku_rust hard-sudoku.txt > /dev/null; done

echo ""
echo "Python speed 1000 times"
time for i in {1..1000}; do python3 sudoku.py hard-sudoku.txt > /dev/null; done

echo ""
echo "Python calling c speed 1000 times"
time for i in {1..1000}; do python3 sudoku-with-c.py hard-sudoku.txt > /dev/null; done

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
echo "Compiling Zig calling C"
zig build-exe sudoku-with-c.zig sudoku.c -lc -I.
mv sudoku-with-c sudoku-with-c-zig
echo ""
echo "Compiling pure Zig"
zig build-exe sudoku.zig -O ReleaseFast
mv sudoku sudoku_zig

echo ""
echo "C speed 1,000,000 times"
time ./sudoku_c hard-sudoku.txt 1000000 > /dev/null

echo ""
echo "C++ speed 1,000,000 times"
time ./sudoku_cpp --iterations 1000000 hard-sudoku.txt > /dev/null

echo ""
echo "C++ speed 1,000,000 times, parallel"
time ./sudoku_cpp --iterations 1000000 -p hard-sudoku.txt > /dev/null

echo ""
echo "Nim speed 1,000,000 times"
time  ./sudoku_nim hard-sudoku.txt 1000000 > /dev/null

echo ""
echo "Rust speed 1,000,000 times"
time ./sudoku_rust -f hard-sudoku.txt -c 1000000 > /dev/null

echo ""
echo "Rust speed 1,000,000 times, parallel"
time ./sudoku_rust -p -f hard-sudoku.txt -c 1000000 > /dev/null

echo ""
echo "Zig speed calling C 1,000,000 times"
time ./sudoku-with-c-zig hard-sudoku.txt 1000000 > /dev/null

echo ""
echo "Pure Zig speed 1,000,000 times"
time ./sudoku_zig hard-sudoku.txt 1000000 > /dev/null

echo ""
echo "Python speed 1,000 times"
time for i in {1..1000}; do python3 sudoku.py hard-sudoku.txt > /dev/null; done

echo ""
echo "Python calling c speed 1,000 times"
time for i in {1..1000}; do python3 sudoku-with-c.py hard-sudoku.txt > /dev/null; done

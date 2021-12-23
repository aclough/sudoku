#!/bin/sh

set -e

cargo build --release
cp ./target/release/sudoku_rust sudoku_rust

echo "Finished building\n"

echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid

echo 0 | sudo tee /proc/sys/kernel/kptr_restrict

flamegraph $(pwd)/sudoku_rust $(pwd)/hard-sudoku.txt 10000

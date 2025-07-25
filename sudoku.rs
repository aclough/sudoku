// Rust sudoku implementation
// This is a heuristic guided search with backout like the Nim version

use clap::Parser;
use std::assert;
use std::fs;
use std::fs::read_to_string;
use std::sync::{Arc, Mutex};
use std::thread;
use std::thread::available_parallelism;

// TODO
//   Threading?
//   Some structure for iterating repetition, passing in a closure or using a macro?
//   Check if compile time index calculation is possible now.

// The field type is a bitfield where each of the bits represents the possibility that the
// corresponding digit could be found there.  For instance, if the bitfield is 0x03 then either a 1
// or a 2 could be there.
type Field = u16;

#[derive(Copy, Clone)]
struct Sudoku {
    // The final derived value for each cell. 0 if still unknown or a bitfield with a single
    // bit set if the solution has been found. Stored as a field to better interface with
    // other arrays and improve performance for set_value and clear_value.
    cells: [Field; 81],

    // Remaining possibilities for each block, column, or row.
    blocks: [Field; 9],
    cols: [Field; 9],
    rows: [Field; 9],

    // Number of remaining unsolved cells
    remaining: usize,
}

impl Sudoku {
    fn print(&self) {
        let mut s = "".to_string();
        for (i, v) in self.into_iter().enumerate() {
            if i == 0 {
            } else if i % 27 == 0 {
                s.push_str("\n\n");
            } else if i % 9 == 0 {
                s.push('\n');
            } else if i % 3 == 0 {
                s.push_str("   ");
            } else {
                s.push(' ');
            }
            s.push_str(&field_to_val(*v).to_string());
        }
        println!("{s}\n\n");
    }

    fn set_value(&mut self, val: Field, location: usize) {
        assert!(self.cells[location] == 0);
        self.cells[location] = val;

        let (blk, col, row) = get_indices(location);
        self.blocks[blk] &= !val;
        self.cols[col] &= !val;
        self.rows[row] &= !val;
        self.remaining -= 1;
    }

    fn clear_value(&mut self, location: usize) {
        assert!(self.cells[location] != 0);
        let val = self.cells[location];
        self.cells[location] = 0;

        let (blk, col, row) = get_indices(location);
        self.blocks[blk] |= val;
        self.cols[col] |= val;
        self.rows[row] |= val;
        self.remaining += 1;
    }

    fn back_out_moves(&mut self, eager_moves: Vec<usize>) {
        for guess in eager_moves {
            self.clear_value(guess);
        }
    }

    fn solve(&mut self) -> bool {
        // Stage once, do as many forced moves as possible
        // Keep trying until they stop coming
        let mut eager_moves: Vec<usize> = Vec::with_capacity(16);

        loop {
            let mut solved_count = 0;
            let mut lowest_space: usize = 0;
            let mut lowest_count: u32 = std::u32::MAX;
            let mut lowest_possibles: Field = 0;
            for i in 0..81 {
                if self.cells[i] != 0 {
                    // this cell is already solved
                    continue;
                }
                let (b, c, r) = get_indices(i);
                let possibles = self.blocks[b] & self.cols[c] & self.rows[r];
                let count: u32 = possibles.count_ones();

                if count == 1 {
                    self.set_value(possibles, i);
                    solved_count += 1;
                    eager_moves.push(i);
                } else if count == 0 {
                    // We're down a blind ally, abort
                    self.back_out_moves(eager_moves);
                    return false;
                } else if count < lowest_count {
                    lowest_count = count;
                    lowest_space = i;
                    lowest_possibles = possibles;
                }
            }
            if solved_count != 0 {
                // If we're finding moves by elimination don't start guessing yet, just keep on
                // solving this way
                continue;
            } else if self.remaining == 0 {
                // We won!
                return true;
            }
            let possibles = field_to_vals(lowest_possibles);
            for guess in possibles {
                self.set_value(val_to_field(guess), lowest_space);
                if self.solve() {
                    return true;
                }
                self.clear_value(lowest_space);
            }
            self.back_out_moves(eager_moves);
            // None of the guesses worked, we're on a bad branch.  Abort
            return false;
        }
    }
}

fn val_to_field(x: u8) -> Field {
    1 << (x - 1)
}

fn field_to_val(x: Field) -> u8 {
    if x == 0 {
        return 0;
    }
    assert!(x.count_ones() == 1);
    return x.trailing_zeros() as u8 + 1;
}

fn field_to_vals(x: Field) -> Vec<u8> {
    // In theory, the maximum number of ones returned from count_ones() on a 16 bit number should
    // be 16, which should fit in a u8, which would let me use `usize::from` here instead of `as`,
    // sadly it returns a u32 for some reason so we have to do something dangerous looking to the
    // compiler.
    let mut ret: Vec<u8> = Vec::with_capacity(x.count_ones() as usize);
    if x == 0 {
        return ret;
    }

    let mut x = x;
    let mut val = 1u8;

    while x > 0 {
        if 1 == (x % 2) {
            ret.push(val);
        }
        val += 1;
        x >>= 1;
    }
    ret
}

fn get_indices(i: usize) -> (usize, usize, usize) {
    let row = i / 9;
    let col = i % 9;
    let blk = row / 3 + 3 * (col / 3);
    (blk, col, row)
}

fn load_sudoku(file: &String) -> Sudoku {
    // Bits 1 through 9 are set since any of those might be a valid guess to start with.
    let start_possibilties: Field = 0x1FF;
    let mut s = Sudoku {
        cells: [0; 81],
        blocks: [start_possibilties; 9],
        cols: [start_possibilties; 9],
        rows: [start_possibilties; 9],
        remaining: 81,
    };

    let contents = fs::read_to_string(file).expect("Something went wrong reading the file");
    for (i, c) in (0..81).zip(contents.split_whitespace()) {
        let x = c.parse().unwrap();
        if x == 0 {
            continue;
        }
        s.set_value(val_to_field(x), i)
    }

    return s;
}

impl<'a> IntoIterator for &'a Sudoku {
    type Item = &'a Field;
    type IntoIter = std::slice::Iter<'a, Field>;

    fn into_iter(self) -> Self::IntoIter {
        self.cells.iter()
    }
}

fn solve_n(filename: &String, n: usize) -> Sudoku {
    // Solves a sudoku N times, and then returns the last Sudoku
    // Load the initial state once and copy it for each iteration
    let initial_sudoku = load_sudoku(&filename);
    let mut s = initial_sudoku.clone();
    s.solve();
    for _ in 1..n {
        s = initial_sudoku.clone();
        s.solve();
    }
    return s;
}

fn solve_alt_format(filename: &String) {
    let mut count = 0;
    for line in read_to_string(filename).unwrap().lines() {
        if line.len() != 81 {
            println!("Solving {line}");
            continue;
        }

        let start_possibilties: Field = 0x1FF;
        let mut s = Sudoku {
            cells: [0; 81],
            blocks: [start_possibilties; 9],
            cols: [start_possibilties; 9],
            rows: [start_possibilties; 9],
            remaining: 81,
        };
        for (i, c) in (0..81).zip(line.chars()) {
            let x = c.to_digit(10).unwrap() as u8;
            if x == 0 {
                continue;
            }
            s.set_value(val_to_field(x), i)
        }
        s.solve();
        count += 1;
        if count % 1000 == 0 {
            println!("  {count}");
        }
    }
}

#[derive(Parser)]
#[command(version, about, long_about = None)]
struct Args {
    #[arg(short, long, default_value = "puzzle.txt")]
    filename: String,
    #[arg(short, long, default_value_t = 1)]
    count: usize,
    #[arg(short, long, action = clap::ArgAction::SetTrue)]
    parallel: bool,
    #[arg(short, long, action = clap::ArgAction::SetTrue)]
    alt_format: bool,
}

fn main() {
    let args = Args::parse();

    let file = Arc::new(args.filename);
    let count = args.count;

    if args.alt_format {
        solve_alt_format(&file);
        return;
    }

    println!("Solving {file} {count} times");


    let s =
    if args.parallel {
        println!("Parallel");
        let thread_count = available_parallelism().unwrap().get();
        println!("Spawning {thread_count} threads");

        // Load initial sudoku once and share it across threads
        let initial_sudoku = Arc::new(load_sudoku(&file));
        let mut handles: Vec<thread::JoinHandle<()>> = vec![];
        let shared_results = Arc::new(Mutex::new(Vec::new()));

        for i in 0..thread_count {
            let thread_results = Arc::clone(&shared_results);
            let thread_initial = Arc::clone(&initial_sudoku);
            let n = count / thread_count + if i < (count % thread_count) {1} else {0};
            let handle = thread::spawn(move || {
                let mut s = thread_initial.as_ref().clone();
                s.solve();
                for _ in 1..n {
                    s = thread_initial.as_ref().clone();
                    s.solve();
                }
                let mut results = thread_results.lock().unwrap();
                results.push(s);
            });
            handles.push(handle);
        }

        for handle in handles {
            handle.join().unwrap();
        }
        let final_results = shared_results.lock().unwrap();
        final_results[0]

    } else {
        solve_n(&file, args.count)
    };
    s.print();
}

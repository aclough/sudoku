// Rust sudoku implementation
// This is a heuristic guided search with backout like the Nim version

use std::assert;
use std::env;
use std::fs;
use std::vec;

// TODO
//   Threading?
//   Some structure for iterating repetition, passing in a closure or using a macro?

// The field type is a bitfield where each of the bits represents the possibility that the
// corresponding digit could be found there.  For instance, if the bitfield is 0x03 then either a 1
// or a 2 could be there.
type Field = u16;

struct Sudoku {
    // The final derived value, this will either be 0 if still unknown or a bitfield with a single
    // bit set if the solution has been found.  Left as a field to better interface with the other
    // arrays.  Left as a Field because it makes set_value and clear_value slightly faster without
    // the Field/value conversion and we call those much more frequently than we do the print
    // function which wants these as actual values.
    results: [Field; 81],
    // These are the remaining possibilities for the corresponding block, column, or row.
    blocks: [Field; 9],
    cols: [Field; 9],
    rows: [Field; 9],
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
        println!("{}\n\n", s);
    }

    fn set_value(&mut self, val: Field, location: usize) {
        assert!(self.results[location] == 0);
        self.results[location] = val;

        let (blk, col, row) = get_indices(location);
        self.blocks[blk] &= !val;
        self.cols[col] &= !val;
        self.rows[row] &= !val;
        self.remaining -= 1;
    }

    fn clear_value(&mut self, location: usize) {
        assert!(self.results[location] != 0);
        let val = self.results[location];
        self.results[location] = 0;

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
                if self.results[i] != 0 {
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

fn val_to_field(x: u8) -> Field  {
    1 << (x-1)
}

fn field_to_val(x: Field) -> u8 {
    if x == 0 {
        return 0;
    }
    let mut val = 1u8;
    let mut x = x;
    while x > 0 {
        if 1 == (x % 2) {
            return val;
        } else {
            val += 1;
            x >>= 1;
        }
        assert!(x != 0);
    }
    // Impossible to get here
    unreachable!()
}

fn field_to_vals(x: Field) -> Vec<u8> {
    let mut ret: Vec<u8> = vec![];
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

fn load_sudoku(filename: &String) -> Sudoku {

    // Bits 1 through 9 are set since any of those might be a valid guess to start with.
    let start_possibilties:Field = 0x1FF;
    let mut s = Sudoku { results: [0; 81],
                         blocks: [start_possibilties; 9],
                         cols: [start_possibilties; 9],
                         rows: [start_possibilties; 9],
                         remaining: 81};

    let contents = fs::read_to_string(filename)
        .expect("Something went wrong reading the file");
    for (i, c) in (0..81).zip(contents.split_whitespace()) {
        let x = c.parse().unwrap();
        if x == 0 {
            continue;
        }
        s.set_value(val_to_field(x), i)
    }

    return s
}

impl<'a> IntoIterator for &'a Sudoku {
   type Item = &'a Field;
   type IntoIter = std::slice::Iter<'a, Field>;

   fn into_iter(self) -> Self::IntoIter {
       self.results.iter()
   }
}

fn main() {
    let args: Vec<String> = env::args().collect();

    let file = if args.len() > 1 {
        args[1].clone()
    } else {
        "puzzle.txt".to_string()
    };

    let count: u32 = if args.len() > 2 {
        args[2].trim().parse().expect("Please type a number!")
    } else {
        1
    };
    println!("Solving {} {} times", file, count);

    for i in 0..count {
        let mut s = load_sudoku(&file);
        if i == 0 {s.print();}
        s.solve();
        if i == 0 {s.print();}
    }
}

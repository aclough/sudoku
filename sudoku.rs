// Rust sudoku implementation
// This is a heuristic guided search with backout like the Nim version

use std::assert;
use std::env;
use std::fs;
use std::vec;

// TODO
//   Make fields and vals their own types
//   Remaining list to cut down on iteration?
//   Threading?
//   Some structure for iterating repitition, passing in a closure or using a macro?

struct Sudoku {
    vals: [u16; 81],
    blocks: [u16; 9],
    cols: [u16; 9],
    rows: [u16; 9],
}

impl Sudoku {
    fn print(&self) {
        let mut s = "".to_string();
        for (i, v) in self.into_iter().enumerate() {
            if i == 0 {
            } else if i % 27 == 0 {
                s.push_str("\n\n");
            } else if i % 9 == 0 {
                s.push_str("\n");
            } else if i % 3 == 0 {
                s.push_str("   ");
            } else {
                s.push_str(" ");
            }
            s.push_str(&field_to_val(*v).to_string());
        }
        println!("{}\n\n", s);
    }
    
    fn set_value(&mut self, val: u16, location: usize) {
        assert!(self.vals[location] == 0);
        self.vals[location] = val;

        let (blk, col, row) = get_indices(location);
        self.blocks[blk] = self.blocks[blk] & !val;
        self.cols[col] = self.cols[col] & !val;
        self.rows[row] = self.rows[row] & !val;
    }

    fn clear_value(&mut self, location: usize) {
        assert!(self.vals[location] != 0);
        let val = self.vals[location];
        self.vals[location] = 0;

        let (blk, col, row) = get_indices(location);
        self.blocks[blk] = self.blocks[blk] | val;
        self.cols[col] = self.cols[col] | val;
        self.rows[row] = self.rows[row] | val;
    }

    fn back_out_moves(&mut self, eager_moves: Vec<usize>) {
        for guess in eager_moves {
            self.clear_value(guess);
        }
    }

    fn solve(&mut self) -> bool {

        // Stage once, do as many forced moves as possible
        // Keep trying until they stop coming
        let mut eager_moves: Vec<usize> = vec![];

        loop {
            let mut solved_count = 0;
            for i in 0..81 {
                if self.vals[i] != 0 {
                    // solved
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
                }
            }
            if solved_count == 0 {
                // We aren't solving any more, so go on to guess and check
                break;
            }
        }
        
        // Now go through again with up to date info and find the best place to guess
        let mut lowest_space: usize = 0;
        let mut lowest_count: u32 = 10;
        for i in 0..81 {
            let (b, c, r) = get_indices(i);
            let possibles = self.blocks[b] & self.cols[c] & self.rows[r];
            let count: u32 = possibles.count_ones();
            if self.vals[i] != 0 {
                // solved
                continue;
            }
            if count < lowest_count {
                lowest_count = count;
                lowest_space = i;
            }
            if count == 2 {
                // We won't do better than this.
                break;
            }
        }
        if lowest_count == 10 {
            // We won!
            return true;
        }

        let (b, c, r) = get_indices(lowest_space);
        let possibles = self.blocks[b] & self.cols[c] & self.rows[r];
        let possibles = field_to_vals(possibles);
        //println!("Checking {} with {} possibles, {:?}", lowest_space, possibles.len(), possibles);
        //self.print();
        assert!(possibles.len() == lowest_count as usize); // There's no way we can have more than 9 possibilities so using `as`
        for guess in possibles {
            self.set_value(val_to_field(guess), lowest_space);
            if self.solve() {
                return true;
            }
            self.clear_value(lowest_space);
        }
        self.back_out_moves(eager_moves);
        return false;
    }
}

fn val_to_field(x: u8) -> u16  {
    return 1 << (x-1);
}

fn field_to_val(x: u16) -> u8 {
    if x == 0 {
        return 0;
    }
    assert!(x.count_ones() == 1);
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
    assert!(false);
    return 0;
}

fn field_to_vals(x: u16) -> Vec<u8> {
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
    return ret;
}

fn get_indices(i: usize) -> (usize, usize, usize) {
    let row = i / 9;
    let col = i % 9;
    let blk = row / 3 + 3 * (col / 3);
    return (blk, col, row)
}

fn load_sudoku(filename: String) -> Sudoku {
    // Bits 1 through 9 are set since any of those might be a valid guess to start with.
    let start_possibilties:u16 = 0x1FF;
    let mut s = Sudoku { vals: [0; 81], 
                         blocks: [start_possibilties; 9],
                         cols: [start_possibilties; 9],
                         rows: [start_possibilties; 9]};

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
   type Item = &'a u16;
   type IntoIter = std::slice::Iter<'a, u16>;

   fn into_iter(self) -> Self::IntoIter {
       self.vals.iter()
   }
}

fn main() {
    let args: Vec<String> = env::args().collect();

    let file = if args.len() > 1 {
        args[1].clone()
    } else {
        "puzzle.txt".to_string()
    };

    let mut s = load_sudoku(file);
    s.print();
    s.solve();
    s.print();
}

// Rust sudoku implementation
// This is a heuristic guided search with backout like the Nim version
// I'll be trying to add threading later

use std::assert;
use std::env;
use std::fs;

struct Sudoku {
    vals: [u8; 81],
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
            s.push_str(&v.to_string());
        }
        println!("{}", s);
    }
    
    fn set_value(&mut self, val: u8, location: usize) {
        assert!(self.vals[location] == 0);
        self.vals[location] = val;
        let (blk, col, row) = get_indices(location);
        assert!(self.blocks[blk] & (1 << val) != 0);
        self.blocks[blk] = self.blocks[blk] ^ (1 << val);
        assert!(self.cols[col] & (1 << val) != 0);
        self.cols[col] = self.cols[col] ^ (1 << val);
        assert!(self.rows[row] & (1 << val) != 0);
        self.rows[row] = self.rows[row] ^ (1 << val);
    }
}

fn get_indices(i: usize) -> (usize, usize, usize) {
    let row = i / 9;
    let col = i % 9;
    let blk = row / 3 + 3 * (col / 3);
    return (blk, col, row)
}

fn load_sudoku(filename: String) -> Sudoku {
    // Bits 1 through 9 are set since any of those might be a valid guess to start with.
    let start_possibilties:u16 = 0x3FE;
    let mut s = Sudoku { vals: [0; 81], 
                         blocks: [start_possibilties; 9],
                         cols: [start_possibilties; 9],
                         rows: [start_possibilties; 9]};

    let contents = fs::read_to_string(filename)
        .expect("Something went wrong reading the file");
    for (i, c) in (0..81).zip(contents.split_whitespace()) {
        s.set_value(c.parse().unwrap(), i)
    }



    // Go through and remove possibilities based on initial state

    return s
}

impl<'a> IntoIterator for &'a Sudoku {
   type Item = &'a u8;
   type IntoIter = std::slice::Iter<'a, u8>;

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

    let s = load_sudoku(file);
    s.print();
}

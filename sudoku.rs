// Rust sudoku implemenation
// This is a heuristic guided search with backout like the Nim version
// I'll be trying to add threading later

use std::fs;

struct Sudoku {
    vals: [u8; 81],
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
}

fn load_sudoku(filename: &str) -> Sudoku {
    let contents = fs::read_to_string(filename)
        .expect("Something went wrong reading the file");
    let mut numbers: [u8; 81] = [0; 81];
    for (i, c) in (0..81).zip(contents.split_whitespace()) {
        numbers[i] = c.parse().unwrap()
    }
    return Sudoku { vals: numbers }
}

impl<'a> IntoIterator for &'a Sudoku {
   type Item = &'a u8;
   type IntoIter = std::slice::Iter<'a, u8>;

   fn into_iter(self) -> Self::IntoIter {
       self.vals.iter()
   }
}

fn main() {
    let s = load_sudoku("puzzle.txt");
    s.print();
}

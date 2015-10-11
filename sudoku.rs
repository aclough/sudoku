// Rust sudoku implemenation
// This is a heuristic guided search with backout like the Nim version
// There's some threading here, just to explore it


struct Sudoku {
    vals: [u8; 81],
}

impl Sudoku {
    fn print(&self) {
        let mut s = "".to_string();
        for (i, v) in self.into_iter().enumerate() {
            if i == 0 {
                ;
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

impl<'a> IntoIterator for &'a Sudoku {
   type Item = &'a u8;
   type IntoIter = std::slice::Iter<'a, u8>;

   fn into_iter(self) -> Self::IntoIter {
       self.vals.into_iter()
   }
}

fn main() {
    let s = Sudoku {
        vals: [1,2,3,4,5,6,7,8,9,
               2,2,3,4,5,6,7,8,9,
               3,2,3,4,5,6,7,8,9,
               4,2,3,4,5,6,7,8,9,
               5,2,3,4,5,6,7,8,9,
               6,2,3,4,5,6,7,8,9,
               7,2,3,4,5,6,7,8,9,
               8,2,3,4,5,6,7,8,9,
               9,2,3,4,5,6,7,8,9],
    };
    s.print();
}

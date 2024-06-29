#include <iostream>
#include <bitset>

using namespace std;

// A type representing the possibilities for a single cell in the Sudoku board.
// For blocks, rows, and columns, there will be a number of bits set fo the unused
// values in that location.  For the cells it will be either 0 for a blank cell or
// have a single value for the filled cell
typedef bitset<9> Field;

class SudokuProblem {
private:
    // 0 for unknown,
    // 1-9 for known value
    Field cells[81];

    // Stores the remaining unused value in each region.
    Field blocks[9];
    Field rows[9];
    Field cols[9];
public:
    SuodkuProblem(ifstream inpput_file) {
    }
};


int main() {
    return 0;
}

#include <iostream>
#include <bitset>
#include <fstream>

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

    void set_value(int value) {
        // FIXME
    }
public:
    SudokuProblem(ifstream& input_file) {
        string value;
        int i = 0;
        while  (input_file >> value) {
            try {
                int spot_value = stoi(value);
                set_value(spot_value);
                i++;
            } catch (const invalid_argument& e) {
                // As long as we have 81 numbers, extraneous values aren't a problem
            }
        }
        if (i != 81) {
            cerr << "Got " << i << " numbers" << endl;
            throw invalid_argument("Wrong number of numbers for a sudoku puzzle");
            // Handle the error (e.g., throw an exception)
        }

        // Todo, check for more values
        // Todo, close the file here or at the call site
    }
};


int main() {
    ifstream input_file("puzzle.txt");
    if (!input_file.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }
    SudokuProblem problem(input_file);
    return 0;
}

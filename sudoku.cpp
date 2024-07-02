#include <iostream>
#include <bitset>
#include <fstream>

using namespace std;

// A type representing the possibilities for a single cell in the Sudoku board.
// For blocks, rows, and columns, there will be a number of bits set fo the unused
// values in that location.  For the cells it will be either 0 for a blank cell or
// have a single value for the filled cell
typedef bitset<9> Field;

class Indices {
public:
    unsigned char block;
    unsigned char row;
    unsigned char col;

    Indices (int index) {
        row = index / 9;
        col = index % 9;
        block = row / 3 + 3 * col / 3;
    }
};

class SudokuProblem {
private:
    // 0 for unknown,
    // 1-9 for known value
    Field cells[81];

    // Stores the remaining unused value in each region.
    Field blocks[9];
    Field rows[9];
    Field cols[9];

    void set_value(int location, Field value) {
        if (cells[location].any()) {
            throw invalid_argument("Tried to set already set cell");
        }
        cells[location] = value;
        Indices indices(location);
        blocks[indices.block] &= ~value;
        rows[indices.row] &= ~value;
        cols[indices.col] &= ~value;
    }
public:
    // A copy of the input that produced this problem for debugging purposes.
    string input;

    SudokuProblem(ifstream& input_file) {
        string value;
        int i = 0;
        while  (input_file >> value) {
            try {
                int spot_value = stoi(value);
                set_value(i, spot_value);
                i++;
            } catch (const invalid_argument& e) {
                // As long as we have 81 numbers, extraneous values aren't a problem
            }
            input += value;
        }
        if (i != 81) {
            cerr << "Got " << i << " numbers" << endl;
            throw invalid_argument("Wrong number of numbers for a sudoku puzzle");
            // Handle the error (e.g., throw an exception)
        }

    }

    string to_string() const {
        string result;
        for (int i = 0; i < 81; i++) {
            if (i && i % 27 == 0) {
                result += "\n";
            }
            if (i % 9 == 0) {
                result += "\n";
            } else if (i % 3 == 0) {
                result += "   ";
            } else {
                result += " ";
            }

            if (cells[i].none()) {
                result += ".";
            } else {
                result += std::to_string(cells[i].to_ulong());
            }
        }
        return result;
    }

};

ostream& operator<<(ostream &out, SudokuProblem const& sudoku) {
    out << sudoku.to_string() << endl;
    return out;
}


int main() {
    ifstream input_file("puzzle.txt");
    if (!input_file.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }
    SudokuProblem problem(input_file);
    input_file.close();
    cout << problem << endl;
    return 0;
}

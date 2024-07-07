#include <iostream>
#include <bitset>
#include <fstream>
#include <vector>
#include <array>

// Running the risk that if something new is defined in std that conflicts with one
// of my definitions I'll get a namespace collision.
using namespace std;

// To try:
// Make Field its own class

// A type representing the possibilities for a single cell in the Sudoku board.
// For blocks, rows, and columns, there will be a number of bits set fo the unused
// values in that location.  For the cells it will be either 0 for a blank cell or
// have a single value for the filled cell
// The set is 1 bit larger than is needed to avoid special logic to deal with
// the 0 position.
typedef bitset<10> Field;

// Constant to initialize the field with all values possible
const int field_init = 0b1111111110;

string field_to_string(Field field) {
    if (field.count() > 1) {
        throw invalid_argument("Can only convert single solution Field to string");
    }

    string result;
    for (int i = 1; i < 10; i++) {
        if (field.test(i)) {
            return std::to_string(i);
        }
    }
    return ".";
}

vector<Field> get_possible_moves(Field field) {
    vector<Field> result;
    for (int i = 1; i < 10; i++) {
        if (field.test(i)) {
            result.push_back(Field(1 << i));
        }
    }
    return result;
}

class Indices {
public:
    const int block;
    const int row;
    const int col;

    Indices (int index) :
        row{index / 9},
        col{index % 9},
        // 3*(row/3) + (col/3)
        block{(index / 27) * 3 + (index % 9) / 3}
    {}

    void print() const {
        cout << "Block: " << block << " Row: " <<  row << " Col: " << col << endl;
    }
};

class SudokuProblem {
private:
    // 0 for unknown,
    // Bits 1-9 set for known value
    array<Field, 81> cells;

    // Stores the remaining unused value in each region.
    array<Field, 9> blocks;
    array<Field, 9> rows;
    array<Field, 9> cols;
    int unsolved_spaces = 81;

    void set_value(const int location, const Field value) {
        if (cells[location].any()) {
            throw invalid_argument("Tried to set already set cell");
        }
        if (value == 0) {
            throw invalid_argument("Can't set cell to 0.");
        }
        if (location < 0 || location >= 81) {
            throw invalid_argument("Invalid location");
        }
        cells[location] = value;

        Indices indices(location);
        blocks[indices.block] &= ~value;
        rows[indices.row] &= ~value;
        cols[indices.col] &= ~value;
        unsolved_spaces--;
    }

    void clear_value(const int location) {
        if (!cells[location].any()) {
            throw invalid_argument("Tried to clear already clear cell");
        }
        if (location < 0 || location >= 81) {
            throw invalid_argument("Invalid location");
        }
        Field value = cells[location];
        cells[location].reset();

        Indices indices(location);
        blocks[indices.block] |= value;
        rows[indices.row] |= value;
        cols[indices.col] |= value;
        unsolved_spaces++;
    }

    void clear_moves(vector<int> moves) {
        for (auto location: moves) {
            clear_value(location);
            forceing_passes--;
        }
    }
public:
    // A copy of the input that produced this problem for debugging purposes.
    string input;
    int forceing_passes{0};
    int guesses{0};

    SudokuProblem(ifstream& input_file) {
        for (int i = 0; i < 9; i++) {
            // Making Field a class could obviate this
            blocks[i] = field_init;
            rows[i] = field_init;
            cols[i] = field_init;
        }
        string value;
        // Index of the current value being read
        int i{0};
        while  (input_file >> value) {
            try {
                int spot_value = stoi(value);
                if (spot_value) {
                    set_value(i, Field(1 << spot_value));
                }
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
        const int size = 81 * 2 // 81 cells, 2 characters per cell
            + 9 * 2 * 2 // Extra spaces for the block boundaries
            + 9 // 9 newlines
            + 2; // 2 extra newlines for block boundaries
        string result;
        result.reserve(size);
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

            result += field_to_string(cells[i]);
        }
        return result;
    }

    bool solve() {
        vector<int> forced_moves;
        while (true) {
            int solutions_found{0};
            int most_constrained_space{0};
            int most_constrained_count{10};
            Field most_constrained_possibilities;
            for (int i{0}; i < 81; i++) {
                if (cells[i].count() != 0) {
                    // Already solved
                    continue;
                }
                Indices indices(i);
                Field possibilities = blocks[indices.block] & rows[indices.row] & cols[indices.col];
                if (possibilities.count() == 1) {
                    set_value(i, possibilities);
                    forced_moves.push_back(i);
                    solutions_found++;
                } else if (possibilities.count() == 0) {
                    // We've eliminated all possible solutions here
                    clear_moves(forced_moves);
                    return false;
                } else if (possibilities.count() < most_constrained_count) {
                    most_constrained_space = i;
                    most_constrained_count = possibilities.count();
                    most_constrained_possibilities = possibilities;
                }
            }
            forceing_passes++;
            // Continue working
            if (unsolved_spaces == 0) {
                // We won!
                return true;
            }
            if (solutions_found != 0) {
                continue; // Go to another pass
            }

            // Move to guess and check
            vector<Field> moves = get_possible_moves(most_constrained_possibilities);
            guesses++;
            for (auto move: moves) {
                set_value(most_constrained_space, move);
                if (solve()) {
                    return true;
                }
                clear_value(most_constrained_space);
            }
            guesses--;
            clear_moves(forced_moves);
            return false;
        }
    }

};

ostream& operator<<(ostream &out, SudokuProblem const& sudoku) {
    out << sudoku.to_string() << endl;
    return out;
}

int main(int argc, char** argv) {
    string filename = "hard-sudoku.txt";
    if (argc > 1) {
        filename = argv[1];
    }
    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }
    SudokuProblem problem(input_file);
    input_file.close();
    bool succeeded = problem.solve();
    if (succeeded) {
        cout << "Solved!" << endl;
    } else {
        cout << "Failed to solve" << endl;
    }
    // cout << problem.forceing_passes << " forcing passes" << endl;
    // cout << problem.guesses << " guesses" << endl;
    cout << problem << endl;
    return 0;
}

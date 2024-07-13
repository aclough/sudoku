#include <array>
#include <bitset>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

// TODO:  This solver still allocates 84k in heap in O0, comapred to 5.6k in the C
//        version. To get the same performance I'd have to do something about that,
//        but that also risks writing in a non-idiomatic style.

using std::cout, std::endl, std::cerr, std::ifstream, std::string,
      std::bitset, std::array, std::vector, std::ostream;

// A bitset representing all 9 possible numbers that a Sudoku cell could take, plus
// an extra bit in position 0 to make the indexing easier.
// This is used directly to represent values on the board, in which case either no
// bits should be set to represent an unknown value, or exactly one bit should be set
// for a known value.
// Used inside a Field, every set bit is one available value.
using Value = bitset<10>;

// Class to wrap the Value type when used to represent the remaining possiblities for
// a row, column, or block.  Mainly provides iniitalization and type safety.
class Field {
    private:
        Value data;
    public:
        // Nine 1s then a 0 to represent the 9 possible values
        Field() : data(0b11111111110) {}
        Field(Value init) : data(init) {}
        bool test(int i) const {
            return data.test(i);
        }
        void remove(Value value) {
            data &= ~value;
        }
        void add(Value value) {
            data |= value;
        }
        Field operator&(Field other) const {
            return Field(data & other.data);
        }
        int count() const {
            return data.count();
        }
        Value value() const {
            if (data.count() != 1) {
                throw std::invalid_argument("Can only get value for a single solution field");
            }
            return data;
        }
};

// *Note* Fields and Values are both the same size as pointers so there's no benefit to
// passing them by reference.  They're also small enough that copying them is cheap.
string cell_value_to_string(Value value) {
    // Can only convert single solution Value to string
    assert(value.count() <= 1);

    string result;
    for (int i = 1; i <= 9; i++) {
        if (value.test(i)) {
            return std::to_string(i);
        }
    }
    return ".";
}

vector<Value> get_possible_moves(Field field) {
    vector<Value> result;
    for (int i = 1; i <= 9; i++) {
        if (field.test(i)) {
            result.push_back(Value(1 << i));
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
        cout << "Block: " << block << " Row: " <<  row << " Col: " << col << "\n";
    }
};

class SudokuProblem {
private:
    // 0 for unknown,
    // Bits 1-9 set for known value
    array<Value, 81> cells;

    // Stores the remaining unused value in each region.
    array<Field, 9> blocks;
    array<Field, 9> rows;
    array<Field, 9> cols;
    int unsolved_spaces = 81;

    void set_value(const int location, const Value value) {
        assert(cells[location].none());
        assert(value != 0);
        assert(location >= 0 && location < 81);

        cells[location] = value;

        Indices indices(location);
        blocks[indices.block].remove(value);
        rows[indices.row].remove(value);
        cols[indices.col].remove(value);
        unsolved_spaces--;
    }

    void clear_value(const int location) {
        assert(cells[location].any());
        assert(location >= 0 && location < 81);

        Value value = cells[location];
        cells[location].reset();

        Indices indices(location);
        blocks[indices.block].add(value);
        rows[indices.row].add(value);
        cols[indices.col].add(value);
        unsolved_spaces++;
    }

    void clear_moves(const vector<int>& moves) {
        for (auto location: moves) {
            clear_value(location);
            forcing_passes_count--;
        }
    }
public:
    // A copy of the input that produced this problem for debugging purposes.
    string input;
    // How many passes through the different forcings we've made.
    int forcing_passes_count{0};
    // How many times we guessed the value rather than forcing it.
    int guesses_count{0};

    SudokuProblem(string filename) {
        ifstream input_file(filename);
        if (!input_file.is_open()) {
            cout << "Error opening file " << filename << endl;
            throw std::runtime_error("Error opening file");
        }

        int i{0};
        int number{0};
        while (input_file >> number) {
            assert(number >= 0);
            assert(number < 10);
            if(number) set_value(i, Value(1 << number));
            i++;
            input += std::to_string(i);
        }
        input_file.close();

        if (i != 81) {
            cerr << "Got " << i << " numbers" << endl;
            throw std::invalid_argument("Wrong number of numbers for a sudoku puzzle");
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

            result += cell_value_to_string(cells[i]);
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
                    set_value(i, possibilities.value());
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
            forcing_passes_count++;
            // Continue working
            if (unsolved_spaces == 0) {
                // We won!
                return true;
            }
            if (solutions_found != 0) {
                continue; // Go to another pass
            }

            // Move to guess and check
            vector<Value> moves = get_possible_moves(most_constrained_possibilities);
            guesses_count++;
            for (auto move: moves) {
                set_value(most_constrained_space, move);
                if (solve()) {
                    return true;
                }
                clear_value(most_constrained_space);
            }
            guesses_count--;
            clear_moves(forced_moves);
            return false;
        }
    }

};

ostream& operator<<(ostream &out, SudokuProblem const& sudoku) {
    out << sudoku.to_string();
    return out;
}

int main(int argc, char** argv) {
    string filename = "hard-sudoku.txt";
    if (argc > 1) {
        filename = argv[1];
    }
    SudokuProblem problem(filename);
    bool succeeded = problem.solve();
    if (succeeded) {
        cout << "Solved!\n";
    } else {
        cout << "Failed to solve.\n";
    }
    // cout << problem.forcing_passes_count << " forcing passes.\n";
    // cout << problem.guesses_count << " guesses\n.";
    cout << problem << endl;
    return 0;
}

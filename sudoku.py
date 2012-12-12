import sys

possibilities = {1, 2, 3, 4, 5, 6, 7, 8, 9}
BLOCK_SIZE = len(possibilities)
NUM_ROWS = len(possibilities)
NUM_COLS = len(possibilities)
SUDOKU_SIZE = NUM_ROWS * NUM_COLS
NUM_BLOCKS = SUDOKU_SIZE // BLOCK_SIZE
debug = False

class Space:
    """Data structure representing a sudoku space."""
    value = 0
    possible = set(possibilities)

class SudokuGrid:
    """Represents a sudoku grid.

    It includes methods for loading, solving, and displaying."""
    _spaces = []
    _rows = []
    _colums = []
    _boxes = []
    _checks = []
    _check_flag = False
    def __init__(self):
        for i in range(SUDOKU_SIZE):
            self._spaces.append(Space())
            self._checks.append(Space())
        for i in range(NUM_ROWS):
            self._rows.append(self._spaces[i*NUM_COLS:(i+1)*NUM_COLS])
        for i in range(NUM_COLS):
            self._colums.append( self._spaces[i::NUM_ROWS] )
        for i in range(NUM_BLOCKS):
            row_divider = NUM_BLOCKS * 3 // NUM_COLS
            first_row   = (i // row_divider) * 3
            col_mod = NUM_BLOCKS * 3 // NUM_ROWS
            first_colum = (i % col_mod) * 3
            self._boxes.append( [self._rows[x][y] 
                    for x in range(first_row, first_row + 3) 
                    for y in range(first_colum, first_colum + 3) ] )
    def load(self, filename):
        """Load a file as the puzzle."""
        self._load_helper(filename, self._spaces)
    def load_check(self, filename):
        """Load a file as the solution."""
        self._load_helper(filename, self._checks)
        self._check_flag = True
    def _load_helper(self, filename, destination):
        index = 0
        with open(filename, 'r') as data:
            for line in data:
                if line.startswith('#'):
                    continue
                elements = line.split()
                for element in elements:
                    try:
                        destination[index].value = int(element)
                        index += 1
                    except ValueError:
                        print("Invalid number \"" + element + "\"")
        if index != SUDOKU_SIZE:
            raise ValueError("File contained " + str(index) + " elements!\n" +
                    str(SUDOKU_SIZE) + " required")
    def print(self):
        """Print out the state of the sudoku."""
        for row in self._rows:
            line = ""
            for space in row:
                line += str(space.value) + " "
            print(line)
        print("")
    def solve(self):
        """Does a round of solving a sudoku

        This looks both for the only placea a number can go, and the
        only number that can fit in a space.
        You'll need to call this multiple times to actually finish solving.
        It returns the number of blank spaces remaining."""
        if debug: print("***Rows***")
        for row in self._rows: 
            self._reduce_block(row)
            self._fill_block(row)
        if debug: print("***Colums***")
        for colum in self._colums:
            self._reduce_block(colum)
            self._fill_block(colum)
        if debug: print("***Boxes***")
        for box in self._boxes:
            self._reduce_block(box)
            self._fill_block(box)
        #return number of unsolved elements
        return len([element for row in self._rows
                            for element in row 
                            if element.value == 0])
    def _reduce_block(self, block):
        """Takes a block and reduces the possibles to match the values

        Takes a list of Space objects
        It also fills in any values when there is only one associated
        possibilty for an element"""
        if debug: print("*Reducing*")
        values = set([element.value for element in block])
        for element in block:
            element.possible = element.possible.difference(values)
            if element.value != 0:
                element.possible = set()
                continue
            elif len(element.possible) == 1:
                element.value = element.possible.pop()
                if debug: print("!Element now", element.value)
            elif len(element.possible) == 0 and element.value == 0:
                self.print()
                raise Exception("Sudoku not solvable")
        if self._check_flag:
           self.do_check()
    def _fill_block(self, block):
        """Finds places where a number can only go in one place, and inserts it

        Operates on a list of Space objects
        The list must be passed in already reduced"""
        if debug: print("*Filling*")
        for i in possibilities:
            possibles = [element for element in block 
                    if (i in element.possible and element.value == 0)]
            if len(possibles) == 1:
                if possibles[0].value != 0:
                    raise Exception("Overwrote value in fill " +
                            str(i) + " over " + str(possibles[0].value))
                    possibles[0].value = i
                    possibles[0].possible = set()
                    if debug: print("!Element now", i)

        if self._check_flag:
            self.do_check()
    def do_check(self):
       """Checks to solution being made

       Compares the current progress on the solution to an official solution
       provided, element by element"""
       for index in range(SUDOKU_SIZE):
           if self._checks[index].value == 0:
               continue
           if self._spaces[index].value == 0:
               continue
           if self._checks[index].value != self._spaces[index].value:
               raise Exception("Generated table does not match solution at " 
                       + str(index))



if __name__=='__main__':
    grid = SudokuGrid()
    grid.load(sys.argv[1])
    grid.print()
    if(len(sys.argv) > 2):
        grid.load_check(sys.argv[2])
    grid.do_check()
    unsolved = grid.solve()
    old_unsolved = unsolved
    while(unsolved != 0):
        unsolved = grid.solve()
        print(unsolved, "remaining")
        if old_unsolved == unsolved:
            print("Can't solve")
            break
        else:
            old_unsolved = unsolved
    grid.print()

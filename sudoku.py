import sys

numbers = {1,2,3,4,5,6,7,8,9}
debug = False

class Puzzle:
    """Represents a sudoku grid.

    It includes methods for loading, solving, and displaying."""
    _spaces = [] # All the spaces in the puzzle [Space]
    _possibles = []
    _solution = []
    def __init__(self, origonal=None):
        if debug: print("Starting init")
        if origonal:
            self._unsolved = origonal._unsolved
            self._spaces = []
            self._possibles = []
            if origonal._solution:
                self._solution = origonal._solution
            # Can't just copy the lists, we need to mutate the new copy.
            for space in origonal._spaces:
                self._spaces.append(space)
            for possible in origonal._possibles:
                self._possibles.append(set(possible))
        if debug: print("Finishing init")

    def get_row_iter(self, x):
        for i in range(9):
            index = 9*x + i
            yield self._spaces[index], self._possibles[index]
    def get_col_iter(self, x):
        for i in range(9):
            index = x + 9*i
            yield self._spaces[index], self._possibles[index]
    def get_blk_iter(self, x):
        block_index = 3*(x%3) + 27*(x//3)
        for i in range(9):
            index = block_index + (i%3) + 9*(i//3)
            yield self._spaces[index], self._possibles[index]

    def load(self, filename):
        """Load a file as the puzzle."""
        self._spaces = []
        self._possibles = []
        self._unsolved = 0
        self._load_helper(filename, self._spaces)
        for space in self._spaces:
            self._possibles.append( set() if space else set(numbers) )
            if not space: self._unsolved += 1
    def load_solution(self, filename):
        """Load a file as the solution."""
        self._solution = []
        self._load_helper(filename, self._solution)
    def _load_helper(self, filename, destination):
        with open(filename, 'r') as data:
            for line in data:
                if line.startswith('#'):
                    continue
                elements = line.split()
                for element in elements:
                    try:
                        destination.append( int(element))
                    except ValueError:
                        print('Invalid number "' + element + '"')
        if len(destination) is not len(numbers)**2:
            raise ValueError("File contained " + str(index) + " elements!")

    def print(self):
        """Print out the state of the sudoku."""
        for x in range(9):
            print([ str(num) + " " for num,pos in self.get_row_iter(x)])
        print("")

    def solve(self):
        """Solves the sudoku

        First tries to do as much solving as possible without guessing,
        then finds the space with the fewest possibilities, creates a new
        Puzzle there with a guess at that space, calls solve() on that puzzle
        and if it doens't work guesses again"""
        while True:
            for i in range(9):
                self._reduce(self.get_row_iter(i))
                self._reduce(self.get_col_iter(i))
                self._reduce(self.get_blk_iter(i))
            for i in range(len(self._spaces)):
                if len( self._possibles[i]) is 1:
                    self._spaces[i] = self.possibles[i].pop()
            unsolved, index = self.check()
            if unsolved < self._unsolved:
                if debug: print(unsolved)
                self._unsolved = unsolved
                if unsolved == 0:
                    self.print()
                    return self._spaces
            else:
                break
        if debug: print("Entering")
        for num in self._possibles[index]:
            new_puzzle = Puzzle(self)
            new_puzzle._spaces[index] = num
            try:
                self._spaces = new_puzzle.solve()
                return self._spaces
            except ValueError:
                continue
            if debug: print("Exiting")
            raise ValueError("No solution found")
    
    def check(self):
        """Performs pre-recursion checks

        Checks to see how much remains to solve, what index we should guess
        at to recurse (if neccesary), and checks against the known solution
        if one is present"""
        min = 999; min_index = 0; unsolved = 0
        for i in range(len(self._spaces)):
            num = self._spaces[i]
            if len(self._possibles[i]) < min and not self._spaces[i]:
                min_index = i
            if num == 0:
                unsolved += 1
            if self._solution:
                if num is not self._solution[i] and num is not 0:
                    if debug: print("Exiting2")
                    raise ValueError("Sudoku not consistant with solution")
        if min is 0:
            if debug: print("Exiting3")
            raise ValueError("Sudoku cannot be solved due to overrestriction")
        return unsolved, min_index

    def _reduce(self, local_iter):
       value_set = set(i[0] for i in local_iter)
       for num,pos in local_iter:
           pos.difference_update(value_set)


if __name__=='__main__':
    puzzle = Puzzle()
    puzzle.load(sys.argv[1])
    puzzle.print()
    if(len(sys.argv) > 2):
        puzzle.load_solution(sys.argv[2])
    puzzle.solve()
    puzzle.print()

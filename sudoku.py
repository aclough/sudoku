import sys

class Puzzle:
    "A represenation of a sudoku puzzle"

    _allowed = {x for x in range(1,10)}

    def __init__(self):
        self.spaces = [0] * 81
        self.rowFree = []
        self.colFree = []
        self.blkFree = []
        for i in range(9):
            self.rowFree.append(self._allowed.copy())
            self.colFree.append(self._allowed.copy())
            self.blkFree.append(self._allowed.copy())

    def __iter__(self):
        return iter(self.spaces)

    def getFree(self, loc):
        rowFree = self.rowFree[loc // 9]
        colFree = self.colFree[loc % 9]
        blkFree = self.blkFree[(loc // 27) + (((loc % 9) // 3) * 3)]
        return rowFree, colFree, blkFree

    def set(self, loc, val):
        if not val in self._allowed:
            raise ValueError("{} not valid space value".format(val))
        rowFree, colFree, blkFree = self.getFree(loc)
        if val in rowFree and val in colFree and val in blkFree:
            self.spaces[loc] = val
            rowFree.remove(val)
            colFree.remove(val)
            blkFree.remove(val)
        else:
            raise IndexError("Illegal move")

    def unset(self, loc, val):
        if not val in self._allowed:
            raise ValueError("{} not valid space value".format(val))
        rowFree, colFree, blkFree = self.getFree(loc)
        self.spaces[loc] = 0
        rowFree.add(val)
        colFree.add(val)
        blkFree.add(val)

    def load(self, filename):
        """Load a file as the puzzle."""
        with open(filename, 'r') as f:
            vals = [x for line in f for x in line.split()]
            if len(vals) != 81:
                raise IndexError("File not a sudoku")
            for i, v in enumerate(vals):
                if (v != "0"):
                    self.set(i, int(v))

    def print(self):
        for i, v in enumerate(self):
            if i == 0:
                pass
            elif i % 27 == 0:
                print("\n\n", end="")
            elif i % 9 == 0:
                print("\n", end="")
            elif i % 3 == 0:
                print("   ", end="")
            else:
                print(" ", end="")
            print(str(v) + " ", end="")


if __name__=='__main__':
    puzzle = Puzzle()
    puzzle.load(sys.argv[1])
    puzzle.print()
    #puzzle.solve()
    print("\n\n\n")
    puzzle.print()

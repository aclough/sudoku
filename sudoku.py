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

    def getFreeSets(self, loc):
        rowFree = self.rowFree[loc // 9]
        colFree = self.colFree[loc % 9]
        blkFree = self.blkFree[(loc // 27) + (((loc % 9) // 3) * 3)]
        return rowFree, colFree, blkFree

    def getFree(self, loc):
        row, col, blk = self.getFreeSets(loc)
        return row & col & blk

    def set(self, loc, val):
        if not val in self._allowed:
            raise ValueError("{} not valid space value".format(val))
        rowFree, colFree, blkFree = self.getFreeSets(loc)
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
        rowFree, colFree, blkFree = self.getFreeSets(loc)
        self.spaces[loc] = 0
        rowFree.add(val)
        colFree.add(val)
        blkFree.add(val)

    def trySolveWith(self, loc, toTry):
        self.set(loc, toTry)
        try:
            self.solve()
        except (ValueError, IndexError):
            self.unset(loc, toTry)
            raise

    def load(self, filename):
        """Load a file as the puzzle."""
        with open(filename, 'r') as f:
            vals = [x for line in f for x in line.split()]
            if len(vals) != 81:
                raise IndexError("File not a sudoku")
            for i, v in enumerate(vals):
                if (v != "0"):
                    self.set(i, int(v))

    def __str__(self):
        slist = ['\n']*3
        for i, v in enumerate(self):
            if i == 0:
                pass
            elif i % 27 == 0:
                slist.append("\n\n")
            elif i % 9 == 0:
                slist.append("\n")
            elif i % 3 == 0:
                slist.append("   ")
            else:
                slist.append(" ")
            slist.append(str(v))
        return "".join(slist)


    def solve(self):
        leastFree = len(self._allowed) + 1
        bestMove = None
        for i, v in enumerate(self):
            frees = self.getFree(i)
            freeSize = len(frees)
            if v != 0:
                continue
            if freeSize == 0:
                raise ValueError("Sudoku unsolvable")
            elif freeSize == 1:
                bestMove = None
                self.trySolveWith(i, frees.pop())
                break;
            elif freeSize < leastFree:
                leastFree = freeSize
                bestMove = i
        print(self)
        if bestMove:
            frees = self.getFree(bestMove)
            for v in frees:
                try:
                    self.trySolveWith(bestMove, v)
                    break
                except (ValueError, IndexError):
                    pass
            else:
                raise ValueError("No solutions on this branch")


if __name__=='__main__':
    puzzle = Puzzle()
    puzzle.load(sys.argv[1])
    print(puzzle)
    try:
        puzzle.solve()
    except (ValueError, IndexError):
        print("\n\nFailed to solve, here's the current state")
    print(puzzle)

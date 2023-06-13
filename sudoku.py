from typing import Iterator
import sys


class Puzzle:
    "A represenation of a sudoku puzzle"

    DEBUG = False

    _allowed = {x for x in range(1, 10)}

    def __init__(self):
        self.cells = [0] * 81
        self.rowFree = []
        self.colFree = []
        self.blkFree = []
        for i in range(9):
            self.rowFree.append(self._allowed.copy())
            self.colFree.append(self._allowed.copy())
            self.blkFree.append(self._allowed.copy())

    def __iter__(self) -> Iterator[float]:
        return iter(self.cells)

    def getFreeSets(self, loc: int) -> tuple[set[int], set[int], set[int]]:
        rowFree = self.rowFree[loc // 9]
        colFree = self.colFree[loc % 9]
        blkFree = self.blkFree[(loc // 27) + (((loc % 9) // 3) * 3)]
        return rowFree, colFree, blkFree

    def getFree(self, loc: int):
        row, col, blk = self.getFreeSets(loc)
        return row & col & blk

    def set(self, loc: int, val: int):
        if val not in self._allowed:
            raise ValueError(f"{val} not valid space value")
        rowFree, colFree, blkFree = self.getFreeSets(loc)
        if val in rowFree and val in colFree and val in blkFree:
            self.cells[loc] = val
            rowFree.remove(val)
            colFree.remove(val)
            blkFree.remove(val)
        else:
            raise IndexError("Illegal move")

    def unset(self, loc: int, val: int):
        if val not in self._allowed:
            raise ValueError(f"{val} not valid space value")
        rowFree, colFree, blkFree = self.getFreeSets(loc)
        self.cells[loc] = 0
        rowFree.add(val)
        colFree.add(val)
        blkFree.add(val)

    def trySolveWith(self, loc: int, toTry: int):
        self.set(loc, toTry)
        try:
            self.solve()
        except (ValueError, IndexError):
            self.unset(loc, toTry)
            raise

    def load(self, filename: str):
        """Load a file as the puzzle."""
        with open(filename, 'r') as f:
            vals = [x for line in f for x in line.split()]
            if len(vals) != 81:
                raise IndexError("File not a sudoku")
            for i, v in enumerate(vals):
                if (v != "0"):
                    self.set(i, int(v))

    def __str__(self):
        result_string = ['\n']
        for i, v in enumerate(self):
            if i == 0:
                pass
            elif i % 27 == 0:
                result_string.append("\n")
                result_string.append("-"*21)
                result_string.append("\n")
            elif i % 9 == 0:
                result_string.append("\n")
            elif i % 3 == 0:
                result_string.append(" | ")
            else:
                result_string.append(" ")
            result_string.append(str(v))
        result_string.append('\n')
        return "".join(result_string)

    def solve(self):
        leastFree = len(self._allowed) + 1
        bestMove = None
        for i, v in enumerate(self):
            if v != 0:
                # We've already assigned a value here
                continue
            frees = self.getFree(i)
            freeSize = len(frees)
            if freeSize == 0:
                raise ValueError("Sudoku unsolvable")
            elif freeSize < leastFree:
                leastFree = freeSize
                bestMove = i
            if freeSize == 1:
                break
        if self.DEBUG:
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


if __name__ == '__main__':
    puzzle = Puzzle()
    puzzle.load(sys.argv[1])
    print(puzzle)
    try:
        puzzle.solve()
    except (ValueError, IndexError):
        print("\n\nFailed to solve, here's the current state")
    print(f"\nSolution is:\n{puzzle}")

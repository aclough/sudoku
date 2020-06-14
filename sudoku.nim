import os

const debug = true

# This is a quick program to solve a sudoku
# Sudokus are represented as an array of 81 values, set to 1 to 9 if
# they've been given a value and set to 0 if they're still blank.
#
# Each cell in the grid is a member of three segments: the row, the
# column, and the block which are used to constrain placement in that
# spot.
#
# This solver works by a heuristic guided search that does not need to
# create a copy for each guess because everything needed to back out a
# guess is stored on the stack.

type
  TSegmentNums = tuple[blck,col,row: int]
  TPossibles = set[1..9] # Values remaining for placement in a segment
  TSudoku = tuple[grid: array[0..80, 0..9], blck,col,row:array[0..8, TPossibles]]

proc getIndices(i: int): TSegmentNums =
  result.row = i div 9
  result.col = i mod 9
  result.blck = (result.row div 3) + 3*(result.col div 3)

proc getAllIndices(): array[0..80, TSegmentNums] =
  for i in 0..80:
    result[i] = getIndices(i)

# I love that Nim can do this
# In theory these are looked up exponentially more than the number of Sudoku spaces
# but heurists and forced moves mean that the difference is less than a factor of 10
# Still, that's speed savings especially if there's function call overhead with just
# using getIndices direction division can be slow, and we don't have enough data here
# that I'm worried about anything getting pushed out of L1$
const segmentIndices = getAllIndices()


proc `$`(sudoku: TSudoku): string =
  var str = ""
  for x in 0..8:
    for y in 0..8:
      if (y mod 3) == 0:
        str = str & " "
      str = str & " " & $sudoku.grid[9 * x + y]
    str &= "\n"
    if (x mod 3) == 2:
      str &= "\n"
  return str


proc setInitialConstraints(sudoku: var TSudoku) =
  var all: TPossibles
  for i in 1..9:
    incl(all, i)
  for i in 0..8:
    sudoku.blck[i] = all
    sudoku.col[i] = all
    sudoku.row[i] = all

proc setLoc(sudoku: var TSudoku, index, value: int) =
  let (blck, col, row) = segmentIndices[index]
  sudoku.grid[index] = value
  excl(sudoku.blck[blck], value)
  excl(sudoku.col[col], value)
  excl(sudoku.row[row], value)

proc clrLoc(sudoku: var TSudoku, index, value: int) =
  let (blck, col, row) = segmentIndices[index]
  when debug:
    if sudoku.grid[index] != value:
      raise newException(ValueError, $index & " cannot be cleared of value " & $value)
  sudoku.grid[index] = 0
  incl(sudoku.blck[blck], value)
  incl(sudoku.col[col], value)
  incl(sudoku.row[row], value)

proc loadSudoku(fileName: string): TSudoku =
  result.setInitialConstraints()
  var file = open(fileName)
  try:
    var index = 0
    while true:
      let c = readChar(file)
      let n = int(c) - int('0')
      if n >= 0 and n <= 10:
        if n != 0:
          result.setLoc(index, n)
        index += 1
      if index == 81:
        break
  except ValueError:
    echo "Could not parse input file"
  except IOError:
    echo "IO error"
  finally:
    close(file)

# I'm not sure that this template actually buys me any real clarity over
# just substituting the code manually since it's only used twice and people
# looking this this will probably be surprised by the fact that it causes
# returns.  But I'm really writing this to learn Nimrod so might as well put
# it in.  I guess I do prefer Rust's requiring macros to have a '!' to warn
# readers in cases like this.
template backout(sudoku: TSudoku, toRemove: seq[tuple[i, n: int]]) =
  for b in toRemove:
    sudoku.clrLoc(b.i, b.n)
  return false


proc solve(sudoku: var TSudoku): bool =
  var
    mostConstrained: int
    possibilities: TPossibles
    eagerMoves: seq[tuple[i,n:int]] = @[]
  block getMostConstrained:
    var lowCount = 100
    for i in 0..80:
      if sudoku.grid[i] != 0:
        continue
      let (blck, col, row) = segmentIndices[i]
      let possibles = sudoku.blck[blck] * sudoku.col[col] * sudoku.row[row]
      let count = card(possibles)
      if count < lowCount:
        if count == 0:
          sudoku.backout(eagerMoves) # No valid moves so back out of this search branch
        elif count == 1:
          # There's a single valid move for this spot so we'll set it eagerly now rather
          # than waiting and trying it.
          for n in possibles:
            sudoku.setLoc(i,n)
            eagerMoves.add((i,n))
          continue
        else:
          mostConstrained = i
          possibilities = possibles
          lowCount = count
    if lowCount == 100: # We never found a blank square
      return true
  for n in possibilities:
    sudoku.setLoc(mostConstrained, n)
    if sudoku.solve():
      return true
    sudoku.clrLoc(mostConstrained, n)
  sudoku.backout(eagerMoves) # there were no valid moves at c.pos so go back

var
  m_sudoku: TSudoku
try:
  m_sudoku = loadSudoku(commandLineParams()[0])
except ValueError:
  m_sudoku = loadSudoku("puzzle.txt")
echo m_sudoku

echo "Solving"

if solve(m_sudoku):
  echo "Solved it!"
  echo m_sudoku
else:
  echo "Unable to solve"

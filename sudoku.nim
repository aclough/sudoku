import strutils, os

# This is a quick problem to solve a sudoku
# Sudokus are represented as an array of 81 values, set to 1 to 9 if
# they've been given a value and set to 0 if they're still blank.
#
# Each cell in the grid is a member of three segments: the row, the
# column, and the block which are used to constrain placement in that
# spot.  For brevity row, column, and block are refered to as r,c, and b
#
# This solver works by a heuristic guided search that does not need to
# create a copy for each guess because everything needed to back out a
# guess is stored on the stack.

type
  TSegmentNums = tuple[b,c,r:int]
  TPossibles = set[1..9] # Values remaining for placement in a segment
  TSudoku = tuple[grid: array[0..80, 0..9], b,c,r:array[0..8, TPossibles]]

proc getSegments(i: int): TSegmentNums =
  result.r = i div 9
  result.c = i mod 9
  result.b = (result.r div 3) + 3*(result.c div 3)

proc setInitialConstraints(sudoku: var TSudoku) =
  var all: TPossibles
  for i in 1..9:
    incl(all, i)
  for i in 0..8:
    sudoku.b[i] = all
    sudoku.c[i] = all
    sudoku.r[i] = all

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

proc setLoc(sudoku: var TSudoku, index, value: int) =
  let s = getSegments(index)
  sudoku.grid[index] = value
  excl(sudoku.b[s.b], value)
  excl(sudoku.c[s.c], value)
  excl(sudoku.r[s.r], value)

proc clrLoc(sudoku: var TSudoku, index, value: int) =
  let s = getSegments(index)
  sudoku.grid[index] = 0
  incl(sudoku.b[s.b], value)
  incl(sudoku.c[s.c], value)
  incl(sudoku.r[s.r], value)

proc loadSudoku(fileName: string): TSudoku =
  result.setInitialConstraints()
  var file = open(fileName)
  try:
    var index = 0
    while true:
      let c = readChar(file)
      try:
        let n = parseInt($c) # There must be a better way than "parseInt($c)"
        if n != 0:
          result.setLoc(index, n)
        index += 1
      except EInvalidValue:
        continue
      if index == 81:
        break
  except EInvalidValue:
    echo "Could not parse input file"
  except EIO:
    echo "IO error"
  finally:
    close(file)


proc getMostConstrained(sudoku: TSudoku): tuple[index: int, pos: TPossibles, finished, fail: bool] =
  var numPos = 100;
  for i in 0..80:
    if sudoku.grid[i] != 0:
      continue
    let s = getSegments(i)
    let pos = sudoku.b[s.b] * sudoku.c[s.c] * sudoku.r[s.r]
    let num = card(pos)
    if num < numPos:
      result.index = i
      result.pos = pos
      numPos = num
      if num == 0:
        result.fail = true
  if numPos == 100: # We never found a blank square
    result.finished = true

proc solve(sudoku: var TSudoku): bool =
  let c = getMostConstrained(sudoku)
  if c.finished:
    return true
  if c.fail:
    return false
  for n in c.pos:
    sudoku.setLoc(c.index, n)
    if sudoku.solve():
      return true
    sudoku.clrLoc(c.index, n)
  return false # there were no valid moves at c.pos so go back

var
  m_sudoku: TSudoku
try:
  m_sudoku = loadSudoku(commandLineParams()[0])
except EInvalidIndex:
  m_sudoku = loadSudoku("puzzle.txt")
echo m_sudoku

echo "Solving"

if solve(m_sudoku):
  echo "Solved it!"
  echo m_sudoku
else:
  echo "Unable to solve"

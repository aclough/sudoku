import strutils, os

# This is a quick problem to solve a sudoku
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
  TSegmentNums = tuple[blck,col,row:int]
  TPossibles = set[1..9] # Values remaining for placement in a segment
  TSudoku = tuple[grid: array[0..80, 0..9], blck,col,row:array[0..8, TPossibles]]

proc getSegmentIndices(i: int): TSegmentNums =
  result.row = i div 9
  result.col = i mod 9
  result.blck = (result.row div 3) + 3*(result.col div 3)

proc setInitialConstraints(sudoku: var TSudoku) =
  var all: TPossibles
  for i in 1..9:
    incl(all, i)
  for i in 0..8:
    sudoku.blck[i] = all
    sudoku.col[i] = all
    sudoku.row[i] = all

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
  let s = getSegmentIndices(index)
  sudoku.grid[index] = value
  excl(sudoku.blck[s.blck], value)
  excl(sudoku.col[s.col], value)
  excl(sudoku.row[s.row], value)

proc clrLoc(sudoku: var TSudoku, index, value: int) =
  let s = getSegmentIndices(index)
  sudoku.grid[index] = 0
  incl(sudoku.blck[s.blck], value)
  incl(sudoku.col[s.col], value)
  incl(sudoku.row[s.row], value)

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


proc getMostConstrained(sudoku: TSudoku): tuple[index: int, possibles: TPossibles, finished, fail: bool] =
  var lowNum = 100;
  for i in 0..80:
    if sudoku.grid[i] != 0:
      continue
    let s = getSegmentIndices(i)
    let possibles = sudoku.blck[s.blck] * sudoku.col[s.col] * sudoku.row[s.row]
    let num = card(possibles)
    if num < lowNum:
      result.index = i
      result.possibles = possibles
      lowNum = num
      if num == 0:
        result.fail = true
  if lowNum == 100: # We never found a blank square
    result.finished = true

proc solve(sudoku: var TSudoku): bool =
  let c = getMostConstrained(sudoku)
  if c.finished:
    return true
  if c.fail:
    return false
  for n in c.possibles:
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

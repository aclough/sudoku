import strutils, os

type
  TSegmentNums = tuple[b,c,r:int]
  TPossibles = set[1..9]
  TSudoku = tuple[grid: array[0..80, 0..9], b,c,r:array[0..8, TPossibles]]

proc getRow(i: int): int =
  return i div 9

proc getColumn(i: int): int =
  return i mod 9

proc getBlock(i: int): int =
  return (getRow(i) div 3) + 3*(getColumn(i) div 3)

proc getSegments(i: int): TSegmentNums =
  result.b = getBlock(i)
  result.r = getRow(i)
  result.c = getColumn(i)

proc setInitialConstraints(sudoku: var TSudoku) =
  var full: TPossibles
  for i in 1..9:
    incl(full, i)
  for i in 0..8:
    sudoku.b[i] = full
    sudoku.c[i] = full
    sudoku.r[i] = full

# Utility functions

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
  setInitialConstraints(result)
  var file = open(fileName)
  try:
    var index = 0
    while true:
      let c = readChar(file)
      try:
        let n = parseInt($c) # There must be a better way than "parseInt($c)"
        if n != 0:
          setLoc(result, index, n)
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


proc getMostConstrained(sudoku: TSudoku): tuple[most: int, pos: TPossibles, fin, fail: bool] =
  var numPos = 100;
  for i in 0..80:
    if sudoku.grid[i] != 0:
      if i == 80 and numPos == 100:
        result.fin = true
      continue
    let s = getSegments(i)
    let pos = sudoku.b[s.b] * sudoku.c[s.c] * sudoku.r[s.r]
    let num = card(pos)
    if num < numPos:
      result.most = i
      result.pos = pos
      numPos = num
      if num == 1:
        break
      elif num == 0:
        result.fail = true

proc solve(sudoku: var TSudoku): bool =
  let c = getMostConstrained(sudoku)
  if c.fin:
    return true
  if c.fail:
    return false
  for n in c.pos:
    setLoc(sudoku, c.most, n)
    if solve(sudoku):
      return true
    clrLoc(sudoku, c.most, n)
  return false

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
  echo "Error"

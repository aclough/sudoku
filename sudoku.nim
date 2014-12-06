import strutils, os

type
  TSegmentNums = tuple[b,c,r:int]
  TPossibles = set[1..9]
  TSudoku = array[0..80, 0..9] # 0 is blank
  TConstraints = tuple[b,c,r:array[0..8, TPossibles]]

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

proc getInitialConstraints(): TConstraints =
  var full: TPossibles
  for i in 1..9:
    incl(full, i)
  for i in 0..8:
    result.b[i] = full
    result.c[i] = full
    result.r[i] = full

# Utility functions

proc `$`(sudoku: TSudoku): string =
  var str = ""
  for x in 0..8:
    for y in 0..8:
      if (y mod 3) == 0:
        str = str & " "
      str = str & " " & $sudoku[9 * x + y]
    str &= "\n"
    if (x mod 3) == 2:
      str &= "\n"
  return str

proc setLoc(sudoku: var TSudoku, con: var TConstraints, index, value: int) =
  let s = getSegments(index)
  sudoku[index] = value
  excl(con.b[s.b], value)
  excl(con.c[s.c], value)
  excl(con.r[s.r], value)

proc clrLoc(sudoku: var TSudoku, con: var TConstraints, index, value: int) =
  let s = getSegments(index)
  sudoku[index] = 0
  incl(con.b[s.b], value)
  incl(con.c[s.c], value)
  incl(con.r[s.r], value)

proc loadSudoku(fileName: string, con: var TConstraints): TSudoku =
  var file = open(fileName)
  try:
    var index = 0
    while true:
      let c = readChar(file)
      try:
        let n = parseInt($c) # There must be a better way than "parseInt($c)"
        if n != 0:
          setLoc(result, con, index, n)
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


proc getMostConstrained(sudoku: TSudoku, con: TConstraints): tuple[most: int, pos: TPossibles, fin, fail: bool] =
  var numPos = 100;
  for i in 0..80:
    if sudoku[i] != 0:
      if i == 80 and numPos == 100:
        result.fin = true
      continue
    let s = getSegments(i)
    let pos = con.b[s.b] * con.c[s.c] * con.r[s.r]
    let num = card(pos)
    if num < numPos:
      result.most = i
      result.pos = pos
      numPos = num
      if num == 1:
        break
      elif num == 0:
        result.fail = true

proc solve(sudoku: var TSudoku, con: var TConstraints): bool =
  let c = getMostConstrained(sudoku, con)
  if c.fin:
    return true
  if c.fail:
    return false
  for n in c.pos:
    setLoc(sudoku, con, c.most, n)
    if solve(sudoku, con):
      return true
    clrLoc(sudoku, con, c.most, n)
  return false

var
  m_sudoku: TSudoku
  m_con = getInitialConstraints()
try:
  m_sudoku = loadSudoku(commandLineParams()[0], m_con)
except EInvalidIndex:
  m_sudoku = loadSudoku("puzzle.txt", m_con)
echo m_sudoku

echo "Solving"

if solve(m_sudoku, m_con):
  echo "Solved it!"
  echo m_sudoku
else:
  echo "Error"

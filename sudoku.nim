import strutils

type
  TSudoku = array[0..80, 0..9] # 0 is blank
  TSegment = array[0..8, int]
  TSegmentList = array[0..8, Tsegment]
  TPossibles = set[1..9]
  TSegmentPosList = array[0..8, TPossibles]

# Create indexing constants

proc getRow(i: int): int =
  return i div 9

proc getColumn(i: int): int =
  return i mod 9

proc getRowSegments(): TSegmentList =
  for i in low(TSudoku)..high(TSudoku):
    result[getRow(i)][getColumn(i)] = i

proc getColumnSegments(): TSegmentList =
  for i in low(TSudoku)..high(TSudoku):
    result[getColumn(i)][getRow(i)] = i

proc getBlock(i: int): int =
  return (getRow(i) div 3) + 3*(getColumn(i) div 3)

proc getBlockIndex(i: int): int =
  return (getRow(i) mod 3) + 3*(getColumn(i) mod 3)

proc getBlockSegments(): TSegmentList =
  for i in low(TSudoku)..high(TSudoku):
    result[getBlock(i)][getBlockIndex(i)] = i

const
  blocks = getBlockSegments()
  rows = getRowSegments()
  columns = getColumnSegments()

# Create Variables

proc getInitialSegmentPos(): TSegmentPosList =
  var full: TPossibles
  for i in 1..9:
    incl(full, i)
  for i in low(result)..high(result):
    result[i] = full

var
  sudoku: TSudoku
  blockPos: TSegmentPosList = getInitialSegmentPos()
  colPos: TSegmentPosList = getInitialSegmentPos()
  rowPos: TSegmentPosList = getInitialSegmentPos()

# Utility functions

proc printSudoku(sudoku: TSudoku) =
  echo ""
  for x in 0..8:
    var str = ""
    for y in 0..8:
      if (y mod 3) == 0:
        str = str & " "
      str = str & " " & $sudoku[9 * x + y]
    echo str
    if (x mod 3) == 2:
      echo ""

proc setLoc(index, value: int) =
  let b = getBlock(index)
  let r = getRow(index)
  let c = getColumn(index)
  if value in (blockPos[b] * colPos[c] * rowPos[r]):
    sudoku[index] = value
    excl(blockPos[b], value)
    excl(colPos[c], value)
    excl(rowPos[r], value)
  else:
    echo("Error setting ", index, " to ", value)
    printSudoku(sudoku)
    echo blockPos[b]
    echo colPos[c]
    echo rowPos[r]
    echo (blockPos[b] + colPos[c] + rowPos[r])
    raise newException(EOS, "set fail")

proc clrLoc(index, value: int) =
  let b = getBlock(index)
  let r = getRow(index)
  let c = getColumn(index)
  if value notin (blockPos[b] + colPos[c] + rowPos[r]):
    sudoku[index] = 0
    incl(blockPos[b], value)
    incl(colPos[c], value)
    incl(rowPos[r], value)
  else:
    echo("Error clearing ", index, " of ", value)
    printSudoku(sudoku)
    echo blockPos[b]
    echo colPos[c]
    echo rowPos[r]
    echo (blockPos[b] + colPos[c] + rowPos[r])
    raise newException(EOS, "clear fail")

proc loadSudoku(fileName: String): TSudoku =
  var file = open(fileName)
  try:
    var index = 0
    while true:
      let c = readChar(file)
      try:
        let n = parseInt($c) # This is a hack and I should figure out a better way
        if n != 0:
          setLoc(index, n)
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


proc getMostConstrained(): int =
  var numPos = 100;
  for i in 0..80:
    if sudoku[i] != 0:
      continue
    let b = getBlock(i)
    let r = getRow(i)
    let c = getColumn(i)
    let pos = blockPos[b] * colPos[c] * rowPos[r]
    let num = card(pos)
    if num < numPos:
      result = i
      numPos = num
  echo(result, " ", numPos)

proc solve(): bool =
  let i = getMostConstrained()
  if i == 0:
    return true
  if sudoku[i] != 0:
    return false
  let b = getBlock(i)
  let r = getRow(i)
  let c = getColumn(i)
  let pos = blockPos[b] * colPos[c] * rowPos[r]
  if pos == {}:
    return false
  for n in pos:
    setLoc(i, n)
    if solve():
      return true
    clrLoc(i, n)
  return false

sudoku = loadSudoku("puzzle.txt")
printSudoku(sudoku)

echo "Solving"

if solve():
  echo "Solved it!"
  printSudoku sudoku
else:
  echo "Error"

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

proc loadSudoku(fileName: String): TSudoku =
  var file = open(fileName)
  try:
    var index = 0
    while true:
      let c = readChar(file)
      echo c
      try:
        result[index] = parseInt($c) # This is a hack and I should figure out
                                     # a better way
        echo result[index]
        index += 1
      except EInvalidValue:
        continue
      if index == 81:
        break
    echo "success"
  except EInvalidValue:
    echo "Could not parse input file"
  except EIO:
    echo "IO error"
  finally:
    close(file)

sudoku = loadSudoku("puzzle.txt")
printSudoku(sudoku)

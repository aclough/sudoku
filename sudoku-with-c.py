import sys
import os
import ctypes as c

_file = 'sudoku.so'
_path = os.path.join(os.path.split(os.path.realpath(__file__))[0], _file)
sudokuer = c.cdll.LoadLibrary(_path)

class Sudoku(c.Structure):
    _fields_ = [('elements', c.c_int * 81)]
s = Sudoku()

sudokuer.load_sudoku(c.byref(s), c.c_char_p(sys.argv[1].encode()))

print(sudokuer.solve_sudoku(c.byref(s)))

sudokuer.print_sudoku(c.byref(s))

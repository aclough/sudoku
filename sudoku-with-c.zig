const std = @import("std");
const c = @cImport({
    @cInclude("sudoku.h");
});

pub fn main() void {
    var puzzle: c.sudoku = undefined;
    var i: u32 = 0;
    while (i < 100_000) : (i += 1) {
        const loaded = c.load_sudoku(&puzzle, "puzzle.txt");
        if (loaded != 0) {
            std.debug.print("Failed to load puzzle\n", .{});
            return;
        }
        const solved = c.solve_sudoku(&puzzle);
        if (solved != 0) {
            std.debug.print("Failed to solve puzzle\n", .{});
            return;
        }
        if (i == (100_000 - 1)) {
            std.debug.print("Solved puzzle\n", .{});
            c.print_sudoku(&puzzle);
        }
    }
}

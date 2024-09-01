const std = @import("std");
const c = @cImport({
    @cInclude("sudoku.h");
});

pub fn main() !void {
    var filename: [*c]const u8 = "puzzle.txt";
    var iterations: u32 = 100_000;

    var args = std.process.args();
    var arg_count: usize = 0;
    while (args.next()) |arg| {
        arg_count += 1;
        if (arg_count == 2) {
            filename = arg;
        } else if (arg_count == 3) {
            iterations = try std.fmt.parseInt(u32, arg, 10);
        }
    }

    var puzzle: c.sudoku = undefined;
    var i: u32 = 0;
    while (i < iterations) : (i += 1) {
        const loaded = c.load_sudoku(&puzzle, filename);
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

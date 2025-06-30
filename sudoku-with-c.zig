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

    var master_puzzle: c.sudoku = undefined;
    var puzzle: c.sudoku = undefined;
    var i: u32 = 0;
    const loaded = c.load_sudoku(&master_puzzle, filename);
    if (loaded != 0) {
        std.debug.print("Failed to load puzzle\n", .{});
        return;
    }
    while (i < iterations) : (i += 1) {
        c.copy_sudoku(&puzzle, &master_puzzle);
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

const std = @import("std");

// The bit field type used to represent possible values (1-9) in a cell
// Using u16 for consistency with other implementations
const Field = u16;

// Structure to represent a Sudoku puzzle
const Sudoku = struct {
    // The final derived value for each cell. 0 if still unknown or a bitfield with a single
    // bit set if the solution has been found.
    cells: [81]Field,

    // Remaining possibilities for each block, column, or row.
    blocks: [9]Field,
    cols: [9]Field,
    rows: [9]Field,

    // Number of remaining unsolved cells
    remaining: usize,

    // Initialize the Sudoku with all cells empty and all possibilities available
    pub fn init() Sudoku {
        // Bits 1 through 9 are set since any of those might be a valid guess to start with
        const start_possibilities: Field = 0x1FF;
        return Sudoku{
            .cells = [_]Field{0} ** 81,
            .blocks = [_]Field{start_possibilities} ** 9,
            .cols = [_]Field{start_possibilities} ** 9,
            .rows = [_]Field{start_possibilities} ** 9,
            .remaining = 81,
        };
    }

    // Set a value at a specific location
    pub fn setValue(self: *Sudoku, val: Field, location: usize) void {
        std.debug.assert(self.cells[location] == 0);
        self.cells[location] = val;

        const indices = getIndices(location);
        self.blocks[indices.blk] &= ~val;
        self.cols[indices.col] &= ~val;
        self.rows[indices.row] &= ~val;
        self.remaining -= 1;
    }

    // Clear a value at a specific location
    pub fn clearValue(self: *Sudoku, location: usize) void {
        std.debug.assert(self.cells[location] != 0);
        const val = self.cells[location];
        self.cells[location] = 0;

        const indices = getIndices(location);
        self.blocks[indices.blk] |= val;
        self.cols[indices.col] |= val;
        self.rows[indices.row] |= val;
        self.remaining += 1;
    }

    // Back out moves from a list of locations
    pub fn backOutMoves(self: *Sudoku, eager_moves: std.ArrayList(usize)) void {
        for (eager_moves.items) |guess| {
            self.clearValue(guess);
        }
    }

    // Solve the Sudoku puzzle
    pub fn solve(self: *Sudoku, allocator: std.mem.Allocator) bool {
        // Stage once, do as many forced moves as possible
        // Keep trying until they stop coming
        var eager_moves = std.ArrayList(usize).init(allocator);
        defer eager_moves.deinit();

        while (true) {
            var solved_count: usize = 0;
            var lowest_space: usize = 0;
            var lowest_count: u32 = std.math.maxInt(u32);
            var lowest_possibles: Field = 0;

            for (0..81) |i| {
                if (self.cells[i] != 0) {
                    // This cell is already solved
                    continue;
                }
                const indices = getIndices(i);
                const possibles = self.blocks[indices.blk] & self.cols[indices.col] & self.rows[indices.row];
                const count = @popCount(possibles);

                if (count == 1) {
                    self.setValue(possibles, i);
                    solved_count += 1;
                    eager_moves.append(i) catch unreachable;
                } else if (count == 0) {
                    // We're down a blind alley, abort
                    self.backOutMoves(eager_moves);
                    return false;
                } else if (count < lowest_count) {
                    lowest_count = count;
                    lowest_space = i;
                    lowest_possibles = possibles;
                }
            }

            if (solved_count != 0) {
                // If we're finding moves by elimination don't start guessing yet, just keep on
                // solving this way
                continue;
            } else if (self.remaining == 0) {
                // We won!
                return true;
            }

            const possibles = fieldToVals(lowest_possibles, allocator);
            defer possibles.deinit();

            for (possibles.items) |guess| {
                self.setValue(valToField(guess), lowest_space);
                if (self.solve(allocator)) {
                    return true;
                }
                self.clearValue(lowest_space);
            }

            self.backOutMoves(eager_moves);
            // None of the guesses worked, we're on a bad branch. Abort
            return false;
        }
    }

    // Print the Sudoku board
    pub fn print(self: *const Sudoku) void {
        const stdout = std.io.getStdOut().writer();
        
        for (0..81) |i| {
            if (i == 0) {
                // First row
            } else if (i % 27 == 0) {
                stdout.print("\n\n", .{}) catch unreachable;
            } else if (i % 9 == 0) {
                stdout.print("\n", .{}) catch unreachable;
            } else if (i % 3 == 0) {
                stdout.print("   ", .{}) catch unreachable;
            } else {
                stdout.print(" ", .{}) catch unreachable;
            }
            stdout.print("{d}", .{fieldToVal(self.cells[i])}) catch unreachable;
        }
        stdout.print("\n\n", .{}) catch unreachable;
    }
};

// Struct to hold the block, column, and row indices for a given cell
const Indices = struct {
    blk: usize,
    col: usize,
    row: usize,
};

// Convert a position to its block, column, and row indices
fn getIndices(pos: usize) Indices {
    const row = pos / 9;
    const col = pos % 9;
    const blk = (row / 3) * 3 + (col / 3);
    return Indices{ .blk = blk, .col = col, .row = row };
}

// Convert a value (1-9) to its field representation
fn valToField(x: u8) Field {
    return @as(Field, 1) << @intCast(x - 1);
}

// Convert a field to its value (1-9 or 0 if no bits are set)
fn fieldToVal(x: Field) u8 {
    if (x == 0) {
        return 0;
    }
    std.debug.assert(@popCount(x) == 1);
    return @as(u8, @intCast(@ctz(x))) + 1;
}

// Convert a field to a list of possible values
fn fieldToVals(x: Field, allocator: std.mem.Allocator) std.ArrayList(u8) {
    var ret = std.ArrayList(u8).init(allocator);

    if (x == 0) {
        return ret;
    }

    var val: u8 = 1;
    var field = x;
    while (field > 0) {
        if ((field & 1) == 1) {
            ret.append(val) catch unreachable;
        }
        val += 1;
        field >>= 1;
    }

    return ret;
}

// Load a Sudoku from a file
fn loadSudoku(filename: []const u8, allocator: std.mem.Allocator) !Sudoku {
    var s = Sudoku.init();

    const file = try std.fs.cwd().openFile(filename, .{});
    defer file.close();

    var buf_reader = std.io.bufferedReader(file.reader());
    var in_stream = buf_reader.reader();

    var buf: [1024]u8 = undefined;
    var numbers = std.ArrayList(u8).init(allocator);
    defer numbers.deinit();

    while (try in_stream.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        // Skip empty lines
        if (line.len == 0) continue;
        
        var it = std.mem.tokenizeAny(u8, line, " \t");
        while (it.next()) |token| {
            if (token.len == 0) continue;
            
            const x = try std.fmt.parseInt(u8, token, 10);
            try numbers.append(x);
            
            // Debugging: print the numbers being read
            // std.debug.print("Read number: {d}\n", .{x});
        }
    }

    if (numbers.items.len < 81) {
        std.debug.print("Not enough numbers in the input file: got {d}, need 81\n", .{numbers.items.len});
        return error.InvalidPuzzle;
    }

    for (numbers.items[0..81], 0..) |x, i| {
        if (x == 0) {
            continue;
        }
        s.setValue(valToField(x), i);
    }

    return s;
}

// Solve a Sudoku puzzle n times and return the last solved puzzle
fn solveN(filename: []const u8, n: usize, allocator: std.mem.Allocator) !Sudoku {
    var s = try loadSudoku(filename, allocator);
    
    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();
    
    _ = s.solve(arena.allocator());

    // If there are more iterations, solve them
    if (n > 1) {
        for (1..n) |_| {
            s = try loadSudoku(filename, allocator);
            
            arena.deinit();
            arena = std.heap.ArenaAllocator.init(allocator);
            
            _ = s.solve(arena.allocator());
        }
    }

    return s;
}

pub fn main() !void {
    // Set up command line argument parsing
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();
    defer _ = gpa.deinit();

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);

    var filename: []const u8 = "puzzle.txt";
    var iterations: usize = 1;

    if (args.len > 1) {
        filename = args[1];
        
        if (args.len > 2) {
            iterations = try std.fmt.parseInt(usize, args[2], 10);
        }
    }

    std.debug.print("Solving {s} {d} times\n", .{ filename, iterations });

    var s = try solveN(filename, iterations, allocator);
    s.print();
}
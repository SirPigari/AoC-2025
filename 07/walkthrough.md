# Day 07: Laboratories - Walkthrough

## Problem Summary

We need to analyze a tachyon manifold where beams split when encountering splitters. Beams always move downward through the manifold.

For the full problem description, see: https://adventofcode.com/2025/day/7

**Part 1**: Count how many times beams are split (classical physics - multiple beams exist simultaneously).

**Part 2**: Count how many different timelines exist (quantum physics - single particle takes all paths, creating parallel timelines).

## Input Format

A grid where:
- `S` marks the starting position (top row)
- `.` is empty space (beam passes through)
- `^` is a splitter (stops beam, creates two new beams going left and right)

```
.......S.......
...............
.......^.......
...............
......^.^......
```

## Key Concepts

### Part 1: Classical Beam Splitting
When a beam hits a splitter:
1. The beam stops
2. Two new beams emerge (left and right of the splitter)
3. Count each split event
4. Track which splitters have already been encountered to avoid double-counting

### Part 2: Quantum Timeline Counting
When a particle hits a splitter:
1. Reality itself splits into two timelines
2. In one timeline, the particle went left
3. In another timeline, the particle went right
4. Count total number of distinct timelines at the end

This is a dynamic programming problem - we track how many timelines reach each position.

## Solution Approach

**Part 1 (Simulation)**:
1. Start with one beam at position S
2. Move all beams down one row
3. When a beam hits a splitter, replace it with two beams (left and right)
4. Track splitters we've already counted
5. Continue until all beams exit the manifold

**Part 2 (Dynamic Programming)**:
1. Use DP where `dp[col]` = number of timelines at this column
2. For each row, calculate next row's timeline counts
3. When timeline hits a splitter, it splits into two timelines (left and right)
4. When timeline hits empty space, it continues straight down
5. Sum all timelines that reach the bottom

## Code Walkthrough

### Setup and Input Parsing

```lucia
import fs

input := fs.read_file(__dir__ + "input.txt")
grid := input.split_lines().map((line) => line.iter().collect()).collect()

height := len(grid)
width := len(grid[0])

mutable start_col: int = 0
for (col in range(width)):
    if grid[0][col] == 'S':
        start_col = col
        break
    end
end
```

- Parse input into a 2D character grid
- Find the starting column by locating 'S' in the first row
- Store grid dimensions for bounds checking

### Part 1: Beam Simulation

```lucia
fun part1() -> int:
    mutable beams: list[(int, int)] = [(0, start_col)]
    mutable split_count: int = 0
    mutable processed_splitters: list[(int, int)] = []
    
    while len(beams) > 0:
        mutable new_beams: list[(int, int)] = []
        
        for ((row, col) in beams):
            next_row := row + 1
            
            if next_row >= height:
                continue
            end
            
            next_cell := grid[next_row][col]
            
            if next_cell == '^':
                splitter_pos := (next_row, col)
                
                mutable already_processed: bool = false
                for (pos in processed_splitters):
                    if pos[0] == next_row && pos[1] == col:
                        already_processed = true
                        break
                    end
                end
                
                if !already_processed:
                    split_count++
                    processed_splitters += [splitter_pos]
                    
                    if col > 0:
                        new_beams += [(next_row, col - 1)]
                    end
                    if col < width - 1:
                        new_beams += [(next_row, col + 1)]
                    end
                end
            else:
                new_beams += [(next_row, col)]
            end
        end
        
        beams = new_beams
    end
    return split_count
end
```

**Logic**:

1. **Initialize**:
   - Start with one beam at `(0, start_col)`
   - Track split count and processed splitter positions

2. **Simulation loop** (while beams exist):
   - For each beam, calculate its next position (one row down)
   - Check if beam exits the manifold (bounds check)

3. **Handle next cell**:
   - **Empty space (`.`)**: Beam continues straight down
   - **Splitter (`^`)**: 
     - Check if we've already counted this splitter
     - If not: increment split count, mark as processed
     - Create two new beams (left and right of splitter)
     - Handle edge boundaries (don't create beams outside grid)

4. **Update beams**: Replace old beams with new beams for next iteration

**Why track processed splitters?** 
Multiple beams can hit the same splitter. We only count each splitter once (the first time any beam hits it).

### Part 2: Dynamic Programming

```lucia
fun part2() -> int:
    mutable dp_curr: list[int] = array(width, 0)
    mutable dp_next: list[int] = array(width, 0)

    dp_curr[start_col] = 1

    for (r in range(height - 1)):
        for (c in range(width)):
            if dp_curr[c] == 0:
                continue
            end

            next_cell := grid[r + 1][c]

            if next_cell == '^':
                if c > 0:
                    dp_next[c - 1] += dp_curr[c]
                end
                if c < width - 1:
                    dp_next[c + 1] += dp_curr[c]
                end
            else:
                dp_next[c] += dp_curr[c]
            end
        end

        dp_curr = dp_next
        dp_next = array(width, 0)
    end

    mutable total: int = 0
    for (c in range(width)):
        total += dp_curr[c]
    end

    return total
end
```

**Logic**:

1. **Initialize DP arrays**:
   - `dp_curr[c]` = number of timelines at column `c` in current row
   - `dp_next[c]` = number of timelines at column `c` in next row
   - Start with 1 timeline at `start_col`

2. **Process each row**:
   - For each column with timelines (`dp_curr[c] > 0`)
   - Check what's in the next row at that column

3. **Propagate timelines**:
   - **Empty space**: All timelines continue straight down
     - `dp_next[c] += dp_curr[c]`
   - **Splitter**: Each timeline splits into two
     - Left: `dp_next[c-1] += dp_curr[c]`
     - Right: `dp_next[c+1] += dp_curr[c]`
   - Handle boundaries (don't go outside grid)

4. **Update for next iteration**:
   - Move `dp_next` to `dp_curr`
   - Reset `dp_next` to zeros

5. **Sum final timelines**: Add up all timelines across all columns in the last row

**Example**:
- Start: `[0, 0, 0, 1, 0, 0, 0]` (1 timeline at column 3)
- After splitter: `[0, 0, 1, 0, 1, 0, 0]` (2 timelines, at columns 2 and 4)
- After another splitter on column 2: `[0, 1, 0, 0, 2, 0, 0]` (3 timelines total)

**Why this works**:
Each time a timeline hits a splitter, the number of timelines doubles (for that specific path). By tracking counts at each position, we avoid exponential memory usage and can handle the massive numbers in Part 2.

### Main Execution

```lucia
print(f"Part 1: {part1()}")
print(f"Part 2: {part2()}")
```

Execute both parts and display results.

## Performance Notes

**Part 1**:
- Time: O(B * H) where B is number of beams, H is height
- B can grow exponentially but is limited by grid width
- Tracking processed splitters prevents redundant work

**Part 2**:
- Time: O(H * W) where H is height, W is width
- Space: O(W) for the DP arrays
- Very efficient even with massive timeline counts (36 trillion!)
- The DP approach avoids actually tracking individual timelines

## Edge Cases Handled

1. **Grid boundaries**: Checks prevent beams/timelines from going outside the grid
2. **Duplicate splitter hits**: Part 1 tracks processed splitters
3. **Multiple beams converging**: Part 2 naturally handles this by summing counts
4. **Beams exiting manifold**: Continue loop only processes beams still in bounds

## Complexity

- **Part 1 Time**: O(S * H) where S is splits, H is height
- **Part 1 Space**: O(S) for tracking splitters and beams
- **Part 2 Time**: O(H * W) 
- **Part 2 Space**: O(W) for DP arrays

Part 2's DP approach is crucial - trying to simulate individual timelines would be impossible with 36 trillion timelines!

## Key Insight

Part 2 transforms an exponential problem into a linear one by recognizing that we don't need to track individual timelines - we only need to know how many timelines pass through each position. This is a classic application of dynamic programming to counting problems.
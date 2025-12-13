# Day 04: Printing Department - Walkthrough

## Problem Summary

We need to optimize forklift work by identifying which paper rolls can be accessed and removed from a grid. A roll can be accessed if it has fewer than 4 adjacent rolls (in 8 directions: orthogonal + diagonal).

For the full problem description, see: https://adventofcode.com/2025/day/4

**Part 1**: Count how many rolls can be accessed initially (have fewer than 4 neighbors).

**Part 2**: Keep removing accessible rolls until no more can be removed. Count the total removed.

## Input Format

A grid where `@` represents a paper roll and `.` represents empty space:
```
..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
```

## Key Concepts

### Neighbor Counting
Each cell has 8 possible neighbors (including diagonals):
```
NW  N  NE
 W  X  E
SW  S  SE
```

A roll is accessible if it has fewer than 4 neighbors with rolls (`@` symbols).

### Iterative Removal
Part 2 requires repeatedly:
1. Find all accessible rolls
2. Remove them all simultaneously
3. Repeat until no more rolls are accessible

This is similar to a cellular automaton or erosion algorithm.

## Solution Approach

1. Parse the grid into a 2D boolean array (`@` = true, `.` = false)
2. For Part 1: Count cells with fewer than 4 adjacent true cells
3. For Part 2: Iteratively remove accessible cells until none remain
4. Track the first iteration count (Part 1) and total count (Part 2)

## Code Walkthrough

### Setup and Input Parsing

```lucia
import fs

input := fs.read_file(__dir__ + "input.txt")
grid: list[list[bool]] = input.split_lines().map((line) => line.iter().map((ch) => ch == '@').collect()).collect()

mutable state: list[list[bool]] = grid
```

- Read the input file
- Parse each line character by character
- Convert `@` to `true`, everything else to `false`
- Store in a 2D list of booleans
- Create a mutable state copy for simulation

### Neighbor Check Function

```lucia
fun has_fewer_than_four_adjacent(x: int, y: int) -> bool:
    directions: list[(int, int)] = [(-1, -1), (-1, 0), (-1, 1),
                                    (0, -1),          (0, 1),
                                    (1, -1),  (1, 0),  (1, 1)]
    count: int = 0
    n := len(state)
    m := len(state[0])

    for ((dx, dy) in directions):
        nx := x + dx
        ny := y + dy

        if nx >= 0 && nx < n && ny >= 0 && ny < m:
            if state[nx][ny]:
                count++
            end
            if count >= 4:
                return false
            end
        end
    end

    return true
end
```

**Logic**:
1. Define 8 direction vectors for all neighbors (orthogonal + diagonal)
2. Count how many adjacent cells contain paper rolls
3. Check bounds to avoid going outside the grid
4. Early exit optimization: return false as soon as we find 4 neighbors
5. Return true if fewer than 4 neighbors found

**Direction mapping**:
- `(-1, -1)` = northwest, `(-1, 0)` = north, `(-1, 1)` = northeast
- `(0, -1)` = west, `(0, 1)` = east
- `(1, -1)` = southwest, `(1, 0)` = south, `(1, 1)` = southeast

### Main Simulation Loop

```lucia
mutable total: int = 0
mutable first: int = 0

while true:
    mutable local_total: int = 0
    for i in range(len(state)):
        for j in range(len(state[0])):
            if state[i][j]:
                l := has_fewer_than_four_adjacent(i, j)
                print(f"Cell ({i}, {j}) has fewer than four adjacent '@': {l}")
                if l:
                    local_total++
                    state[i][j] = false
                end
            end
        end
    end
    print(local_total)
    if local_total == 0:
        break
    end
    if first == 0:
        first = local_total
    end
    total += local_total
end
```

**Process**:
1. Loop indefinitely until no more rolls can be removed
2. For each iteration:
   - `local_total` tracks rolls removed in this iteration
   - Check every cell in the grid
   - If cell contains a roll and has fewer than 4 neighbors, remove it
   - Set `state[i][j] = false` to remove the roll
3. If no rolls were removed (`local_total == 0`), stop
4. Save the first iteration count (Part 1 answer)
5. Add each iteration's count to the total (Part 2 answer)

**Important**: All removals in an iteration happen based on the state at the start of that iteration. We check all cells before modifying any, ensuring simultaneous removal.

### Final Output

```lucia
print(f"First: {first}")
print(f"Total: {total}")
```

- `first`: Number of rolls removed in the first iteration (Part 1)
- `total`: Total rolls removed across all iterations (Part 2)

## Example Walkthrough

Initial grid (small portion):
```
..@@.
@@@.@
```

**Iteration 1**:
- Position (0,2): Has 2 neighbors -> accessible, remove
- Position (0,3): Has 3 neighbors -> accessible, remove
- Position (1,0): Has 2 neighbors -> accessible, remove
- ... and so on

After removal, some rolls that previously had 4+ neighbors now have fewer, making them accessible in the next iteration.

## Why Multiple Iterations Are Needed

Removing outer rolls exposes inner rolls:
```
Initial:    After Iter 1:    After Iter 2:
@@@@@       .@@@@.           ..@@..
@@@@@       @@@@@            .@@@.
@@@@@       .@@@@.           ..@@..
```

The center roll starts with 8 neighbors (not accessible), then 4 neighbors (still not accessible), then 0 neighbors (accessible).

## Performance Notes

- The algorithm is straightforward but not optimized
- Each iteration checks every cell in the grid
- For large grids, this could be slow
- Could be optimized by tracking only cells near removed rolls
- But it works fine for the puzzle input

## Edge Cases Handled

1. **Grid boundaries**: Neighbor check includes bounds checking
2. **Empty grid**: Loop terminates immediately if no rolls exist
3. **Isolated rolls**: Rolls with 0 neighbors are removed in first iteration
4. **Dense clusters**: Rolls with 8 neighbors never get removed

## Complexity

- **Time**: O(I * N * M) where I is number of iterations, N is grid height, M is grid width
- **Space**: O(N * M) for storing the grid state

The number of iterations depends on the grid structure but is typically small (the "erosion" converges quickly).

## Alternate Interpretation Note

The problem states rolls are removed if they have "fewer than four rolls" adjacent. This means 0, 1, 2, or 3 neighbors makes a roll accessible. The code correctly implements this with the `< 4` comparison.
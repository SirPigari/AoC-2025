# Day 05: Cafeteria - Walkthrough

## Problem Summary

We need to help the kitchen Elves figure out which ingredients are fresh using their new inventory management system. The system uses ingredient ID ranges to define what's fresh.

For the full problem description, see: https://adventofcode.com/2025/day/5

**Part 1**: Count how many of the available ingredient IDs fall within any fresh ID range.

**Part 2**: Count the total number of ingredient IDs covered by all fresh ID ranges (merge overlapping ranges).

## Input Format

Two sections separated by a blank line:

```
3-5
10-14
16-20
12-18

1
5
8
11
17
32
```

- First section: Fresh ingredient ID ranges (inclusive)
- Second section: Available ingredient IDs to check

## Key Concepts

### Range Checking (Part 1)
For each available ingredient ID, check if it falls within any of the fresh ranges. An ID is fresh if `low <= id <= high` for at least one range.

### Range Merging (Part 2)
When ranges overlap or are adjacent, they should be merged:
- `3-5` and `4-7` overlap -> merge to `3-7`
- `10-14` and `15-20` are adjacent -> merge to `10-20`
- `3-5` and `10-14` don't overlap -> keep separate

The total count is the sum of all merged range sizes.

## Solution Approach

**Part 1**:
1. For each available ingredient ID
2. Check if it falls within any range
3. Count how many are fresh

**Part 2**:
1. Sort ranges by their start value
2. Merge overlapping/adjacent ranges
3. Sum up the size of all merged ranges

## Code Walkthrough

### Setup and Input Parsing

```lucia
import fs

input := fs.read_file(__dir__ + "input.txt").trim().to_lf()
(ranges_raw, ids_raw) := input.split("\n\n")
ranges: list[(int, int)] = ranges_raw.split_lines().map((line) => scope (line):
    parts := line.split("-")
    (parts[0] as int, parts[1] as int)
end).collect()
ids: list[int] = ids_raw.split_lines().map((line) => line as int).collect()
```

**Breakdown**:
1. Read and normalize the input (trim whitespace, normalize line endings)
2. Split on double newline to separate ranges from IDs
3. Parse ranges:
   - Split each line on `-`
   - Convert to tuple of two integers `(low, high)`
4. Parse IDs:
   - Convert each line to an integer

### Part 1: Check Available IDs

```lucia
fun part_1() -> int:
    mutable total: int = 0

    id_len := len(ids)
    
    for ((i, id) in ids.enumerate()):
        mutable valid: bool = false
        for ((low, high) in ranges):
            if id >= low && id <= high:
                valid = true
                break
            end
        end
        print(f"ID {id} -> {valid} ({i+1}/{id_len})")
        if valid:
            total++
        end
    end

    return total
end
```

**Logic**:
1. Iterate through each available ingredient ID
2. For each ID, check all ranges
3. If the ID falls within any range (`low <= id <= high`), mark it as valid
4. Use `break` for early exit once we find a matching range
5. Count how many IDs are valid (fresh)
6. Progress indicator shows which ID we're checking

**Example**:
- ID `5` with range `3-5`: `5 >= 3 && 5 <= 5` -> true -> fresh
- ID `8` with ranges `3-5`, `10-14`: Neither matches -> spoiled

### Part 2: Merge Ranges

```lucia
fun part_2() -> int:
    ranges = ranges.sort_by_unstable((a, b) => a[0] < b[0])

    total := 0
    current_low := ranges[0][0]
    current_high := ranges[0][1]

    for ((low, high) in ranges[1..]):
        if low <= current_high + 1:
            if high > current_high:
                current_high = high
            end
        else:
            total += current_high - current_low + 1
            current_low = low
            current_high = high
        end
    end

    total += current_high - current_low + 1
    return total
end
```

**Logic**:

1. **Sort ranges by start value**: `ranges.sort_by_unstable((a, b) => a[0] < b[0])`
   - This ensures we process ranges in order from left to right
   - Uses unstable sort which is faster than stable sort for this use case
   - We don't need stability (preserving order of equal elements) here, so unstable is preferred
   - Example: `[(10,14), (3,5), (16,20)]` becomes `[(3,5), (10,14), (16,20)]`

2. **Initialize with first range**:
   - `current_low` and `current_high` track the current merged range

3. **Process remaining ranges**:
   - For each range `(low, high)`:
   
   **Case 1: Overlap or adjacent** (`low <= current_high + 1`):
   - The new range overlaps or touches the current range
   - Example: current is `10-14`, new is `12-18` -> they overlap
   - Example: current is `10-14`, new is `15-20` -> they're adjacent (14+1=15)
   - Extend the current range: `current_high = max(current_high, high)`
   
   **Case 2: Gap** (`low > current_high + 1`):
   - The new range doesn't connect to the current range
   - Example: current is `3-5`, new is `10-14` -> gap from 6-9
   - Add the size of the current range to total: `current_high - current_low + 1`
   - Start a new current range: `current_low = low, current_high = high`

4. **Add final range**: Don't forget to add the last merged range to the total

**Why `+1` matters**:
- Range `3-5` has size `5 - 3 + 1 = 3` (includes 3, 4, 5)
- Adjacent check uses `current_high + 1` because `10-14` and `15-20` should merge

### Main Execution

```lucia
print(f"Part 1: {part_1()}")
print(f"Part 2: {part_2()}")
```

Execute both parts and display results.

## Example Walkthrough

Given ranges: `3-5`, `10-14`, `16-20`, `12-18`

**Part 1** with IDs `[1, 5, 8, 11, 17, 32]`:
- `1`: Not in any range -> spoiled
- `5`: In `3-5` -> fresh
- `8`: Not in any range -> spoiled
- `11`: In `10-14` -> fresh
- `17`: In both `16-20` and `12-18` -> fresh
- `32`: Not in any range -> spoiled
- Result: 3 fresh IDs

**Part 2** merging:
1. Sort: `[(3,5), (10,14), (12,18), (16,20)]`
2. Start with `3-5` (size 3)
3. Check `10-14`: Gap (5+1=6 < 10), add 3 to total, start new range `10-14`
4. Check `12-18`: Overlap (12 <= 14+1), extend to `10-18`
5. Check `16-20`: Overlap (16 <= 18+1), extend to `10-20`
6. Add final range size: `20-10+1 = 11`
7. Total: `3 + 11 = 14`

## Performance Notes

**Part 1**:
- Time: O(N * M) where N is number of IDs, M is number of ranges
- Could be optimized by sorting ranges and using binary search
- But it's fast enough for the puzzle input

**Part 2**:
- Time: O(M log M) for sorting + O(M) for merging = O(M log M)
- Very efficient even for large numbers of ranges
- The range merging is a classic interval merging algorithm

## Edge Cases Handled

1. **Overlapping ranges**: Correctly merged by extending `current_high`
2. **Adjacent ranges**: Detected with `low <= current_high + 1`
3. **Contained ranges**: When `high <= current_high`, no extension needed
4. **Single range**: Works correctly (just adds its size)
5. **Large gaps**: Each gap creates a separate merged range

## Complexity

- **Part 1 Time**: O(N * M) where N is IDs, M is ranges
- **Part 1 Space**: O(1) for the checking logic
- **Part 2 Time**: O(M log M) for sorting + O(M) for merging
- **Part 2 Space**: O(1) for the merging (in-place sort)

The solution is efficient and handles the potentially large range values in Part 2 well (352 trillion IDs!).
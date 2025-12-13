# Day 02: Gift Shop - Walkthrough

## Problem Summary

An Elf accidentally added invalid product IDs to the gift shop database. We need to identify and sum up all invalid IDs within given ranges.

For the full problem description, see: https://adventofcode.com/2025/day/2

**Part 1**: An ID is invalid if it's made of some digit sequence repeated exactly twice (e.g., `55`, `6464`, `123123`).

**Part 2**: An ID is invalid if it's made of some digit sequence repeated at least twice (e.g., `111`, `12341234`, `1212121212`).

## Input Format

A single line containing comma-separated ID ranges:
```
11-22,95-115,998-1012,1188511880-1188511890
```

Each range is formatted as `start-end` (inclusive).

## Key Concepts

### String Pattern Matching
- Convert numbers to strings to check for repeated patterns
- Part 1: Check if the ID can be split into two identical halves
- Part 2: Check if the ID can be split into N identical segments (where N >= 2)

### Pattern Detection Logic
**Part 1**: Only checks if length is even and first half equals second half.
**Part 2**: Tests all possible pattern lengths that evenly divide the ID length.

## Solution Approach

1. Parse the input to extract all ID ranges
2. For each range, check every ID against the validation rules
3. Collect invalid IDs and sum them up
4. Use two separate validation functions for Part 1 and Part 2

## Code Walkthrough

### Setup and Input Parsing

```lucia
import fs
import (floor) from math

input := fs.read_file(__dir__ + "input.txt")
ranges: list[(int, int)] = input.split(',').map((it) => it.trim().split('-')).map((it) => (it[0] as int, it[1] as int)).collect()
```

- Read the input file
- Split by commas to get individual ranges
- For each range, trim whitespace and split by dash
- Convert string parts to integers
- Result is a list of tuples: `[(start, end), ...]`

### Variable Initialization

```lucia
mutable invalid_id_sum_p1: int = 0
mutable invalid_ids_p1: list[int] = []
mutable invalid_id_sum_p2: int = 0
mutable invalid_ids_p2: list[int] = []
```

Track both the sum and list of invalid IDs for each part.

### Part 1 Validation Function

```lucia
fun is_valid_id_p1(id: int) -> bool:
    id_str := id as str
    n := len(id_str)

    if n % 2 == 1:
        return true
    end

    half := n / 2
    if id_str[0..half] == id_str[half..]:
        return false
    end

    return true
end
```

**Logic**:
1. Convert ID to string to work with digits
2. If length is odd, it can't be split into two equal halves -> valid
3. Split string in half and compare
4. If both halves are identical -> invalid (return false)
5. Otherwise -> valid (return true)

**Examples**:
- `123` (length 3, odd) -> valid
- `1212` (length 4, `12` == `12`) -> invalid
- `1234` (length 4, `12` != `34`) -> valid

### Part 2 Validation Function

```lucia
fun is_valid_id_p2(id: int) -> bool:
    id_str := id as str
    n := len(id_str)

    // Single-digit numbers cannot repeat at least twice
    if n == 1:
        return true
    end

    max_l := floor(n / 2.0) as int
    for l in [1..max_l]:
        if n % l != 0:
            continue
        end

        seq := id_str[0..l]
        repeated := true

        for i in range(l, n, l):
            if (id_str[i..(i + l)] != seq):
                repeated := false
                break
            end
        end

        if repeated:
            return false
        end
    end

    return true
end
```

**Logic**:
1. Special case: Single-digit IDs (like `5`) are valid because they're just one occurrence of the pattern, not repeated at least twice
2. Try all possible pattern lengths from 1 to n/2
3. Skip lengths that don't evenly divide the total length
4. Extract the first segment as the pattern
5. Check if all subsequent segments match the pattern
6. If any pattern length results in full repetition -> invalid
7. If no pattern works -> valid

**Why n/2?** If a pattern repeats at least twice, it can be at most half the total length.

**Examples**:
- `5` (length 1): Single digit, cannot repeat at least twice -> valid
- `111` (length 3): Try pattern length 1 -> `1` repeats 3 times -> invalid
- `123123` (length 6): Try pattern length 3 -> `123` repeats 2 times -> invalid
- `12341234` (length 8): Try pattern length 4 -> `1234` repeats 2 times -> invalid
- `123456` (length 6): No pattern repeats -> valid

### Main Processing Loop

```lucia
for ((start_id, end_id) in ranges):
    print(f"{start_id} - {end_id}")
    for id in [start_id..end_id]:
        if id % 100 == 0:
            print(f"Checking ID: {id}")
        end
        if !is_valid_id_p1(id):
            invalid_id_sum_p1 += id
            invalid_ids_p1 += [id]
        end
        if !is_valid_id_p2(id):
            invalid_id_sum_p2 += id
            invalid_ids_p2 += [id]
        end
    end
end
```

**Process**:
1. Iterate through each range
2. Check every ID in the range (inclusive)
3. Progress indicator every 100 IDs
4. Test against both validation functions
5. Accumulate invalid IDs and their sums

### Final Output

```lucia
print("Invalid ID sum P1: " + invalid_id_sum_p1)
print("Invalid IDs P1: " + invalid_ids_p1)
print("Invalid ID sum P2: " + invalid_id_sum_p2)
print("Invalid IDs P2: " + invalid_ids_p2)
```

Display the results for both parts.

## Performance Notes

The solution is straightforward but not optimized:
- It checks every single ID in each range individually
- For large ranges (like `1188511880-1188511890`), this is inefficient
- Could be optimized by mathematically generating invalid IDs rather than checking each one
- But it works fine for the given input.

## Edge Cases Handled

1. **Odd-length numbers** (Part 1): Can't be split into two halves, automatically valid
2. **Single-digit numbers** (Part 2): Cannot repeat at least twice, require special case to be valid
3. **Single-digit patterns** (Part 2): Like `111`, `2222` - detected correctly
4. **Multiple valid pattern lengths**: Only needs one to make it invalid
5. **Non-divisible lengths**: Skipped efficiently with the modulo check

## Example Walkthrough

Range `95-115`:
- **Part 1**: Check `99` -> length 2, split into `9` and `9` -> identical -> invalid
- **Part 2**: Check `99` -> pattern length 1, `9` repeats 2 times -> invalid
- **Part 2**: Check `111` -> pattern length 1, `1` repeats 3 times -> invalid

## Complexity

- **Time**: O(R * N * L) where R is number of ranges, N is average range size, L is average ID length
- **Space**: O(I) where I is the number of invalid IDs found

The nested loops and string operations make this relatively slow for large inputs, but acceptable for the puzzle constraints.
# Day 03: Lobby - Walkthrough

## Problem Summary

We need to power an escalator using batteries. Each battery bank is a string of digits (1-9), and we must select exactly k batteries to maximize the joltage output (the number formed by the selected digits in order).

For the full problem description, see: https://adventofcode.com/2025/day/3

**Part 1**: Select exactly 2 batteries from each bank to maximize joltage.

**Part 2**: Select exactly 12 batteries from each bank to maximize joltage.

## Input Format

Each line is a battery bank containing exactly 100 digits (1-9):
```
7383383448669853327488444395431356632533467643668433447312354949444179338343352833433277429334563636
8111111111111198234234234234278818181911112111123456789012345678901234567890123456789012345678901234
...
```

## Key Concepts

### Greedy Algorithm
The key insight is to use a greedy approach:
1. Select the largest available digit
2. From the remaining digits to the right, select the next largest
3. Repeat until k digits are selected

This works because we want to maximize the leftmost digits first (they have the highest place value).

### Constrained Selection
You cannot rearrange batteries - you must select them in the order they appear, maintaining their relative positions.

## Solution Approach

For each bank:
1. Start at the beginning of the bank
2. For each of the k positions in the result:
   - Look ahead in a window that ensures enough digits remain
   - Select the maximum digit in that window
   - Move the start position past the selected digit
3. Concatenate selected digits to form the final joltage number

## Code Walkthrough

### Setup and Input Reading

```lucia
import fs

input := fs.read_file(__dir__ + "input.txt")
banks: list[str] = input.split_lines()
```

Read the input file and split into individual battery banks (one per line).

### Helper Function: Find Maximum

```lucia
fun max(l: list) -> str:
    mutable maximum: str = l[0]
    for item in l:
        if item > maximum:
            maximum = item
        end
    end
    return maximum
end
```

Simple function to find the maximum element in a list. Used to find the largest digit in a substring.

### Core Algorithm: Largest Jolts

```lucia
fun largest_jolts(bank: str, k: int) -> int:
    n := len(bank)
    result: list[str] = []
    start := 0

    for remaining in range(k, 0, -1, as=list):
        e := n - remaining + 1
        max_digit := max(bank[start..e] as list)
        result += [max_digit]
        start = (bank as list).index_of(max_digit, start) + 1
    end
    return ''.join(result) as int
end
```

**Logic breakdown**:

1. `n := len(bank)` - Total number of batteries in the bank
2. `result: list[str] = []` - Will hold the k selected digits
3. `start := 0` - Current position in the bank
4. `for remaining in range(k, 0, -1, as=list):` - Iterate k times (k, k-1, k-2, ..., 1)
   - `remaining` tells us how many more digits we need to select

5. `e := n - remaining + 1` - Calculate the search window end
   - We need to leave enough digits for future selections
   - If we need 3 more digits and the bank has 10 digits total, and we're at position 0, we can search up to position 10 - 3 + 1 = 8
   - This ensures we leave 2 digits after our selection (for the remaining 2 selections)

6. `max_digit := max(bank[start..e] as list)` - Find the largest digit in the valid window

7. `result += [max_digit]` - Add the selected digit to our result

8. `start = (bank as list).index_of(max_digit, start) + 1` - Move past the selected digit
   - Find the first occurrence of max_digit starting from `start`
   - Move start position to one past that digit

9. `return ''.join(result) as int` - Join the selected digits into a string and convert to integer

### Main Processing Loop

```lucia
mutable total_p1: int = 0
mutable total_p2: int = 0

print("Largest jolts per bank:")
for bank in banks:
    l1 := largest_jolts(bank, 2)
    l2 := largest_jolts(bank, 12)
    total_p1 += l1
    total_p2 += l2
    print(f"Bank: {bank} -> Largest Jolts ( 2): {l1}")
    print(f"Bank: {bank} -> Largest Jolts (12): {l2}")
end
```

For each bank:
1. Calculate the maximum joltage with 2 batteries (Part 1)
2. Calculate the maximum joltage with 12 batteries (Part 2)
3. Add to running totals
4. Print results for debugging

### Final Output

```lucia
print(f"Total Largest Jolts Sum for Part 1: {total_p1}")
print(f"Total Largest Jolts Sum for Part 2: {total_p2}")
```

Display the sum of maximum joltages across all banks for both parts.

## Example Walkthrough

Bank: `987654321111111`, k = 2

**Iteration 1** (remaining = 2):
- `start = 0`
- `e = 15 - 2 + 1 = 14` (search from 0 to 14)
- Window: `98765432111111`
- `max_digit = '9'`
- `result = ['9']`
- Find '9' at index 0, so `start = 1`

**Iteration 2** (remaining = 1):
- `start = 1`
- `e = 15 - 1 + 1 = 15` (search from 1 to 15, the rest of the string)
- Window: `87654321111111`
- `max_digit = '8'`
- `result = ['9', '8']`
- Find '8' at index 1, so `start = 2`

**Result**: `'98'` -> 98 jolts

Bank: `987654321111111`, k = 12

Following the same logic, we select: `987654321111` (excluding three '1's at the end).

## Why This Works (Greedy Proof)

The greedy approach is optimal because:
1. A digit in position i contributes `digit * 10^(k-i-1)` to the final value
2. Maximizing leftmost positions has exponentially more impact
3. Once we select the maximum available digit for position i, no other choice for position i could yield a better result
4. The window constraint ensures we always have enough digits remaining for future selections

## Performance Notes

- The algorithm runs in O(n * k) time per bank, where n = 100 (bank length)
- For each of k selections, we scan at most n digits
- With k=2 for Part 1 and k=12 for Part 2, this is very efficient
- Could be optimized using a data structure like a segment tree or priority queue, but it's fast enough for the input size

## Edge Cases Handled

1. **Multiple occurrences of max digit**: Uses the first occurrence (leftmost) via `index_of`
2. **Window calculation**: Ensures we never run out of digits to select
3. **Single digit banks**: Works correctly even when k approaches n

## Complexity

- **Time**: O(B * n * k) where B is number of banks, n is average bank length, k is number of batteries to select
- **Space**: O(k) for storing the result digits

The solution is straightforward and efficient, prioritizing clarity over micro-optimizations.
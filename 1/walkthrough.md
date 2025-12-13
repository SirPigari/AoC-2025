# Day 01: Secret Entrance - Walkthrough

## Problem Summary

We need to open a safe with a circular dial numbered 0-99. The dial starts at position 50, and we follow a series of rotation instructions.

For the full problem description, see: https://adventofcode.com/2025/day/1

**Part 1**: Count how many times the dial lands exactly at position 0 after completing a rotation.

**Part 2**: Count how many times the dial passes through or lands on position 0 during any movement (including during rotations).

## Input Format

Each line contains a rotation instruction:
- Starts with `L` (left/counterclockwise) or `R` (right/clockwise)
- Followed by a number indicating how many positions to rotate

Examples: `R48`, `L68`, `L30`

## Key Concepts

### Circular Dial Behavior
- The dial has positions 0-99 (100 total positions)
- Moving right from 99 wraps to 0
- Moving left from 0 wraps to 99
- This is modular arithmetic: `position % 100`

### Two Different Counts
1. **End-of-turn count**: Dial lands exactly at 0 after a move completes
2. **Total crossing count**: Dial passes through 0 at any point during movement

## Solution Approach

The solution tracks both counts simultaneously:

1. Parse each instruction to get direction and distance
2. Simulate the movement step-by-step
3. Count crossings through position 0 during movement
4. Check if final position is 0 after movement completes
5. Update the head position for the next instruction

## Code Walkthrough

### Setup and Input Reading

```lucia
import fs

input := fs.read_file(__dir__ + "input.txt")
lines := input.split_lines()
```

Read the puzzle input and split it into individual rotation instructions.

### Variable Initialization

```lucia
mutable head: int = 50
mutable head_at_zero_count_at_end_of_turn: int = 0
mutable head_at_zero_count: int = 0
```

- `head`: Current dial position (starts at 50 per problem statement)
- `head_at_zero_count_at_end_of_turn`: Answer to Part 1 (lands on 0)
- `head_at_zero_count`: Answer to Part 2 (passes through or lands on 0)

### Main Processing Loop

```lucia
for line in lines:
    sign := if line[0] == 'R' then 1 else -1
    value := (line[1..] as int)
    move := sign * value
```

For each instruction:
- Extract direction: `R` becomes `+1`, `L` becomes `-1`
- Extract distance value from the rest of the line
- Calculate signed movement: positive for right, negative for left

### Movement Simulation

```lucia
    mutable c: int = 0

    mutable pos: int = head
    step: int = if move > 0 then 1 else -1

    for i in [1..|move|]:
        pos += step
        if pos < 0:
            pos += 100
        else if pos >= 100:
            pos -= 100
        end

        if pos == 0:
            c++
        end
    end
```

This simulates the dial moving click-by-click:
- `c`: Counts crossings through 0 during this specific move
- `pos`: Current position during simulation
- `step`: Direction of movement (+1 for right, -1 for left)
- Loop `|move|` times (absolute value of movement)
- Handle wraparound: if position goes below 0 or above 99, wrap around
- Count each time position equals 0

**Important**: This counts crossings *during* the move, not including the starting position.

**Performance Note**: This approach is SLOW because it simulates every single click. For large movements (e.g., `R1000`), it iterates 1000 times. This could be optimized using modular arithmetic to calculate crossings mathematically.

### Update Counters

```lucia
    head_at_zero_count += c
    print(f"Move: {move}, Head {head}, Crossings: {c}")
```

Add the crossings from this move to the total count. The debug output shows the move details.

### Update Head Position

```lucia
    head = (head + move) % 100
    if head < 0:
        head += 100
    end
```

Calculate the final position after the move:
- Use modulo arithmetic for wraparound
- Handle negative modulo results (when `head + move < 0`)

### Check End Position

```lucia
    if head == 0:
        head_at_zero_count_at_end_of_turn++
    end
```

If the dial lands exactly at 0 after completing the move, increment Part 1's counter.

### Final Output

```lucia
print("Head at: " + head)
print("Head at zero count at end of turn: " + head_at_zero_count_at_end_of_turn)
print("Head at zero count: " + head_at_zero_count)
```

Display final results:
- Final dial position
- Part 1 answer: times dial landed at 0
- Part 2 answer: total times dial was at 0 (including during moves)

## Example Walkthrough

Given the example instruction `R48` starting from position 50:

1. `move = +48`
2. Simulate 48 steps, each moving position by +1
3. Path: 50 -> 51 -> 52 -> ... -> 97 -> 98 -> 99 -> 0 -> 1 -> ... -> 98
4. Position 0 is crossed once during this movement
5. Final position: `(50 + 48) % 100 = 98`
6. Final position is not 0, so only Part 2 counter increases

## Edge Cases Handled

1. **Wraparound**: Positions wrap correctly using modulo arithmetic
2. **Negative positions**: When moving left past 0, positions wrap to 99
3. **Large movements**: Moving more than 100 positions works correctly (e.g., `R1000` from 50 crosses 0 ten times)
4. **Zero crossings vs. landings**: Distinguishes between passing through 0 and ending at 0

## Potential Optimizations

The step-by-step simulation is inefficient. A better approach would:
- Calculate the number of complete circles: `|move| / 100`
- Calculate crossings mathematically based on start position, end position, and direction
- This would reduce time complexity from O(m) to O(1) per move

But this solution prioritizes clarity over performance, and it works well enough for the given input.
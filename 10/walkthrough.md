# Day 10: Factory - Walkthrough

## Problem Summary

We need to configure factory machines by pressing buttons that toggle indicator lights or increment joltage counters. Each machine requires finding the minimum number of button presses needed.

For the full problem description, see: https://adventofcode.com/2025/day/10

**Part 1**: Configure indicator lights to match target pattern. Each button toggles specific lights (on/off).

**Part 2**: Configure joltage counters to match target values. Each button increments specific counters.

## Input Format

Each line describes one machine:
```
[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
```

- `[.##.]`: Target indicator light pattern (`.` = off, `#` = on)
- `(3)`, `(1,3)`, etc.: Button configurations (which lights/counters they affect)
- `{3,5,4,7}`: Joltage requirements (Part 2)

## Key Concepts

### Part 1: Toggle Problem (XOR)
Buttons toggle lights between on and off:
- Pressing button `(0,2)` toggles lights 0 and 2
- Each light can be toggled by multiple buttons
- Need to find combination where each light ends up in correct state
- This is essentially solving a system of XOR equations

### Part 2: Counter Problem
Buttons increment counters:
- Pressing button `(0,2)` three times adds 3 to counters 0 and 2
- Need to find how many times to press each button
- This is solving a system of linear equations with non-negative integer constraints

## Solution Approach

**Part 1 (Brute Force with Bitmask)**:
1. Try all possible combinations of which buttons to press
2. Each button is either pressed once or not pressed
3. Check if resulting light pattern matches target
4. Track minimum number of presses needed

**Part 2 (Backtracking Search)**:
1. Try increasing total press counts (0, 1, 2, ...)
2. For each total, try all ways to distribute presses among buttons
3. Check if resulting counter values match target
4. Return first working solution (minimum presses)

## Code Walkthrough

### Parsing Input

```lucia
fun parse_line(line: str) -> (list[bool], list[list[int]], list[int]):
    // Extract indicator lights pattern [.##.]
    start_bracket := line.chars().index_of('[')
    end_bracket := line.chars().index_of(']')
    lights_str := line[start_bracket+1..end_bracket]
    
    target: list[bool] = []
    for ch in lights_str.iter():
        target += [ch == '#']
    end
```

**Parsing lights**:
1. Find `[` and `]` to extract pattern
2. Convert `.` to `false`, `#` to `true`
3. Store as list of booleans

```lucia
    // Extract button configurations
    buttons: list[list[int]] = []
    mutable pos: int = end_bracket + 1
    
    while pos < len(line):
        if line[pos] == '(':
            end_paren := line.chars().index_of(')', pos)
            button_str := line[pos+1..end_paren]
            
            if len(button_str) > 0:
                button: list[int] = []
                for num_str in button_str.split(','):
                    button += [num_str.trim() as int]
                end
                buttons += [button]
            end
            
            pos = end_paren + 1
```

**Parsing buttons**:
1. Scan for `(` to find button start
2. Find matching `)` 
3. Split by comma and parse integers
4. Each button is a list of light/counter indices

```lucia
        else if line[pos] == '{':
            // Extract joltage requirements
            end_brace := line.chars().index_of('}', pos)
            joltage_str := line[pos+1..end_brace]
            
            joltages: list[int] = []
            for num_str in joltage_str.split(','):
                joltages += [num_str.trim() as int]
            end
            
            return (target, buttons, joltages)
```

**Parsing joltages**:
1. Find `{` and `}`
2. Split by comma and parse integers
3. Return all three components

### Part 1: Solving Lights (Bitmask Enumeration)

```lucia
fun solve_lights(target: list[bool], buttons: list[list[int]]) -> int:
    n := len(target)
    m := len(buttons)
    
    max_combinations: int = 1 << m
    mutable min_presses: int = 999999
```

**Setup**:
- `n`: Number of lights
- `m`: Number of buttons
- `max_combinations`: 2^m possible button combinations
- Use bitmask to represent which buttons are pressed

```lucia
    for mask in range(max_combinations):
        state: list[bool] = []
        for i in range(n):
            state += [false]
        end
        
        mutable presses: int = 0
        
        for btn_idx in range(m):
            if (mask band (1 << btn_idx)) != 0:
                presses += 1
                
                for light_idx in buttons[btn_idx]:
                    state[light_idx] = !state[light_idx]
                end
            end
        end
```

**Try each combination**:
1. Initialize all lights to off (`false`)
2. For each button, check if bit is set in mask
3. If button is pressed, toggle all its lights
4. Count total presses

**Example**:
- Mask `0b101` (binary 5) means press buttons 0 and 2
- Button 0 affects lights `[0,2]`, button 2 affects lights `[1]`
- After: light 0 toggled once, light 1 toggled once, light 2 toggled once

```lucia
        mutable matches: bool = true
        for i in range(n):
            if state[i] != target[i]:
                matches = false
                break
            end
        end
        
        if matches && presses < min_presses:
            min_presses = presses
        end
    end
    
    return min_presses
end
```

**Check if valid**:
1. Compare final state with target
2. If all lights match and fewer presses than current minimum, update
3. Return minimum found

### Part 2: Solving Joltage (Backtracking)

```lucia
fun solve_joltage(joltages: list[int], buttons: list[list[int]]) -> int:
    n := len(joltages)
    m := len(buttons)
    
    max_joltage := joltages.max()
    
    mutable min_total: int = 999999
    
    max_presses_per_button := max_joltage + 5
    
    max_total_presses := max_joltage * 3
    
    for total_try in range(0, max_total_presses + 1):
        if check_joltage_combination(joltages, buttons, total_try):
            return total_try
        end
    end
    
    return min_total
end
```

**Iterative deepening**:
1. Try total press counts from 0 upward
2. For each total, check if any distribution of presses works
3. Return first working solution (guaranteed minimum)

**Why this works**:
- We try smallest totals first
- First solution found is guaranteed to be minimum
- Upper bound prevents infinite search

```lucia
fun check_joltage_combination(target: list[int], buttons: list[list[int]], total_presses: int) -> bool:
    m := len(buttons)
    return try_distribution(target, buttons, total_presses, 0, [])
end
```

**Distribution check**:
Delegates to recursive backtracking to find valid press distribution.

```lucia
fun try_distribution(target: list[int], buttons: list[list[int]], remaining: int, btn_idx: int, presses: list[int]) -> bool:
    if btn_idx == len(buttons):
        counters: list[int] = []
        for i in range(len(target)):
            counters += [0]
        end
        
        for ((i, press_count) in presses.enumerate()):
            for counter_idx in buttons[i]:
                counters[counter_idx] += press_count
            end
        end
        
        mutable matches: bool = true
        for i in range(len(target)):
            if counters[i] != target[i]:
                matches = false
                break
            end
        end
        
        return matches
    end
```

**Base case (all buttons assigned)**:
1. Calculate final counter values
2. For each button, add its press count to all affected counters
3. Check if all counters match target
4. Return true if valid

```lucia
    for p in range(0, remaining + 1):
        new_presses := presses + [p]
        if try_distribution(target, buttons, remaining - p, btn_idx + 1, new_presses):
            return true
        end
    end
    
    return false
end
```

**Recursive case**:
1. Try pressing current button 0 to `remaining` times
2. Recursively assign remaining presses to other buttons
3. Return true if any assignment works

**Example**:
- Total 5 presses, 3 buttons
- Try: button 0 gets 0-5, button 1 gets 0-(5-button0), button 2 gets rest
- All combinations like (0,0,5), (0,1,4), (1,2,2), etc.

### Main Functions

```lucia
fun part1() -> int:
    mutable total: int = 0
    
    for ((i, line) in input.split_lines().enumerate()):
        if len(line.trim()) == 0:
            continue
        end
        
        (target, buttons, _) := parse_line(line)
        min_presses := solve_lights(target, buttons)
        
        print(f"Machine {i+1} (lights): {min_presses} presses")
        total += min_presses
    end
    
    return total
end
```

**Part 1**:
1. Parse each machine
2. Solve lights problem
3. Sum minimum presses for all machines

```lucia
fun part2() -> int:
    mutable total: int = 0
    
    for ((i, line) in input.split_lines().enumerate()):
        if len(line.trim()) == 0:
            continue
        end
        
        (_, buttons, joltages) := parse_line(line)
        min_presses := solve_joltage(joltages, buttons)
        
        print(f"Machine {i+1} (joltage): {min_presses} presses")
        total += min_presses
    end
    
    return total
end
```

**Part 2**:
1. Parse each machine
2. Solve joltage problem
3. Sum minimum presses for all machines

## Performance Notes

**Part 1**:
- Time: O(2^m * n) where m is buttons, n is lights
- Exponential but fast for small m (typically < 20 buttons)
- Could be optimized with Gaussian elimination over GF(2)

**Part 2**:
- Time: O(T^m) where T is total presses, m is buttons
- Uses iterative deepening to find minimum quickly
- Early termination on first solution

Both parts use brute force but work well for puzzle-sized inputs. Part 2's backtracking with early termination keeps it practical.

## Edge Cases Handled

1. **Empty button presses**: Handled by allowing 0 presses
2. **Multiple solutions**: Takes minimum
3. **No solution**: Returns large value (999999)
4. **Empty lines**: Skipped in input processing

## Complexity

- **Part 1 Time**: O(machines * 2^buttons * lights)
- **Part 1 Space**: O(lights + buttons)
- **Part 2 Time**: O(machines * T^buttons) where T is solution
- **Part 2 Space**: O(buttons) for recursion stack

## Key Insights

**Part 1** is a toggle problem (XOR system):
- Each button press toggles specific lights
- Order doesn't matter, only which buttons are pressed
- Bitmask enumeration tries all 2^m possibilities

**Part 2** is a linear counter problem:
- Each button press adds to specific counters
- Need exact target values, not just on/off
- Iterative deepening finds minimum efficiently

Both problems are constraint satisfaction problems, but Part 1 is binary (toggle) while Part 2 is counting (accumulation).
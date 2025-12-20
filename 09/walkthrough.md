# Day 09: Movie Theater - Walkthrough

## Problem Summary

We need to find the largest rectangle that can be formed using red tiles as opposite corners on a movie theater floor.

For the full problem description, see: https://adventofcode.com/2025/day/9

**Part 1**: Find the largest rectangle using any two red tiles as opposite corners (corners only need to exist, interior can be anything).

**Part 2**: Find the largest rectangle where the entire perimeter consists of red tiles.

## Input Format

List of red tile coordinates (X,Y):
```
7,1
11,1
11,7
9,7
```

## Key Concepts

### Part 1: Simple Area Calculation
For any two points (x1,y1) and (x2,y2), the rectangle area is:
- Width: `|x2 - x1| + 1`
- Height: `|y2 - y1| + 1`
- Area: `width * height`

We only need the corners to be red tiles - the interior doesn't matter.

### Part 2: Coordinate Compression + Grid Validation
This is much more complex:
1. **Coordinate compression**: Map sparse coordinates to dense grid
2. **Build connectivity**: Fill in lines between aligned red tiles
3. **Fill interiors**: Connect horizontal segments within each row
4. **Validate rectangles**: Check if entire perimeter is red

## Solution Approach

**Part 1**:
1. Try all pairs of red tiles
2. Calculate rectangle area for each pair
3. Return maximum area

**Part 2**:
1. Compress coordinates to manageable grid size
2. Build grid marking red tiles
3. Connect aligned tiles (same row/column) with red paths
4. Fill interiors of each row between leftmost and rightmost red tiles
5. Generate all possible rectangles, sorted by area descending
6. Validate each rectangle's perimeter until one is fully red

## Code Walkthrough

### Setup and Input Parsing

```lucia
import fs

input := fs.read_file(__dir__ + "input.txt")

red_tiles: list[(int, int)] =
    input.split_lines().map((line) => scope(line):
        parts := line.split(',')
        (parts[0] as int, parts[1] as int)
    end).collect()
```

Parse each line as comma-separated X,Y coordinates into tuples.

### Part 1: Brute Force Area Calculation

```lucia
fun part1() -> int:
    mutable max_area: int = 0
    n := len(red_tiles)

    for i in range(n):
        for j in range(i + 1, n):
            (x1,y1) := red_tiles[i]
            (x2,y2) := red_tiles[j]

            if x1 == x2 || y1 == y2 then continue

            w := if x1 < x2 then x2 - x1 + 1 else x1 - x2 + 1
            h := if y1 < y2 then y2 - y1 + 1 else y1 - y2 + 1

            area := w * h
            if area > max_area:
                max_area = area
            end
        end
    end

    return max_area
end
```

**Logic**:
1. Try all pairs of tiles (i, j where i < j)
2. Skip pairs that share same X or Y coordinate (would form a line, not rectangle)
3. Calculate width: `|x2 - x1| + 1` (inclusive of both endpoints)
4. Calculate height: `|y2 - y1| + 1`
5. Track maximum area found

**Example**:
- Tiles at (2,5) and (9,7)
- Width: `9 - 2 + 1 = 8`
- Height: `7 - 5 + 1 = 3`
- Area: `8 * 3 = 24`

### Part 2: Coordinate Compression

```lucia
coords: list[int] = []
for ((x,y) in red_tiles):
    coords += [x, y]
end

coords = coords.sort().dedup()

fun compress(v: int) -> int:
    coords.index_of(v)
end

tiles: list[(int,int)] = []
print("Compressing coordinates...")
for ((x,y) in red_tiles):
    tiles += [(compress(x), compress(y))]
end
```

**Why compress coordinates?**
Red tiles might be at positions like (1000, 2000). Creating a 1000x2000 grid wastes memory. Coordinate compression maps sparse coordinates to dense indices:
- Original: `[2, 7, 9, 11]`
- Compressed: `[0, 1, 2, 3]`

**Process**:
1. Collect all unique X and Y coordinates
2. Sort and remove duplicates
3. Create mapping function: original coordinate -> compressed index
4. Convert all tiles to compressed coordinates

### Part 2: Build Grid

```lucia
xs := tiles.map((p)=>p[0]).collect()
ys := tiles.map((p)=>p[1]).collect()

width := xs.max() + 1
height := ys.max() + 1

grid := array(height, array(width, 0))

print("Building grid...")
for ((x,y) in tiles):
    grid[y][x] = 1
end
```

Create a 2D grid where `grid[y][x] = 1` means red tile at compressed position (x,y).

### Part 2: Connect Aligned Tiles

```lucia
n := len(tiles)

for i in range(n):
    for j in range(n):
        if i == j then continue

        (x1,y1) := tiles[i]
        (x2,y2) := tiles[j]

        if x1 == x2:
            lo := if y1 < y2 then y1 else y2
            hi := if y1 > y2 then y1 else y2
            mutable k: int = lo
            while (k <= hi):
                grid[k][x1] = 1
                k++
            end
        end

        if y1 == y2:
            lo := if x1 < x2 then x1 else x2
            hi := if x1 > x2 then x1 else x2
            mutable k: int = lo
            while (k <= hi):
                grid[y1][k] = 1
                k++
            end
        end
    end
    print(f"Processed {i+1}/{n} tiles")
end
```

**Logic**:
For each pair of tiles:
- If they share the same X (vertical alignment): Fill all cells between them vertically
- If they share the same Y (horizontal alignment): Fill all cells between them horizontally

This creates "lines" connecting aligned red tiles, which will form the perimeter edges.

### Part 2: Fill Row Interiors

```lucia
for y in range(height):
    borders: list[int] = []
    for x in range(width):
        if grid[y][x] != 0:
            borders += [x]
        end
    end

    if len(borders) >= 2:
        lo := borders.min()
        hi := borders.max()
        mutable x: int = lo
        while (x <= hi):
            grid[y][x] = 1
            x++
        end
    end
    print(f"Processed row {y+1}/{height}")
end
```

**Logic**:
For each row:
1. Find all red tiles in that row
2. If there are at least 2 red tiles, fill everything between leftmost and rightmost
3. This ensures horizontal connectivity within rows

This step is crucial for creating valid rectangular perimeters.

### Part 2: Generate and Validate Rectangles

```lucia
rects: list[(int,int,int,int,int)] = []

for i in range(n):
    for j in range(i+1, n):
        (x1,y1) := tiles[i]
        (x2,y2) := tiles[j]

        if x1 == x2 || y1 == y2 then continue

        minx := if x1 < x2 then x1 else x2
        maxx := if x1 > x2 then x1 else x2
        miny := if y1 < y2 then y1 else y2
        maxy := if y1 > y2 then y1 else y2

        area :=
            (1 + coords[maxx] - coords[minx]) *
            (1 + coords[maxy] - coords[miny])

        rects += [(minx,miny,maxx,maxy,area)]
    end
    print(f"Generated rectangles for tile {i+1}/{n}")
end

rects = rects.sort((r)=>-r[4])
```

**Generate rectangles**:
1. Try all pairs of tiles as opposite corners
2. Skip degenerate cases (same row or column)
3. Calculate area using ORIGINAL coordinates (not compressed)
4. Store rectangle as (minx, miny, maxx, maxy, area)
5. Sort by area descending (largest first)

**Why use original coordinates for area?**
The actual distance between compressed coordinates 2 and 5 might be `coords[5] - coords[2]`, not just `5 - 2`.

### Part 2: Find First Valid Rectangle

```lucia
for ((x1,y1,x2,y2,area) in rects):
    valid := true
    for y in range(y1, y2+1):
        for x in range(x1, x2+1):
            if grid[y][x] == 0:
                valid = false
                break
            end
        end
        if !valid then break
    end

    if valid:
        return area
    end
end

return 0
```

**Logic**:
1. Check rectangles in order of decreasing area
2. For each rectangle, verify entire interior is filled with red (grid[y][x] = 1)
3. Return the first valid rectangle's area

Since we sorted by area descending, the first valid rectangle is the largest.

### Main Execution

```lucia
print(f"Part 1: {part1()}")
print(f"Part 2: {part2()}")
```

## Performance Notes

**Part 1**:
- Time: O(n^2) where n is number of red tiles
- Very fast, just checking all pairs

**Part 2**:
- Coordinate compression: O(n log n) for sorting
- Grid building: O(n)
- Connecting tiles: O(n^2) pairs * O(grid size)
- Row filling: O(height * width)
- Rectangle generation: O(n^2)
- Validation: O(n^2) rectangles * O(area) each

This solution is not optimized but works for reasonable input sizes. The progress print statements help track long-running operations.

## Edge Cases Handled

1. **Degenerate rectangles**: Skip pairs with same X or Y
2. **Coordinate order**: Use min/max to handle any corner order
3. **Sparse coordinates**: Compression handles large coordinate values
4. **No valid rectangle**: Return 0 if none found

## Complexity

- **Part 1 Time**: O(n^2)
- **Part 1 Space**: O(1)
- **Part 2 Time**: O(n^2 * grid_size + n^2 * max_area)
- **Part 2 Space**: O(grid_size) for the compressed grid

Part 2 is significantly slower due to grid building and validation, hence the progress indicators.

## Key Insight

Part 2 requires understanding that "red tiles for opposite corners" means the entire perimeter must be red, not just the four corners. The solution builds connectivity by:
1. Connecting aligned tiles (creating edges)
2. Filling row interiors (creating horizontal spans)
3. Validating complete coverage

This is essentially a geometric connectivity problem on a sparse grid.
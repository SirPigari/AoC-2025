# Day 08: Playground - Walkthrough

## Problem Summary

We need to connect electrical junction boxes using light strings. Electricity flows between connected boxes, forming circuits. We want to connect boxes that are closest together to save on string lights.

For the full problem description, see: https://adventofcode.com/2025/day/8

**Part 1**: After making the 1000 shortest connections, multiply the sizes of the three largest circuits.

**Part 2**: Keep connecting until all boxes form a single circuit. Find the X-coordinate product of the last two boxes connected.

## Input Format

3D coordinates of junction boxes, one per line:
```
162,817,812
57,618,57
906,360,560
```

Each line is `X,Y,Z` coordinates.

## Key Concepts

### Minimum Spanning Tree (MST)
This is a classic MST problem using **Kruskal's algorithm**:
1. Sort all possible connections by distance
2. Connect boxes starting with shortest distances
3. Skip connections that would create cycles (already in same circuit)

### Union-Find (Disjoint Set Union)
Efficiently track which boxes belong to which circuits:
- **Find**: Determine which circuit a box belongs to
- **Union**: Merge two circuits into one
- **Path compression**: Optimize find operations
- **Union by size**: Keep trees balanced

## Solution Approach

1. Calculate distances between all pairs of junction boxes
2. Sort connections by distance (shortest first)
3. Use union-find to track circuits as we connect boxes
4. For Part 1: Stop after 1000 connections, find three largest circuits
5. For Part 2: Continue until only one circuit remains

## Code Walkthrough

### Setup and Input Parsing

```lucia
import fs

input := fs.read_file(__dir__ + "input.txt")
box_coords: list[(int, int, int)] = input.split_lines().map((line) => scope (line):
    parts := line.split(',')
    (parts[0] as int, parts[1] as int, parts[2] as int)
end).collect()

n := len(box_coords)
```

- Parse each line into (X, Y, Z) tuple
- Store total number of boxes

### Distance Calculation

```lucia
fun dist2(a: (int,int,int), b: (int,int,int)) -> int:
    dx := a[0] - b[0]
    dy := a[1] - b[1]
    dz := a[2] - b[2]
    return dx*dx + dy*dy + dz*dz
end
```

**Why squared distance?**
- We only need relative distances for sorting
- Squared distance avoids expensive square root operations
- Formula: `distance^2 = (x2-x1)^2 + (y2-y1)^2 + (z2-z1)^2`

### Build Edge List

```lucia
fun build_edges() -> list[(int, int, int)]:
    mutable edges: list[(int, int, int)] = []
    for i in range(n):
        for j in range(i+1, n):
            edges += [(i, j, dist2(box_coords[i], box_coords[j]))]
        end
    end
    return edges.sort((a) => a[2])
end
```

**Logic**:
1. Generate all possible pairs of boxes
2. Each edge is `(box_i_index, box_j_index, squared_distance)`
3. Use `j in range(i+1, n)` to avoid duplicates and self-connections
4. Sort edges by distance (shortest first)

**Complexity**: O(n^2) pairs, O(n^2 log n) with sorting

### Union-Find Initialization

```lucia
fun make_union_find() -> (&list, &list):
    mutable parent: &list = &[i for i in range(n)]
    mutable size: &list = &array(n, 1)
    return ((parent, size))
end
```

**Data structures**:
- `parent[i]`: The parent of box `i` in the tree (initially itself)
- `size[i]`: Size of the circuit rooted at box `i` (initially 1)
- Uses reference types (`&list`) for efficient mutation

### Find Operation (with Path Compression)

```lucia
fun find(x: int, parent: &list) -> int:
    if ((*parent)[x] != x):
        p := *parent
        p[x] = find(p[x], parent)
        (*parent) = p
    end
    return (*parent)[x]
end
```

**Logic**:
1. If `x` is its own parent, it's the root -> return it
2. Otherwise, recursively find the root
3. **Path compression**: Update `parent[x]` to point directly to root
4. This flattens the tree structure for faster future lookups

**Example**:
- Before: `1 -> 2 -> 3 -> 4` (chain)
- After find(1): `1 -> 4, 2 -> 4, 3 -> 4` (all point to root)

### Union Operation (by Size)

```lucia
fun union(x: int, y: int, parent: &list, size: &list) -> bool:
    rx := find(x, parent)
    ry := find(y, parent)
    if rx == ry:
        return false
    end
    if ((*size)[rx] < (*size)[ry]):
        (rx, ry) = (ry, rx)
    end
    p := *parent
    s := *size
    p[ry] = rx
    s[rx] += s[ry]
    (*parent) = p
    (*size) = s
    return true
end
```

**Logic**:
1. Find the root of both boxes
2. If same root -> already in same circuit -> return false
3. **Union by size**: Attach smaller tree to larger tree
4. Update parent: make `ry`'s root point to `rx`'s root
5. Update size: add `ry`'s circuit size to `rx`'s
6. Return true (successful merge)

**Why union by size?**
Keeps trees balanced, preventing degeneration into long chains. This ensures O(log n) average find/union time.

### Part 1: First 1000 Connections

```lucia
fun part1() -> int:
    edges := build_edges()
    (parent, size) := make_union_find()

    mutable connections: int = 0
    for ((i, j, _) in edges):
        if connections >= 1000:
            break
        end
        union(i, j, parent, size)
        connections++
    end

    mutable circuit_sizes: list[int] = []
    for i in range(n):
        if ((*parent)[i] == i):
            circuit_sizes += [(*size)[i]]
        end
    end

    circuit_sizes = circuit_sizes.sort((x) => -x)  // descending
    return circuit_sizes[0] * circuit_sizes[1] * circuit_sizes[2]
end
```

**Logic**:
1. Build and sort all edges
2. Initialize union-find
3. Make the first 1000 connections (regardless of whether union succeeds)
4. Find all circuit roots (boxes where `parent[i] == i`)
5. Collect circuit sizes
6. Sort sizes in descending order
7. Multiply three largest sizes

**Note**: We connect 1000 pairs, but some might already be in the same circuit (union returns false), so we don't check the return value here.

### Part 2: Connect Until One Circuit

```lucia
fun part2() -> int:
    edges := build_edges()
    (parent, size) := make_union_find()
    mutable remaining_circuits: int = n

    for ((i, j, _) in edges):
        if union(i, j, parent, size):
            remaining_circuits -= 1
            if remaining_circuits == 1:
                return box_coords[i][0] * box_coords[j][0]
            end
        end
    end
end
```

**Logic**:
1. Start with `n` circuits (each box is its own circuit)
2. Try connecting boxes in order of distance
3. If union succeeds (boxes were in different circuits):
   - Decrement circuit count
   - Check if only 1 circuit remains
   - If so, return product of X coordinates
4. Continue until all boxes are connected

**Why check union return value here?**
We only decrement circuit count when we actually merge two circuits. If boxes are already connected, the circuit count doesn't change.

### Main Execution

```lucia
print(f"Part 1: {part1()}")
print(f"Part 2: {part2()}")
```

## Example Walkthrough

With 20 boxes, first 10 connections:

1. Connect boxes at positions `162,817,812` and `425,690,689` (closest pair)
   - Circuits: 19 (one circuit of size 2, eighteen of size 1)

2. Connect `162,817,812` and `431,825,988`
   - These merge into existing circuit
   - Circuits: 18 (one circuit of size 3, seventeen of size 1)

3. Connect `906,360,560` and `805,96,715`
   - Circuits: 17 (one of size 3, one of size 2, fifteen of size 1)

4. Try to connect `431,825,988` and `425,690,689`
   - Already in same circuit, union returns false
   - Circuits: still 17

After 10 connections: 11 circuits with sizes [5, 4, 2, 2, 1, 1, 1, 1, 1, 1, 1]
Product of three largest: 5 * 4 * 2 = 40

## Performance Notes

- **Edge generation**: O(n^2) pairs
- **Sorting edges**: O(n^2 log n)
- **Union-find operations**: Nearly O(1) amortized with path compression and union by size
- **Overall**: O(n^2 log n) dominated by sorting

For large inputs (many junction boxes), this is efficient and practical.

## Edge Cases Handled

1. **Self-loops**: Prevented by `j in range(i+1, n)`
2. **Duplicate edges**: Only one edge per pair (i < j)
3. **Already connected**: Union returns false, handled appropriately
4. **Empty circuits**: Only count boxes that are circuit roots

## Complexity

- **Time**: O(n^2 log n) for sorting, O(n^2 * α(n)) for unions where α is inverse Ackermann (nearly constant)
- **Space**: O(n^2) for storing all edges
- **Union-Find**: O(α(n)) ≈ O(1) per operation with optimizations

## Key Insight

This is Kruskal's algorithm for minimum spanning tree! The union-find data structure is essential for efficiently tracking connected components and detecting cycles. Path compression and union by size ensure near-constant time operations even with many boxes.
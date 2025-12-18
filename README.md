# Advent of Code 2025

Solutions to [Advent of Code 2025](https://adventofcode.com/2025/) written in [Lucia Programming Language](https://github.com/SirPigari/lucia-rust/) (commit `93c3c2a`).

## Purpose

This repository contains my solutions to the Advent of Code 2025 challenges.  
It is intended to showcase problem-solving in the Lucia programming language and serve as a reference for others exploring Lucia.

## Structure

Each day's solution is contained in its own directory named `XX`, where `XX` is the day number (e.g., `01`, `02`, ..., `25`).  
Each directory contains:
- `input.txt`: The input data for that day's challenge.
- `solution.lc`: The Lucia code that solves the challenge.
- `walkthrough.md`: A detailed explanation of the solution approach and logic.

## Usage

To run a solution for a specific day, navigate to that day's directory and execute the `solution.lc` file using the Lucia interpreter. For example, to run the solution for Day 1:
```bash
lucia ./01/solution.lc -q
```

The `-q` flag suppresses debug output, showing only the final results. (Speeding up the execution slightly.)

You do not need to cd into the directory; just provide the path to the file.
Lucia handles this with the `__dir__` variable to locate the `input.txt` file.

## Completed Solutions

| Day | Title                     | Part 1 | Part 2 | Walkthrough                 |
|-----|---------------------------|--------|--------|-----------------------------|
| 01  | Secret Entrance           | Yes    | Yes    | [Link](./01/walkthrough.md) |
| 02  | Gift Shop                 | Yes    | Yes    | [Link](./02/walkthrough.md) |
| 03  | Lobby                     | Yes    | Yes    | [Link](./03/walkthrough.md) |
| 04  | Printing Department       | Yes    | Yes    | [Link](./04/walkthrough.md) |
| 05  | Cafeteria                 | Yes    | Yes    | [Link](./05/walkthrough.md) |
| 06  | Trash Compactor           | Yes    | Yes    | [Link](./06/walkthrough.md) |
| 07  | Laboratories              | Yes    | Yes    | [Link](./07/walkthrough.md) |
| 08  | Playground                | Yes    | Yes    | [Link](./08/walkthrough.md) |
| 09  | Movie Theater             | No     | No     | No                          |
| 10  | Factory                   | No     | No     | No                          |
| 11  | Reactor                   | No     | No     | No                          |
| 12  | Christmas Tree Farm       | No     | No     | No                          |

## Contribution

Since this is primarily a personal collection of solutions, contributions are not required.  
If you do want to submit improvements or fixes, feel free to open an issue or pull request.

## Notes

- The solutions are optimized for clarity and learning, not necessarily for performance.
- Some solutions may rely on features specific to Lucia at the referenced commit.

## License

This repository is released into the **public domain**. You may use, modify, distribute, or do anything you like with the code without restriction. No license is applied, and the author offers no warranty or guarantee of any kind.

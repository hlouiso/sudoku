# Sudoku (C)

A **generalized Sudoku solver** written in C. Built as part of my **Master’s in Cryptology & Computer Security at the University of Bordeaux**, it was also my first time learning C.
## Features
- Solves classic 9×9 and larger **N×N** boards (where `N` is a perfect square, e.g., 4, 9, 16, 25, 36, 64).
- Reads a puzzle from via file redirection and prints the solved grid (all solutions or only one), or reports failure.
- Can also generate solvable grids, with unique or multiple solutions.
- Minimal dependencies: a C compiler and `make`.

## Build
```bash
make
# optional cleanup
make clean

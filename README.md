# Sudoku (C)

A **generalized Sudoku solver** written in C. Built as part of my **Master’s in Cryptology & Computer Security at the University of Bordeaux**—it was also my first time learning C, so the code favors clarity over clever tricks. The project is intentionally minimal and easy to compile/run.

## Features
- Solves classic 9×9 and other sizes supported by the code.
- Reads a puzzle from **stdin** (or via file redirection) and prints the solved grid or reports failure.
- Minimal dependencies: a C compiler and `make`.

## Build
```bash
make
# optional cleanup
make clean

#ifndef GRID_H
#define GRID_H

#define MAX_GRID_SIZE 64
#define EMPTY_CELL '_'
#define DICE 4
#define RATIO 4

#include "colors.h"

static const char color_table[] = "123456789"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "@"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "&*";

typedef struct
{
  size_t row;
  size_t column;
  colors_t color;
} choice_t;

typedef enum
{
  mode_first,
  mode_all
} mode_t;

/* Sudoku grid (forward declaration to hide the implementation) */
typedef struct _grid_t grid_t;

/* Allocates memory and returns a pointer to a new grid with a choosen size */
grid_t *grid_alloc(size_t size);

/* Frees the memory allocated for a grid */
void grid_free(grid_t *grid);

/* Prints a grid in a file */
void grid_print(const grid_t *grid, FILE *fd);

/* Returns a boolean telling if a character is accepted in a sized grid */
bool grid_check_char(const grid_t *grid, const char c);

/* Returns a boolean telling if grid has an accepted size */
bool grid_check_size(const size_t size);

/* Copies a grid in a new allocated memory and returns a pointer to the copy */
grid_t *grid_copy(const grid_t *grid);

/* Copies a grid 'grid' in another grid 'copy' */
void grid_copy2(const grid_t *grid, grid_t *copy);

/* Returns a character string containing all colors of grid cell
   (seen as a color set) */
char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column);

/* Returns the size of a grid */
size_t grid_get_size(const grid_t *grid);

/* Sets a grid cell as a choosen singleton */
void grid_set_cell(grid_t *grid, const size_t row, const size_t column,
                   const char color);

/* Returns a boolean telling if a grid has only singletons */
bool grid_is_solved(grid_t *grid);

/* Returns a boolean telling if a grid has an inconsistency
   (regarding sudoku rules) */
bool grid_is_consistent(grid_t *grid);

/* Applies a function to all subgrids of a grid, and returns true if the
   function returned true for each subgrid */
bool subgrid_apply(grid_t *grid,
                   bool (*func)(colors_t *subgrid[], const size_t size));

typedef enum
{
  grid_solved,
  grid_unsolved,
  grid_inconsistent
} status_t;

/* Applies subgrid_heuristics on each subgrid as many times as it becomes
   useless, and return status of the modified grid */
status_t grid_heuristics(grid_t *grid);

/* Behaves like grid_print but prints in stdout all possible characters instead
   of _ (debug purpose)*/
void grid_print2(const grid_t *grid);

/* Returns a boolean telling if the color set of a choice is empty */
bool grid_choice_is_empty(const choice_t choice);

/* Applies a choice to a grid */
void grid_choice_apply(grid_t *grid, const choice_t choice);

/* Discards a choice from a grid */
void grid_choice_discard(grid_t *grid, const choice_t choice);

/* Writes a description of a choice in a file */
void grid_choice_print(const choice_t choice, FILE *fd);

/* Chooses the smallest set of colors in a whole grid, selects the rightmost
   color and returns this choice */
choice_t grid_choice(grid_t *grid);

/* Chooses the smallest set of colors in a whole grid, selects a random color
   color and returns this choice */
choice_t grid_choice_random(grid_t *grid);

/* Will solve a given grid, searching for at least one solution if mode is
   mode_first, all solutions possible if mode_all.
   Boolean pointed by 'error' will be set to true if 0 solution is found in
   mode_all
   Output file is used only in mode_all, to print all found solutions
   if boolean 'random' is true, backtrack_first will call grid_choice_random
   instead of grid_choice()                                                  */
grid_t *grid_solver(grid_t *grid, const mode_t mode, bool *error, FILE *output,
                    bool random);

/* Uses backtrack method to search to a grid solution */
void backtrack_first(grid_t *grid, bool *solution_found, bool random);

/* Uses backtrack method to search to""back" all solutions of a grid */
void backtrack_all(grid_t *grid, int *solution_count, FILE *output);

/* Generates a grid of a choosen size and returns a pointer to it */
grid_t *grid_generation(int size, bool unique);

/* Returns a boolean telling if a grid has a unique solution */
bool solution_is_unique(grid_t *grid);

/* It's a backtrack method applied on a grid that we know has a solution,
   it will set *solution_count to 2 if grid has at least 2 solutions.
   Used only in solution_is_unique()                                     */
void backtrack_unique_solution(grid_t *grid, int *solution_count);

#endif /* GRID_H */
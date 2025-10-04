#include "grid.h"

#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include "colors.h"

/* Internal structure (hidden from outside) for a sudoku grid */

struct _grid_t
{
  size_t size;
  colors_t **cells;
};

grid_t *grid_alloc(size_t size)
{
  if (!grid_check_size(size))
  {
    return NULL;
  }

  grid_t *ptr = malloc(sizeof(struct _grid_t));

  if (ptr == NULL)
  {
    return NULL;
  }

  ptr->size = size;
  colors_t **ptr2 = calloc(size, sizeof(colors_t *));

  if (ptr2 == NULL)
  {
    free(ptr);
    return NULL;
  }

  for (size_t i = 0; i < size; i++)
  {
    ptr2[i] = calloc(size, sizeof(colors_t));
    if (ptr2[i] == NULL)
    {
      for (size_t j = 0; j < i; j++)
      {
        free(ptr2[j]);
      }
      free(ptr2);
      free(ptr);
      return NULL;
    }
  }
  ptr->cells = ptr2;
  return ptr;
}

void grid_free(grid_t *grid)
{
  if (grid != NULL)
  {
    for (size_t i = 0; i < grid->size; i++)
    {
      free(grid->cells[i]);
    }
    free(grid->cells);
    free(grid);
  }
}

void grid_print(const grid_t *grid, FILE *fd)
{
  if (grid != NULL)
  {
    for (size_t i = 0; i < grid->size; i++)
    {
      for (size_t j = 0; j < grid->size; j++)
      {
        if (!colors_is_singleton(grid->cells[i][j]))
        {
          fprintf(fd, "%c ", EMPTY_CELL);
        }
        else
        {
          char *ptr = grid_get_cell(grid, i, j);
          fprintf(fd, "%s ", ptr);
          free(ptr);
        }
      }
      fprintf(fd, "\n");
    }
    fprintf(fd, "\n");
  }
}

void grid_print2(const grid_t *grid)
{
  if (grid != NULL)
  {
    for (size_t i = 0; i < grid->size; i++)
    {
      for (size_t j = 0; j < grid->size; j++)
      {
        char *ptr = grid_get_cell(grid, i, j);
        printf("%s ", ptr);
        free(ptr);
      }
      printf("\n");
    }
    printf("\n");
  }
}

bool grid_check_char(const grid_t *grid, const char c)
{
  if (grid == NULL)
  {
    return false;
  }
  if (c == EMPTY_CELL)
  {
    return true;
  }
  for (size_t i = 0; i < grid->size; i++)
  {
    if (c == color_table[i])
    {
      return true;
    }
  }
  return false;
}

bool grid_check_size(const size_t size)
{
  return (size == 1 || size == 4 || size == 9 || size == 16 || size == 25 ||
          size == 36 || size == 49 || size == 64);
}

grid_t *grid_copy(const grid_t *grid)
{
  if (grid == NULL)
  {
    return NULL;
  }

  grid_t *copy = grid_alloc(grid->size);

  if (copy == NULL)
  {
    return NULL;
  }

  copy->size = grid->size;

  for (size_t i = 0; i < grid->size; i++)
  {
    for (size_t j = 0; j < grid->size; j++)
    {
      copy->cells[i][j] = grid->cells[i][j];
    }
  }

  return copy;
}

void grid_copy2(const grid_t *grid, grid_t *copy)
{
  if ((grid != NULL) && (copy != NULL) && (copy->size == grid->size))
  {
    for (size_t i = 0; i < grid->size; i++)
    {
      for (size_t j = 0; j < grid->size; j++)
      {
        copy->cells[i][j] = grid->cells[i][j];
      }
    }
  }
}

char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column)
{
  if (grid == NULL || (row + 1) > grid->size || (column + 1) > grid->size)
  {
    return NULL;
  }

  colors_t cell = grid->cells[row][column];
  char *s = calloc(colors_count(cell) + 1, sizeof(char));
  if (s == NULL)
  {
    return NULL;
  }
  size_t s_row = 0;

  for (size_t i = 0; i < grid->size; i++)
  {
    if (colors_is_in(cell, i))
    {
      s[s_row] = color_table[i];
      s_row++;
    }
  }
  return s;
}

size_t grid_get_size(const grid_t *grid)
{
  if (grid == NULL)
  {
    return 0;
  }
  return grid->size;
}

void grid_set_cell(grid_t *grid, const size_t row, const size_t column,
                   const char color)
{
  if (grid != NULL && row < grid->size && column < grid->size)
  {
    if (color == EMPTY_CELL)
    {
      grid->cells[row][column] = colors_full(grid->size);
    }
    else
    {
      char c = 1;
      size_t i = 0;
      while (c != color)
      {
        c = color_table[i];
        i++;
      }
      grid->cells[row][column] = colors_set(i - 1);
      /* i - 1 because colors_set(-1) returns 0, colors_set(0) returns 1... */
    }
  }
}

bool grid_is_solved(grid_t *grid)
{
  for (size_t row = 0; row < grid->size; row++)
  {
    for (size_t column = 0; column < grid->size; column++)
    {
      if (!colors_is_singleton(grid->cells[row][column]))
      {
        return false;
      }
    }
  }
  return true;
}

bool subgrid_apply(grid_t *grid,
                   bool (*func)(colors_t *subgrid[], const size_t size))
{
  size_t size = grid->size;
  colors_t *subgrid[size];

  /* ============================== Rows ============================== */
  for (size_t row = 0; row < size; row++)
  {
    for (size_t column = 0; column < size; column++)
    {
      subgrid[column] = &grid->cells[row][column];
    }
    if (!func(subgrid, size))
    {
      return false;
    }
  }

  /* ============================= Columns ============================= */
  for (size_t column = 0; column < size; column++)
  {
    for (size_t row = 0; row < size; row++)
    {
      subgrid[row] = &grid->cells[row][column];
    }
    if (!func(subgrid, size))
    {
      return false;
    }
  }

  /* ============================= Blocks ============================= */
  size_t sqr = sqrt(size);
  size_t block_row = 0;
  size_t block_column = 0;
  for (size_t block = 0; block < size; block++)
  {
    for (size_t i = 0; i < sqr; i++)
    {
      for (size_t j = 0; j < sqr; j++)
      {
        subgrid[sqr * i + j] = &grid->cells[block_row + i][block_column + j];
      }
    }

    if (!func(subgrid, size))
    {
      return false;
    }

    if (block_column < size - sqr)
    {
      block_column += sqr;
    }
    else
    {
      block_column = 0;
      block_row += sqr;
    }
  }
  return true;
}

bool grid_is_consistent(grid_t *grid)
{
  return subgrid_apply(grid, subgrid_consistency);
}

status_t grid_heuristics(grid_t *grid)
{
  size_t size = grid->size;
  colors_t *subgrid[size];
  bool changed = true;

  while (changed)
  {
    changed = false;

    /* ============================== Rows ============================== */
    for (size_t row = 0; row < size; row++)
    {
      for (size_t column = 0; column < size; column++)
      {
        subgrid[column] = &grid->cells[row][column];
      }
      changed = (subgrid_heuristics(subgrid, size) || changed);
    }

    /* ============================= Columns ============================= */
    for (size_t column = 0; column < size; column++)
    {
      for (size_t row = 0; row < size; row++)
      {
        subgrid[row] = &grid->cells[row][column];
      }
      changed = (subgrid_heuristics(subgrid, size) || changed);
    }

    /* ============================= Blocks ============================= */
    size_t sqr = sqrt(size);
    size_t block_row = 0;
    size_t block_column = 0;
    for (size_t block = 0; block < size; block++)
    {
      for (size_t i = 0; i < sqr; i++)
      {
        for (size_t j = 0; j < sqr; j++)
        {
          subgrid[sqr * i + j] = &grid->cells[block_row + i][block_column + j];
        }
      }
      changed = (subgrid_heuristics(subgrid, size) || changed);
      if (block_column < size - sqr)
      {
        block_column += sqr;
      }
      else
      {
        block_column = 0;
        block_row += sqr;
      }
    }

    if (!grid_is_consistent(grid))
    {
      return grid_inconsistent;
    }
  }

  if (grid_is_solved(grid))
  {
    return grid_solved;
  }

  return grid_unsolved;
}

bool grid_choice_is_empty(const choice_t choice) { return (choice.color == 0); }

void grid_choice_apply(grid_t *grid, const choice_t choice)
{
  grid->cells[choice.row][choice.column] = choice.color;
}

void grid_choice_discard(grid_t *grid, const choice_t choice)
{
  grid->cells[choice.row][choice.column] =
      colors_subtract(grid->cells[choice.row][choice.column], choice.color);
}

void grid_choice_print(const choice_t choice, FILE *fd)
{
  fprintf(fd, "row: %zu\ncolumn: %zu\n", choice.row, choice.column);
  int i = 0;

  while (!colors_is_in(choice.color, i))
  {
    i++;
  }

  fprintf(fd, "choice: %c\n", color_table[i]);
}

choice_t grid_choice(grid_t *grid)
{
  choice_t choice;

  if (grid_is_solved(grid))
  {
    choice.color = 0;
    return choice;
  }

  size_t size_of_choice = grid->size + 1;
  for (size_t row = 0; row < grid->size; row++)
  {
    for (size_t column = 0; column < grid->size; column++)
    {
      if (!colors_is_singleton(grid->cells[row][column]))
      {
        if (colors_count(grid->cells[row][column]) == 2)
        {
          choice.row = row;
          choice.column = column;
          choice.color = colors_rightmost(grid->cells[row][column]);
          return choice;
        }

        else
        {
          if (colors_count(grid->cells[row][column]) < size_of_choice)
          {
            size_of_choice = colors_count(grid->cells[row][column]);
            choice.row = row;
            choice.column = column;
            choice.color = colors_rightmost(grid->cells[row][column]);
          }
        }
      }
    }
  }
  return choice;
}

choice_t grid_choice_random(grid_t *grid)
{
  choice_t choice;

  if (grid_is_solved(grid))
  {
    choice.color = 0;
    return choice;
  }

  size_t size_of_choice = grid->size + 1;
  for (size_t row = 0; row < grid->size; row++)
  {
    for (size_t column = 0; column < grid->size; column++)
    {
      if (!colors_is_singleton(grid->cells[row][column]))
      {
        if (colors_count(grid->cells[row][column]) == 2)
        {
          choice.row = row;
          choice.column = column;
          choice.color = colors_random(grid->cells[row][column]);
          return choice;
        }

        else
        {
          if (colors_count(grid->cells[row][column]) < size_of_choice)
          {
            size_of_choice = colors_count(grid->cells[row][column]);
            choice.row = row;
            choice.column = column;
            choice.color = colors_random(grid->cells[row][column]);
          }
        }
      }
    }
  }
  return choice;
}

void backtrack_first(grid_t *grid, bool *solution_found, bool random)
{
  choice_t choice;

  status_t result = grid_heuristics(grid);

  if (result == grid_inconsistent)
  {
    return;
  }

  if (result == grid_solved)
  {
    *solution_found = true;
    return;
  }

  if (random)
  {
    choice = grid_choice_random(grid);
  }

  else
  {
    choice = grid_choice(grid);
  }

  grid_t *copy = grid_copy(grid);
  if (copy == NULL)
  {
    return;
  }
  grid_choice_apply(grid, choice);
  backtrack_first(grid, solution_found, random);

  if (*solution_found)
  {
    grid_free(copy);
    return;
  }

  copy->cells[choice.row][choice.column] =
      colors_subtract(copy->cells[choice.row][choice.column], choice.color);
  grid_copy2(copy, grid);
  grid_free(copy);
  backtrack_first(grid, solution_found, random);
}

void backtrack_all(grid_t *grid, int *solution_count, FILE *output)
{
  status_t result = grid_heuristics(grid);

  if (result == grid_inconsistent)
  {
    return;
  }

  if (result == grid_solved)
  {
    (*solution_count)++;
    fprintf(output, "Solution %d:\n", *solution_count);
    grid_print(grid, output);
    return;
  }

  choice_t choice = grid_choice(grid);
  grid_t *copy = grid_copy(grid);
  if (copy == NULL)
  {
    return;
  }
  grid_choice_apply(grid, choice);
  backtrack_all(grid, solution_count, output);

  copy->cells[choice.row][choice.column] =
      colors_subtract(copy->cells[choice.row][choice.column], choice.color);
  grid_copy2(copy, grid);
  grid_free(copy);

  backtrack_all(grid, solution_count, output);
}

grid_t *grid_solver(grid_t *grid, mode_t mode, bool *error, FILE *output,
                    bool random)
{
  if (mode == mode_first)
  {
    bool solution_found = false;
    backtrack_first(grid, &solution_found, random);

    if (grid_is_solved(grid))
    {
      return grid;
    }

    else
    {
      grid_free(grid);
      return NULL;
    }
  }

  int solution_count = 0;
  backtrack_all(grid, &solution_count, output);
  fprintf(output, "%d solution(s) found\n", solution_count);

  if (solution_count == 0)
  {
    *error = true;
  }

  return NULL;
}

/* =========== All these last functions are for grid generation =========== */

void backtrack_unique_solution(grid_t *grid, int *solution_count)
{
  status_t result = grid_heuristics(grid);

  if (result == grid_inconsistent)
  {
    return;
  }

  if (result == grid_solved)
  {
    (*solution_count)++;
    return;
  }

  choice_t choice = grid_choice(grid);
  grid_t *copy = grid_copy(grid);
  if (copy == NULL)
  {
    return;
  }
  grid_choice_apply(grid, choice);
  backtrack_unique_solution(grid, solution_count);

  if (*solution_count == 2)
  {
    grid_free(copy);
    return;
  }

  copy->cells[choice.row][choice.column] =
      colors_subtract(copy->cells[choice.row][choice.column], choice.color);
  grid_copy2(copy, grid);
  grid_free(copy);

  backtrack_unique_solution(grid, solution_count);
}

bool solution_is_unique(grid_t *grid)
{
  int solution_count = 0;
  backtrack_unique_solution(grid, &solution_count);
  if (solution_count == 2)
  {
    return false;
  }
  return true;
}

grid_t *grid_generation(int size, bool unique)
{
  float ratio = size * size / RATIO;
  int hidden_cases = 0;
  size_t size_2 = (size_t)size;
  grid_t *grid = grid_alloc(size_2);
  if (grid == NULL)
  {
    return NULL;
  }
  colors_t fill_rate = colors_full(DICE);

  for (size_t row = 0; row < size_2; row++) /* Let's get a totally full grid */
  {
    for (size_t column = 0; column < size_2; column++)
    {
      grid->cells[row][column] = colors_full(size_2);
    }
  }

  grid_solver(grid, mode_first, NULL, NULL, true);

  while (hidden_cases <= ratio)
  {
    for (size_t row = 0; row < size_2; row++)
    {
      for (size_t column = 0; column < size_2; column++)
      {
        if (colors_random(fill_rate) == 1)
        {
          if (grid->cells[row][column] != colors_full(size_2))
          {
            hidden_cases++;
            grid->cells[row][column] = colors_full(size_2);
            if (hidden_cases > ratio)
            {
              goto hidden_enough;
            }
          }
        }
      }
    }
  }

hidden_enough:

  if (unique)
  {
    grid_t *copy = grid_copy(grid);
    if (copy == NULL)
    {
      return NULL;
    }

    if (!solution_is_unique(copy))
    {
      grid_free(copy);
      grid_free(grid);
      return grid_generation(size, unique);
    }

    grid_free(copy);
  }

  return grid;
}
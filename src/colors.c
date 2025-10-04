#include "colors.h"

#include <stdlib.h>
#include <unistd.h> /* getpid() */

#include <math.h>
#include <time.h> /* random function */

bool seed_initialized = false;

colors_t colors_full(const size_t size)
{
  if (size == 0)
  {
    return 0;
  }
  if (size > MAX_COLORS - 1)
  {
    return -1ULL;
  }
  return -1ULL >> (MAX_COLORS - size);
}

colors_t colors_empty(void) { return 0; }

colors_t colors_set(const size_t color_id)
{
  if (color_id > MAX_COLORS - 1)
  {
    return 0;
  }
  return 1ULL << color_id;
}

colors_t colors_add(const colors_t colors, const size_t color_id)
{
  return colors_set(color_id) | colors;
}

colors_t colors_discard(const colors_t colors, const size_t color_id)
{
  return (~colors_set(color_id)) & colors;
}

bool colors_is_in(const colors_t colors, const size_t color_id)
{
  return (colors & colors_set(color_id)) != 0;
}

colors_t colors_negate(const colors_t colors) { return ~colors; }

colors_t colors_and(const colors_t colors1, const colors_t colors2)
{
  return colors1 & colors2;
}

colors_t colors_or(const colors_t colors1, const colors_t colors2)
{
  return colors1 | colors2;
}

colors_t colors_xor(const colors_t colors1, const colors_t colors2)
{
  return colors1 ^ colors2;
}

colors_t colors_subtract(const colors_t colors1, const colors_t colors2)
{
  return colors1 & (~colors2);
}

bool colors_is_equal(const colors_t colors1, const colors_t colors2)
{
  return colors1 == colors2;
}

bool colors_is_subset(const colors_t colors1, const colors_t colors2)
{
  return ((~colors2) & colors1) == 0;
}

bool colors_is_singleton(const colors_t colors)
{
  if (colors == 0)
  {
    return false;
  }
  return (colors & (colors - 1)) == 0;
}

size_t colors_count(const colors_t colors)
{
  colors_t B5 = -1ULL >> 32;
  colors_t B4 = B5 ^ (B5 << 16);
  colors_t B3 = B4 ^ (B4 << 8);
  colors_t B2 = B3 ^ (B3 << 4);
  colors_t B1 = B2 ^ (B2 << 2);
  colors_t B0 = B1 ^ (B1 << 1);
  colors_t x = colors;
  x = ((x >> 1) & B0) + (x & B0);
  x = ((x >> 2) & B1) + (x & B1);
  x = ((x >> 4) + x) & B2;
  x = ((x >> 8) + x) & B3;
  x = ((x >> 16) + x) & B4;
  x = ((x >> 32) + x) & B5;
  return x;
}

colors_t colors_rightmost(const colors_t colors) { return colors & -colors; }

colors_t colors_leftmost(const colors_t colors)
{
  size_t i = 0;
  if (colors == 0)
  {
    return 0;
  }
  colors_t a = colors;
  while (a != 0)
  {
    a = a >> 1;
    i += 1;
  }
  return colors_set(i - 1);
}

colors_t colors_random(const colors_t colors)
{
  if (colors == 0)
  {
    return 0;
  }

  /* Let's put all the colors of colors in the smallest array */
  size_t size_of_array = colors_count(colors);
  size_t T[size_of_array];
  int k = 0;

  for (size_t i = 0; i < MAX_COLORS; i++)
  {
    if (colors_is_in(colors, i))
    {
      T[k] = i;
      k++;
    }
  }

  if (!seed_initialized)
  {
    srand((time(NULL) * getpid()) % MAX_COLORS);
    seed_initialized = true;
  }

  int a = rand() % size_of_array;
  return colors_set(T[a]);
}

bool subgrid_consistency(colors_t *subgrid[], const size_t size)
{
  colors_t do_all_colors_appear = colors_empty();
  for (size_t cell = 0; cell < size; cell++)
  {
    if (*subgrid[cell] == colors_empty())
    {
      return false;
    }

    if (colors_is_singleton(*subgrid[cell]))
    {
      for (size_t i = cell + 1; i < size; i++)
      {
        if (*subgrid[cell] == *subgrid[i])
        {
          return false;
        }
      }
    }

    do_all_colors_appear = colors_or(do_all_colors_appear, *subgrid[cell]);
  }
  return (do_all_colors_appear == colors_full(size));
}

bool subgrid_heuristics(colors_t *subgrid[], size_t size)
{
  bool changed = false;
  colors_t color = 0;
  size_t count;

  /* ========================= cross-hatching ========================= */

  for (size_t i = 0; i < size; i++)
  {
    if (colors_is_singleton(*subgrid[i]))
    {
      color = colors_or(color, *subgrid[i]);
    }
  }

  for (size_t i = 0; i < size; i++)
  {
    if (!colors_is_singleton(*subgrid[i]))
    {
      if (colors_and(*subgrid[i], color) != 0)
      {
        *subgrid[i] = colors_subtract(*subgrid[i], color);
        changed = true;
      }
    }
  }

  if (changed) /* (1) */
  {
    return true;
  }

  /* ========================= lone number ========================== */
  int occurence_count[size];

  for (size_t i = 0; i < size; i++)
  {
    occurence_count[i] = 0;
  }

  size_t bit_position = 0;

  for (size_t i = 0; i < size; i++)
  {
    color = *subgrid[i];
    if (!colors_is_singleton(color))
    {
      /* No need to deal with singletons because we called cross-hatching as
         many time as needed to make them unique and isolated, thanks to (1) */
      while (color != 0)
      {
        /* We want to increment by 1 the counts of all colours
               appearing in color, let's think about bits !   */
        if ((color & 1) == 1)
        {
          occurence_count[bit_position]++;
        }
        color = (color >> 1);
        bit_position++;
      }
      bit_position = 0;
    }
  }

  for (size_t i = 0; i < size; i++)
  {
    if (occurence_count[i] == 1) /* The key point */
    {
      color = colors_set(i);
      for (size_t j = 0; j < size; j++)
      {
        if (colors_is_subset(color, *subgrid[j]))
        {
          changed = true;
          *subgrid[j] = color;
          break; /* There was only 1 cell to change */
        }
      }
    }
  }

  if (changed)
  {
    return true;
  }

  /* ========================= naked-subset ========================== */

  count = 1;
  colors_t memory[size / 2];
  size_t size_memory = 0;

  for (size_t i = 0; i < size; i++)
  {
    if (!colors_is_singleton(*subgrid[i]))
    {
      for (size_t j = 0; j < size_memory; j++)
      {
        if (*subgrid[i] == memory[j])
        {
          goto no_need;
        }
      }

      memory[size_memory] = *subgrid[i];
      size_memory++;

      for (size_t j = 0; j < size; j++)
      {
        if ((i != j) && (*subgrid[i] == *subgrid[j]))
        {
          count++;
        }
      }

      if (count == colors_count(*subgrid[i]))
      {
        for (size_t j = 0; j < size; j++)
        {
          if (colors_is_subset(*subgrid[i], *subgrid[j]) &&
              (*subgrid[i] != *subgrid[j]))
          {
            changed = true;
            *subgrid[j] = colors_xor(*subgrid[i], *subgrid[j]);
          }
        }
      }
      count = 1;
    }
  no_need:
  }

  return changed;

  /* I'm not removing the code of hidden-subset but I don't
     use it, you can read my report to know why            */

  if (changed)
  {
    return true;
  }

  /* ========================= hidden-subset ========================== */

  count = 0;
  size_t N; /* The N candidates from the Homework 4 subject */

  for (size_t i = 0; i < size; i++)
  {
    for (size_t j = 0; j < size; j++)
    {
      if ((j != i) && (colors_and(*subgrid[i], *subgrid[j]) != 0) &&
          (!colors_is_singleton(colors_and(*subgrid[i], *subgrid[j]))))
      {
        color = colors_and(*subgrid[i], *subgrid[j]);
        N = colors_count(color);
        for (size_t k = 0; k < size; k++)
        {
          if (colors_and(color, *subgrid[k]) != 0)
          {
            count++;
          }
        }
        if (count == N)
        {
          for (size_t k = 0; k < size; k++)
          {
            if ((colors_and(color, *subgrid[k]) != 0) &&
                (colors_and(color, *subgrid[k]) != *subgrid[k]))
            {
              changed = true;
              *subgrid[k] = colors_and(color, *subgrid[k]);
            }
          }
        }
        count = 0;
      }
    }
  }

  return changed;
}
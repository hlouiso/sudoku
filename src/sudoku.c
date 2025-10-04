#include "sudoku.h"

#include <stdlib.h>
#include <unistd.h>

#include <err.h>
#include <getopt.h>

#include "grid.h"

#define DEFAULT_SIZE 9

static bool verbose = false;
static FILE *output;
static bool error = false;

static grid_t *file_parser(char *file_name)
{
  FILE *file = fopen(file_name, "r");
  if (file == NULL)
  {
    warn("Error on file %s", file_name);
    return NULL;
  }

  size_t size_row = 0; /* for size of row */
  char row[MAX_GRID_SIZE];
  int c = fgetc(file);
  bool comment = false;

  /* ======================== Processing first line ======================== */
  while (c != EOF && !(c == '\n' && size_row != 0))
  {
    switch (c)
    {
      case '#':
        comment = true;
        break;

      case '\n':
        comment = false;
        break;
    }

    if (!(comment) && c != '\n' && c != '\t' && c != '\r' && c != ' ')
    {
      row[size_row] = c;
      size_row++;
    }
    /* '\r' for Windows Users... */
    c = fgetc(file);
  }

  if (c == EOF && size_row != 1)
  {
    if (size_row == 0)
    {
      warnx("Warning: File %s has no significant character.\n", file_name);
    }
    else
    {
      warnx("Warning: File %s has only one line: then should contain only"
            " one significant character, not %zu.\n",
            file_name, size_row);
    }
    fclose(file);
    return NULL;
  }

  if (size_row != 1 && size_row != 4 && size_row != 9 && size_row != 16 &&
      size_row != 25 && size_row != 36 && size_row != 49 && size_row != 64)
  {
    warnx("Warning: In file %s, number of significant characters in line"
          " 1: %zu is not an accepted size.\n",
          file_name, size_row);
    fclose(file);
    return NULL;
  }

  grid_t *grid = grid_alloc(size_row);

  if (grid == NULL)
  {
    warnx("Error: Impossible to alloc memory for a new grid\n");
    fclose(file);
    return NULL;
  }

  for (size_t i = 0; i < size_row; i++)
  {
    if (!grid_check_char(grid, row[i]))
    {
      warnx("Warning: In file %s, character '%c' in column %zu, line 1 is "
            "not accepted for grids of size %zu.\n",
            file_name, row[i], i + 1, size_row);
      goto error_file;
    }
  }

  for (size_t i = 0; i < size_row; i++)
  {
    grid_set_cell(grid, 0, i, row[i]);
  }

  /* ======================== Processing other lines ======================= */
  size_t expected_size = size_row;
  for (size_t line = 1; line < expected_size; line++)
  {
    size_row = 0;
    comment = false;
    c = fgetc(file);
    while (c != EOF && !(c == '\n' && size_row != 0))
    {
      switch (c)
      {
        case '#':
          comment = true;
          break;

        case '\n':
          comment = false;
          break;
      }

      if (!(comment) && c != '\n' && c != '\t' && c != '\r' && c != ' ')
      {
        if (!grid_check_char(grid, c))
        {
          warnx("Warning: In file %s, character '%c' in column %zu, line %zu"
                " is not accepted for grids of size %zu.\n",
                file_name, c, size_row + 1, line + 1, expected_size);
          goto error_file;
        }
        row[size_row] = c;
        size_row++;
      }
      c = fgetc(file);
    }

    if (c == EOF && line != expected_size - 1)
    {
      warnx("Warning: In file %s, you have only %zu lines. You need %zu to "
            "respect your first line size.\n",
            file_name, line, expected_size);
      goto error_file;
    }

    if (size_row != expected_size)
    {
      warnx("Warning: In file %s, line %zu has %zu significant characters. "
            "You need %zu to respect your first line size.\n",
            file_name, line + 1, size_row, expected_size);
      goto error_file;
    }

    for (size_t j = 0; j < expected_size; j++)
    {
      grid_set_cell(grid, line, j, row[j]);
    }
  }

  while (c != EOF) /* Let's check there is not another line */
  {
    c = fgetc(file);

    if (c == '#')
    {
      while (c != '\n' && c != EOF)
      {
        c = fgetc(file);
      }
    }

    if (c != ' ' && c != '\n' && c != '\r' && c != EOF)
    {
      warnx("Warning: Too much lines in your file %s.\n", file_name);
      goto error_file;
    }
  }
  fclose(file);
  return grid;

  /* ====================== Where goto:error_file leads ===================== */
error_file:
  grid_free(grid);
  fclose(file);
  return NULL;
}

int main(int argc, char *argv[])
{
  const struct option long_opts[] = {{"all", no_argument, NULL, 'a'},
                                     {"generate", optional_argument, NULL, 'g'},
                                     {"output", required_argument, NULL, 'o'},
                                     {"unique", no_argument, NULL, 'u'},
                                     {"verbose", no_argument, NULL, 'v'},
                                     {"version", no_argument, NULL, 'V'},
                                     {"help", no_argument, NULL, 'h'},
                                     {NULL, 0, NULL, 0}};

  int optc;
  bool all = false;
  bool unique = false;
  bool generator = false;
  int size = DEFAULT_SIZE;
  output = stdout;
  char *output_name = NULL;

  while ((optc = getopt_long(argc, argv, "ag::o:uvVh", long_opts, NULL)) != -1)
    switch (optc)
    {
      case 'a':
        all = true;
        break;

      case 'h':
        printf(
            "Usage: sudoku [-a|-o FILE|-v|-V|-h] FILE...\n"
            "       sudoku -g[SIZE] [-u|-o FILE|-v|-V|-h]\n"
            "Solve or generate Sudoku grids of size: "
            "1, 4, 9, 16, 25, 36, 49, 64\n"
            "\n"
            " -a,--all               search for all possible solutions\n"
            " -g[N],--generate[N]    generate a grid of size NxN (default:9)\n"
            " -o FILE,--output FILE  write output to FILE "
            "(appends the end of FILE if it already exists)\n"
            " -u,--unique            generate a grid with unique solution\n"
            " -v,--verbose           verbose output\n"
            " -V,--version           display version and exit\n"
            " -h,--help              display this help and exit\n");
        exit(EXIT_SUCCESS);

      case 'g':
        generator = true;
        if (optarg)
        {
          size = atoi(optarg);
          if (size != 1 && size != 4 && size != 9 && size != 16 && size != 25 &&
              size != 36 && size != 49 && size != 64)
          {
            errx(EXIT_FAILURE, "Error: Please choose one of "
                               "these sizes: 1, 4, 9, 16, 25, 36,"
                               " 49, 64");
          }
        }
        break;

      case 'V':
        printf("sudoku %d.%d.%d\nSolve/generate sudoku grids of size: 1,"
               " 4, 9, 16, 25, 36, 49, 64\n",
               VERSION, SUBVERSION, REVISION);
        exit(EXIT_SUCCESS);

      case 'o':
        output_name = optarg; /* In case of multiple uses of '-o' */
        break;

      case 'v':
        verbose = true;
        break;

      case 'u':
        unique = true;
        break;

      default:
        errx(EXIT_FAILURE,
             "Error: You chose an unrecognized option: "
             "'%s'.\n"
             "Software is now closing.\n",
             argv[optind - 1]);
    }

  if (output_name != NULL) /* Means '-o' has been used. */
  {
    output = fopen(output_name, "a");
    if (output == NULL)
    {
      error = true;
      warn("Error on output file %s", output_name);
      output = stdout;
    }
    else
    {
      fprintf(output, "# Here is your software output:\n\n");
    }
  }

  if (generator && all)
  {
    warnx("Warning: You are in GENERATOR mode and therefore, you can't"
          " search for solutions. Option '-a/--all' has been disabled.\n");
    all = false;
  }

  if (unique && !generator)
  {
    warnx("Warning: You are in SOLVER mode and therefore, you can't  "
          "generate a grid. Option '-u/--unique' has been disabled.\n");
    unique = false;
  }

  if (!generator) /* User mode */
  {
    if (argc == optind)
    {
      errx(EXIT_FAILURE, "Error: In solver mode, please give an existing and"
                         " readable file as last argument.");
    }

    for (int i = optind; i < argc; i++)
    {
      grid_t *grid = file_parser(argv[i]);

      if (grid != NULL)
      {
        fprintf(output, "\nHere is the grid of file %s:\n\n", argv[i]);
        grid_print(grid, output);

        if (!all)
        {
          grid = grid_solver(grid, mode_first, NULL, NULL, false);

          if (grid == NULL)
          {
            fprintf(output, "Grid is not consistent.\n");
            error = true;
          }

          else
          {
            fprintf(output, "Grid has been solved, here is the solution:\n");
            grid_print(grid, output);
            grid_free(grid);
          }
        }

        else /* --all */
        {
          grid_solver(grid, mode_all, &error, output, false);
          grid_free(grid);
        }
      }

      else /* file_parser returned NULL */
      {
        error = true;
      }
    }

    if (error)
    {
      if (output != stdout)
      {
        fclose(output);
      }
      errx(EXIT_FAILURE, "Error on file(s)");
    }
  }

  else /* Generator mode */
  {
    fprintf(output, "# Here is your generated grid:\n\n");
    grid_t *gen_grid = grid_generation(size, unique);
    grid_print(gen_grid, output);
    grid_free(gen_grid);
  }

  if (output != stdout)
  {
    fclose(output);
  }

  return EXIT_SUCCESS;
}
#define MAX_COLORS 64

#include <stdbool.h>
#include <stdio.h>

#include <inttypes.h>

typedef uint64_t colors_t;

/* Returns a full color set, containing all the colors of a grid size */
colors_t colors_full(const size_t size);

/* Returns an empty color set */
colors_t colors_empty(void);

/* Returns a choosen singleton color set */
colors_t colors_set(const size_t color_id);

/* Adds a color to a color set */
colors_t colors_add(const colors_t colors, const size_t color_id);

/* Removes a color from a color set */
colors_t colors_discard(const colors_t colors, const size_t color_id);

/* Returns a boolean telling if a color is in a color set */
bool colors_is_in(const colors_t colors, const size_t color_id);

/* Returns the bitwise negation of a color set */
colors_t colors_negate(const colors_t colors);

/* Returns a color set with all common colors two sets have */
colors_t colors_and(const colors_t colors1, const colors_t colors2);

/* Returns a color set sharing colors two sets have */
colors_t colors_or(const colors_t colors1, const colors_t colors2);

/* Returns a color set, which colors are those which only one set owns,
   regarding two given sets */
colors_t colors_xor(const colors_t colors1, const colors_t colors2);

/* Returns a color set which colors are in a given set, but not in another */
colors_t colors_subtract(const colors_t colors1, const colors_t colors2);

/* Returns a boolean telling if two color sets are the same */
bool colors_is_equal(const colors_t colors1, const colors_t colors2);

/* Returns a boolean telling if a color set is include in another */
bool colors_is_subset(const colors_t colors1, const colors_t colors2);

/* Returns a boolean telling if a color set is a singleton */
bool colors_is_singleton(const colors_t colors);

/* Returns the number of colors in a colors set */
size_t colors_count(const colors_t colors);

/* Returns a singleton containing the bitwise rightmost color of a color set */
colors_t colors_rightmost(const colors_t colors);

/* Returns a singleton containing the bitwise leftmost color of a color set */
colors_t colors_leftmost(const colors_t colors);

/* Returns a singleton with a random color choosen from colors */
colors_t colors_random(const colors_t colors);

/* Returns a boolean telling if a subgrid follows some consistency rules,
   regarding some sudoku rules */
bool subgrid_consistency(colors_t *subgrid[], const size_t size);

/* Applies several heuristics on a subgrid */
bool subgrid_heuristics(colors_t *subgrid[], size_t size);
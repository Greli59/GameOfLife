#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define HEIGHT 10
#define WIDTH 10

// Load seeds for the Game of Life

/*
  Load a seed 
 * - Parameters : 
 *		file_name: Name of the input file
 *		image: array where to store the pixel intensities
 * - Return value : 1 if everything went well, 0 otherwise
 */
int load_seed(const char *file_name, bool universe[HEIGHT][WIDTH]);

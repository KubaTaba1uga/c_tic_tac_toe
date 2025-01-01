#include <stddef.h>
#include <stdio.h>

/* #define RESET_COLOR "\033[0m" */
/* #define COLOR_RED "\033[31m" */
/* #define COLOR_GREEN "\033[32m" */
/* #define COLOR_BLUE "\033[34m" */
/* #define COLOR_YELLOW "\033[33m" */
/* #define COLOR_CYAN "\033[36m" */
/* #define COLOR_MAGENTA "\033[35m" */

// Array of color strings
/* const int max_x = 3; */
/* const int max_y = 3; */

/* const char *colors[max_y][max_x] = {{COLOR_RED, COLOR_GREEN, COLOR_BLUE}, */
/*                                     {COLOR_YELLOW, COLOR_CYAN,
 * COLOR_MAGENTA}}; */

int main(void) {
  const int max_x = 3;
  const int max_y = 3;

  for (size_t index_y = 0; index_y < max_y; index_y++) {
    for (size_t index_x = 0; index_x < max_x; index_x++) {
      char *str_to_print;

      if (index_y != max_y - 1) {
        str_to_print = "_|";

        if (index_x == max_x - 1) {
          str_to_print = "_\n";
        }
      } else {
        str_to_print = " |";

        if (index_x == max_x - 1) {
          str_to_print = " \n";
        }
      }

      printf("%s", str_to_print);
    }
  }

  return 0;
}

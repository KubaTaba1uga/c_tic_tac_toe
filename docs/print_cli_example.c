#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Define ANSI escape codes for background colors
#define RESET_COLOR "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_CYAN "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_BLUE "\033[44m"
#define BG_YELLOW "\033[43m"
#define BG_CYAN "\033[46m"
#define BG_MAGENTA "\033[45m"

#define MAX_X 3
#define MAX_Y 3

const char *colors[MAX_Y][MAX_X] = {
    {BG_RED, "", ""}, {"", "", ""}, {"", "", ""}};

int main(void) {

  for (size_t index_y = 0; index_y < MAX_Y; index_y++) {
    for (size_t index_x = 0; index_x < MAX_X; index_x++) {
      char *delimeter_to_print;

      delimeter_to_print = "|";

      if (index_x == MAX_X - 1) {
        delimeter_to_print = "\n";
      }

      printf("%s %s%s", colors[index_y][index_x], RESET_COLOR,
             delimeter_to_print);
    }
  }

  return 0;
}

// C standard library
#include <stddef.h>

// App's internal libs
#include "display/display.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"

struct CliDisplay {
  size_t width;
  size_t height;
};

struct CliDisplayPrivateOps {
  display_display_func_t display;
  int (*init_display)(struct CliDisplay *data);
  int (*get_terminal_size)(struct CliDisplay *data);
  void (*sort_user_moves)(int n, struct UserMove user_moves[n]);
  int (*format_game_row)(struct CliDisplay *display, struct DataToDisplay *data,
                         size_t row_no, size_t buffer_size,
                         char buffer[buffer_size]);
};

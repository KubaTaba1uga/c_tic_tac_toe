#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "game_user.h"

static int init_game_user(struct GameUser *user, const char *display_name,
                          input_device_id_t device_id) {
  if (!user || !display_name || device_id < 0) {
    return EINVAL;
  }

  user->device_id = device_id;
  user->display_name = display_name;

  return 0;
}

static struct GameUserOps game_user_ops = {
    .init_user = init_game_user,
};

struct GameUserOps *get_game_user_ops(void) { return &game_user_ops; }

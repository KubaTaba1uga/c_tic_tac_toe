#include <stdlib.h>

struct Keyboard1PrivateOps {
  int (*start)(void);
  int (*stop)(void);
  int (*wait)(void);
  int (*callback)(size_t n, char buffer[n]);
};

struct Keyboard1PrivateOps *get_keyboard1_priv_ops(void);

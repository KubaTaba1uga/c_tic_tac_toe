#include <stdlib.h>

struct Keyboard1PrivateOps {
  int (*init)(void);
  void (*destroy)(void);
  int (*start)(void);
  void (*wait)(void);
  int (*callback)(size_t n, char buffer[n]);
};

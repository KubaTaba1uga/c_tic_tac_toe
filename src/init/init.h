#ifndef INIT_H
#define INIT_H

#include <stddef.h>

/* Operations for initialization */
struct InitOps {
  int (*initialize)(void);
  void (*destroy)(void);
};

/* Function to retrieve InitOps */
struct InitOps *get_init_ops(void);

#endif // INIT_H

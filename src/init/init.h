#ifndef INIT_H
#define INIT_H

#include <stddef.h>

/* Data structure for initialization registration */
struct InitRegistration {
  int (*init)(void);
  void (*destroy)(void);
  const char *display_name;
};

/* Operations for initialization */
struct InitOps {
  int (*initialize)(void);
  void (*destroy)(void);
};

/* Function to retrieve InitOps */
struct InitOps *get_init_ops(void);

#endif // INIT_H

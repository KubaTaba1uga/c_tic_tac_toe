#ifndef INIT_H
#define INIT_H
/*******************************************************************************
 * @file config.h
 * @brief Header file for the configuration subsystem.
 *
 * This file contains the definitions and declarations for the configuration
 * subsystem, including data structures and function prototypes for registering
 * and retrieving configuration variables.
 *
 ******************************************************************************/

#include <stddef.h>

/* Data structure for initialization registration */
struct InitRegistrationData {
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

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

// Logging library
#include <stdlib.h>
#include <stumpless.h>

// App's data
#include "../config.h"

/*******************************************************************************
 *    MACROS
 ******************************************************************************/
#define GET_VA_CHAR_ARGS(buffer, buffer_size)                                  \
  va_list vl;                                                                  \
  va_start(vl, fmt);                                                           \
  vsnprintf(buffer, buffer_size, fmt, vl);                                     \
  va_end(vl);

/*******************************************************************************
 *    PRIVATE DECLARATIONS
 ******************************************************************************/
struct stumpless_target *loggers[] = {NULL};
size_t loggers_no = sizeof(loggers) / sizeof(struct stumpless_target *);

static void log_msg(char *msg, char *msg_id, enum stumpless_severity severity);
static int create_log_entry(char *msg, char *msg_id,
                            // Stumpless data
                            struct stumpless_entry **entry,
                            enum stumpless_severity severity);
static int emmit_log_entry(struct stumpless_entry *entry);
static void print_error(int errnum);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int init_loggers(void) {
  errno = 0;

  loggers[0] = stumpless_open_stdout_target("console logger");

  if (loggers[0] == NULL) {
    print_error(errno);
    return errno;
  }

  return 0;
}

void destroy_loggers(void) {
  size_t i;

  for (i = 0; i < loggers_no; i++) {
    stumpless_close_target(loggers[i]);
  }

  stumpless_free_all();
}

void log_info(char *msg_id, char *fmt, ...) {
  char local_log_entry[255];

  GET_VA_CHAR_ARGS(local_log_entry, sizeof(local_log_entry));

  log_msg(local_log_entry, msg_id, STUMPLESS_SEVERITY_INFO);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
void log_msg(char *msg, char *msg_id, enum stumpless_severity severity) {
  struct stumpless_entry *entry = NULL;
  int err;

  err = create_log_entry(msg, msg_id, &entry, severity);

  if (err) {
    print_error(err);
    goto OUT;
  }

  err = emmit_log_entry(entry);

  if (err) {
    print_error(err);
    goto FREE;
  }

FREE:
  stumpless_destroy_entry_and_contents(entry);

OUT:
  return;
}

int create_log_entry(char *msg, char *msg_id,
                     // Stumpless data
                     struct stumpless_entry **entry,
                     enum stumpless_severity severity) {
  errno = 0;

  *entry = stumpless_new_entry(STUMPLESS_FACILITY_USER, severity, PROJECT_NAME,
                               msg_id, msg);
  if (!*entry) {
    return errno;
  }

  return 0;
}

int emmit_log_entry(struct stumpless_entry *entry) {
  int err;
  size_t i;

  errno = 0;

  for (i = 0; i < loggers_no; i++) {

    err = stumpless_add_entry(loggers[i], entry);

    if (err < 0) {
      return errno;
    }
  }

  return 0;
}

void print_error(int errnum) {
  fprintf(stderr, "!!! Logging Error: %s !!!\n", strerror(errnum));
}

/*******************************************************************************
 * @file logging_utils.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <string.h>

// Logging library
#include <stdlib.h>
#include <stumpless.h>

// App's internal libs
#include "config/config.h"
#include "init/init.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define GET_VA_CHAR_ARGS(buffer, buffer_size, fmt)                             \
  va_list vl;                                                                  \
  va_start(vl, fmt);                                                           \
  vsnprintf(buffer, buffer_size, fmt, vl);                                     \
  va_end(vl);

#define LOGGING_MODULE_ID "logging_subsystem"

static struct stumpless_target *loggers[] = {NULL};

static size_t loggers_no = sizeof(loggers) / sizeof(struct stumpless_target *);

static int init_loggers(void);
static void destroy_loggers(void);
static void log_info(const char *msg_id, char *fmt, ...);
static void log_err(const char *msg_id, char *fmt, ...);
static void log_msg(char *msg, const char *msg_id,
                    enum stumpless_severity severity);
static int create_log_entry(char *msg, const char *msg_id,
                            struct stumpless_entry **entry,
                            enum stumpless_severity severity);
static int emmit_log_entry(struct stumpless_entry *entry);
static void print_errno(void);
static void print_error(char *error);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct LoggingUtilsPrivateOps {
  void (*log_msg)(char *msg, const char *msg_id,
                  enum stumpless_severity severity);
  int (*create_log_entry)(char *msg, const char *msg_id,
                          struct stumpless_entry **entry,
                          enum stumpless_severity severity);
  int (*emmit_log_entry)(struct stumpless_entry *entry);
  void (*print_errno)(void);
  void (*print_error)(char *error);
};

static struct LoggingUtilsPrivateOps logging_utils_priv_ops = {
    .log_msg = log_msg,
    .create_log_entry = create_log_entry,
    .emmit_log_entry = emmit_log_entry,
    .print_errno = print_errno,
    .print_error = print_error,
};

static struct LoggingUtilsOps logging_utils_ops = {
    .init_loggers = init_loggers,
    .destroy_loggers = destroy_loggers,
    .log_info = log_info,
    .log_err = log_err,
    .private = &logging_utils_priv_ops};

struct LoggingUtilsOps *get_logging_utils_ops(void) {
  return &logging_utils_ops;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_logging_reg = {
    .id = LOGGING_MODULE_ID,
    .init = init_loggers,
    .destroy = destroy_loggers,
};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int init_loggers(void) {
  errno = 0;

  if (loggers[0] == NULL)
    loggers[0] = stumpless_open_stdout_target("console logger");

  if (loggers[0] == NULL)
    logging_utils_priv_ops.print_error("Unable to open console logger");

  return errno;
}

void destroy_loggers(void) {
  size_t i;

  for (i = 0; i < loggers_no; i++) {
    if (loggers[i] == NULL)
      continue;

    stumpless_close_target(loggers[i]);

    loggers[i] = NULL;
  }

  stumpless_free_all();
}

void log_info(const char *msg_id, char *fmt, ...) {
  char local_log_entry[255];

  GET_VA_CHAR_ARGS(local_log_entry, 254, fmt);

  logging_utils_priv_ops.log_msg(local_log_entry, msg_id,
                                 STUMPLESS_SEVERITY_INFO);
}

void log_err(const char *msg_id, char *fmt, ...) {
  char local_log_entry[255];

  GET_VA_CHAR_ARGS(local_log_entry, sizeof(local_log_entry) / sizeof(char),
                   fmt);

  logging_utils_priv_ops.log_msg(local_log_entry, msg_id,
                                 STUMPLESS_SEVERITY_ERR);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
void log_msg(char *msg, const char *msg_id, enum stumpless_severity severity) {
  struct stumpless_entry *entry = NULL;
  int err;

  err = logging_utils_priv_ops.create_log_entry(msg, msg_id, &entry, severity);

  if (err) {
    logging_utils_priv_ops.print_error("Unable to create log entry");
    goto OUT;
  }

  err = logging_utils_priv_ops.emmit_log_entry(entry);

  if (err) {
    logging_utils_priv_ops.print_error("Unable to emmit log entry");
    goto FREE;
  }

FREE:
  stumpless_destroy_entry_and_contents(entry);

OUT:

  return;
}

int create_log_entry(char *msg, const char *msg_id,
                     struct stumpless_entry **entry,
                     enum stumpless_severity severity) {
  *entry = stumpless_new_entry_str(STUMPLESS_FACILITY_USER, severity,
                                   PROJECT_NAME, msg_id, msg);
  if (*entry == NULL) {
    return EINVAL;
  }

  return 0;
}

int emmit_log_entry(struct stumpless_entry *entry) {
  int err;
  size_t i;

  for (i = 0; i < loggers_no; i++) {
    if (loggers[i] == NULL)
      continue;

    err = stumpless_add_entry(loggers[i], entry);

    if (err < 0) {
      return EINVAL;
    }
  }

  return 0;
}

void print_error(char *msg) {
  fprintf(stderr, "!!! Logging Error: %s !!!\n", msg);
  logging_utils_priv_ops.print_errno();
}

void print_errno(void) { stumpless_perror("logging"); }

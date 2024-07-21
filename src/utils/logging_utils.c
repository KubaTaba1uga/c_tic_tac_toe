/*******************************************************************************
 * @file logging_utils.c
 * @brief This file contains the implementation of a logging utility subsystem
 *        for managing log entries, logging initialization, and destruction.
 *
 * The logging subsystem utilizes the Stumpless library for logging to various
 * targets and provides functions to log informational and error messages with
 * different severity levels. It also manages the creation and emission of log
 * entries.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
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
 *    MACROS
 ******************************************************************************/
#define GET_VA_CHAR_ARGS(buffer, buffer_size, fmt)                             \
  va_list vl;                                                                  \
  va_start(vl, fmt);                                                           \
  vsnprintf(buffer, buffer_size, fmt, vl);                                     \
  va_end(vl);

/*******************************************************************************
 *    PRIVATE DECLARATIONS
 ******************************************************************************/
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

static struct init_registration_data init_logging_reg = {
    .id = LOGGING_MODULE_ID,
    .init_func = init_loggers,
    .destroy_func = destroy_loggers,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct logging_utils_private_ops {
  void (*log_msg)(char *msg, const char *msg_id,
                  enum stumpless_severity severity);
  int (*create_log_entry)(char *msg, const char *msg_id,
                          struct stumpless_entry **entry,
                          enum stumpless_severity severity);
  int (*emmit_log_entry)(struct stumpless_entry *entry);
  void (*print_errno)(void);
  void (*print_error)(char *error);
};

static struct logging_utils_private_ops logging_utils_priv_ops = {
    .log_msg = log_msg,
    .create_log_entry = create_log_entry,
    .emmit_log_entry = emmit_log_entry,
    .print_errno = print_errno,
    .print_error = print_error,
};

struct logging_utils_ops logging_utils_ops = {
    .init_loggers = init_loggers,
    .destroy_loggers = destroy_loggers,
    .log_info = log_info,
    .log_err = log_err,
    .private = &logging_utils_priv_ops};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int init_loggers(void) {
  errno = 0;

  loggers[0] = stumpless_open_stdout_target("console logger");

  if (loggers[0] == NULL) {
    logging_utils_priv_ops.print_error("Unable to open console logger");

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
    err = stumpless_add_entry(loggers[i], entry);

    if (err < 0) {
      return EINVAL;
    }
  }

  return 0;
}

void print_error(char *msg) {
  logging_utils_priv_ops.print_errno();
  fprintf(stderr, "!!! Logging Error: %s !!!\n", msg);
}

void print_errno(void) { stumpless_perror("logging"); }

INIT_REGISTER_SUBSYSTEM(init_logging_reg, INIT_MODULE_ORDER_LOGGING);

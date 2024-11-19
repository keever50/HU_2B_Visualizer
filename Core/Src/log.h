/*
 * @file log.h
 *
 *  Created on: Nov 14, 2024
 *      Author: kevin
 */

#ifndef SRC_LOG_H_
#define SRC_LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define LOG_MAX_MSG_SIZE  128
#define _LOG_LOCATION __FILE__, __func__, __LINE__
/**
 * Logs to the board logging output
 * @param lvl which can be LOG_xxx found in enum log_level. Example: LOG_DEBUG.
 * @param msg message to log in printf formatting. const char*
 * @param ... The variables added to the string formatter.
 */
#define LOGF(lvl, msg, ...) _log(lvl, msg, _LOG_LOCATION, __VA_ARGS__)

enum log_level
{
  LOG_DEBUG,
  LOG_INFO,
  LOG_NOTICE,
  LOG_WARNING,
  LOG_ERROR,
  LOG_CRITICAL
};


/**
 * User prototypes
 */

void log_out(const char *msg);

/**
 * Public functions
 */

void log_setlevel(enum log_level lvl);

/**
 * Private functions
 */

void _log(enum log_level lvl, const char *msg, const char *file, const char *func, int line, ...);


#endif /* SRC_LOG_H_ */

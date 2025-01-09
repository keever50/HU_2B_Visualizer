/**
 * @file log.h
 * @author Kevin Witteveen (kevinwit1999@gmail.com)
 * @brief A super simple logging output
 * @version 1
 * @date 2024-12-04
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */


#ifdef __cplusplus
    extern "C" {
#endif

#ifndef SRC_LOG_H_
#define SRC_LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define LOG_MAX_MSG_SIZE  128
#define _LOG_LOCATION __FILE__, __func__, __LINE__
/**
 * @brief Logs your message in printf style. Example usage: LOGF(LOG_DEBUG, "HELLO!", 0); (a placeholder 0 has to be placed when no variables are used.)
 * @param lvl which can be LOG_xxx found in enum log_level. Example: LOG_DEBUG.
 * @param msg message to log in printf formatting style. It MUST be null terminated.
 * @param ... The variables added to the string formatter. When no variables are used, use a 0 for placeholder.
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

/**
 * @brief User defined logging output. For example put your printf, Serial.print or UART_Puts here.
 * 
 * @param msg The message. It is already pre formatted for you. It is null terminated. It also contains escape codes for colors.
 */
void log_out(const char *msg);

/**
 * Public functions
 */

/**
 * @brief Sets the logging level. This filters out unwanted logging levels, such as debugging messages in production.
 * 
 * @param lvl The minimum level to allow through.
 */
void log_setlevel(enum log_level lvl);

/**
 * Private functions
 */

void _log(enum log_level lvl, const char *msg, const char *file, const char *func, int line, ...);


#endif /* SRC_LOG_H_ */


#ifdef __cplusplus
    }
#endif
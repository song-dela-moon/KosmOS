/**
 * @file logger.hpp
 *
 * Implementation of the kernel logger.
 */

#pragma once

enum LogLevel {
  kError = 3,
  kWarn  = 4,
  kInfo  = 6,
  kDebug = 7,
};

/** @brief Changes the global threshold for log priority.
 *
 * Sets the global log threshold to level.
 * Subsequent calls to Log will record only messages at or above this priority.
 */
void SetLogLevel(LogLevel level);

/** @brief Records a log message with the specified priority.
 *
 * If the specified priority is at or above the threshold, the log is recorded.
 * If it is below the threshold, the message is discarded.
 *
 * @param level  Log priority. Only messages at or above this level are recorded.
 * @param format  Format string. Compatible with printk.
 */
int Log(LogLevel level, const char* format, ...);

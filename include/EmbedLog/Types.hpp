/**
 * @file EmbedLogError.hpp
 * @brief Defines the logging levels, timestamp, token types, and function types for the EmbedLog library.
 *
 * Copyright (c) 2025, Joe Inman
 *
 * Licensed under the MIT License.
 * You may obtain a copy of the License at:
 *     https://opensource.org/licenses/MIT
 *
 * This file is part of the EmbedLog Library.
 */

#pragma once

#include <stdint.h>

#include <functional>
#include <string>
#include <array>

namespace EmbedLog
{

/**
 * @enum LogLevel
 * @brief Enumerates the different logging levels.
 *
 * The LogLevel enum class defines several levels of logging to classify log messages
 * by their severity.
 */
enum class LogLevel : uint8_t
{
    Alert    = 0,
    Critical = 1,
    Error    = 2,
    Warning  = 3,
    Notice   = 4,
    Info     = 5,
    Debug    = 6,
    Trace    = 7,
    None     = 8
};

/**
 * @brief Converts a LogLevel value to its string representation.
 *
 * This function converts the provided LogLevel into a string which includes ANSI
 * color codes to visually differentiate log levels when output in terminal.
 *
 * @param level The LogLevel to be converted.
 * @return A string representing the log level, with ANSI color codes for formatting.
 */
static std::string logLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Alert:
        // Bright red
        return "\033[1;91mALERT\u001b[0m\u001b[0m";
    case LogLevel::Critical:
        // Bright magenta
        return "\033[1;95mCRITICAL\u001b[0m\u001b[0m";
    case LogLevel::Error:
        // Bright red
        return "\033[1;91mERROR\u001b[0m\u001b[0m";
    case LogLevel::Warning:
        // Bright yellow
        return "\033[1;93mWARNING\u001b[0m\u001b[0m";
    case LogLevel::Notice:
        // Bright cyan
        return "\033[1;96mNOTICE\u001b[0m\u001b[0m";
    case LogLevel::Info:
        // Bright green
        return "\033[1;92mINFO\u001b[0m\u001b[0m";
    case LogLevel::Debug:
        // Bright blue
        return "\033[1;94mDEBUG\u001b[0m\u001b[0m";
    case LogLevel::Trace:
        // Bright white
        return "\033[1;97mTRACE\u001b[0m\u001b[0m";
    case LogLevel::None:
        return "NONE";
    default:
        return "UNKNOWN";
    }
}

/**
 * @struct TimeStamp
 * @brief Represents a timestamp with detailed date and time components.
 *
 * The TimeStamp structure contains fields for microseconds, seconds, minutes, hours,
 * day, month, and year. It is used to provide precise time information for log entries.
 */
struct TimeStamp
{
    uint64_t microseconds;
    uint8_t  seconds;
    uint8_t  minutes;
    uint8_t  hours;
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
};

/**
 * @enum TokenType
 * @brief Enumerates the types of tokens used for log message formatting.
 *
 * The TokenType enum class identifies different parts of the format string,
 * distinguishing between literal text and placeholders for dynamic content.
 */
enum class TokenType
{
    Literal,
    Year,
    Month,
    Day,
    Hour,
    Minute,
    Second,
    Micro,
    Name,
    Level,
    Text
};

/**
 * @struct Token
 * @brief Represents a token extracted from a log format string.
 *
 * The Token structure encapsulates a part of the log format string. It indicates
 * whether the token is literal text or a placeholder (e.g., year, month) and stores
 * additional formatting information such as width.
 */
struct Token
{
    TokenType   type;
    int         width;
    std::string literal;
};

/**
 * @typedef PrintFunction
 * @brief A function type for printing log messages.
 *
 * This function type defines the signature for functions used to output formatted log messages.
 * It takes the final log message as a string and the associated log level.
 */
using PrintFunction = std::function<void(const std::string&, LogLevel)>;

/**
 * @typedef TimeStampFunction
 * @brief A function type for retrieving the current timestamp.
 *
 * This function type defines the signature for functions that return a TimeStamp
 * structure containing the current date and time.
 */
using TimeStampFunction = std::function<TimeStamp()>;

}  // namespace EmbedLog

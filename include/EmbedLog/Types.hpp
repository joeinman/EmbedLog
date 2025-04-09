#pragma once

#include <stdint.h>

#include <functional>
#include <string>

namespace EmbedLog
{

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

std::string logLevelToString(LogLevel level)
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
        // Bright cyan (you can adjust if preferred)
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

struct TimeStamp
{
    uint64_t microseconds;
    uint8_t  seconds;
    uint8_t  minutes;
    uint8_t  hours;
    uint8_t  day;
    uint8_t  month;
    uint8_t  year;
};

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

struct Token
{
    TokenType   type;
    int         width;
    std::string literal;
};

using PrintFunction     = std::function<void(const std::string&)>;
using TimeStampFunction = std::function<TimeStamp()>;

}  // namespace EmbedLog

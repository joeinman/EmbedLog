/**
 * @file EmbedLogError.hpp
 * @brief Defines the EmbedLog class for logging operations.
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
#include <vector>

#include "Error.hpp"
#include "Types.hpp"

namespace EmbedLog
{

/**
 * @brief Default format string for log messages.
 *
 * This format uses custom tokens (prefixed with '%') to insert various
 * information such as date, time, log level, logger name, and the message text.
 */
constexpr const char* defaultFormat = "[%YYYY:%MM:%DD:%hh:%mm:%ss.%uuuuuu] [%N] [%L] - %T";

/**
 * @class EmbedLog
 * @brief Handles log formatting and printing using a custom format.
 *
 * The EmbedLog class uses a print function to output log messages and a
 * timestamp function to generate date/time stamps. It tokenizes a given
 * format string to dynamically create the final log string.
 */
class EmbedLog
{
public:
    /**
     * @brief Constructs an EmbedLog instance.
     *
     * @param print_function A function used to print the formatted log message.
     * @param timestamp_function A function that returns the current timestamp.
     * @param name The identifier name for the logger.
     * @param format The format string for the log output. Defaults to defaultFormat.
     *
     * The constructor tokenizes the provided format string for later use in formatting.
     */
    EmbedLog(const PrintFunction&     print_function,
             const TimeStampFunction& timestamp_function,
             const std::string&       name,
             const std::string&       format = defaultFormat) :
        print_function_(print_function), timestamp_function_(timestamp_function), name_(name), format_(format)
    {
        tokens_ = tokenizeFormat(format_);
    }

    /**
     * @brief Logs a formatted message.
     *
     * This function formats a log message with the provided parameters and outputs it
     * using the print function. It checks that the log level is sufficient and ensures the
     * final output string does not exceed a preset length.
     *
     * @tparam Args Variadic types for formatting arguments.
     * @param level The log level of the message.
     * @param fmt The format string for the log message.
     * @param args Arguments to be formatted into the log message.
     * @return An EmbedLogError indicating the success or type of error encountered.
     *
     * @note If the provided log level is below the set log level threshold, or if the
     *        resulting string is too long, an appropriate error is returned.
     */
    template <typename... Args>
    EmbedLogError log(LogLevel level, const std::string& fmt, Args&&... args) const noexcept
    {
        if (level > log_level_)
            return EmbedLogError{EmbedLogErrorType::LogLevelError, "Log level is too low."};

        char buffer[256];  // NOSONAR
        snprintf(buffer, sizeof(buffer), fmt.c_str(), std::forward<Args>(args)...);
        std::string message(buffer);
        if (message.size() > 255)
        {
            return EmbedLogError{EmbedLogErrorType::OutputLengthError, "Output string is too long."};
        }

        std::string levelStr = logLevelToString(level);
        TimeStamp   ts       = timestamp_function_();

        std::string output = formatOutput(message, ts, levelStr);
        if (output.size() > 255)
        {
            return EmbedLogError{EmbedLogErrorType::OutputLengthError, "Output string is too long."};
        }

        print_function_(output, level);
        return EmbedLogError{EmbedLogErrorType::Success, "Log message printed successfully."};
    }

    /**
     * @brief Sets the current log level.
     *
     * Only messages with a level equal to or higher than this will be logged.
     *
     * @param level The minimum log level required for messages to be printed.
     */
    void setLogLevel(const LogLevel& level) noexcept { log_level_ = level; }

private:
    PrintFunction      print_function_;
    TimeStampFunction  timestamp_function_;
    std::string        name_;
    std::string        format_;
    LogLevel           log_level_ = LogLevel::None;
    std::vector<Token> tokens_;

    /**
     * @brief Tokenizes the log format string.
     *
     * Parses the format string to extract tokens that represent literal text,
     * date/time components, logger name, log level, and the actual log text.
     *
     * @param format The format string to tokenize.
     * @return A vector of Token objects representing the parsed components.
     */
    static std::vector<Token> tokenizeFormat(const std::string& format)
    {
        std::vector<Token> tokens;
        size_t             i = 0;
        while (i < format.size())
        {
            if (format[i] == '%')
            {
                if (i + 1 < format.size() && format[i + 1] == '%')
                {
                    tokens.push_back(Token{TokenType::Literal, 0, "%"});
                    i += 2;
                    continue;
                }

                size_t j = i + 1;
                if (j < format.size())
                {
                    char   tokenChar = format[j];
                    size_t k         = j;
                    while (k < format.size() && format[k] == tokenChar)
                    {
                        k++;
                    }
                    int count = static_cast<int>(k - j);

                    Token token;
                    token.width   = count;
                    token.literal = "";
                    switch (tokenChar)
                    {
                    case 'Y':
                        token.type = TokenType::Year;
                        break;
                    case 'M':
                        token.type = TokenType::Month;
                        break;
                    case 'D':
                        token.type = TokenType::Day;
                        break;
                    case 'h':
                        token.type = TokenType::Hour;
                        break;
                    case 'm':
                        token.type = TokenType::Minute;
                        break;
                    case 's':
                        token.type = TokenType::Second;
                        break;
                    case 'u':
                        token.type = TokenType::Micro;
                        break;
                    case 'N':
                        token.type = TokenType::Name;
                        break;
                    case 'L':
                        token.type = TokenType::Level;
                        break;
                    case 'T':
                        token.type = TokenType::Text;
                        break;
                    default:
                        token.type    = TokenType::Literal;
                        token.literal = format.substr(i, k - i);
                        break;
                    }
                    tokens.push_back(token);
                    i = k;
                    continue;
                }
            }

            size_t start = i;
            while (i < format.size() && format[i] != '%')
            {
                i++;
            }
            tokens.push_back(Token{TokenType::Literal, 0, format.substr(start, i - start)});
        }
        return tokens;
    }

    /**
     * @brief Generates the final formatted output string.
     *
     * Uses the tokenized format to generate a complete log string by replacing tokens
     * with their corresponding runtime values. These values include parts of the timestamp,
     * the logger name, the log level, and the actual message text.
     *
     * @param message The log message text.
     * @param ts The current timestamp containing detailed date and time information.
     * @param levelStr The string representation of the current log level.
     * @return A formatted string ready to be printed.
     */
    std::string formatOutput(const std::string& message, const TimeStamp& ts, const std::string_view& levelStr) const
    {
        auto formatNumber = [](int number, int width) {
            std::string result = std::to_string(number);
            if (result.size() < static_cast<size_t>(width))
            {
                result.insert(0, width - result.size(), '0');
            }
            return result;
        };

        std::string output;
        for (const auto& token : tokens_)
        {
            switch (token.type)
            {
            case TokenType::Literal:
                output += token.literal;
                break;
            case TokenType::Year:
                if (token.width == 2)
                {
                    output += "\033[1;97m" + formatNumber(ts.year % 100, token.width) + "\033[0m";
                }
                else
                {
                    output += "\033[1;97m" + formatNumber(ts.year, token.width) + "\033[0m";
                }
                break;
            case TokenType::Month:
                output += "\033[1;97m" + formatNumber(ts.month, token.width) + "\033[0m";
                break;
            case TokenType::Day:
                output += "\033[1;97m" + formatNumber(ts.day, token.width) + "\033[0m";
                break;
            case TokenType::Hour:
                output += "\033[1;97m" + formatNumber(ts.hours, token.width) + "\033[0m";
                break;
            case TokenType::Minute:
                output += "\033[1;97m" + formatNumber(ts.minutes, token.width) + "\033[0m";
                break;
            case TokenType::Second:
                output += "\033[1;97m" + formatNumber(ts.seconds, token.width) + "\033[0m";
                break;
            case TokenType::Micro:
            {
                const int totalDigits    = 6;
                uint64_t  effectiveMicro = ts.microseconds;
                if (token.width < totalDigits)
                {
                    int divisor = 1;
                    for (int l = 0; l < totalDigits - token.width; l++)
                    {
                        divisor *= 10;
                    }
                    effectiveMicro /= divisor;
                }
                output += "\033[1;97m" + formatNumber(static_cast<int>(effectiveMicro), token.width) + "\033[0m";
                break;
            }
            case TokenType::Name:
                output += "\033[1;97m" + name_ + "\033[0m";
                break;
            case TokenType::Level:
                output += std::string(levelStr);
                break;
            case TokenType::Text:
                output += "\033[0m" + message;
                break;
            default:
                break;
            }
        }
        return output;
    }
};

}  // namespace EmbedLog

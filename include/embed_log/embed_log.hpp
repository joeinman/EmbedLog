/*
 * Copyright (c) 2025, Joe Inman
 *
 * Licensed under the MIT License.
 * You may obtain a copy of the License at:
 *     https://opensource.org/licenses/MIT
 *
 * This file is part of the EmbedLog Library.
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "embed_log/error.hpp"
#include "embed_log/types.hpp"

namespace EmbedLog
{

inline constexpr std::string_view default_format = "[%YYYY:%MM:%DD:%hh:%mm:%ss.%uuuuuu] [%N] [%L] - %T";

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
     * @param format The format string for the log output. Defaults to default_format.
     *
     * The constructor tokenizes the provided format string for later use in formatting.
     */
    EmbedLog(const PrintFunction&     print_function,
             const TimeStampFunction& timestamp_function,
             std::string              name,
             std::string              format = std::string{default_format}) :
        name_(std::move(name)),
        format_(std::move(format)),
        print_function_(print_function),
        timestamp_function_(timestamp_function),
        tokens_(tokenize_format(format_)),
        log_level_(LogLevel::None)
    {}

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
    [[nodiscard]] EmbedLogError log(LogLevel level, const std::string& fmt, Args&&... args) const noexcept
    {
        if (level > log_level_)
        {
            return EmbedLogError{EmbedLogErrorType::LogLevelError, "Log level is too low."};
        }
        constexpr std::size_t max_message_length = 255U;

        const std::string message = std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
        if (message.size() > max_message_length)
        {
            return EmbedLogError{EmbedLogErrorType::OutputLengthError, "Output string is too long."};
        }

        const std::string level_string = log_level_to_string(level);
        const TimeStamp   timestamp    = timestamp_function_();

        const std::string output = format_output(message, timestamp, level_string);
        if (output.size() > max_message_length)
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
    void set_log_level(const LogLevel& level) noexcept { log_level_ = level; }

private:
    PrintFunction      print_function_;
    TimeStampFunction  timestamp_function_;
    std::string        name_;
    std::string        format_;
    std::vector<Token> tokens_;
    LogLevel           log_level_ = LogLevel::None;

    /**
     * @brief Tokenizes the log format string.
     *
     * Parses the format string to extract tokens that represent literal text,
     * date/time components, logger name, log level, and the actual log text.
     *
     * @param format The format string to tokenize.
     * @return A vector of Token objects representing the parsed components.
     */
    [[nodiscard]] static std::vector<Token> tokenize_format(const std::string& format)
    {
        std::vector<Token> tokens;
        std::size_t        index = 0U;
        while (index < format.size())
        {
            if (format[index] == '%')
            {
                if ((index + 1U) < format.size() && format[index + 1U] == '%')
                {
                    tokens.push_back(Token{TokenType::Literal, 0U, "%"});
                    index += 2U;
                    continue;
                }

                const std::size_t token_start = index + 1U;
                if (token_start < format.size())
                {
                    const char  token_char = format[token_start];
                    std::size_t token_end  = token_start;
                    while (token_end < format.size() && format[token_end] == token_char)
                    {
                        ++token_end;
                    }
                    const auto width = static_cast<std::uint8_t>(token_end - token_start);

                    Token token{};
                    token.width = width;
                    switch (token_char)
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
                        token.literal = format.substr(index, token_end - index);
                        break;
                    }
                    tokens.push_back(token);
                    index = token_end;
                    continue;
                }
            }

            const std::size_t literal_start = index;
            while (index < format.size() && format[index] != '%')
            {
                ++index;
            }
            tokens.push_back(Token{TokenType::Literal, 0U, format.substr(literal_start, index - literal_start)});
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
     * @param timestamp The current timestamp containing detailed date and time information.
     * @param level_string The string representation of the current log level.
     * @return A formatted string ready to be printed.
     */
    [[nodiscard]] std::string format_output(const std::string& message,
                                            const TimeStamp&   timestamp,
                                            std::string_view   level_string) const
    {
        const auto format_number = [](int number, std::uint8_t width) -> std::string {
            std::string result = std::to_string(number);
            const auto  width_size =
                static_cast<std::size_t>(width);  // ensures consistent comparison without implicit conversions
            if (result.size() < width_size)
            {
                result.insert(0, width_size - result.size(), '0');
            }
            return result;
        };

        std::string output;
        output.reserve(message.size() + format_.size());
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
                    output += "\033[1;97m" + format_number(timestamp.year % 100, token.width) + "\033[0m";
                }
                else
                {
                    output += "\033[1;97m" + format_number(timestamp.year, token.width) + "\033[0m";
                }
                break;
            case TokenType::Month:
                output += "\033[1;97m" + format_number(timestamp.month, token.width) + "\033[0m";
                break;
            case TokenType::Day:
                output += "\033[1;97m" + format_number(timestamp.day, token.width) + "\033[0m";
                break;
            case TokenType::Hour:
                output += "\033[1;97m" + format_number(timestamp.hours, token.width) + "\033[0m";
                break;
            case TokenType::Minute:
                output += "\033[1;97m" + format_number(timestamp.minutes, token.width) + "\033[0m";
                break;
            case TokenType::Second:
                output += "\033[1;97m" + format_number(timestamp.seconds, token.width) + "\033[0m";
                break;
            case TokenType::Micro:
            {
                constexpr std::uint8_t microsecond_digits = 6U;
                std::uint64_t          effective_micro    = timestamp.microseconds;
                if (token.width < microsecond_digits)
                {
                    std::uint32_t divisor = 1U;
                    for (std::uint8_t digit = 0U; digit < (microsecond_digits - token.width); ++digit)
                    {
                        divisor *= 10U;
                    }
                    effective_micro /= divisor;
                }
                output += "\033[1;97m" + format_number(static_cast<int>(effective_micro), token.width) + "\033[0m";
                break;
            }
            case TokenType::Name:
                output += "\033[1;97m" + name_ + "\033[0m";
                break;
            case TokenType::Level:
                output += std::string(level_string);
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

#pragma once

#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

#include "Error.hpp"
#include "Types.hpp"

namespace EmbedLog
{

class EmbedLog
{
public:
    EmbedLog(const PrintFunction&     print_function,
             const TimeStampFunction& timestamp_function,
             const std::string&       name,
             const std::string&       format = "[%YYYY:%MM:%DD:%hh:%mm:%ss.%uuuuuu] [%N] [%L] - %T") :
        print_function_(print_function), timestamp_function_(timestamp_function), name_(name), format_(format)
    {
        tokens_ = tokenizeFormat(format_);
    }

    // Logging function using a printf-like interface.
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

        print_function_(output);
        return EmbedLogError{EmbedLogErrorType::Success, "Log message printed successfully."};
    }

    void setLogLevel(const LogLevel& level) noexcept { log_level_ = level; }

private:
    PrintFunction      print_function_;
    TimeStampFunction  timestamp_function_;
    std::string        name_;
    std::string        format_;
    LogLevel           log_level_ = LogLevel::None;
    std::vector<Token> tokens_;

    // Tokenises the format string into a vector of tokens.
    static std::vector<Token> tokenizeFormat(const std::string& format)
    {
        std::vector<Token> tokens;
        size_t             i = 0;
        while (i < format.size())
        {
            if (format[i] == '%')
            {
                // Handle escaped percent signs.
                if (i + 1 < format.size() && format[i + 1] == '%')
                {
                    tokens.push_back(Token{TokenType::Literal, 0, "%"});
                    i += 2;
                    continue;
                }

                // Process a token starting at i+1.
                size_t j = i + 1;
                if (j < format.size())
                {
                    char   tokenChar = format[j];
                    size_t k         = j;
                    // Count consecutive characters for the token (e.g., "YYYY" has a count of 4).
                    while (k < format.size() && format[k] == tokenChar)
                    {
                        k++;
                    }
                    int count = static_cast<int>(k - j);

                    Token token;
                    token.width   = count;
                    token.literal = "";
                    // Map the token character to our TokenType.
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
                        // For unrecognized tokens, treat as literal.
                        token.type    = TokenType::Literal;
                        token.literal = format.substr(i, k - i);
                        break;
                    }
                    tokens.push_back(token);
                    i = k;
                    continue;
                }
            }

            // Process literal segments (all characters until next '%').
            size_t start = i;
            while (i < format.size() && format[i] != '%')
            {
                i++;
            }
            tokens.push_back(Token{TokenType::Literal, 0, format.substr(start, i - start)});
        }
        return tokens;
    }

    // Uses the pre-tokenized format to build the final output string.
    std::string formatOutput(const std::string& message, const TimeStamp& ts, const std::string_view& levelStr) const
    {
        // Helper lambda to format numbers with padding.
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

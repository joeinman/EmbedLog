/*
 * EmbedLog - A Minimal Logging Library for Embedded Systems
 *
 * Copyright (C) 2023 Joe Inman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License as published by
 * the Open Source Initiative.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT>.
 *
 * Author: Joe Inman
 * Email: joe.inman8@gmail.com
 * Version: 1.0
 *
 * Description:
 * EmbedLog is designed to provide a lightweight and flexible logging system 
 * for embedded environments. It supports multiple log levels, allows 
 * user-defined output mechanisms, and includes timestamping based on 
 * microsecond-resolution functions.
 *
 */

#include "EmbedLog/EmbedLog.hpp"

#include <iomanip>

namespace EmbedLog
{
    uint64_t unique_id(std::string file, int line) {
        return std::hash<std::string>{}(file + std::to_string(line));
    }

    EmbedLog::EmbedLog(OpenFunction openFunc,
                       CloseFunction closeFunc,
                       PrintFunction printFunc,
                       MicrosecondFunction microsecondFunc,
                       std::string name,
                       std::string format)
        : openFunc(openFunc),
          closeFunc(closeFunc),
          printFunc(printFunc),
          microsecondFunc(microsecondFunc),
          name(name),
          format(format)
    {
    }

    EmbedLog::~EmbedLog()
    {
        if (isOpen)
            closeFunc();
    }

    bool EmbedLog::open()
    {
        if (!isOpen)
            isOpen = openFunc();
        return isOpen;
    }

    bool EmbedLog::close()
    {
        bool result = closeFunc();
        isOpen = !result;
        return result;
    }

    void EmbedLog::log(LogLevel level, const std::string& format, ...)
    {
        if (!isOpen)
            return;

        if (level < logLevel)
            return;

        va_list args;
        va_start(args, format);
        
        // First pass to get the required buffer size
        int size = vsnprintf(nullptr, 0, format.c_str(), args);
        va_end(args);

        if (size < 0)
            return; // Handle error in formatting
        
        // Allocate a buffer of the required size
        std::vector<char> buffer(size + 1); // +1 for the null terminator
        
        va_start(args, format);
        vsnprintf(buffer.data(), buffer.size(), format.c_str(), args);
        va_end(args);

        print(level, buffer.data());
    }

    void EmbedLog::log_throttled(size_t throttle_id, uint32_t throttle_ms, LogLevel level,  const std::string& format, ...)
    {
        if (!isOpen)
            return;

        if (level < logLevel)
            return;

        auto now = microsecondFunc();
        auto last = throttleMap[throttle_id];
        if (now - last > throttle_ms * 1000)
        {
            va_list args;
            va_start(args, format);
            
            // First pass to get the required buffer size
            int size = vsnprintf(nullptr, 0, format.c_str(), args);
            va_end(args);

            if (size < 0)
                return; // Handle error in formatting
            
            // Allocate a buffer of the required size
            std::vector<char> buffer(size + 1); // +1 for the null terminator
            
            va_start(args, format);
            vsnprintf(buffer.data(), buffer.size(), format.c_str(), args);
            va_end(args);

            print(level, buffer.data()); // Use buffer.data() for the pointer to the char array
            throttleMap[throttle_id] = now;
        }
    }

    void EmbedLog::print(LogLevel level, const std::string& message)
    {
        uint64_t microseconds = microsecondFunc();
        uint64_t totalSeconds = microseconds / 1000000;
        uint64_t remainingMicroseconds = microseconds % 1000000;

        uint64_t hours = totalSeconds / 3600;
        uint64_t minutes = (totalSeconds % 3600) / 60;
        uint64_t seconds = totalSeconds % 60;

        std::stringstream result;
        for (size_t i = 0; i < format.size(); ++i)
        {
            if (format[i] == '%')
            {
                ++i; // Skip the '%' and check the next character
                switch (format[i])
                {
                case 'N':
                    result << name; // Name
                    break;
                case 'L':
                    result << getLogLevelString(level); // Level
                    break;
                case 'T':
                    result << message; // Text
                    break;
                case 'D':
                    result << std::setfill('0') << std::setw(2) << (hours / 24); // Days
                    break;
                case 'H':
                    result << std::setfill('0') << std::setw(2) << (hours % 24); // Hours
                    break;
                case 'M':
                    result << std::setfill('0') << std::setw(2) << minutes; // Minutes
                    break;
                case 'S':
                    result << std::setfill('0') << std::setw(2) << seconds; // Seconds
                    break;
                case 'U':
                    result << std::setfill('0') << std::setw(6) << remainingMicroseconds; // Microseconds
                    break;
                default:
                    result << '%' << format[i]; // Unknown
                    break;
                }
            }
            else
            {
                result << format[i]; // Normal character
            }
        }
        result << "\n";

        printFunc(result.str());
    }

    void EmbedLog::setLogLevel(LogLevel level)
    {
        logLevel = level;
    }

    std::string EmbedLog::getLogLevelString(LogLevel level)
    {
        switch (level)
        {
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case DEBUG:
            return "DEBUG";
        case NONE:
            return "NONE";
        default:
            return "UNKNOWN";
        }
    }

} // namespace EmbedLog

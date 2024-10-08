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
    EmbedLog::EmbedLog(OpenFunction openFunc,
                       CloseFunction closeFunc,
                       PrintFunction printFunc,
                       MicrosecondFunction microsecondFunc,
                       std::string name,
                       std::string timestamp_format)
        : openFunc(openFunc),
          closeFunc(closeFunc),
          printFunc(printFunc),
          microsecondFunc(microsecondFunc),
          timestamp_format(timestamp_format)
    {
        if (!name.empty())
            this->name = " " + name + " ";
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

    void EmbedLog::print(LogLevel level, const std::string& message)
    {
        std::string logLevelString;
        switch (level)
        {
        case LogLevel::INFO:
            logLevelString = "INFO";
            break;
        case LogLevel::WARNING:
            logLevelString = "WARNING";
            break;
        case LogLevel::ERROR:
            logLevelString = "ERROR";
            break;
        case LogLevel::DEBUG:
            logLevelString = "DEBUG";
            break;
        }

        printFunc("[" + getTimestamp() + name + logLevelString + "] " + message);
    }

    void EmbedLog::setLogLevel(LogLevel level)
    {
        logLevel = level;
    }

    std::string EmbedLog::getTimestamp()
    {
        uint64_t microseconds = microsecondFunc();
        uint64_t totalSeconds = microseconds / 1000000;
        uint64_t remainingMicroseconds = microseconds % 1000000;

        uint64_t hours = totalSeconds / 3600;
        uint64_t minutes = (totalSeconds % 3600) / 60;
        uint64_t seconds = totalSeconds % 60;

        std::stringstream result;
        for (size_t i = 0; i < timestamp_format.size(); ++i)
        {
            if (timestamp_format[i] == '%')
            {
                ++i; // Skip the '%' and check the next character
                switch (timestamp_format[i])
                {
                case 'D':  // Interpret as days, which can be calculated from hours
                    result << (hours / 24);
                    break;
                case 'H':
                    result << std::setfill('0') << std::setw(2) << (hours % 24); // Hours within the day
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
                    result << '%' << timestamp_format[i]; // Unknown format specifier, output as is
                    break;
                }
            }
            else
            {
                result << timestamp_format[i]; // Copy non-format characters directly
            }
        }

        return result.str();
    }

} // namespace EmbedLog

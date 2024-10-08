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

#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <cstdint>
#include <cstdarg>

#define EMBDLID EmbedLog::unique_id(__FILE__, __LINE__)

namespace EmbedLog
{
    // Function Types for Logging
    using OpenFunction = std::function<bool()>;
    using CloseFunction = std::function<bool()>;
    using PrintFunction = std::function<void(const std::string&)>;
    using MicrosecondFunction = std::function<uint64_t()>;
    using ThrottleMap = std::unordered_map<size_t, uint64_t>;

    // Log Levels
    enum LogLevel { INFO, WARNING, ERROR, DEBUG, NONE };

    // Unique Identifier for Throttling
    uint64_t unique_id(std::string file, int line);

    /**
     * @class EmbedLog
     * @brief A minimal logging library designed for embedded systems.
     *
     * EmbedLog provides functionality to log messages at various levels of severity 
     * (INFO, WARNING, ERROR, DEBUG, NONE). It supports user-defined functions for opening 
     * and closing the log, printing messages, and fetching timestamps in microseconds.
     */
    class EmbedLog
    {
    public:
        /**
         * @brief Constructs a new EmbedLog object.
         *
         * @param openFunc Function to be called when opening the log.
         * @param closeFunc Function to be called when closing the log.
         * @param printFunc Function to print log messages.
         * @param microsecondFunc Function to retrieve the current time in microseconds.
         * @param name A name for the log.
         * @param format Optional: The desired format for the log messages. Defaults to "[%D:%H:%M:%S.%U %N %L] %T".
         *
         * @note It is important to provide valid open, close, and print functions 
         * to enable proper operation of the log system.
         */
        EmbedLog(OpenFunction openFunc, 
                 CloseFunction closeFunc, 
                 PrintFunction printFunc, 
                 MicrosecondFunction microsecondFunc, 
                 std::string name, 
                 std::string format = "[%D:%H:%M:%S.%U %N %L] %T");

        /**
         * @brief Destroys the EmbedLog object.
         *
         * Cleans up resources and ensures the log is properly closed if it was open.
         */
        ~EmbedLog();

        /**
         * @brief Opens the log by calling the user-defined open function.
         *
         * @return True if the log was successfully opened, false otherwise.
         */
        bool open();

        /**
         * @brief Closes the log by calling the user-defined close function.
         *
         * @return True if the log was successfully closed, false otherwise.
         */
        bool close();

        /**
         * @brief Sets the log level.
         *
         * @param level The desired log level (INFO, WARNING, ERROR, DEBUG).
         *
         * @note Messages below the current log level will not be printed.
         */
        void setLogLevel(LogLevel level);

        /**
         * @brief Logs a message if the specified log level is high enough.
         *
         * @param level The log level for this message.
         * @param format The format string for the message.
         * @param ... The values to log.
         */
        void log(LogLevel level, const std::string& format, ...);

        /**
         * @brief Logs a message if the specified log level is high enough.
         *
         * @param throttle_id The unique identifier for this log message.
         * @param throttle_ms The minimum time in milliseconds between messages.
         * @param level The log level for this message.
         * @param format The format string for the message.
         * @param ... The values to log.
         */
        void log_throttled(size_t throttle_id, uint32_t throttle_ms, LogLevel level,  const std::string& format, ...);

    private:
        OpenFunction openFunc;                // Function for opening the log.
        CloseFunction closeFunc;              // Function for closing the log.
        PrintFunction printFunc;              // Function for printing log messages.
        MicrosecondFunction microsecondFunc;  // Function for getting microsecond timestamps.

        ThrottleMap throttleMap;              // Map of throttle IDs to last message times.
        LogLevel logLevel = INFO;             // Current log level.
        bool isOpen = false;                  // Tracks whether the log is currently open.
        std::string format;                   // Format for the timestamp.
        std::string name;                     // Log name.

        /**
         * @brief Prints a message at a specified log level.
         *
         * @param level The log level of the message.
         * @param message The message to print.
         *
         * @note This function is called by the log function after a message 
         * has been formatted and the log level has been checked.
         */
        void print(LogLevel level, const std::string& message);

        /**
         * @brief Gets a string representation of the log level.
         *
         * @param level The log level to convert.
         * @return A string representation of the log level.
         */
        std::string getLogLevelString(LogLevel level);
    };
}

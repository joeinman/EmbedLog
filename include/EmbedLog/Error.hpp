/**
 * @file EmbedLogError.hpp
 * @brief Defines error types and error handling for the EmbedLog library.
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

#include <iostream>
#include <string>
#include <array>

/**
 * @enum EmbedLogErrorType
 * @brief Enumerates the different types of errors that can occur within EmbedLog.
 */
enum class EmbedLogErrorType
{
    Success           = 0,  ///< No error occurred.
    InputLengthError  = 1,  ///< Error due to incorrect input length.
    OutputLengthError = 2,  ///< Error due to incorrect output length.
    LogLevelError     = 3,  ///< Error due to invalid log level.
};

/**
 * @brief Array mapping EmbedLogErrorType values to their string representations.
 */
const static std::array<std::string, 4> EmbedLogErrorTypeToString = {"Success",
                                                                     "Input Length Error",
                                                                     "Output Length Error",
                                                                     "Log Level Error"};

/**
 * @struct EmbedLogError
 * @brief Represents an error encountered in the EmbedLog library.
 *
 * This structure encapsulates an error type and a corresponding descriptive message.
 */
struct EmbedLogError
{
    EmbedLogErrorType error;    ///< The type of error.
    std::string       message;  ///< A detailed error message.

    /**
     * @brief Conversion operator to std::string.
     *
     * Converts the EmbedLogError instance into a string representation that
     * combines the error type and the error message.
     *
     * @return A string formatted as "ErrorType: message".
     */
    explicit operator std::string() const
    {
        return EmbedLogErrorTypeToString[static_cast<size_t>(error)] + ": " + message;
    }

    /**
     * @brief Stream insertion operator for EmbedLogError.
     *
     * Enables outputting the error details directly to an output stream.
     *
     * @param os The output stream.
     * @param error The EmbedLogError instance to output.
     * @return The output stream with the error details appended.
     */
    friend std::ostream& operator<<(std::ostream& os, const EmbedLogError& e)
    {
        os << static_cast<std::string>(e);
        return os;
    }
};

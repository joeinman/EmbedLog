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

#include <ostream>
#include <string>
#include <cstdint>
#include <string_view>

namespace EmbedLog
{

enum class EmbedLogErrorType : std::uint8_t
{
    Success           = 0,  ///< No error occurred.
    InputLengthError  = 1,  ///< Error due to incorrect input length.
    OutputLengthError = 2,  ///< Error due to incorrect output length.
    LogLevelError     = 3,  ///< Error due to invalid log level.
};

[[nodiscard]] inline std::string_view embed_log_error_type_to_string(EmbedLogErrorType error) noexcept
{
    switch (error)
    {
    case EmbedLogErrorType::Success:
        return "Success";
    case EmbedLogErrorType::InputLengthError:
        return "Input Length Error";
    case EmbedLogErrorType::OutputLengthError:
        return "Output Length Error";
    case EmbedLogErrorType::LogLevelError:
        return "Log Level Error";
    default:
        return "Unknown Error";
    }
}

struct EmbedLogError
{
    EmbedLogErrorType error;    ///< The type of error.
    std::string       message;  ///< A detailed error message.

    [[nodiscard]] explicit operator std::string() const
    {
        return std::string{embed_log_error_type_to_string(error)} + ": " + message;
    }

    friend std::ostream& operator<<(std::ostream& os, const EmbedLogError& e)
    {
        os << static_cast<std::string>(e);
        return os;
    }
};

}  // namespace EmbedLog

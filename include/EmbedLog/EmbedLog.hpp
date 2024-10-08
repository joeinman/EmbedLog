#pragma once

#include <functional>
#include <cstdint>
#include <string>

namespace EmbedLog
{
    using OpenFunction = std::function<bool()>;
    using CloseFunction = std::function<bool()>;
    using PrintFunction = std::function<void(const std::string&)>;
    using MicrosecondFunction = std::function<uint64_t()>;

    enum LogLevel { INFO, WARNING, ERROR, DEBUG };

    class EmbedLog
    {
    public:
        EmbedLog(OpenFunction openFunc, CloseFunction closeFunc, PrintFunction printFunc, MicrosecondFunction microsecondFunc, std::string name = "", LogLevel logLevel = LogLevel::INFO);
        ~EmbedLog();

        bool open();
        bool close();

        void log(LogLevel level, const std::string& message);
        void setLogLevel(LogLevel level);

    private:
        OpenFunction openFunc;
        CloseFunction closeFunc;
        PrintFunction printFunc;
        MicrosecondFunction microsecondFunc;

        bool isOpen = false;
        LogLevel logLevel;
        std::string name = " ";

        std::string getTimestamp();
    };
}

#include "EmbedLog/EmbedLog.hpp"

namespace EmbedLog
{
    EmbedLog::EmbedLog(OpenFunction openFunc,
                       CloseFunction closeFunc,
                       PrintFunction printFunc,
                       MicrosecondFunction microsecondFunc,
                       std::string name,
                       LogLevel logLevel)
        : openFunc(openFunc),
          closeFunc(closeFunc),
          printFunc(printFunc),
          microsecondFunc(microsecondFunc),
          logLevel(logLevel)
    {
        if (!name.empty())
            this->name = " " + name + " ";
    }

    EmbedLog::~EmbedLog()
    {
        closeFunc();
    }

    bool EmbedLog::open()
    {
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
        uint64_t seconds = microseconds / 1000000;
        uint64_t minutes = seconds / 60;
        uint64_t hours = minutes / 60;

        int hour = hours % 24;
        int minute = minutes % 60;
        int second = seconds % 60;
        int microsec = microseconds % 1000000;

        // Format the timestamp as HH:MM:SS:Microseconds
        char timestamp[30];
        snprintf(timestamp, sizeof(timestamp), "%02d:%02d:%02d:%06d", hour, minute, second, microsec);
        
        return std::string(timestamp);
    }
}

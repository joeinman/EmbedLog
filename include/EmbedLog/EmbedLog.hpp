#include <functional>
#include <string>

namespace EmbedLog
{
    using OpenFunction = std::function<bool()>;
    using CloseFunction = std::function<bool()>;
    using PrintFunction = std::function<void(const std::string&)>;
    using MicrosecondFunction = std::function<long()>;

    enum LogLevel { INFO, WARNING, ERROR, DEBUG };

    class EmbedLog
    {
    public:
        EmbedLog(OpenFunction openFunc, CloseFunction closeFunc, PrintFunction printFunc, MicrosecondFunction microsecondFunc, LogLevel logLevel = LogLevel::INFO);
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
        LogLevel logLevel;
        bool isOpen = false;

        std::string getTimestamp();
    };
}

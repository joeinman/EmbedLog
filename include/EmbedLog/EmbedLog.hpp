#include <functional>
#include <string>
#include <sstream>

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

        void setLogLevel(LogLevel level);

        template <typename T, typename... Types>
        void log(LogLevel level, T var1, Types... var2)
        {
            if ((level >= logLevel) && isOpen)
            {
                std::stringstream ss;
                ss << var1;
                (ss << ... << var2);
                ss << "\n";
                print(level, ss.str());
            }
        }

    private:
        OpenFunction openFunc;
        CloseFunction closeFunc;
        PrintFunction printFunc;
        MicrosecondFunction microsecondFunc;

        bool isOpen = false;
        LogLevel logLevel;

        void print(LogLevel level, const std::string& message);
        std::string getTimestamp();
    };
}

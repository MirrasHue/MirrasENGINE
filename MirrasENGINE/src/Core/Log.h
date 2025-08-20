#pragma once

#ifndef DISTRIBUTION_BUILD
    #include <stdio.h>
    #include <format>
    #include <string_view>

    namespace mirras
    {
        enum class LogLevel
        {
            Trace,
            Info,
            Warn,
            Error
        };

        template<typename... Args>
        inline void log(LogLevel level, std::string_view fmt, Args&&... args)
        {
            auto msg = std::vformat(fmt, std::make_format_args(args...));

            switch(level)
            {
                // std::println is still experimental in GCC-15.2 (winlibs.com version)
                case LogLevel::Trace:
                    printf("[TRACE] %s\n", msg.c_str());
                    break;

                case LogLevel::Info:
                    printf("[INFO] %s\n", msg.c_str());
                    break;

                case LogLevel::Warn:
                    printf("[WARN] %s\n", msg.c_str());
                    break;

                case LogLevel::Error:
                    printf("[ERROR] %s\n", msg.c_str());
                    break;
            }
        }
    } // namespace mirras

    #define LOG_TRACE(fmt, ...) mirras::log(mirras::LogLevel::Trace, fmt, ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...)  mirras::log(mirras::LogLevel::Info,  fmt, ##__VA_ARGS__)
    #define LOG_WARN(fmt, ...)  mirras::log(mirras::LogLevel::Warn,  fmt, ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) mirras::log(mirras::LogLevel::Error, fmt, ##__VA_ARGS__)

#else

    #define LOG_TRACE(fmt, ...)
    #define LOG_INFO(fmt, ...)
    #define LOG_WARN(fmt, ...)
    #define LOG_ERROR(fmt, ...)

#endif

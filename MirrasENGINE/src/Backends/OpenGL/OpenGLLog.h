#pragma once

#ifndef DISTRIBUTION_BUILD
    #include "Core/Asserts.h"

    // Adapted from rlgl.h, instead of including the header just for the rlTraceLogLevel enum
    enum class rlLogLevel
    {
        All = 0,     // Display all logs
        Trace,       // Trace logging, intended for internal use only
        Debug,       // Debug logging, used for internal debugging, it should be disabled on release builds
        Info,        // Info logging, used for program execution info
        Warning,     // Warning logging, used on recoverable failures
        Error,       // Error logging, used on unrecoverable failures
        Fatal,       // Fatal logging, used to abort program: exit(EXIT_FAILURE)
        None         // Disable logging
    };

    namespace mirras
    {
        template<typename... Args>
        inline void logGL(int level, std::string_view fmt, Args&&... args)
        {
            auto msg = std::vformat(fmt, std::make_format_args(args...));

            // Use std::println when it gets available by default on GCC (Windows)
            switch(rlLogLevel{level})
            {
                case rlLogLevel::Trace:
                    printf("[TRACE] %s\n", msg.c_str());
                    break;

                case rlLogLevel::Info:
                    printf("[INFO] %s\n", msg.c_str());
                    break;

                case rlLogLevel::Warning:
                    printf("[WARN] %s\n", msg.c_str());
                    break;

                case rlLogLevel::Error:
                    MIRR_ASSERT(false, msg.c_str());
                    break;
            }
        }
    } // namespace mirras

    #define TRACELOG(level, fmt, ...) mirras::logGL(level, fmt, ##__VA_ARGS__)

    #ifdef DEBUG_BUILD
        #define RLGL_ENABLE_OPENGL_DEBUG_CONTEXT
        #define TRACELOGD(fmt, ...) mirras::logGL((int)rlLogLevel::Debug, fmt, ##__VA_ARGS__)
    #else
        #define TRACELOGD(fmt, ...)
    #endif

#else

    #define TRACELOG(level, fmt, ...)
    #define TRACELOGD(fmt, ...)

#endif
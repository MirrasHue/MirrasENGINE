#pragma once

#include "Core/Asserts.h"
#include "Core/BasicTypes.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fmt/printf.h>

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
    class OpenGLLog
	{
	public:
        static void init()
        {
    #ifndef DISTRIBUTION_BUILD
            openGLLogger = spdlog::stdout_color_mt("OpenGL");
            openGLLogger->set_pattern("%^[%l - %n] %v%$");
            openGLLogger->set_level(spdlog::level::trace);
    #endif
        }

        template<typename... Args>
        static void logCStyleFmt(int logLevel, const char* fmt, Args&&... args)
    #ifndef DISTRIBUTION_BUILD
        {
            auto formatted = fmt::sprintf(fmt, std::forward<Args>(args)...);

            // Only 4 log levels are used by rlgl.h at the moment
            switch(rlLogLevel{logLevel})
            {
                case rlLogLevel::Debug:
                    openGLLogger->debug(formatted);
                    break;

                case rlLogLevel::Info:
                    openGLLogger->info(formatted);
                    break;

                case rlLogLevel::Warning:
                    openGLLogger->warn(formatted);
                    break;

                case rlLogLevel::Error:
                    //openGLLogger->error(formatted);
                    MIRR_ASSERT_CORE(false, formatted);
                    break;
            }
        }

        inline static shared_ref<spdlog::logger> openGLLogger;
    #else
        { }
    #endif
	};
} // namespace mirras

#ifndef DISTRIBUTION_BUILD

    #define TraceLogFmt(level, fmt, ...) mirras::OpenGLLog::logCStyleFmt(level, fmt, ##__VA_ARGS__)
    #define TRACELOG(level, ...) TraceLogFmt(level, __VA_ARGS__)
    
    #ifdef DEBUG_BUILD
        #define RLGL_ENABLE_OPENGL_DEBUG_CONTEXT
        #define TRACELOGD(...) TraceLogFmt((int)rlLogLevel::Debug, __VA_ARGS__)
    #else
        #define TRACELOGD(...)
    #endif

#else

    #define TRACELOG(level, ...)
    #define TRACELOGD(...)

#endif
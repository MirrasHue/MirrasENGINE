#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mirras
{
    class Log
	{
	public:
        static void init()
        {
    #ifndef DISTRIBUTION_BUILD
            spdlog::set_pattern("%^[%l - %T] %n: %v%$");

            coreLogger = spdlog::stdout_color_mt("MirrasENGINE");
            coreLogger->set_level(spdlog::level::trace);
    #endif
        }

        static void initAppLog(std::string_view appTitle)
    #ifndef DISTRIBUTION_BUILD
        {
            appLogger = spdlog::stdout_color_mt(appTitle.data());
            appLogger->set_level(spdlog::level::trace);
        }

        inline static std::shared_ptr<spdlog::logger> coreLogger;
		inline static std::shared_ptr<spdlog::logger> appLogger;
    #else
        { }
    #endif
	};
}

#ifndef DISTRIBUTION_BUILD

    #define ENGINE_LOG_TRACE(...) mirras::Log::coreLogger->trace(__VA_ARGS__)
    #define ENGINE_LOG_INFO(...)   mirras::Log::coreLogger->info(__VA_ARGS__)
    #define ENGINE_LOG_WARN(...)   mirras::Log::coreLogger->warn(__VA_ARGS__)
    #define ENGINE_LOG_ERROR(...) mirras::Log::coreLogger->error(__VA_ARGS__)

    #define LOG_TRACE(...) mirras::Log::appLogger->trace(__VA_ARGS__)
    #define LOG_INFO(...)   mirras::Log::appLogger->info(__VA_ARGS__)
    #define LOG_WARN(...)   mirras::Log::appLogger->warn(__VA_ARGS__)
    #define LOG_ERROR(...) mirras::Log::appLogger->error(__VA_ARGS__)

#else

    #define ENGINE_LOG_TRACE(...)
    #define ENGINE_LOG_INFO(...)
    #define ENGINE_LOG_WARN(...)
    #define ENGINE_LOG_ERROR(...)

    #define LOG_TRACE(...)
    #define LOG_INFO(...)
    #define LOG_WARN(...)
    #define LOG_ERROR(...)

#endif

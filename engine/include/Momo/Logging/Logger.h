#pragma once
#include <string>
#include <string_view>
#include <format>
#include <mutex>

namespace Momo
{
    enum class LogLevel
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    class Logger
    {
    public:
        // Template function for formatted logging
        template<typename... Args>
        static void Log(LogLevel level, std::string_view component, std::format_string<Args...> fmt, Args&&... args)
        {
            std::string message = std::format(fmt, std::forward<Args>(args)...);
            LogImpl(level, component, message);
        }

        // Overload for simple string messages (no formatting)
        static void Log(LogLevel level, std::string_view component, std::string_view message);
        static void SetLevel(LogLevel level);
        static bool IsEnabled(LogLevel level) {
            return level >= s_CurrentLevel.load(std::memory_order_relaxed);
        }

    private:
        static void LogImpl(LogLevel level, std::string_view component, const std::string& message);
        static const char* GetLevelString(LogLevel level);
        static const char* GetLevelColor(LogLevel level);
        static std::mutex s_Mutex;
        static std::atomic<LogLevel> s_CurrentLevel;
    };
}

#ifndef MOMO_LOG_COMPILE_LEVEL
  #ifndef NDEBUG
    #define MOMO_LOG_COMPILE_LEVEL ::Momo::LogLevel::Debug
  #else
    #define MOMO_LOG_COMPILE_LEVEL ::Momo::LogLevel::Info
  #endif
#endif

// Convenience macros for logging
#define LOG_TRACE(component, ...) \
    do { if (::Momo::Logger::IsEnabled(::Momo::LogLevel::Trace)) \
         ::Momo::Logger::Log(::Momo::LogLevel::Trace, component, __VA_ARGS__); \
    } while(0)
#define LOG_DEBUG(component, ...) \
    do { if (::Momo::Logger::IsEnabled(::Momo::LogLevel::Debug)) \
         ::Momo::Logger::Log(::Momo::LogLevel::Debug, component, __VA_ARGS__); \
    } while(0)
#define LOG_INFO(component, ...) \
    do { if (::Momo::Logger::IsEnabled(::Momo::LogLevel::Info)) \
         ::Momo::Logger::Log(::Momo::LogLevel::Info, component, __VA_ARGS__); \
    } while(0)
#define LOG_WARN(component, ...) \
    do { if (::Momo::Logger::IsEnabled(::Momo::LogLevel::Warning)) \
         ::Momo::Logger::Log(::Momo::LogLevel::Warning, component, __VA_ARGS__); \
    } while(0)
#define LOG_ERROR(component, ...) \
    do { if (::Momo::Logger::IsEnabled(::Momo::LogLevel::Error)) \
         ::Momo::Logger::Log(::Momo::LogLevel::Error, component, __VA_ARGS__); \
    } while(0)
#define LOG_FATAL(component, ...) \
    do { if (::Momo::Logger::IsEnabled(::Momo::LogLevel::Fatal)) \
         ::Momo::Logger::Log(::Momo::LogLevel::Fatal, component, __VA_ARGS__); \
    } while(0)

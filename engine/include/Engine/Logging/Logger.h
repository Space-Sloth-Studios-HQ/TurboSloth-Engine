#pragma once
#include <string>
#include <string_view>
#include <format>
#include <mutex>

namespace Engine
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

    private:
        static void LogImpl(LogLevel level, std::string_view component, const std::string& message);
        static const char* GetLevelString(LogLevel level);
        static const char* GetLevelColor(LogLevel level);
        static std::mutex s_Mutex;
    };
}

// Convenience macros for logging
#define LOG_TRACE(component, ...) ::Engine::Logger::Log(::Engine::LogLevel::Trace, component, __VA_ARGS__)
#define LOG_DEBUG(component, ...) ::Engine::Logger::Log(::Engine::LogLevel::Debug, component, __VA_ARGS__)
#define LOG_INFO(component, ...)  ::Engine::Logger::Log(::Engine::LogLevel::Info, component, __VA_ARGS__)
#define LOG_WARN(component, ...)  ::Engine::Logger::Log(::Engine::LogLevel::Warning, component, __VA_ARGS__)
#define LOG_ERROR(component, ...) ::Engine::Logger::Log(::Engine::LogLevel::Error, component, __VA_ARGS__)
#define LOG_FATAL(component, ...) ::Engine::Logger::Log(::Engine::LogLevel::Fatal, component, __VA_ARGS__)

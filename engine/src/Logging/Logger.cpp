#include "Engine/Logging/Logger.h"
#include <iostream>

namespace Engine
{
    // Initialize static mutex
    std::mutex Logger::s_Mutex;

    // ANSI color codes for terminal output
    namespace Colors
    {
        constexpr const char* Reset   = "\033[0m";
        constexpr const char* Gray    = "\033[90m";
        constexpr const char* Cyan    = "\033[36m";
        constexpr const char* Green   = "\033[32m";
        constexpr const char* Yellow  = "\033[33m";
        constexpr const char* Red     = "\033[31m";
        constexpr const char* BoldRed = "\033[1;31m";
    }

    void Logger::Log(LogLevel level, std::string_view component, std::string_view message)
    {
        LogImpl(level, component, std::string(message));
    }

    void Logger::LogImpl(LogLevel level, std::string_view component, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);

        const char* levelStr = GetLevelString(level);
        const char* color = GetLevelColor(level);

        // Format: [LEVEL] [Component] Message
        std::cout << color
                  << "[" << levelStr << "] "
                  << "[" << component << "] "
                  << message
                  << Colors::Reset
                  << '\n';

        // Flush immediately for errors and fatal messages
        if (level >= LogLevel::Error)
        {
            std::cout.flush();
        }

        // Terminate program on fatal errors
        if (level == LogLevel::Fatal)
        {
            std::terminate();
        }
    }

    const char* Logger::GetLevelString(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Trace:   return "TRACE";
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            default:                return "UNKNOWN";
        }
    }

    const char* Logger::GetLevelColor(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Trace:   return Colors::Gray;
            case LogLevel::Debug:   return Colors::Cyan;
            case LogLevel::Info:    return Colors::Green;
            case LogLevel::Warning: return Colors::Yellow;
            case LogLevel::Error:   return Colors::Red;
            case LogLevel::Fatal:   return Colors::BoldRed;
            default:                return Colors::Reset;
        }
    }
}

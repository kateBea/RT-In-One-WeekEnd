// MIT License
//
// Copyright (c) 2026 ケイト
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <mutex>
#include <string>
#include <chrono>
#include <format>
#include <thread>
#include <iostream>
#include <functional> // for std::hash
#include <string_view>

namespace pathtracer::core::io {

    enum class console_color {
        Default,
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        BrightBlack,
        BrightRed,
        BrightGreen,
        BrightYellow,
        BrightBlue,
        BrightMagenta,
        BrightCyan,
        BrightWhite
    };

    constexpr auto get_ansi_color(console_color color) -> const char* {
        switch (color) {
            case console_color::Black:         return "\033[30m";
            case console_color::Red:           return "\033[31m";
            case console_color::Green:         return "\033[32m";
            case console_color::Yellow:        return "\033[33m";
            case console_color::Blue:          return "\033[34m";
            case console_color::Magenta:       return "\033[35m";
            case console_color::Cyan:          return "\033[36m";
            case console_color::White:         return "\033[37m";
            case console_color::BrightBlack:   return "\033[90m";
            case console_color::BrightRed:     return "\033[91m";
            case console_color::BrightGreen:   return "\033[92m";
            case console_color::BrightYellow:  return "\033[93m";
            case console_color::BrightBlue:    return "\033[94m";
            case console_color::BrightMagenta: return "\033[95m";
            case console_color::BrightCyan:    return "\033[96m";
            case console_color::BrightWhite:   return "\033[97m";
            default:                   return "\033[0m";
        }
    }

    enum class log_level {
        trace,
        info,
        warn,
        error,
        critical
    };

    template<typename... Args>
    auto println(const std::format_string<Args...> fmt, Args&&... args) -> void {
        const std::string result{ std::vformat(fmt.get(), std::make_format_args(args...)) };
        std::printf("%s\n", result.c_str());
    }

    template<typename... Args>
    auto print(const std::format_string<Args...> fmt, Args&&... args) -> void {
        const std::string result{ std::vformat(fmt.get(), std::make_format_args(args...)) };
        std::printf("%s", result.c_str());
    }

    template<typename... Args>
    auto print(console_color color, const std::format_string<Args...> fmt, Args&&... args) -> void {
        std::string_view ansi{ get_ansi_color(color) };
        const std::string result{ std::vformat(fmt.get(), std::make_format_args(args...)) };
        std::printf("%s%s\033[0m", ansi.data(), result.c_str());
    }

    template<typename... Args>
    auto println(console_color color, const std::format_string<Args...> fmt, Args&&... args) -> void {
        std::string_view ansi{ get_ansi_color(color) };
        const std::string result{ std::vformat(fmt.get(), std::make_format_args(args...)) };
        std::printf("%s%s\033[0m\n", ansi.data(), result.c_str()); // Resetear color de la consola
    }

    template<typename... Args>
    auto format(const std::format_string<Args...> fmt, Args&&... args) -> std::string {
        return std::vformat(fmt.get(), std::make_format_args(args...));
    }
    
    class logger {
    public:
        // Loggea el mensaje sin cambios
        static auto log(log_level level, std::string_view message) -> void {
            std::lock_guard lock{ s_log_mutex };

            // Formatear tiempo
            const auto now{ std::chrono::system_clock::now() };
            const auto time{ std::chrono::system_clock::to_time_t(now) };
            std::tm local_time{};

            #if defined(_WIN32)
                localtime_s(&local_time, &time);
            #else
                localtime_r(&time, &local_time);
            #endif

            const std::string timestamp{ std::format("{:02}:{:02}:{:02}",
                local_time.tm_hour, local_time.tm_min, local_time.tm_sec) };

            // Formatear nivel traza
            std::string_view level_str{};
            std::string_view color{};

            // Colores consola:
            // https://medium.com/@vitorcosta.matias/print-coloured-texts-in-console-a0db6f589138
            switch (level) {
                case log_level::trace:    level_str = "TRACE";    color = "\033[90m"; break;
                case log_level::info:     level_str = "INFO";     color = "\033[32m"; break;
                case log_level::warn:     level_str = "WARN";     color = "\033[33m"; break;
                case log_level::error:    level_str = "ERROR";    color = "\033[31m"; break;
                case log_level::critical: level_str = "CRITICAL"; color = "\033[1;31m"; break;
            }

            const std::thread::id& tid{ std::this_thread::get_id() };
            std::uint16_t thread_id{ static_cast<std::uint16_t>( std::hash<std::thread::id>{}( tid ) ) };

            println("Thread [{}] {}[{}] [{}] {}{}", thread_id, color, timestamp, level_str, message, "\033[0m");
        }

        // Permite formatear el mensaje antes de loggear
        // cppreference para ver formato de std::format: https://en.cppreference.com/w/cpp/utility/format/format
        template<typename... Args>
        static auto logf(log_level level, std::string_view fmt, Args&&... args) -> void {
            log(level, std::vformat(fmt, std::make_format_args(args...)));
        }

    private:
        inline static std::mutex s_log_mutex{};
    };
}

#define LOG_TRACE(...)    pathtracer::core::io::logger::logf(pathtracer::core::io::log_level::trace, __VA_ARGS__)
#define LOG_INFO(...)     pathtracer::core::io::logger::logf(pathtracer::core::io::log_level::info, __VA_ARGS__)
#define LOG_WARN(...)     pathtracer::core::io::logger::logf(pathtracer::core::io::log_level::warn, __VA_ARGS__)
#define LOG_ERROR(...)    pathtracer::core::io::logger::logf(pathtracer::core::io::log_level::error, __VA_ARGS__)
#define LOG_CRITICAL(...) pathtracer::core::io::logger::logf(pathtracer::core::io::log_level::critical, __VA_ARGS__)
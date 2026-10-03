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

#include <chrono>
#include <string>
#include <string_view>

namespace pathtracer::core {

    class scoped_timer {
    public:
        explicit scoped_timer(std::string_view name = "", bool print_on_destruction = false)
            : _name{ std::move(name) },
              _print_on_destruction{ print_on_destruction },
              _start{ std::chrono::high_resolution_clock::now() } {}
        
        ~scoped_timer() {
            const auto end{ std::chrono::high_resolution_clock::now() };
            const auto duration{ std::chrono::duration_cast<std::chrono::microseconds>(end - _start).count() };

            if (_print_on_destruction) {
                LOG_INFO( "[Timer] {} took {} ms", ( _name.empty() ? "Scope" :  _name), duration / 1000.0 );
            }
        }

        // Non-copyable
        scoped_timer(const scoped_timer&) = delete;
        scoped_timer& operator=(const scoped_timer&) = delete;

        // Movable
        scoped_timer(scoped_timer&&) noexcept = default;
        scoped_timer& operator=(scoped_timer&&) noexcept = default;

        // Timer management
        auto reset() -> void {
            _start = std::chrono::high_resolution_clock::now();
        }

        [[nodiscard]] auto elapsed() const -> double { 
            const auto end{ std::chrono::high_resolution_clock::now() };
            const auto duration{ std::chrono::duration_cast<std::chrono::microseconds>(end - _start).count() };
            return duration / 1000.0;
        }

        [[nodiscard]] auto name() const -> std::string_view { return _name; }

    private:
        std::string _name{};
        bool _print_on_destruction{};
        std::chrono::high_resolution_clock::time_point _start{};
    };

#define START_SCOPED_TIMER(name) pathtracer::core::scoped_timer timer##__LINE__{name, true};

}
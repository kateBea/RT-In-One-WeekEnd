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

#include <pathtracer/math/utility.hh>

namespace pathtracer::math {

    class interval {
    public:
    
        interval() : _min(+infinity), _max(-infinity) {} // Default interval is empty
        interval(double min, double max) : _min(min), _max(max) {}

        [[nodiscard]] double min() const { return _min; }
        [[nodiscard]] double max() const { return _max; }
        
        [[nodiscard]] double size() const { return _max - _min; }
        [[nodiscard]] bool contains(double x) const { return _min <= x && x <= _max; }
        [[nodiscard]] bool surrounds(double x) const { return _min < x && x < _max; }
    
        [[nodiscard]] auto clamp(double x) const -> double {
            if (x < _min) return _min;
            if (x > _max) return _max;

            return x;
        }
    private:
        double _min{}; 
        double _max{};

        static const interval empty; 
        static const interval universe;
    };

    const interval interval::empty    = interval(+infinity, -infinity);
    const interval interval::universe = interval(-infinity, +infinity);
}
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

#include <pathtracer/math/point.hh>
#include <pathtracer/math/vector.hh>

namespace pathtracer::math {

    class ray final {
    public:
        ray() = default;
        ray(const point3& origin, const vec3& direction) 
            : _origin(origin), _direction(direction) {}

        auto origin() const -> point3 { return _origin; }
        auto direction() const -> vec3 { return _direction; }

        [[nodiscard]] auto at(double t) const -> point3 { return _origin + t * _direction; }

    private:
        point3 _origin{};
        vec3 _direction{};
    };
}
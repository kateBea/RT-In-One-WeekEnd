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

#include <cmath>
#include <cstdint>
#include <iostream>

namespace pathtracer::math {
    
    using color = vec3;

    inline auto linear_to_gamma(double linear_component) -> double {
        if (linear_component > 0) {
            return std::sqrt(linear_component);
        }

        return 0;
    }

    inline auto write( std::ostream& out, const color& pixel_color ) -> void {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // Apply a linear to gamma transform for gamma 2
        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // Translate the [0,1] component values to the byte range [0,255].
        static const interval intensity(0.000, 0.999);

        std::int32_t rbyte{ static_cast<std::int32_t>(255.999 * intensity.clamp(r)) };
        std::int32_t gbyte{ static_cast<std::int32_t>(255.999 * intensity.clamp(g)) };
        std::int32_t bbyte{ static_cast<std::int32_t>(255.999 * intensity.clamp(b)) };

        // Write out the pixel color components.
        out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
    }

}
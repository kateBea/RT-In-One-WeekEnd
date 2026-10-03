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

#include <pathtracer/math/ray.hh>
#include <pathtracer/math/point.hh>
#include <pathtracer/math/vector.hh>

namespace pathtracer::math {
    
    class hit_record {
    public:

        [[nodiscard]] auto point() const -> point3 { return _point; }
        [[nodiscard]] auto normal() const -> vec3 { return _normal; }
        [[nodiscard]] auto t() const -> double { return _t; }

        auto set_point(const point3& p) -> void { _point = p; }
        auto set_normal(const vec3& n) -> void { _normal = n; }
        auto set_t(double t) -> void { _t = t; }

        // Adding front-face tracking to hit_record
        auto set_face_normal(const ray& r, const vec3& outward_normal) -> void {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.

            _front_face = vec3::dot(r.direction(), outward_normal) < 0;
            _normal = _front_face ? outward_normal : -outward_normal;
        }

    private:
        point3 _point;
        vec3 _normal;
        double _t;

        bool _front_face;
    };

    class hittable {
    public:
        virtual ~hittable() = default;

        [[nodiscard]] virtual auto hit(const ray& r, interval ray_t, hit_record& rec) const -> bool = 0;
    };
}
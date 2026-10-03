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

#include <pathtracer/math/ray.hh>
#include <pathtracer/math/point.hh>
#include <pathtracer/math/vector.hh>
#include <pathtracer/math/interval.hh>
#include <pathtracer/math/hittable.hh>

#include <pathtracer/renderer/material.hh>

namespace pathtracer::math {

    class sphere : public hittable {
    public:
        sphere(const point3& center, double radius, std::shared_ptr<renderer::material> mat)
            : _center(center), _radius(std::fmax(0,radius)), _material(mat) {}

        [[nodiscard]] auto hit(const math::ray& r, math::interval ray_t, math::hit_record& rec) const -> bool override {
            math::vec3 oc = _center - r.origin();
            auto a = r.direction().length_squared();
            auto h = math::vec3::dot(r.direction(), oc);
            auto c = oc.length_squared() - _radius*_radius;

            auto discriminant = h*h - a*c;
            if (discriminant < 0) {
                return false;
            }

            auto sqrtd = std::sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;

                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }

            rec.set_t(root);
            rec.set_point(r.at(rec.t()));
            rec.set_normal((rec.point() - _center) / _radius);

            vec3 outward_normal = (rec.point() - _center) / _radius;
            rec.set_face_normal(r, outward_normal);
            rec.set_material(_material);

            return true;
        }

    private:
        point3 _center;
        double _radius;

        std::shared_ptr<renderer::material> _material{};
    };
}
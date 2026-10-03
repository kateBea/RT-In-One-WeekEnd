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

#include <cstdint>

#include <pathtracer/math/interval.hh>

namespace pathtracer::math {

    class vec3 final {
    public:
        vec3() = default;
        vec3(double x, double y, double z) : _x(x), _y(y), _z(z) {}

        [[nodiscard]] auto x() const -> double { return _x; }
        [[nodiscard]] auto y() const -> double { return _y; }
        [[nodiscard]] auto z() const -> double { return _z; }

        [[nodiscard]] auto r() const -> double { return _r; }
        [[nodiscard]] auto g() const -> double { return _g; }
        [[nodiscard]] auto b() const -> double { return _b; }

        [[nodiscard]] auto s() const -> double { return _s; }
        [[nodiscard]] auto t() const -> double { return _t; }
        [[nodiscard]] auto p() const -> double { return _p; }

        [[nodiscard]] auto length() const -> double { return std::sqrt(length_squared()); }
        [[nodiscard]] auto length_squared() const -> double { return _x * _x + _y * _y + _z * _z; }

        [[nodiscard]] auto operator-() const -> vec3 { return vec3(-_x, -_y, -_z); }
        [[nodiscard]] auto operator-(const vec3& other) const -> vec3 { return vec3(_x - other._x, _y - other._y, _z - other._z); }

        [[nodiscard]] auto operator+(double scalar) const -> vec3 { return vec3(_x + scalar, _y + scalar, _z + scalar); }
        [[nodiscard]] auto operator+(const vec3& other) const -> vec3 { return vec3(_x + other._x, _y + other._y, _z + other._z); }

        [[nodiscard]] auto operator/(double scalar) const -> vec3 { return vec3(_x / scalar, _y / scalar, _z / scalar); }
        [[nodiscard]] auto operator/(const vec3& other) const -> vec3 { return vec3(_x / other._x, _y / other._y, _z / other._z); }

        [[nodiscard]] auto operator==(const vec3& other) const -> bool { return _x == other._x && _y == other._y && _z == other._z; }
        [[nodiscard]] auto operator!=(const vec3& other) const -> bool { return !(*this == other); }

        auto operator+=(const vec3& other) -> vec3& { _x += other._x; _y += other._y; _z += other._z; return *this; }
        auto operator-=(const vec3& other) -> vec3& { _x -= other._x; _y -= other._y; _z -= other._z; return *this; }
        auto operator*=(double scalar) -> vec3& { _x *= scalar; _y *= scalar; _z *= scalar; return *this; }
        auto operator/=(double scalar) -> vec3& { _x /= scalar; _y /= scalar; _z /= scalar; return *this; }

        [[nodiscard]] auto operator[](std::size_t index) -> double& { return index == 0 ? _x : (index == 1 ? _y : _z); }
        [[nodiscard]] auto operator[](std::size_t index) const -> double { return index == 0 ? _x : (index == 1 ? _y : _z); }

        [[nodiscard]] auto dot(const vec3& other) const -> double { return _x * other._x + _y * other._y + _z * other._z; }
        [[nodiscard]] auto cross(const vec3& other) const -> vec3 {
            return vec3(
                _y * other._z - _z * other._y,
                _z * other._x - _x * other._z,
                _x * other._y - _y * other._x
            );
        }

        inline static auto reflect(const vec3& v, const vec3& n) -> vec3 {
            return v - 2*dot(v,n)*n;
        }

        inline static auto refract(const vec3& uv, const vec3& n, double etai_over_etat) -> vec3 {
            auto cos_theta{ std::fmin(dot(-uv, n), 1.0) };
            vec3 r_out_perp{ etai_over_etat * (uv + cos_theta*n) };
            vec3 r_out_parallel{ -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n };

            return r_out_perp + r_out_parallel;
        }

        [[nodiscard]] auto near_zero() const -> bool {
            // Return true if the vector is close to zero in all dimensions.
            auto s{ 1e-8 };
            return (std::fabs(_x) < s) && (std::fabs(_y) < s) && (std::fabs(_z) < s);
        }

        [[nodiscard]] auto normalized() const -> vec3 { return *this / length(); }

        [[nodiscard]] static auto normalized( const vec3& v ) -> vec3 { return v / v.length(); }

        [[nodiscard]] friend auto operator*(double scalar, const vec3& v) -> vec3 { return vec3(v._x * scalar, v._y * scalar, v._z * scalar); }
        [[nodiscard]] friend auto operator*( const vec3& v, double scalar) -> vec3 { return vec3(v._x * scalar, v._y * scalar, v._z * scalar); }
        [[nodiscard]] friend auto operator*( const vec3& v, const vec3& other) -> vec3 { return vec3(v._x * other._x, v._y * other._y, v._z * other._z); }

        [[nodiscard]] static auto dot(const vec3& v, const vec3& other) -> double { return v._x * other._x + v._y * other._y + v._z * other._z; }

        [[nodiscard]] static auto random() -> vec3 {
            return vec3(random_double(), random_double(), random_double());
        }

        [[nodiscard]] static auto random(double min, double max) -> vec3 {
            return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
        }

        [[nodiscard]] inline static auto random_unit_vector() -> vec3 {
            while (true) {
                auto p{ vec3::random(-1,1) };
                auto lensq{ p.length_squared() };
                if (1e-160 < lensq && lensq <= 1.0) {
                    return p / std::sqrt(lensq);
                }
            }

            return vec3(1,0,0); // Should never reach here
        }

        inline static auto random_in_unit_disk() -> vec3 {
            while (true) {
                auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
                if (p.length_squared() < 1) {
                    return p;
                }
            }
        }

        [[nodiscard]] inline static auto random_on_hemisphere(const vec3& normal) -> vec3 {
            vec3 on_unit_sphere{ random_unit_vector() };

            if (dot(on_unit_sphere, normal) > 0.0) {
                // In the same hemisphere as the normal
                return on_unit_sphere;
            }
            else {
                return -on_unit_sphere;
            }
        }

    private:
        // This defines 3 member variables but it is as if they had different names
        // So I can access x as r or s, y as g or t and so on.
        union { double _x, _r, _s; };
		union { double _y, _g, _t; };
        union { double _z, _b, _p; };
    };
}
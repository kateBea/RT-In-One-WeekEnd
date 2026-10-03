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
#include <pathtracer/math/color.hh>
#include <pathtracer/math/hittable.hh>

namespace pathtracer::renderer {

    class material {
    public:
        virtual ~material() = default;

        virtual bool scatter(const math::ray& r_in, 
            const math::hit_record& rec, 
            math::color& attenuation, 
            math::ray& scattered) const 
        { 
            return false; 
        }
    };

    class lambertian : public material {
    public:
        lambertian(const math::color& albedo) : _albedo(albedo) {}

        bool scatter(const math::ray& r_in, 
            const math::hit_record& rec, 
            math::color& attenuation, math::ray& scattered) const override {
            auto scatter_direction = rec.normal() + math::vec3::random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal();
            }

            scattered = math::ray(rec.point(), scatter_direction);
            attenuation = _albedo;
            return true;
        }

    private:
        math::color _albedo{};
    };

    class metal : public material {
    public:
        metal(const math::color& albedo, double fuzz) 
            : albedo(albedo), _fuzz(fuzz < 1 ? fuzz : 1) {}

        [[nodiscard]] auto scatter(const math::ray& r_in, 
            const math::hit_record& rec, 
            math::color& attenuation, math::ray& scattered) const -> bool override {
            math::vec3 reflected{ math::vec3::reflect(r_in.direction(), rec.normal()) };
            
            reflected = reflected.normalized() + (_fuzz * math::vec3::random_unit_vector());

            scattered = math::ray(rec.point(), reflected);
            attenuation = albedo;

            return (math::vec3::dot(scattered.direction(), rec.normal()) > 0);
        }

    private:
        math::color albedo;
        double _fuzz{};
    };

    class dielectric : public material {
    public:
        dielectric(double refraction_index) : refraction_index(refraction_index) {}

        [[nodiscard]] auto scatter(const math::ray& r_in, 
            const math::hit_record& rec, 
            math::color& attenuation, math::ray& scattered) const -> bool override 
        {
            attenuation = math::color(1.0, 1.0, 1.0);
            double ri = rec.front_face() ? (1.0/refraction_index) : refraction_index;

            math::vec3 unit_direction = math::vec3::normalized(r_in.direction());
            
            double cos_theta = std::fmin(math::vec3::dot(-unit_direction, rec.normal()), 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = ri * sin_theta > 1.0;
            math::vec3 direction;

            if (cannot_refract || reflectance(cos_theta, ri) > math::random_double()) {
                direction = math::vec3::reflect(unit_direction, rec.normal());
            }
            else {
                direction = math::vec3::refract(unit_direction, rec.normal(), ri);
            }

            scattered = math::ray(rec.point(), direction);

            math::vec3 refracted = math::vec3::refract(unit_direction, rec.normal(), ri);

            scattered = math::ray(rec.point(), refracted);
            return true;
        }

    private:
        static auto reflectance(double cosine, double refraction_index) -> double {
            // Use Schlick's approximation for reflectance.
            auto r0{ (1 - refraction_index) / (1 + refraction_index) };
            r0 = r0*r0;

            return r0 + (1-r0)*std::pow((1 - cosine),5);
        }

    private:
        // Refractive index in vacuum or air, or the ratio of the material's refractive index over
        // the refractive index of the enclosing media
        double refraction_index;
    };

} // namespace pathtracer::renderer
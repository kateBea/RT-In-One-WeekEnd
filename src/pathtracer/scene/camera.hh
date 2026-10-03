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

#include <memory>
#include <cstdint>
#include <iostream>

#include <pathtracer/core/io.hh>
#include <pathtracer/core/timer.hh>
#include <pathtracer/core/logger.hh>

#include <pathtracer/math/ray.hh>
#include <pathtracer/math/color.hh>
#include <pathtracer/math/point.hh>
#include <pathtracer/math/vector.hh>
#include <pathtracer/math/sphere.hh>
#include <pathtracer/math/utility.hh>
#include <pathtracer/math/interval.hh>
#include <pathtracer/math/hittable.hh>
#include <pathtracer/math/hittable_list.hh>

namespace pathtracer::scene {
    class camera {
    public:

        auto render(const math::hittable& world) -> void {
            START_SCOPED_TIMER("Camera::render");

            initialize();

            std::cout << "P3\n" << image_width << ' ' << _image_height << "\n255\n";

            for (std::int32_t j{}; j < _image_height; j++) {
                // Debug log progress to console
                LOG_TRACE("Scanlines remaining: {}", (_image_height - j) );

                for (std::int32_t i{}; i < image_width; i++) {
                    math::color pixel_color(0,0,0);

                    for (std::int32_t sample{}; sample < _samples_per_pixel; sample++) {
                        math::ray r{ get_ray(i, j) };
                         pixel_color += ray_color(r, _max_depth, world);
                    }

                    pixel_color *= _pixel_samples_scale;
                    math::write(std::cout, pixel_color);
                }
            }

            LOG_TRACE("Done.");
        }

        auto set_aspect_ratio(double ratio) -> void {
            aspect_ratio = ratio;
        }

        auto set_image_width(std::int32_t width) -> void {
            image_width = width;
        }

        auto set_samples_per_pixel(std::int32_t samples) -> void {
            _samples_per_pixel = samples;
        }

        auto set_max_depth(std::int32_t depth) -> void {
            _max_depth = depth;
        }

        auto set_field_of_view(double vfov_degrees) -> void {
            _vfov = vfov_degrees;
        }

        auto set_lookfrom(const math::point3& lookfrom) -> void {
            _lookfrom = lookfrom;
        }

        auto set_lookat(const math::point3& lookat) -> void {
            _lookat = lookat;
        }

        auto set_vup(const math::vec3& vup) -> void {
            _vup = vup;
        }

        auto set_defocus_angle(double defocus_angle) -> void {
            _defocus_angle = defocus_angle;
        }

        auto set_focus_dist(double focus_dist) -> void {
            _focus_dist = focus_dist;
        }

    private:
        auto initialize() -> void {
            _image_height = int(image_width / aspect_ratio);
            _image_height = (_image_height < 1) ? 1 : _image_height;

            _pixel_samples_scale = 1.0 / _samples_per_pixel;

            _center = _lookfrom;

            // Determine viewport dimensions.
             auto focal_length = (_lookfrom - _lookat).length();

            auto theta = math::to_radians(_vfov);
            auto h = std::tan(theta/2);
            auto viewport_height = 2 * h * _focus_dist;
            auto viewport_width = viewport_height * (double(image_width)/_image_height);

            // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
            _w = math::vec3::normalized(_lookfrom - _lookat);
            _u = math::vec3::normalized(_vup.cross(_w));
            _v = _w.cross(_u);

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            math::vec3 viewport_u = viewport_width * _u;    // Vector across viewport horizontal edge
            math::vec3 viewport_v = viewport_height * -_v;  // Vector down viewport vertical edge

            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            _pixel_delta_u = viewport_u / image_width;
            _pixel_delta_v = viewport_v / _image_height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left = _center - (_focus_dist * _w) - viewport_u/2 - viewport_v/2;
            _pixel00_loc = viewport_upper_left + 0.5 * (_pixel_delta_u + _pixel_delta_v);

             // Calculate the camera defocus disk basis vectors.
            auto defocus_radius{ _focus_dist * std::tan(math::to_radians(_defocus_angle / 2)) };
            _defocus_disk_u = _u * defocus_radius;
            _defocus_disk_v = _v * defocus_radius;
        }

        [[nodiscard]] auto get_ray(int i, int j) const -> math::ray {
            // Construct a camera ray originating from the 
            // origin and directed at randomly sampled
            // point around the pixel location i, j.

            auto offset{ sample_square() };
            auto pixel_sample = _pixel00_loc
                            + ((i + offset.x()) * _pixel_delta_u)
                            + ((j + offset.y()) * _pixel_delta_v);

            auto ray_origin = (_defocus_angle <= 0) ? _center : defocus_disk_sample();
            auto ray_direction{ pixel_sample - ray_origin };

            return math::ray(ray_origin, ray_direction);
        }

        inline auto defocus_disk_sample() const -> math::point3 {
            // Returns a random point in the camera defocus disk.
            auto p = math::vec3::random_in_unit_disk();
            return _center + (p[0] * _defocus_disk_u) + (p[1] * _defocus_disk_v);
        }


        [[nodiscard]] auto sample_square() const -> math::vec3 {
            // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
            return math::vec3(math::random_double() - 0.5, math::random_double() - 0.5, 0.0);
        }

        [[nodiscard]] auto ray_color(const math::ray& r, std::int32_t depth, const math::hittable& world) const -> math::color {
            // If we've exceeded the ray bounce limit, no more light is gathered.
            if (depth <= 0) {
                return math::color(0, 0, 0);
            }

            math::hit_record rec{};

            // Fix shadow acne by setting the minimum t value 
            // to a small positive number (0.001) instead of 0.
            if (world.hit(r, math::interval(0.001, math::infinity), rec)) {
                // Lambdea function to generate a random unit vector
                // in the hemisphere of the hit point's normal.
                math::ray scattered;
                math::color attenuation;
                if (rec.material()->scatter(r, rec, attenuation, scattered)) {
                    return attenuation * ray_color(scattered, depth-1, world);
                }

                return math::color(0,0,0);
            }

            math::vec3 unit_direction{ r.direction().normalized() };
            auto a{ 0.5*(unit_direction.y() + 1.0) };

            return (1.0-a)*math::color(1.0, 1.0, 1.0) + a*math::color(0.5, 0.7, 1.0);
        }

        private:
            double aspect_ratio{ 1.0 };  // Ratio of image width over height
            std::int32_t    image_width{ 100 };  // Rendered image width in pixel count
            std::int32_t    _image_height{};   // Rendered image height

            math::point3 _center{};         // Camera center
            math::point3 _pixel00_loc{};    // Location of pixel 0, 0
            math::vec3   _pixel_delta_u{};  // Offset to pixel to the right
            math::vec3   _pixel_delta_v{};  // Offset to pixel below

            double _pixel_samples_scale{};  // Color scale factor for a sum of pixel samples
            std::int32_t _samples_per_pixel{ 10 };   // Count of random samples for each pixel

            std::int32_t _max_depth{ 10 };   // Maximum number of ray bounces into scene

            double _vfov{ 90 };  // Vertical view angle (field of view) in degrees
            math::point3 _lookfrom = math::point3(0,0,0);   // Point camera is looking from
            math::point3 _lookat   = math::point3(0,0,-1);  // Point camera is looking at
            math::vec3   _vup      = math::vec3(0,1,0);     // Camera-relative "up" direction

            math::vec3   _u{}, _v{}, _w{};              // Camera frame basis vectors

            double _defocus_angle{};  // Variation angle of rays through each pixel
            double _focus_dist{ 10 };    // Distance from camera lookfrom point to plane of perfect focus

            math::vec3   _defocus_disk_u{};       // Defocus disk horizontal radius
            math::vec3   _defocus_disk_v{};       // Defocus disk vertical radius
    };
}
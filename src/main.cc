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

#include <iostream>
#include <cstdint>

#include <pathtracer/core/io.hh>
#include <pathtracer/core/logger.hh>

#include <pathtracer/math/ray.hh>
#include <pathtracer/math/color.hh>
#include <pathtracer/math/point.hh>
#include <pathtracer/math/vector.hh>

using namespace pathtracer;

auto ray_color(const math::ray& r) -> math::color {
    math::vec3 unit_direction{ r.direction().normalized() };
    auto a{ 0.5 * (unit_direction.y() + 1.0) };
    
    return (1.0 - a) * math::color(1.0, 1.0, 1.0) + a * math::color(0.5, 0.7, 1.0);
}

auto main(int argc, char** argv) -> int {
    pathtracer::core::io::redirect_stdout("image.ppm");

    // Image
    auto aspect_ratio{ 16.0 / 9.0 };
    std::int32_t image_width{ 400 };

    // Calculate the image height, and ensure that it's at least 1.
    std::int32_t image_height = std::int32_t(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    auto focal_length{ 1.0 };
    auto viewport_height{ 2.0 };
    auto camera_center{ math::point3(0, 0, 0) };
    auto viewport_width{ viewport_height * (double(image_width)/image_height) };

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u{ math::vec3(viewport_width, 0, 0) };
    auto viewport_v{ math::vec3(0, -viewport_height, 0) };

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u{ viewport_u / image_width };
    auto pixel_delta_v{ viewport_v / image_height };

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left{ camera_center
        - math::vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2 };
    auto pixel00_loc{ viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v) };

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (std::int32_t j{}; j < image_height; j++) {
        LOG_INFO("Scanlines remaining: {}", image_height - j - 1);
        for (std::int32_t i{}; i < image_width; i++) {
            auto pixel_center{ pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v) };
            auto ray_direction{ pixel_center - camera_center };
            math::ray r(camera_center, ray_direction);

            math::color pixel_color{ ray_color(r) };
            pixel_color.write(std::cout);
        }
    }

    LOG_INFO("Done.             \n");

    return 0;
}
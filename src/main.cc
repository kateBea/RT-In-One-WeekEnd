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

#include <memory>
#include <cstdint>
#include <iostream>

#include <pathtracer/core/io.hh>
#include <pathtracer/core/logger.hh>

#include <pathtracer/scene/camera.hh>

#include <pathtracer/math/sphere.hh>
#include <pathtracer/renderer/material.hh>

using namespace pathtracer;
using namespace pathtracer::math;
using namespace pathtracer::scene;

auto main(int argc, char** argv) -> int {
    pathtracer::core::io::redirect_stdout("image.ppm");

    hittable_list world{};

    auto material_ground = std::make_shared<renderer::lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<renderer::lambertian>(color(0.2, 0.4, 0.35));
    auto material_left   = std::make_shared<renderer::dielectric>(1.50);
    auto material_bubble = std::make_shared<renderer::dielectric>(1.00 / 1.50);
    auto material_right  = std::make_shared<renderer::metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(std::make_shared<math::sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<math::sphere>(point3( 0.0,    0.0, -2.2),   0.5, material_center));
    world.add(std::make_shared<math::sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<math::sphere>(point3( -1.0,   0.0, -1.0),   0.4, material_bubble));
    world.add(std::make_shared<math::sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    camera cam{};

    cam.set_aspect_ratio(16.0 / 9.0);
    cam.set_image_width(1280);
    cam.set_samples_per_pixel(100);
    cam.set_max_depth(50);

    cam.render(world);

    return 0;
}
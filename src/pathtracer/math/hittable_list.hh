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
#include <vector>

#include <pathtracer/math/hittable.hh>

namespace pathtracer::math {

    class hittable_list : public hittable {
    public:
        hittable_list() = default;
        hittable_list(std::shared_ptr<hittable> object) { add(object); }

        auto clear() -> void { 
            _objects.clear(); 
        }
        
        auto add(std::shared_ptr<hittable> object) -> void { 
            _objects.push_back(object); 
        }

        [[nodiscard]] auto hit(const ray& r, interval ray_t, hit_record& rec) const -> bool override {
            hit_record temp_rec{};
            bool hit_anything{ false };
            auto closest_so_far{ ray_t.max() };

            for (const auto& object : _objects) {
                if (object->hit(r, interval(ray_t.min(), closest_so_far), temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t();
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }

    private:
        std::vector<std::shared_ptr<hittable>> _objects{};
    };
}
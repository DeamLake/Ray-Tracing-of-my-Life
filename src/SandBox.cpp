#include<iostream>
#include "rtweekend.h"
#include "color.h"
#include "sphere.h"
#include "material.h"
#include "bvh.h"
#include "camera.h"
#include <tbb/tbb/parallel_for.h>

color ray_color(const ray& r, const hittable& world, int depth) 
{
    hit_record rec;

    // 限制光的反射次数
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        // 命中之后 计算交点的散射情况
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}


int main() 
{
    // Image
    const int image_width = 400;
    const int image_height = 400;
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    const double aspect_ratio = static_cast<int>(image_width / image_height);
    OutputHelper helper(image_width, image_height, "test.png");

    // World
    hittable_list world;
    world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5, make_shared<lambertian>(vec3(0.1, 0.2, 0.5))));
    world.add(make_shared<sphere>(
        vec3(0, -100.5, -1), 100, make_shared<lambertian>(vec3(0.8, 0.8, 0.0))));
    world.add(make_shared<sphere>(vec3(1, 0, -1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.3)));
    world.add(make_shared<sphere>(vec3(-1, 0, -1), 0.5, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(vec3(-1, 0, -1), -0.45, make_shared<dielectric>(1.5)));

    // Camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.1;
    double vfov = 20;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

    //render
    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            tbb::parallel_for(tbb::blocked_range<size_t>(0, samples_per_pixel),
                [&](tbb::blocked_range<size_t> r) {
                    for (size_t s = r.begin(); s < r.end(); ++s)
                    {
                        // 增加偏移用于多次光线采样
                        double u = (i + random_double()) / (image_width - 1);
                        double v = (j + random_double()) / (image_height - 1);
                        ray r = cam.get_ray(u, v);
                        pixel_color += ray_color(r, world, max_depth);
                    }
                });
            helper.write_color(pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
}
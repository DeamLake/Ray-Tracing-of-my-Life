#include<iostream>
#include <mutex>
#include <tbb/tbb/parallel_for.h>
#include "rtweekend.h"
#include "color.h"
#include "sphere.h"
#include "material.h"
#include "bvh.h"
#include "camera.h"
#include "aarect.h"

color ray_color(const ray& r, const color& background, const hittable& world, int depth)
{
    hit_record rec;

    // 限制光的反射次数
    if (depth <= 0)
        return color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered; // 记录散射光
    color attenuation; // 光反射系数
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p); // 自发光项

    // 如果材质不具备反射/折射能力 直接返回自发光项
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    // 否则根据其他属性进行迭代计算
    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("../Resource/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    return objects;
}

int main() 
{
    // 线程锁
    std::mutex mtx;
    // Image
    int image_width = 600;
    int image_height = 600;
    int samples_per_pixel = 100;
    const int max_depth = 50;
    color background(0, 0, 0);
    double aspect_ratio = static_cast<int>(image_width / image_height);
    OutputHelper helper(image_width, image_height, "./out/Results/light_parallel.png");

    // Camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.0;
    double vfov = 20;

    // World
    hittable_list world;
    switch (3) {
    case 0:
        world = earth();
        lookfrom = point3(13, 2, 3);
        background = color(0.70, 0.80, 1.00);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;
    case 1:
        world = two_spheres();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        break;
    case 2:
        world = simple_light();
        samples_per_pixel = 400;
        background = color(0, 0, 0);
        lookfrom = point3(26, 3, 6);
        lookat = point3(0, 2, 0);
        vfov = 20.0;
        break;
    case 3:
        world = cornell_box();
        samples_per_pixel = 400;
        background = color(0, 0, 0);
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    default:
    case -1:
        background = color(0.0, 0.0, 0.0);
        break;
    }

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

    //  render
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
                        auto color = ray_color(r, background, world, max_depth);
                        {
                            std::lock_guard<std::mutex> lk(mtx);
                            pixel_color += color;
                        }
                    }
                });
            helper.write_color(pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
}
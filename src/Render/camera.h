#pragma once
#include "rtweekend.h"
#include "ray.h"

class camera 
{
public:
    camera(
        point3 lookfrom,
        point3 lookat,
        vec3   vup,
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio,
        double aperture,
        double focus_dist) 
    {
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2);
        double viewport_height = 2.0 * h;
        double viewport_width = aspect_ratio * viewport_height;

        w = unit_vector(lookfrom - lookat); //  指向视线来源方向
        u = unit_vector(cross(vup, w)); // 右向量
        v = cross(w, u); //上向量

        origin = lookfrom; // 视点
        horizontal = focus_dist * viewport_width * u; // 视口宽
        vertical = focus_dist * viewport_height * v; // 视口高
        lower_left_corner = origin - focus_dist * w - horizontal / 2 - vertical / 2; // 空间中视口位置左下角

        lens_radius = aperture / 2; // 棱镜厚度 用于模糊
    }

    ray get_ray(double s, double t) const 
    {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();

        return ray(
            origin + offset,
            lower_left_corner + s * horizontal + t * vertical - origin - offset
        );
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal, vertical;
    vec3 u, v, w;
    double lens_radius;
};
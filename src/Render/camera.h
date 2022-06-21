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

        w = unit_vector(lookfrom - lookat); //  ָ��������Դ����
        u = unit_vector(cross(vup, w)); // ������
        v = cross(w, u); //������

        origin = lookfrom; // �ӵ�
        horizontal = focus_dist * viewport_width * u; // �ӿڿ�
        vertical = focus_dist * viewport_height * v; // �ӿڸ�
        lower_left_corner = origin - focus_dist * w - horizontal / 2 - vertical / 2; // �ռ����ӿ�λ�����½�

        lens_radius = aperture / 2; // �⾵��� ����ģ��
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
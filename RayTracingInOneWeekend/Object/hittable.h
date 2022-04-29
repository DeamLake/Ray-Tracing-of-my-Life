#pragma once
#include "../Render/ray.h"
#include "../Utils/rtweekend.h"

class material;

struct hit_record {
    point3 p; // 交点
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t; // 光距离参数
    bool front_face;

    // 确定法线的正确方向 指向球体内或者球体外
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#pragma once
#include "rtweekend.h"
#include "texture.h"

struct hit_record;

class material
{
public:
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
    virtual color emitted(double u, double v, const point3& p) const {
        return color(0, 0, 0);
    }
};

// 理想散射材质
class lambertian : public material 
{
public:
    lambertian(const color& a) : albedo(make_shared<constant_texture>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}
    // 计算材质散射
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
    {
        // 基于法向量计算随机反射方向  位置为法向量末端单位球内随机点
        auto scatter_direction = rec.normal + random_unit_vector();

        // 捕获退化的散射方向  重置为法向量
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        // 记录反射光线和光衰减系数
        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo; // 材质
};

// 金属材质
class metal : public material 
{
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
    {
        // 计算放射向量
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);

        // 根据预设fuzz模糊程度大小   设置反射向量的随机偏移程度
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz;
};

// 可透视介质
class dielectric : public material 
{
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override 
    {
        // 无光衰减
        attenuation = color(1.0, 1.0, 1.0);
        // 介质折射率
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        // 折射率是否符合科学 
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        // 如果计算出的折射率不符合科学或者 石里克近似结果大于预期 就使用反射而不是折射
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

public:
    double ir; // Index of Refraction
private:
    static double reflectance(double cosine, double ref_idx) 
    {
        // 使用石里克近似计算反射系数 如果从一个陡峭的角度看窗户  它就变成了一面镜子
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<constant_texture>(c)) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<texture> emit;
};

class isotropic : public material {
public:
    isotropic(color c) : albedo(make_shared<constant_texture>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};
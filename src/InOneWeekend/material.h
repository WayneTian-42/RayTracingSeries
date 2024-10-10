#ifndef METAL_H
#define METAL_H

#include "InOneWeekend/ray.h"
#include "InOneWeekend/vec3.h"
#include "color.h"
#include "hittable.h"

class material
{
  public:
    virtual ~material() = default;

    virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scatterd) const
    {
        return false;
    }
};

class lambertian : public material
{
  public:
    lambertian(const color &albedo) : albedo(albedo)
    {
    }

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scatterd) const override
    {
        vec3 scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero())
        {
            scatter_direction = rec.normal;
        }
        scatterd = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

class metal : public material
{
  public:
    metal(const color &albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1)
    {
    }

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scatterd) const override
    {
        // 镜面反射
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        // 对反射进行模糊，fuzz是模糊因子
        reflected = unit(reflected) + fuzz * random_unit_vector();
        scatterd = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scatterd.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
  public:
    dielectric(double refraction_index) : refraction_index(refraction_index)
    {
    }

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scatterd) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        // 根据Snell's law计算入射介质和出射介质比值
        double r_i = rec.outward ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_in_dir = unit(r_in.direction());

        double cos_theta = std::fmin(1.0, dot(-unit_in_dir, rec.normal));
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool can_refract = sin_theta * r_i <= 1.0;
        vec3 direction;

        if (can_refract)
        {
            direction = refract(unit_in_dir, rec.normal, r_i);
        }
        else
        {
            direction = reflect(unit_in_dir, rec.normal);
        }

        scatterd = ray(rec.p, direction);
        return true;
    }

  private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;
};

#endif // !METAL_H

#ifndef METAL_H
#define METAL_H

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
    metal(const color &albedo) : albedo(albedo)
    {
    }

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scatterd) const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        scatterd = ray(rec.p, reflected);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

#endif // !METAL_H

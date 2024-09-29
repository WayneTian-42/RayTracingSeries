#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
  public:
    sphere(const point3 &center, double radius) : center(center), radius(std::fmax(0, radius))
    {
    }

    bool hit(const ray &r, double ray_tmin, double ray_tmax, hit_record &rec) const override
    {
        vec3 oc = center - r.origin();
        double a = r.direction().length_squared();
        // double b = -2.0 * dot(r.direction(), oc);
        double h = dot(r.direction(), oc);
        double c = oc.length_squared() - radius * radius;

        double discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        double sqrt_dis = std::sqrt(discriminant);
        double root = (h - sqrt_dis) / a;

        // 只使用一个变量记录根
        // ray_tmax <= root这种情况会发生吗？个人觉得不会，因为a恒正
        if (root <= ray_tmin || ray_tmax <= root)
        {
            root = (h + sqrt_dis) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);

        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

  private:
    point3 center;
    double radius;
};

#endif // !SPHERE_H

#ifndef SPHERE_H
#define SPHERE_H

#include "global.h"
#include "hittable.h"

class sphere : public hittable
{
  public:
    sphere(const point3 &center, double radius, shared_ptr<material> mat)
        : center(center), radius(std::fmax(0, radius)), mat(mat)
    {
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
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
        // ray_tmax < root这种情况会发生吗？个人觉得不会，因为a恒正
        //! 注意，不能使用contains，必须用surrounds，因为交点必须在(0,
        //! +infity)之间，不能等于二者任何一个，否则在求交点时会无限递归
        if (!ray_t.surrounds(root))
        {
            root = (h + sqrt_dis) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);

        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

  private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif // !SPHERE_H

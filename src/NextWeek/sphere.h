#ifndef SPHERE_H
#define SPHERE_H

#include "global.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include <cmath>

class sphere : public hittable
{
  public:
    sphere(const point3 &static_center, double radius, shared_ptr<material> mat)
        : move(static_center, vec3(0, 0, 0)), radius(std::fmax(0, radius)), mat(mat)
    {
        // 球体被包围在一个正方体中，rvec表示球心到正方体右上前方的顶点向量
        vec3 rvec = vec3(radius, radius, radius);
        bbox = aabb(static_center - rvec, static_center + rvec);
    }

    sphere(const point3 &start, const point3 &end, double radius, shared_ptr<material> mat)
        : move(start, end - start), radius(std::fmax(0, radius)), mat(mat)
    {
        // 球体被包围在一个正方体中，rvec表示球心到正方体右上前方的顶点向量
        vec3 rvec = vec3(radius, radius, radius);
        // 计算两次球体的bbox，然后求并集
        aabb bbox1 = aabb(start - rvec, start + rvec);
        aabb bbox2 = aabb(end - rvec, end + rvec);
        // aabb bbox1 = aabb(move.at(0) - rvec, move.at(0) + rvec);
        // aabb bbox2 = aabb(move.at(1) - rvec, move.at(1) + rvec);
        bbox = aabb(bbox1, bbox2);
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        point3 current_center = move.at(r.time());
        vec3 oc = current_center - r.origin();
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

        vec3 outward_normal = (rec.p - current_center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

  private:
    // point3 center;
    // 添加运动属性
    ray move;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;
};

#endif // !SPHERE_H

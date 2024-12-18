#ifndef QUAD_H
#define QUAD_H

#include "NextWeek/aabb.h"
#include "NextWeek/global.h"
#include "NextWeek/hittable_list.h"
#include "NextWeek/interval.h"
#include "NextWeek/material.h"
#include "NextWeek/vec3.h"
#include "hittable.h"
#include <cmath>
#include <memory>

class quad : public hittable
{
  public:
    quad(const point3 &Q, const vec3 &u, const vec3 &v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat)
    {
        vec3 n = cross(u, v);
        normal = unit(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        double denom = dot(normal, r.direction());

        if (std::fabs(denom) < 1e-8)
            return false;

        double t = (D - dot(normal, r.origin())) / denom;

        if (!ray_t.contains(t))
            return false;

        point3 intersection = r.at(t);
        vec3 planar_hitpt_vec = intersection - Q;

        double alpha = dot(w, cross(planar_hitpt_vec, v));
        double beta = dot(w, cross(u, planar_hitpt_vec));

        if (!is_interior(alpha, beta, rec))
            return false;

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record &rec) const
    {
        interval unit_interval = interval(0, 1);

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
        {
            return false;
        }

        rec.u = a;
        rec.v = b;
        return true;
    }

  private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    vec3 normal;
    double D;
    shared_ptr<material> mat;
    aabb bbox;
};

inline shared_ptr<hittable_list> box(const point3 &a, const point3 &b, shared_ptr<material> mat)
{
    auto sides = make_shared<hittable_list>();

    point3 min = point3(std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z()));
    point3 max = point3(std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z()));

    vec3 dx = vec3((max.x() - min.x()), 0, 0);
    vec3 dy = vec3(0, (max.y() - min.y()), 0);
    vec3 dz = vec3(0, 0, (max.z() - min.z()));

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));  // front，z轴最大，距离摄像机最近的面
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));  // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));  // bottom

    return sides;
}

#endif // !QUAD_H

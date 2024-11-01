#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "NextWeek/global.h"
#include "NextWeek/interval.h"
#include "NextWeek/vec3.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"
#include <memory>

class constant_medium : public hittable
{
  public:
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex)
        : boundary(boundary), neg_inv_density(-1.0 / density), phase_function(make_shared<isotropic>(tex))
    {
    }

    constant_medium(shared_ptr<hittable> boundary, double density, const color &albedo)
        : boundary(boundary), neg_inv_density(-1.0 / density), phase_function(make_shared<isotropic>(albedo))
    {
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        hit_record rec1, rec2;

        // 计算光线在介质中的传播距离
        // 不存在交点或者只存在一个交点说明光线没有进入介质
        if (!boundary->hit(r, interval::universe, rec1))
            return false;

        if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
            return false;

        if (rec1.t < ray_t.min)
            rec1.t = ray_t.min;
        if (rec2.t > ray_t.max)
            rec2.t = ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        // 计算传播距离
        double ray_length = r.direction().length();
        double distance_insid_medium = (rec2.t - rec1.t) * ray_length;

        // 随机生成光线和物体相交所需的传播距离
        double hit_distance = neg_inv_density * std::log(random_double());

        // 光线穿过物体，没有相交
        if (hit_distance > distance_insid_medium)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        rec.normal = random_unit_vector(); // 随机生成法向量
        rec.outward = true;                // 随机设定
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override
    {
        return boundary->bounding_box();
    }

  private:
    shared_ptr<hittable> boundary;
    double neg_inv_density;
    shared_ptr<material> phase_function;
};

#endif // !CONSTANT_MEDIUM_H

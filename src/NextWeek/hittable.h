#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "global.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
#include <cmath>
#include <memory>

class material;

class hit_record
{
  public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    // ture表示光线来自外部，false表示来自内部
    bool outward;
    // 纹理坐标
    double u, v;

    /**
     * @brief 设置交点法向量始终朝外
     *
     * @param r 光线
     * @param outward_normal 朝外的单位向量
     */
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        outward = dot(r.direction(), outward_normal) < 0;
        normal = outward ? outward_normal : -outward_normal;
    }
};

class hittable
{
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;

    virtual aabb bounding_box() const = 0;
};

class translate : public hittable
{
  public:
    translate(shared_ptr<hittable> object, const vec3 &offset) : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        if (!object->hit(offset_r, ray_t, rec))
            return false;

        rec.p += offset;

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

  private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};

class rotate_y : public hittable
{
  public:
    rotate_y(shared_ptr<hittable> object, double angle) : object(object)
    {
        double radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point3 min(infinity, infinity, infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    double x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    double y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    double z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    double new_x = x * cos_theta + z * sin_theta;
                    double new_z = -x * sin_theta + z * cos_theta;

                    point3 tmp(new_x, y, new_z);

                    for (int t = 0; t < 3; ++t)
                    {
                        min[t] = std::min(min[t], tmp[t]);
                        max[t] = std::max(max[t], tmp[t]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // 将光线从世界坐标转换到物体坐标

        point3 origin = point3((r.origin().x() * cos_theta - (r.origin().z() * sin_theta)), r.origin().y(),
                               (r.origin().x() * sin_theta + r.origin().z() * cos_theta));

        vec3 direction = vec3((r.direction().x() * cos_theta - (r.direction().z() * sin_theta)), r.direction().y(),
                              (r.direction().x() * sin_theta + r.direction().z() * cos_theta));

        ray rotate_r(origin, direction, r.time());

        if (!object->hit(rotate_r, ray_t, rec))
            return false;

        // 将交点从物体坐标转换为世界坐标

        rec.p = point3((rec.p.x() * cos_theta + rec.p.z() * sin_theta), rec.p.y(),
                       (-rec.p.x() * sin_theta + rec.p.z() * cos_theta));

        rec.normal = vec3((rec.normal.x() * cos_theta + rec.normal.z() * sin_theta), rec.normal.y(),
                          (-rec.normal.x() * sin_theta + rec.normal.z() * cos_theta));

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

  private:
    shared_ptr<hittable> object;
    double sin_theta, cos_theta;
    aabb bbox;
};

#endif // !HITTABLE_H

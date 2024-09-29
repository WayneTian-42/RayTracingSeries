#ifndef HITTABLE_H
#define HITTABLE_H

#include "global.h"
#include "interval.h"

class hit_record
{
  public:
    point3 p;
    vec3 normal;
    double t;
    // ture表示光线来自外部，false表示来自内部
    bool front_face;

    /**
     * @brief 设置交点法向量始终朝外
     *
     * @param r 光线
     * @param outward_normal 朝外的单位向量
     */
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;

  private:
};

#endif // !HITTABLE_H

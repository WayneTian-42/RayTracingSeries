#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.h"
#include "vec3.h"

class aabb
{
  public:
    interval x, y, z;

    aabb()
    {
    }

    aabb(const interval &x, const interval &y, const interval &z) : x(x), y(y), z(z)
    {
    }

    aabb(const point3 &a, const point3 &b)
    {
        x = a[0] < b[0] ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = a[1] < b[1] ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = a[2] < b[2] ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    aabb(const aabb &a, const aabb &b)
    {
        x = interval(a.x, b.x);
        y = interval(a.y, b.y);
        z = interval(a.z, b.z);
    }

    const interval &axis_interal(int n) const
    {
        if (n == 0)
        {
            return x;
        }
        else if (n == 1)
        {
            return y;
        }
        else
        {
            return z;
        }
    }

    /**
     * @brief 判断光线是否和bbox相交
     *
     * @param r 光线
     * @param ray_t
     * @return
     */
    bool hit(const ray &r, interval ray_t) const
    {
        const point3 &ray_orig = r.origin();
        const vec3 &dir = r.direction();

        // 在三个维度判断是否相交，且判断交集是否重叠
        // 只有当三个维度都相加且交集都重叠时，光线和box相交
        for (int axis = 0; axis < 3; ++axis)
        {
            const interval &ax = axis_interal(axis);
            const double d_inv = 1 / dir[axis];

            double t0 = (ax.min - ray_orig[axis]) * d_inv;
            double t1 = (ax.max - ray_orig[axis]) * d_inv;

            if (t0 < t1)
            {
                if (t0 > ray_t.min)
                    ray_t.min = t0;
                if (t1 < ray_t.max)
                    ray_t.max = t1;
            }
            else
            {

                if (t1 > ray_t.min)
                    ray_t.min = t1;
                if (t0 < ray_t.max)
                    ray_t.max = t0;
            }
            if (ray_t.min >= ray_t.max)
                return false;
        }
        return true;
    }

    int longest_axis() const
    {
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    int surface_area() const
    {
        return 2 * x.size() * y.size() + x.size() * z.size() + y.size() * z.size();
    }

    static const aabb empty, universe;

  private:
    void pad_to_minimums()
    {
        double delta = 0.0001;

        if (x.size() < delta)
        {
            x = x.expand(delta);
        }
        if (y.size() < delta)
        {
            y = y.expand(delta);
        }
        if (z.size() < delta)
        {
            z = z.expand(delta);
        }
    }
};

inline const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
inline const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

inline aabb operator+(const aabb &bbox, const vec3 &offset)
{
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

inline aabb operator+(const vec3 &offset, const aabb &bbox)
{
    return bbox + offset;
}
#endif // !AABB_H

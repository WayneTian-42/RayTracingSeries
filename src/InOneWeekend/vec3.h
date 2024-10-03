#ifndef VEC3_H
#define VEC3_H

#include "global.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>

class vec3
{
  public:
    double e[3];

    vec3() : e{0, 0, 0}
    {
    }

    vec3(double x, double y, double z) : e{x, y, z}
    {
    }

    double x() const
    {
        return e[0];
    }

    double y() const
    {
        return e[1];
    }

    double z() const
    {
        return e[2];
    }

    vec3 operator-() const
    {
        return vec3(-e[0], -e[1], -e[2]);
    }

    double operator[](int i) const
    {
        return e[i];
    }

    double &operator[](int i)
    {
        return e[i];
    }

    vec3 &operator+=(const vec3 &v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];

        return *this;
    }

    vec3 &operator-=(const vec3 &v)
    {
        // 如果重载减号时，不加const，下面代码报错，为什么？
        return *this += -v;
    }

    vec3 &operator*=(double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;

        return *this;
    }

    vec3 &operator/=(double t)
    {
        return *this *= 1 / t;
    }

    double length_squared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    double length() const
    {
        return std::sqrt(length_squared());
    }

    bool near_zero() const
    {
        double s = 1e-8;
        return (std::fabs(e[0] < s)) && (std::fabs(e[1] < s)) && (std::fabs(e[2] < s));
    }

    static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

// point3
using point3 = vec3;

inline std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2] << std::endl;
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(const double t, const vec3 &u)
{
    return vec3(u.e[0] * t, u.e[1] * t, u.e[2] * t);
}

inline vec3 operator*(const vec3 &u, const double t)
{
    return t * u;
}

inline vec3 operator/(const vec3 &u, double t)
{
    return (1.0 / t) * u;
}

inline double dot(const vec3 &u, const vec3 &v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1], u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit(const vec3 &u)
{
    return u / u.length();
}

inline vec3 random_unit_vector()
{
    while (true)
    {
        point3 p = vec3::random(-1, 1);
        double lensp = p.length_squared();

        // 防止len过小，导致向量趋近于正无穷
        // len <= 1可以去掉吗？ 不可以
        if (1e-160 < lensp && lensp <= 1)
        {
            return p / sqrt(lensp);
        }
    }
}

inline vec3 random_on_hemisphere(const vec3 &normal)
{
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
}

/**
 * @brief 根据入射光线和表面法线计算反射光线
 *
 * @param ray_in 入射光线（非单位向量）
 * @param out_normal 法线（单位向量，指向表面外部）
 * @return 反射光线（非单位向量）
 */
inline vec3 reflect(const vec3 &ray_in, const vec3 &out_normal)
{
    return ray_in - 2 * dot(ray_in, out_normal) * out_normal;
}

#endif // !VEC3_H

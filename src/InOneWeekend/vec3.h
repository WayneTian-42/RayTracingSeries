#ifndef VEC3_H
#define VEC3_H

#include <cmath>
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

#endif // !VEC3_H

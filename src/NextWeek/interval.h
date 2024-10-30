#ifndef INTERVAL_H
#define INTERVAL_H

#include "global.h"

class interval
{
  public:
    double min, max;

    // 默认是空区间
    interval() : min(+infinity), max(-infinity)
    {
    }

    interval(double min, double max) : min(min), max(max)
    {
    }

    interval(const interval &a, const interval &b)
    {
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    double size() const
    {
        return max - min;
    }

    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    double clamp(double x) const
    {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    interval expand(double delta) const
    {
        double padding = delta / 2;
        return interval(min - padding, max + padding);
    }

    static const interval empty, universe;
};

inline const interval interval::empty = interval(+infinity, -infinity);
inline const interval interval::universe = interval(-infinity, +infinity);

#endif // !INTERVAL_H

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

    static const interval empty, universe;
};

inline const interval interval::empty = interval(+infinity, -infinity);
inline const interval interval::universe = interval(-infinity, +infinity);

#endif // !INTERVAL_H
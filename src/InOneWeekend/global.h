#ifndef GLOBAL_H
#define GLOBAL_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degree)
{
    return degree * pi / 180.0;
}

/**
 * @brief 返回[0, 1)之间的随机数
 *
 * @return [0, 1)之间的随机数
 */
inline double random_double()
{
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
    return min + (max - min) * random_double();
}

// Common Headers

#include "ray.h"
#include "vec3.h"

#endif // !GLOBAL_H

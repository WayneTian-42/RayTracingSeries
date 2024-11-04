#ifndef GLOBAL_H
#define GLOBAL_H

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <random>

// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Initialize the thread-local RNG and distribution
static thread_local std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<double> distribution(0.0, 1.0);

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
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
    // return rand_r(NULL) / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max)
{
    return int(random_double(min, max + 1));
}

// Common Headers

#endif // !GLOBAL_H

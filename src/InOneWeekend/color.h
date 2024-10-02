#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

#include <ostream>

using color = vec3;

inline void write_color(std::ostream &out, const color &pixel_color)
{
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // 将[0, 1]转换为[0, 255]，不乘以256是为了防止rgb取值为1时超过255
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    out << rbyte << " " << gbyte << " " << bbyte << std::endl;
}

#endif // !COLOR_H

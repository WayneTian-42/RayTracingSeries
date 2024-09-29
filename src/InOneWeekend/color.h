#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <ostream>

using color = vec3;

inline void write_color(std::ostream &out, const color &pixel_color)
{
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // 将[0, 1]转换为[0, 255]，不乘以256是为了防止rgb取值为1时超过255
    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out << rbyte << " " << gbyte << " " << bbyte << std::endl;
}

#endif // !COLOR_H

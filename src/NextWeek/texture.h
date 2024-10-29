#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "global.h"
#include "vec3.h"

class texture
{
  public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture
{
  public:
    solid_color(const color &albedo) : albedo(albedo)
    {
    }

    solid_color(double red, double green, double blue) : albedo(color(red, green, blue))
    {
    }

    color value(double u, double v, const point3 &p) const override
    {
        return albedo;
    }

  private:
    color albedo;
};

class checker_texture : public texture
{
  public:
    checker_texture(double scale, shared_ptr<texture> even_tex, shared_ptr<texture> odd_tex)
        : inv_scale(1.0 / scale), even_tex(even_tex), odd_tex(odd_tex)
    {
    }

    checker_texture(double scale, const color &c1, const color &c2)
        : checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2))
    {
    }

    color value(double u, double v, const point3 &p) const override
    {
        int x_int = int(std::floor(inv_scale * p.x()));
        int y_int = int(std::floor(inv_scale * p.y()));
        int z_int = int(std::floor(inv_scale * p.z()));

        bool is_even = (x_int + y_int + z_int) % 2 == 0;

        return is_even ? even_tex->value(u, v, p) : odd_tex->value(u, v, p);
    }

  private:
    double inv_scale;
    shared_ptr<texture> even_tex, odd_tex;
};

#endif // !TEXTURE_H

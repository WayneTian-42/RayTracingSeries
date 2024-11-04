#ifndef TEXTURE_H
#define TEXTURE_H

#include "NextWeek/interval.h"
#include "NextWeek/perlin.h"
#include "NextWeek/rtw_stb_image.h"
#include "color.h"
#include "global.h"
#include "vec3.h"
#include <cmath>

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

class image_texture : public texture
{
  public:
    image_texture(const char *filename) : image(filename)
    {
    }

    color value(double u, double v, const point3 &p) const override
    {
        if (image.height() <= 0)
            return color(0, 1, 1);

        u = interval(0, 1).clamp(u);
        //! 由于v从[1, 0]表示从上到下，所以这里应该反转v
        v = 1 - interval(0, 1).clamp(v);

        int i = int(u * image.width());
        int j = int(v * image.height());
        auto pixel = image.pixel_data(i, j);

        double color_scale = 1.0 / 255.0;
        return color_scale * color(pixel[0], pixel[1], pixel[2]);
    }

  private:
    rtw_image image;
};

class noise_texture : public texture
{
  public:
    noise_texture(double scale) : scale(scale)
    {
    }
    color value(double u, double v, const point3 &p) const override
    {
        // return color(1, 1, 1) * 0.5 * (noise.turb(p, 7));
        return color(1, 1, 1) * 0.5 * (1.0 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }

  private:
    perlin noise;
    double scale;
};

#endif // !TEXTURE_H

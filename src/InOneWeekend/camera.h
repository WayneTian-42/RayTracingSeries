#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "vec3.h"

class camera
{
  public:
    double aspect_ratio = 1.0; // 默认宽高比
    int image_width = 100;     // 默认图片宽度为100像素
    void render(const hittable &world)
    {
        initialize();

        std::cout << "P3" << std::endl;
        std::cout << image_width << " " << image_height << std::endl;
        std::cout << "255" << std::endl;

        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanline remaining: " << (image_height - j) << " " << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
                // 计算像素中心位置
                // point3 pixel_center = pixel00_loc + j * pixel_delta_v + i * pixel_delta_u;
                point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);

                // 计算光线方向
                vec3 ray_direction = pixel_center - center;

                ray r(center, ray_direction);

                color pixel_color = ray_color(r, world);

                write_color(std::cout, pixel_color);
            }
        }

        std::clog << "\rDone.                 " << std::endl;
    }

  private:
    int image_height;   // 图片高度
    point3 center;      // 相机中心
    point3 pixel00_loc; // 左上角像素位置
    vec3 pixel_delta_u; // 横向像素间隔
    vec3 pixel_delta_v; // 纵向像素间隔

    void initialize()
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0, 0, 0);

        // 设置摄像机属性

        double focal_length = 1.0;

        // 设置viewport属性

        double viewport_height = 2.0;
        double viewport_width = viewport_height * (double(image_width) / image_height);

        // 计算viewport的横纵方向
        vec3 viewport_u = vec3(viewport_width, 0, 0);
        vec3 viewport_v = vec3(0, -viewport_height, 0);

        // vec3 pixel_delta_u = unit(viewport_u);
        // vec3 pixel_delta_v = unit(viewport_v);

        // 在viewport上放置足够多的像素，形成一个image
        // 将viewport拍摄成一个照片即iamge，所以除以image的长宽而不是viewport的长宽
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // 计算viewport左上角以及第一个pixel位置
        point3 viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + pixel_delta_u / 2 + pixel_delta_v / 2;
    }

    color ray_color(const ray &r, const hittable &world) const
    {
        hit_record rec;
        if (world.hit(r, interval(0, infinity), rec))
        {
            return 0.5 * (rec.normal + color(1, 1, 1));
        }
        vec3 unit_direction = unit(r.direction());
        double t = 0.5 * (unit_direction.y() + 1.0);
        color start_color = color(1.0, 1.0, 1.0);
        color end_color = color(0.5, 0.7, 1.0);
        return (1.0 - t) * start_color + t * end_color;
    }
};

#endif // !CAMERA_H

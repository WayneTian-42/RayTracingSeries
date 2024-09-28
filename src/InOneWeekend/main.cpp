#include "color.h"
#include "ray.h"
#include "vec3.h"
#include <iostream>
#include <ostream>

color ray_color(const ray &r)
{
    vec3 unit_direction = unit(r.direction());
    double t = 0.5 * (unit_direction.y() + 1.0);
    color start_color = color(1.0, 1.0, 1.0);
    color end_color = color(0.5, 0.7, 1.0);
    return (1.0 - t) * start_color + t * end_color;
}

int main()
{
    // Image

    // 设置宽度、宽高比并计算高度
    int image_width = 400;
    double aspect_ratio = 16.0 / 9.0;

    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height <= 1) ? 1 : image_height;

    // 设置viewport属性

    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width) / image_height);

    // 设置摄像机属性

    double focal_length = 1.0;
    point3 camera_center = point3(0, 0, 0);

    // 计算viewport的横纵方向
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // vec3 pixel_delta_u = unit(viewport_u);
    // vec3 pixel_delta_v = unit(viewport_v);

    // 在viewport上放置足够多的像素，形成一个image
    // 将viewport拍摄成一个照片即iamge，所以除以image的长宽而不是viewport的长宽
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // 计算viewport左上角以及第一个pixel位置
    point3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    point3 pixel00_loc = viewport_upper_left + pixel_delta_u / 2 + pixel_delta_v / 2;

    // Render

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
            vec3 ray_direction = pixel_center - camera_center;

            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);

            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                 " << std::endl;
}
// int main()
// {
//
//     // Image
//
//     auto aspect_ratio = 16.0 / 9.0;
//     int image_width = 400;
//
//     // Calculate the image height, and ensure that it's at least 1.
//     int image_height = int(image_width / aspect_ratio);
//     image_height = (image_height < 1) ? 1 : image_height;
//
//     // Camera
//
//     auto focal_length = 1.0;
//     auto viewport_height = 2.0;
//     auto viewport_width = viewport_height * (double(image_width) / image_height);
//     auto camera_center = point3(0, 0, 0);
//
//     // Calculate the vectors across the horizontal and down the vertical viewport edges.
//     auto viewport_u = vec3(viewport_width, 0, 0);
//     auto viewport_v = vec3(0, -viewport_height, 0);
//
//     // Calculate the horizontal and vertical delta vectors from pixel to pixel.
//     auto pixel_delta_u = viewport_u / image_width;
//     auto pixel_delta_v = viewport_v / image_height;
//
//     // Calculate the location of the upper left pixel.
//     auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
//     auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
//
//     // Render
//
//     std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
//
//     for (int j = 0; j < image_height; j++)
//     {
//         std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
//         for (int i = 0; i < image_width; i++)
//         {
//             auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
//             auto ray_direction = pixel_center - camera_center;
//             ray r(camera_center, ray_direction);
//
//             color pixel_color = ray_color(r);
//             write_color(std::cout, pixel_color);
//         }
//     }
//
//     std::clog << "\rDone.                 \n";
// }

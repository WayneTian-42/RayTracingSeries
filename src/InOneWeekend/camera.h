#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "global.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"

class camera
{
  public:
    double aspect_ratio = 1.0;  // 默认宽高比
    int image_width = 100;      // 默认图片宽度为100像素
    int samples_per_pixel = 10; // 每个像素采样次数
    int max_depth = 10;         // 每次递归最大深度
    double vfov = 90;           // 垂直方向的fov

    point3 lookfrom = point3(0, 0, 0); // 相机所在位置
    point3 lookat = point3(0, 0, -1);  // 相机看向的位置（确定相机朝向）
    //! 注意：vup不是相机相对正上方，只是用来和相机看向的方向一起确定一个平面，从而计算出相机的x轴
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0; // 光线穿过像素时角度变化范围
    double focus_dis = 10;    // 相机到完美对焦平面的距离

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
                color pixel_color;
                for (int k = 0; k < samples_per_pixel; ++k)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                write_color(std::cout, pixel_color * pixel_sample_scale);
            }
        }

        std::clog << "\rDone.                 " << std::endl;
    }

  private:
    int image_height;          // 图片高度
    point3 center;             // 相机中心
    point3 pixel00_loc;        // 左上角像素位置
    vec3 pixel_delta_u;        // 横向像素间隔
    vec3 pixel_delta_v;        // 纵向像素间隔
    double pixel_sample_scale; // 每一次采样所占比例

    vec3 u, v, w;        // 相机坐标系下的基向量
    vec3 defocus_disk_u; // 散焦时水平方向向量
    vec3 defocus_disk_v; // 散焦时垂直方向向量

    void initialize()
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        pixel_sample_scale = 1.0 / samples_per_pixel;

        // 设置摄像机属性

        center = lookfrom;

        // double focal_length = (lookat - lookfrom).length();

        // 设置viewport属性

        // double viewport_height = 2.0;
        double theta = degrees_to_radians(vfov);
        double h = std::tan(theta / 2);
        double viewport_height = 2 * focus_dis * h;
        double viewport_width = viewport_height * (double(image_width) / image_height);

        // 计算相机坐标系的基向量
        w = unit(lookfrom - lookat); // 观察方向是z轴负方向
        u = unit(cross(vup, w));
        v = cross(w, u);

        // 计算viewport的横纵方向
        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v;

        // vec3 pixel_delta_u = unit(viewport_u);
        // vec3 pixel_delta_v = unit(viewport_v);

        // 在viewport上放置足够多的像素，形成一个image
        // 将viewport拍摄成一个照片即iamge，所以除以image的长宽而不是viewport的长宽
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // 计算viewport左上角以及第一个pixel位置
        point3 viewport_upper_left = center - focus_dis * w - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + pixel_delta_u / 2 + pixel_delta_v / 2;

        // 计算相机散焦环基向量
        double defocus_radius = focus_dis * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;
    }

    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        if (depth <= 0)
            return color(0, 0, 0);
        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec))
        {
            // return 0.5 * (rec.normal + vec3(1, 1, 1));

            // 生成反射光线的方向
            // vec3 direction = random_on_hemisphere(rec.normal);

            // lambertian 模型
            // vec3 direction = rec.normal + random_unit_vector();
            // // 吸收周围颜色的50%，反射剩余的50%，呈现一种灰色
            // // 此处0.5与之前不同，之前是为了保证颜色取值在[0, 1]之间
            // return 0.5 * ray_color(ray(rec.p, direction), depth - 1, world);

            // 添加材质
            ray scattered;
            // 此处的attenuation是衰退率，即经过反射后仍然保留的颜色所占比例，不是反射的颜色
            // 不同材质的albedo也是反射率的含义，而非颜色
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
            {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return color(0, 0, 0);
        }
        vec3 unit_direction = unit(r.direction());
        double t = 0.5 * (unit_direction.y() + 1.0);
        color start_color = color(1.0, 1.0, 1.0);
        color end_color = color(0.5, 0.7, 1.0);
        return (1.0 - t) * start_color + t * end_color;
    }

    ray get_ray(int i, int j)
    {
        vec3 offset = sample_squre();
        point3 pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        point3 ray_origin = (defocus_angle <= 0) ? center : sample_defocus_disk();
        vec3 ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    /**
     * @brief 生成[-0.5, -0.5]到[+0.5, +0.5]正方形区域之间的随机点
     *
     * @return [-0.5, -0.5]到[+0.5, +0.5]正方形区域之间的随机点
     */
    point3 sample_squre() const
    {
        return point3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 sample_defocus_disk() const
    {
        point3 p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};

#endif // !CAMERA_H

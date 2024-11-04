#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "dynamic_thread_pool.h"
#include "global.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>
class camera

{
  public:
    double aspect_ratio = 1.0;  // 默认宽高比
    int image_width = 100;      // 默认图片宽度为100像素
    int samples_per_pixel = 10; // 每个像素采样次数
    int max_depth = 10;         // 每次递归最大深度
    double vfov = 90;           // 垂直方向的fov
    color background;           // 场景背景颜色，默认是黑色

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

        RenderScene(world, 0, image_height, 0, image_width);

        std::clog << "\rDone.                 " << std::endl;

        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                write_color(std::cout, framebuffer[j * image_width + i]);
            }
        }
    }
    void ThreadRender(const hittable &world)
    {
        initialize();

        std::cout << "P3" << std::endl;
        std::cout << image_width << " " << image_height << std::endl;
        std::cout << "255" << std::endl;

        // 多线程
        // 创建一个线程向量，用于存储所有线程
        // std::vector<std::thread> threads(hardware_concurrency);
        std::mutex mtx; // 创建互斥量

        // 计算需要创建的线程数量，这里我们使用硬件并行度
        // int hardware_concurrency = std::thread::hardware_concurrency();
        int hardware_concurrency = 10;
        // std::vector<std::unique_ptr<std::thread>> threads(hardware_concurrency);
        std::vector<std::optional<std::thread>> threads(hardware_concurrency);
        // threads.reserve(hardware_concurrency);
        // threads.resize(hardware_concurrency);
        if (hardware_concurrency <= 0)
            hardware_concurrency = 1;

        std::clog << "Threads: " << hardware_concurrency << std::endl;
        // 每个线程计算一部分任务
        int task_size = image_height / hardware_concurrency;
        // int lines = 0;
        std::atomic<int> lines(0);

        for (int j = 0; j < hardware_concurrency; ++j)
        {
            // 计算线程应该开始和结束的索引
            int start = j * task_size;
            int end = (j == hardware_concurrency - 1) ? image_height : (j + 1) * task_size;
            // 创建线程并启动
            threads[j].emplace([&world, start, end, this]() { RenderScene(world, start, end, 0, image_width); });
        }
        // 等待所有线程完成
        for (auto &t : threads)
        {
            t->join();
        }

        std::clog << "\rDone.                 " << std::endl;
        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                write_color(std::cout, framebuffer[j * image_width + i]);
            }
        }
    }

    /**
     * @brief 采用线程池进行多线程渲染，每个线程负责固定大小的方格区域渲染，默认是12*12的像素区域
     *
     * @param world
     */
    void ThreadPoolRender(const hittable &world, int chunk_size = 12)
    {
        initialize();

        std::cout << "P3" << std::endl;
        std::cout << image_width << " " << image_height << std::endl;
        std::cout << "255" << std::endl;

        std::mutex mtx; // 创建互斥量

        int hardware_concurrency = std::thread::hardware_concurrency();
        // int hardware_concurrency = 10;
        if (hardware_concurrency <= 0)
            hardware_concurrency = 1;

        std::clog << "Threads: " << hardware_concurrency << std::endl;

        std::atomic<int> lines(0);
        DynamicThreadPool thread_pool(hardware_concurrency);
        std::vector<std::future<void>> futures;

        // 设置一个合适的任务块大小（例如一次处理10行）
        // int chunk_size = std::max(image_height / (4 * hardware_concurrency), 1);
        chunk_size = std::min(image_height / hardware_concurrency, chunk_size);
        if (chunk_size <= 0)
            chunk_size = 1;
        std::clog << "Chunk size: " << chunk_size << std::endl;

        for (int j = 0; j < image_height; j += chunk_size)
        // for (int j = 0; j < hardware_concurrency; ++j)
        {
            for (int i = 0; i < image_width; i += chunk_size)
            // for (int i = 0; i < 1; i += chunk_size)
            {

                int end_y = std::min(j + chunk_size, image_height);
                int end_x = std::min(i + chunk_size, image_width);
                futures.emplace_back(thread_pool.enqueue(
                    [&world, j, end_y, i, end_x, this]() { RenderScene(world, j, end_y, i, end_x); }));
            }
        }

        for (auto &fut : futures)
        {
            fut.get();
        }

        std::clog << "\rDone.                 " << std::endl;
        // 输出渲染的结果
        for (int j = 0; j < image_height; ++j)
        {
            for (int i = 0; i < image_width; ++i)
            {
                write_color(std::cout, framebuffer[j * image_width + i]);
            }
        }
    }

  private:
    int image_height;          // 图片高度
    point3 center;             // 相机中心
    point3 pixel00_loc;        // 左上角像素位置
    vec3 pixel_delta_u;        // 横向像素间隔
    vec3 pixel_delta_v;        // 纵向像素间隔
    double pixel_sample_scale; // 每一次采样所占比例
    int sqrt_spp;              // 采样数平方根
    double recip_sqrt_spp;     // 采样数平方根倒数

    vec3 u, v, w;        // 相机坐标系下的基向量
    vec3 defocus_disk_u; // 散焦时水平方向向量
    vec3 defocus_disk_v; // 散焦时垂直方向向量
    std::atomic<int> lines;
    std::vector<color> framebuffer;

    void initialize()
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        // pixel_sample_scale = 1.0 / samples_per_pixel;
        sqrt_spp = std::sqrt(samples_per_pixel);
        pixel_sample_scale = 1.0 / (sqrt_spp * sqrt_spp);
        recip_sqrt_spp = 1.0 / sqrt_spp;

        framebuffer.resize(image_height * image_width);
        lines = 0;

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

            // 自发光
            color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

            if (rec.mat->scatter(r, rec, attenuation, scattered))
            {
                // 反射光
                color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);
                return color_from_emission + color_from_scatter;
            }
            return color_from_emission;
        }
        // 天空颜色
        // vec3 unit_direction = unit(r.direction());
        // double t = 0.5 * (unit_direction.y() + 1.0);
        // color start_color = color(1.0, 1.0, 1.0);
        // color end_color = color(0.5, 0.7, 1.0);
        // return (1.0 - t) * start_color + t * end_color;

        // 背景颜色
        return background;
    }

    ray get_ray(int i, int j, int si, int sj)
    {
        vec3 offset = sample_squre();
        // vec3 offset = sample_square_stratified(si, sj);
        point3 pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        // 根据角度判断是否焦散，不判断也可以
        point3 ray_origin = (defocus_angle <= 0) ? center : sample_defocus_disk();
        // point3 ray_origin = sample_defocus_disk();
        vec3 ray_direction = pixel_sample - ray_origin;
        double ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    point3 sample_square_stratified(int si, int sj) const
    {
        double px = ((si + random_double()) * recip_sqrt_spp) - 0.5;
        double py = ((sj + random_double()) * recip_sqrt_spp) - 0.5;

        return point3(px, py, 0);
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

    void RenderScene(const hittable &world, int start_y, int end_y, int start_x, int end_x)
    {
        for (int j = start_y; j < end_y; ++j)
        {
            if (start_x == 0)
            {
                ++lines;
                std::clog << "\rScanline remaining: " << (image_height - lines) << " " << std::flush;
            }
            for (int i = start_x; i < end_x; ++i)
            {
                color pixel_color;
                for (int si = 0; si < sqrt_spp; ++si)
                {
                    for (int sj = 0; sj < sqrt_spp; ++sj)
                    {
                        ray r = get_ray(i, j, si, sj);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                }
                framebuffer[j * image_width + i] = pixel_color * pixel_sample_scale;
                // write_color(std::cout, pixel_color * pixel_sample_scale);
            }
        }
    }
};

#endif // !CAMERA_H

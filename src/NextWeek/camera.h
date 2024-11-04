#ifndef CAMERA_H
#define CAMERA_H

#include "NextWeek/dynamic_thread_pool.h"
#include "color.h"
#include "global.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include <algorithm>
#include <atomic>
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

        std::vector<color> framebuffer(image_height * image_width);
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

                framebuffer[j * image_width + i] = pixel_color * pixel_sample_scale;
                // write_color(std::cout, pixel_color * pixel_sample_scale);
            }
        }

        std::clog << "\rDone.                 " << std::endl;
    }
    void ThreadRender(const hittable &world)
    {
        initialize();

        std::vector<color> framebuffer(image_height * image_width);
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
            threads[j].emplace([start, end, this, &world, &framebuffer, &lines]() {
                for (int p = start; p < end; ++p)
                {
                    // lines++;
                    // std::clog << "\rScanline remaining: " << (image_height - lines) << " " << std::flush;
                    for (uint32_t q = 0; q < image_width; ++q)
                    {
                        color pixel_color;
                        for (int k = 0; k < samples_per_pixel; ++k)
                        {
                            ray r = get_ray(q, p);
                            // ray_color(r, max_depth, world);
                            pixel_color += ray_color(r, max_depth, world);
                        }
                        // framebuffer[p * image_width + q] = pixel_color * pixel_sample_scale;
                    }
                }
            });
        }
        // 等待所有线程完成
        for (auto &t : threads)
        {
            t->join();
        }

        std::clog << "\rDone.                 " << std::endl;
        // for (int j = 0; j < image_height; ++j)
        // {
        //     for (int i = 0; i < image_width; ++i)
        //     {
        //         write_color(std::cout, framebuffer[j * image_width + i]);
        //     }
        // }
    }

    /**
     * @brief 采用线程池进行多线程渲染，每个线程负责固定大小的方格区域渲染，默认是12*12的像素区域
     *
     * @param world
     */
    void ThreadPoolRender(const hittable &world, int chunk_size = 12)
    {
        initialize();

        std::vector<color> framebuffer(image_height * image_width);

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

        auto sample_ray = [this](int start_y, int end_y, int start_x, int end_x, const hittable &world,
                                 std::atomic<int> &lines, std::vector<color> &framebuffer) {
            for (int y = start_y; y < end_y; ++y)
            // for (int y = start_y; y < end_y; y += 10)
            {
                if (start_x == 0)
                {
                    lines++;
                    std::clog << "\rScanline remaining: " << (image_height - lines) << " " << std::flush;
                }
                for (int x = start_x; x < end_x; ++x)
                {
                    color pixel_color;
                    for (int k = 0; k < samples_per_pixel; ++k)
                    {
                        ray r = get_ray(x, y);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    framebuffer[y * image_width + x] = pixel_color * pixel_sample_scale;
                }
            }
        };

        for (int j = 0; j < image_height; j += chunk_size)
        // for (int j = 0; j < hardware_concurrency; ++j)
        {
            for (int i = 0; i < image_width; i += chunk_size)
            // for (int i = 0; i < 1; i += chunk_size)
            {

                int end_y = std::min(j + chunk_size, image_height);
                int end_x = std::min(i + chunk_size, image_width);
                futures.emplace_back(thread_pool.enqueue(sample_ray, j, end_y, i, end_x, std::ref(world),
                                                         // futures.emplace_back(thread_pool.enqueue(sample_ray, j,
                                                         // image_height, i, image_width, std::ref(world),
                                                         std::ref(lines), std::ref(framebuffer)));
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
    // 为每个线程创建独立的缓存行对齐的buffer
    struct alignas(64) ThreadLocalBuffer
    {
        std::vector<color> local_buffer;
        char padding[64 - sizeof(std::vector<color>) % 64];
    };

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

    color ray_color2(const ray &r, int depth, const hittable &world) const
    {
        // 使用迭代代替递归
        color final_color(0, 0, 0);
        color current_attenuation(1, 1, 1);
        ray current_ray = r;

        for (int i = depth; i > 0; --i)
        {
            hit_record rec;
            if (!world.hit(current_ray, interval(0.001, infinity), rec))
                return final_color + current_attenuation * background;

            color emission = rec.mat->emitted(rec.u, rec.v, rec.p);
            final_color += current_attenuation * emission;

            ray scattered;
            color attenuation;
            if (!rec.mat->scatter(current_ray, rec, attenuation, scattered))
                break;

            current_attenuation = current_attenuation * attenuation;
            current_ray = scattered;

            // 俄罗斯轮盘赌终止
            float p = std::max(current_attenuation.x(), std::max(current_attenuation.y(), current_attenuation.z()));
            if (random_double() > p)
                break;
            current_attenuation *= 1.0f / p;
        }

        return final_color;
    }

    ray get_ray(int i, int j)
    {
        vec3 offset = sample_squre();
        point3 pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        // 根据角度判断是否焦散，不判断也可以
        point3 ray_origin = (defocus_angle <= 0) ? center : sample_defocus_disk();
        // point3 ray_origin = sample_defocus_disk();
        vec3 ray_direction = pixel_sample - ray_origin;
        double ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
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

    struct ThreadStats
    {
        std::atomic<uint64_t> pixels_processed{0}; // 处理的像素数
        std::atomic<uint64_t> rays_traced{0};      // 追踪的射线数
        std::atomic<uint64_t> compute_time_ns{0};  // 计算时间(纳秒)

        // 用于存储更细粒度的时间分布
        std::vector<uint64_t> ray_compute_times; // 每个射线的计算时间
        std::mutex times_mutex;                  // 保护 ray_compute_times

        void add_ray_time(uint64_t ns)
        {
            std::lock_guard<std::mutex> lock(times_mutex);
            ray_compute_times.push_back(ns);
        }
    };

    class PerformanceMonitor
    {
      public:
        PerformanceMonitor(int num_threads) : thread_stats(num_threads)
        {
        }

        ThreadStats &get_stats(int thread_id)
        {
            return thread_stats[thread_id];
        }

        void print_summary()
        {
            for (size_t i = 0; i < thread_stats.size(); ++i)
            {
                auto &stats = thread_stats[i];
                std::cout << "\nThread " << i << " statistics:\n"
                          << "  Pixels processed: " << stats.pixels_processed
                          << "\n  Rays traced: " << stats.rays_traced
                          << "\n  Total compute time: " << stats.compute_time_ns / 1e9 << "s"
                          << "\n  Average time per ray: "
                          << (stats.rays_traced ? (stats.compute_time_ns / stats.rays_traced) : 0) << "ns\n";

                // 计算射线时间的统计信息
                if (!stats.ray_compute_times.empty())
                {
                    std::vector<uint64_t> times;
                    {
                        std::lock_guard<std::mutex> lock(stats.times_mutex);
                        times = stats.ray_compute_times;
                    }
                    std::sort(times.begin(), times.end());
                    std::cout << "  Ray time distribution:\n"
                              << "    Min: " << times.front() << "ns\n"
                              << "    Max: " << times.back() << "ns\n"
                              << "    Median: " << times[times.size() / 2] << "ns\n";
                }
            }
        }

      private:
        std::vector<ThreadStats> thread_stats;
    };
};

#endif // !CAMERA_H

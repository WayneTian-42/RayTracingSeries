#include "camera.h"
#include "global.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

int main()
{
    // camera
    camera cam;

    // 设置宽度、宽高比并计算高度
    int image_width = 400;
    double aspect_ratio = 16.0 / 9.0;

    // 设置场景物体
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(point3(0, 0, -1.2), 0.5, material_right));
    world.add(make_shared<sphere>(point3(-1, 0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(1, 0, -1.2), 0.5, material_right));

    cam.image_width = image_width;
    cam.aspect_ratio = aspect_ratio;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.render(world);
}

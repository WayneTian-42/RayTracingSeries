#include "camera.h"
#include "global.h"
#include "hittable_list.h"
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
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    cam.image_width = image_width;
    cam.aspect_ratio = aspect_ratio;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.render(world);
}

#include <iostream>
#include <ostream>

int main()
{
    // Image

    int image_width = 256;
    int image_height = 256;

    // Render

    std::cout << "P3" << std::endl;
    std::cout << image_width << " " << image_height << std::endl;
    std::cout << "255" << std::endl;

    for (int j = 0; j < image_height; ++j)
    {
        std::clog << "\rScanline remaining: " << (image_height - j) << " " << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            double r = i * 1.0 / (image_width - 1);
            double g = j * 1.0 / (image_height - 1);
            double b = 0.0;

            int ir = int(256 * r);
            int ig = int(256 * g);
            int ib = int(256 * b);

            std::cout << ir << " " << ig << " " << ib << std::endl;
        }
    }

    std::clog << "\rDone.                 " << std::endl;
}

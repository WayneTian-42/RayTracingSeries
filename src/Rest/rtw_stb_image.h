#ifndef RTW_STB_IMAGE_H
#define RTW_STB_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "external/stb/stb_image.h"

#include <cstdlib>
#include <iostream>

class rtw_image
{
  public:
    rtw_image()
    {
    }

    rtw_image(const char *image_filename)
    {
        auto filename = std::string(image_filename);
        auto image_dir = getenv("RTW_IMAGES");

        // Hunt for the image file in some likely locations.
        if (image_dir && load(std::string(image_dir) + "/" + image_filename))
            return;
        if (load(filename))
            return;
        if (load("img/" + filename))
            return;
        if (load("../img/" + filename))
            return;
        if (load("../../img/" + filename))
            return;
        if (load("../../../img/" + filename))
            return;
        if (load("../../../../img/" + filename))
            return;
        if (load("../../../../../img/" + filename))
            return;
        if (load("../../../../../../img/" + filename))
            return;

        std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
    }

    ~rtw_image()
    {
        delete[] bdata;
        STBI_FREE(fdata);
    }

    bool load(const std::string &filename)
    {
        int n = bytes_per_pixel;
        fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);

        if (fdata == nullptr)
            return false;

        bytes_per_scanline = image_width * bytes_per_pixel;
        convert_to_bytes();
        return true;
    }

    int width() const
    {
        return (fdata == nullptr) ? 0 : image_width;
    }

    int height() const
    {
        return (fdata == nullptr) ? 0 : image_height;
    }

    const unsigned char *pixel_data(int x, int y) const
    {
        static unsigned char magenta[] = {255, 0, 255};

        if (bdata == nullptr)
            return magenta;

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);

        return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
    }

  private:
    /**
     * @brief 将给定值限制在[low, high)范围内
     *
     * @param x 给定值
     * @param low
     * @param high
     * @return
     */
    static int clamp(int x, int low, int high)
    {
        if (x < low)
            return low;
        if (x < high)
            return x;
        return high - 1;
    }

    static unsigned char float_to_byte(float value)
    {
        if (value <= 0.0)
            return 0;
        if (1.0 <= value)
            return 255;
        return static_cast<unsigned char>(256.0 * value);
    }

    void convert_to_bytes()
    {
        int total_bytes = image_width * image_height * bytes_per_pixel;
        bdata = new unsigned char[total_bytes];

        auto *bptr = bdata;
        auto *fptr = fdata;

        for (int i = 0; i < total_bytes; ++i, fptr++, bptr++)
        {
            *bptr = float_to_byte(*fptr);
        }
    }

  private:
    const int bytes_per_pixel = 3;
    // 浮点数形式线性空间存储图片
    float *fdata = nullptr;
    // 8bit存储图片
    unsigned char *bdata = nullptr;
    int image_width = 0;
    int image_height = 0;
    // 每一行的字节数
    int bytes_per_scanline = 0;
};

#endif // !RTW_STB_IMAGE_H

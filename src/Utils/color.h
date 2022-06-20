#pragma once
#include<vector>
#include "vec3.h"
#include "svpng.inc"

class OutputHelper {
public:
    OutputHelper(const int width, const int height, const char* name) 
        :image_width(width), image_height(height), image_name(name) , idx(0)
    {
        image_data = std::vector<unsigned char>(image_height * image_width * 3);
    }

    ~OutputHelper()
    {
        FILE* imageFile;
        char imagePath[50] = "../";
        strcat_s(imagePath, image_name);
        fopen_s(&imageFile, imagePath, "wb");
        if (imageFile == NULL) {
            perror("ERROR: Cannot open output file");
            exit(EXIT_FAILURE);// 不要让异常跑出析构函数  会造成析构函数循环调用
        }
        svpng(imageFile, image_width, image_height, image_data.data(), 0);
        fclose(imageFile);
    }

    void write_color(const color& pixel_color, int samples_per_pixel) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // 将多次采样获得的颜色值对采样次数做平均  并做gamma校正
        auto scale = 1.0 / samples_per_pixel;
        r = sqrt(scale * r);
        g = sqrt(scale * g);
        b = sqrt(scale * b);

        unsigned char cr = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
        unsigned char cg = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
        unsigned char cb = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));

        image_data[idx] = cr;
        image_data[idx + 1] = cg;
        image_data[idx + 2] = cb;
        idx += 3;
    }

private:
    std::vector<unsigned char> image_data;
    const int image_width;
    const int image_height;
    const char* image_name;
    int idx;
};
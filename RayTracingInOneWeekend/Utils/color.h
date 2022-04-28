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
            exit(EXIT_FAILURE);// Dont let exception get out of the destructor
        }
        svpng(imageFile, image_width, image_height, image_data.data(), 0);
        fclose(imageFile);
    }

    void write_color(const color& pixel_color) {
        unsigned char cr = static_cast<unsigned char>(255.999 * pixel_color.x());
        unsigned char cg = static_cast<unsigned char>(255.999 * pixel_color.y());
        unsigned char cb = static_cast<unsigned char>(255.999 * pixel_color.z());
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
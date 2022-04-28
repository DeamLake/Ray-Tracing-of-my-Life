#include<iostream>
#include "Utils/color.h"
#include "Utils/vec3.h"

int main() {
    const int   image_width     = 256;
    const int   image_height    = 256;
    const char* image_name   = "test.png";

    OutputHelper helper(image_width, image_height, image_name);
    for (int i = image_height - 1; i >= 0; --i) {
        std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
        for (int j = 0; j < image_width; ++j) {
            auto r = double(j) / (image_width - 1);
            auto g = double(i) / (image_width - 1);
            auto b = 0.25;

            color pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
            helper.write_color(pixel_color);
        }
    }
    std::cerr << "\nDone.\n";
}
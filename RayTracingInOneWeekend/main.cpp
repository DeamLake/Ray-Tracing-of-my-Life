#include<iostream>
#include<vector>
#include "utils/svpng.inc"
using namespace std;

void output(const int image_width, const int image_height, const char* image_name) 
{
    FILE* imageFile;
    fopen_s(&imageFile, image_name, "wb");
    if (imageFile == NULL) {
        perror("ERROR: Cannot open output file");
        exit(EXIT_FAILURE);
    }

    vector<unsigned char> pic(image_height * image_width * 3);
    int idx = 0;

    for (int i= image_height - 1; i >= 0; --i) {
        for (int j = 0; j <image_width; ++j) {
            auto r = double(j) / (image_width - 1);
            auto g = double(i) / (image_width - 1);
            auto b = 0.25;

            int ir  = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            unsigned char cr = static_cast<unsigned char>(ir);
            unsigned char cg = static_cast<unsigned char>(ig);
            unsigned char cb = static_cast<unsigned char>(ib);

            pic[idx]        = cr;
            pic[idx + 1] = cg;
            pic[idx + 2] = cb;
            idx += 3;
        }
    }

    svpng(imageFile, image_width, image_height, pic.data(), 0);
    fclose(imageFile);
}

int main() {
    output(256, 256, "test.png");
}
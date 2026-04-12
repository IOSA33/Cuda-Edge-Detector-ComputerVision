#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#include <iostream>
#include <chrono>
#include <vector>

void GaussianEdgeFilterWidth(int width, int height, int channels, unsigned char* data) {
    std::vector<unsigned char> source(data, data + (width * height * channels));

    for (int y = 0; y < height; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            int index = (y * width + x) * channels;
            int prev  = (y * width + (x - 1)) * channels;
            int next  = (y * width + (x + 1)) * channels;

            int diffR { std::abs(source[next + 0] - source[prev + 0]) };
            int diffG { std::abs(source[next + 1] - source[prev + 1]) };
            int diffB { std::abs(source[next + 2] - source[prev + 2]) };

            int wh{ 25 };

            if ((diffR > wh && diffG > wh) || (diffR > wh && diffB > wh) || (diffG > wh && diffB > wh)) {
                data[index + 0] = 255;
                data[index + 1] = 255;
                data[index + 2] = 255;
            } else {
                data[index + 0] = 0;
                data[index + 1] = 0;
                data[index + 2] = 0;
            }
        }
    }
}

void GaussianEdgeFilterRow(int width, int height, int channels, unsigned char* data) {
    std::vector<unsigned char> source(data, data + (width * height * channels));

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * channels;
            int up  = ((y-1) * width + x) * channels;
            int down  = ((y+1) * width + x) * channels;

            int diffR { std::abs(source[up + 0] - source[down + 0]) };
            int diffG { std::abs(source[up + 1] - source[down + 1]) };
            int diffB { std::abs(source[up + 2] - source[down + 2]) };

            int wh{ 25 };

            if ((diffR > wh && diffG > wh) || (diffR > wh && diffB > wh) || (diffG > wh && diffB > wh)) {
                data[index + 0] = 255;
                data[index + 1] = 255;
                data[index + 2] = 255;
            } else {
                data[index + 0] = 0;
                data[index + 1] = 0;
                data[index + 2] = 0;
            }
        }
    }
}

int main() {
    const auto start { std::chrono::high_resolution_clock::now() };
    int width, height, channels;
    unsigned char* data { stbi_load("../photos/sample1.jpg", &width, &height, &channels, 0) };
    if (!data) {
        std::cout << "Failed to load INPUT image\n";
        return 1;
    }

    GaussianEdgeFilterWidth(width, height, channels, data);
    GaussianEdgeFilterRow(width, height, channels, data);

    if (!stbi_write_jpg("../photos/sample1_copy.jpg", width, height, channels, data, 100)) {
        std::cout << "Failed to write OUTPUT image\n";
    }

    stbi_image_free(data);

    const auto end { std::chrono::high_resolution_clock::now() };
    std::cout << "Time used: " << std::chrono::duration<double>(end - start) << std::endl;
}

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb_image_write.h"
#include <iostream>
#include <chrono>
#include <vector>

void HandVision(int width, int height, int channels, unsigned char* data) {
    unsigned char colR{ 208 };
    unsigned char colG{ 138 };
    unsigned char colB{ 116 };

    unsigned char tolerance { 25 };

    size_t sum_00 { 0 };
    size_t sum_10 { 0 };
    size_t sum_01 { 0 };
    size_t y { 0 };
    size_t x { 0 };

    for (size_t i { 0 }; i < height * width; ++i) {
        size_t index { i * channels };

        if (std::abs(data[index + 0] - colR) <= tolerance && 
            std::abs(data[index + 1] - colG) <= tolerance && 
            std::abs(data[index + 2] - colB) <= tolerance) {
            
            data[index + 0] = 255;
            data[index + 1] = 255;
            data[index + 2] = 255;

            ++sum_00;{}
            sum_10 += x;
            sum_01 += y;
        } else {
            data[index + 0] = 0;
            data[index + 1] = 0;
            data[index + 2] = 0;
        }

        if (++x == width) {
            x = 0;
            ++y;
        }
    }

    size_t x_coord { sum_10 / sum_00 };
    size_t y_coord { sum_01 / sum_00 };

    int r { 1100 };
    int d { 1350 };

    for (int dx = -r; dx <= r; ++dx) {
        int nx = x_coord + dx;

        int ny1 = y_coord - d;
        int ny2 = y_coord + d;

        if (nx >= 0 && nx < width) {
            if (ny1 >= 0 && ny1 < height) {
                size_t index = (ny1 * width + nx) * channels;
                data[index] = 255;
            }
            if (ny2 >= 0 && ny2 < height) {
                size_t index = (ny2 * width + nx) * channels;
                data[index] = 255;
            }
        }
    }

    for (int dy = -d; dy <= d; ++dy) {
        int ny = y_coord + dy;

        int nx1 = x_coord - r;
        int nx2 = x_coord + r;

        if (ny >= 0 && ny < height) {
            if (nx1 >= 0 && nx1 < width) {
                size_t index = (ny * width + nx1) * channels;
                data[index] = 255;
            }
            if (nx2 >= 0 && nx2 < width) {
                size_t index = (ny * width + nx2) * channels;
                data[index] = 255;
            }
        }
    }
}

int main() {
    const auto start { std::chrono::high_resolution_clock::now() };
    int width, height, channels;
    unsigned char* data { stbi_load("../../photos/2.jpg", &width, &height, &channels, 0) };
    if (!data) {
        std::cout << "Failed to load INPUT image\n";
        return 1;
    }

    HandVision(width, height, channels, data);

    if (!stbi_write_jpg("../../photos/sample_output.jpg", width, height, channels, data, 100)) {
        std::cout << "Failed to write OUTPUT image\n";
    }





    stbi_image_free(data);
    const auto end { std::chrono::high_resolution_clock::now() };
    std::cout << "Time used: " << std::chrono::duration<double>(end - start) << std::endl;
}

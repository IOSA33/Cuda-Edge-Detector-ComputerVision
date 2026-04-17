#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#include <iostream>
#include <chrono>
#include <vector>

void GaussianEdgeFilter(int width, int height, int channels, unsigned char* data) {
    std::vector<unsigned char> source(data, data + (width * height * channels));

    for (size_t y { 0 }; y < height; ++y) {
        for (size_t x { 1 }; x < width - 1; ++x) {
            size_t index { (y * width + x) * channels };
            size_t prev  { (y * width + (x - 1)) * channels };
            size_t next  { (y * width + (x + 1)) * channels };

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

    for (size_t y { 1 }; y < height - 1; ++y) {
        for (size_t x { 0 }; x < width; ++x) {
            size_t index { (y * width + x) * channels };
            size_t up    { ((y - 1) * width + x) * channels };
            size_t down  { ((y + 1) * width + x) * channels };

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

        if ((((data[index + 0] - colR) + ((data[index + 0] - colR) >> sizeof(int) * CHAR_BIT - 1)) ^ ((data[index + 0] - colR) >> sizeof(int) * CHAR_BIT - 1)) <= tolerance &&
            (((data[index + 1] - colG) + ((data[index + 1] - colG) >> sizeof(int) * CHAR_BIT - 1)) ^ ((data[index + 1] - colG) >> sizeof(int) * CHAR_BIT - 1)) <= tolerance &&
            (((data[index + 2] - colB) + ((data[index + 2] - colB) >> sizeof(int) * CHAR_BIT - 1)) ^ ((data[index + 2] - colB) >> sizeof(int) * CHAR_BIT - 1)) <= tolerance) {

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
    size_t index_centroid { (y_coord * width + x_coord) * channels };

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
    unsigned char* data { stbi_load("../photos/1.jpg", &width, &height, &channels, 0) };
    if (!data) {
        std::cout << "Failed to load INPUT image\n";
        return 1;
    }

    // GaussianEdgeFilter(width, height, channels, data);
    HandVision(width, height, channels, data);

    if (!stbi_write_jpg("../photos/sample_output.jpg", width, height, channels, data, 100)) {
        std::cout << "Failed to write OUTPUT image\n";
    }

    stbi_image_free(data);
    const auto end { std::chrono::high_resolution_clock::now() };
    std::cout << "Time used: " << std::chrono::duration<double>(end - start) << std::endl;
}

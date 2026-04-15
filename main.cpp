#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#include <iostream>
#include <chrono>
#include <vector>

void GaussianEdgeFilterWidth(int width, int height, int channels, unsigned char* data) {
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
}

void GaussianEdgeFilterRow(int width, int height, int channels, unsigned char* data) {
    std::vector<unsigned char> source(data, data + (width * height * channels));

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

    for (size_t y { 0 }; y < height; ++y) {
        for (size_t x { 0 }; x < width; ++x) {
            size_t index { (y * width + x) * channels };

            if (std::abs(data[index + 0] - colR) <= tolerance && std::abs(data[index + 1] - colG) <= tolerance && std::abs(data[index + 2] - colB) <= tolerance) {
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

void calculateCentroid(int width, int height, int channels, unsigned char* data) {
    size_t sum_00 { 0 };
    size_t sum_10 { 0 };
    size_t sum_01 { 0 };

    for (size_t y { 0 }; y < height; ++y) {
        for (size_t x { 0 }; x < width; ++x) {
            size_t index { (y * width + x) * channels };

            if (data[index + 0] != 0) {
                ++sum_00;
                sum_10 += x * 1;
                sum_01 += y * 1;
            }
        }
    }
    size_t x_coord { sum_10 / sum_00 };
    size_t y_coord { sum_01 / sum_00 };

    std::cout << x_coord << " " << y_coord << '\n';

    size_t index { (y_coord * width + x_coord) * channels };

    for (size_t i { 0 }; i < width; i += 3) {
        data[index + i] = 255;       
    }
}

int main() {
    const auto start { std::chrono::high_resolution_clock::now() };
    int width, height, channels;
    unsigned char* data { stbi_load("../photos/sample2_hand.jpg", &width, &height, &channels, 0) };
    if (!data) {
        std::cout << "Failed to load INPUT image\n";
        return 1;
    }

    // GaussianEdgeFilterWidth(width, height, channels, data);
    // GaussianEdgeFilterRow(width, height, channels, data);

    HandVision(width, height, channels, data);
    calculateCentroid(width, height, channels, data);

    if (!stbi_write_jpg("../photos/sample_output.jpg", width, height, channels, data, 100)) {
        std::cout << "Failed to write OUTPUT image\n";
    }

    stbi_image_free(data);

    const auto end { std::chrono::high_resolution_clock::now() };
    std::cout << "Time used: " << std::chrono::duration<double>(end - start) << std::endl;
}

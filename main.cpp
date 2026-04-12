#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#include <iostream>
#include <chrono>

int main() {
    const auto start { std::chrono::high_resolution_clock::now() };
    int width, height, channels;
    unsigned char* data { stbi_load("../photos/sample1.jpg", &width, &height, &channels, 0) };
    if (!data) {
        std::cout << "Failed to load INPUT image\n";
        return 1;
    }

    for (int i{ 0 }; i < width * height; ++i) {
        int index { i * channels };

        data[index + 0] = 255;
        data[index + 1] = 0;
        data[index + 2] = 0;
    }

    if (!stbi_write_jpg("../photos/sample1_copy.jpg", width, height, channels, data, 100)) {
        std::cout << "Failed to write OUTPUT image\n";
    }

    stbi_image_free(data);

    const auto end { std::chrono::high_resolution_clock::now() };
    std::cout << "Time used: " << std::chrono::duration<double>(end - start) << std::endl;
}

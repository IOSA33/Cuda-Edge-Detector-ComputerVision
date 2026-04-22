#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb_image_write.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <hip/hip_runtime.h>

__global__ void HandVisionGPU(int width, int height, int channels, unsigned char* data) {
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
    //
    // HIP error type for function checks
    //
    hipError_t err;

    //
    // Starting timer
    //
    const auto start { std::chrono::high_resolution_clock::now() };
    
    //
    // Initializing variables and reading .jpg photos to RGB format 
    //
    int width, height, channels;
    unsigned char* data { stbi_load("/home/user/cppProjects/Cuda-ComputerVision/photos/sample1.jpg", &width, &height, &channels, 0) };
    if (!data) {
        std::cout << "Failed to load INPUT image\n";
        return 1;
    }

    //
    // Checking that GPU is available
    //
    int deviceCount = 0;
    err = hipGetDeviceCount(&deviceCount);
    if ( err != hipSuccess ) {
        std::cerr << "Error getting a device count." << std::endl;
        std::cerr << "hipError-code: " << err << std::endl;
        std::cerr << "hipError-string: " << hipGetErrorString(err) << std::endl;
        return 1;
    }

    //
    // Allocating space in global memory
    //
    std::cout << "Allocating " << width * height * channels * sizeof(unsigned char) / 1.0e6 << " MB of global memory." << std::endl;
    unsigned char* deviceBuffer;
    err = hipMalloc(&deviceBuffer, width * height * channels * sizeof(unsigned char));
    if ( err != hipSuccess ) {
        std::cerr << "Failed to allocate memory." << std::endl;
        std::cerr << "hipError-code: " << err << std::endl;
        std::cerr << "hipError-string: " << hipGetErrorString(err) << std::endl;
        return 1;
    }

    //
    // Transfering photo data buffer to device memory
    //
    std::cout << "Copying " << width * height * channels * sizeof(unsigned char) / 1.0e6 << " MB from host to device." << std::endl;
    err = hipMemcpy(deviceBuffer, data, width * height * channels * sizeof(unsigned char), hipMemcpyHostToDevice);
    if ( err != hipSuccess ) {
        std::cerr << "Failed to copy memory to device." << std::endl;
        std::cerr << "hipError-code: " << err << std::endl;
        std::cerr << "hipError-string: " << hipGetErrorString(err) << std::endl;

        (void)hipFree(deviceBuffer);
        return 1;
    }

    //
    // Configuring blocks and threads
    //
    const dim3 numberOfBlocks((width * height - 1) / 256 + 1);
    const dim3 threadsPerBlock(256);

    //
    // Kernel Call
    //
    std::cout << "Calling kernel!" << std::endl;
    HandVisionGPU<<<numberOfBlocks, threadsPerBlock, 0>>>(width, height, channels, deviceBuffer);
    err = hipGetLastError();
    if ( err != hipSuccess ) {
        std::cerr << "Failed to invoke the kernel." << std::endl;
        std::cerr << "hipError-code: " << err << std::endl;
        std::cerr << "hipError-string: " << hipGetErrorString(err) << std::endl;

        (void)hipFree(deviceBuffer);
        return 1;
    }

    //
    // Getting the result from the GPU
    //
    std::cout << "Copying " << width * height * channels * sizeof(unsigned char) / 1.0e6 << " MB from device to host." << std::endl;
    err = hipMemcpy(data, deviceBuffer, width * height * channels * sizeof(unsigned char), hipMemcpyDeviceToHost);
    if (err != hipSuccess) {
        std::cerr << "Failed to copy memory from device." << std::endl;
        std::cerr << "hipError-code: " << err << std::endl;
        std::cerr << "hipError-string: " << hipGetErrorString(err) << std::endl;

        (void)hipFree(deviceBuffer);
        return 1;
    }

    if (!stbi_write_jpg("/home/user/cppProjects/Cuda-ComputerVision/photos/sample_output.jpg", width, height, channels, data, 100)) {
        std::cout << "Failed to write OUTPUT image\n";
    }

    //
    // Doing clean ups
    //
    (void)hipFree(deviceBuffer);
    stbi_image_free(data);

    //
    // Checking How much time it took to complete
    //
    const auto end { std::chrono::high_resolution_clock::now() };
    std::cout << "Time used: " << std::chrono::duration<double>(end - start).count() << std::endl;
    return EXIT_SUCCESS;
}
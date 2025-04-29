#include "image_utils.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <omp.h>

// Загрузка изображения с помощью stb_image
Image loadImage(const std::string& filename, int& width, int& height) {
    int channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
    if (!data) {
        throw std::runtime_error("Failed to load image: " + filename);
    }
    
    Image image(height, std::vector<Color>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            image[y][x] = Color(data[index], data[index+1], data[index+2]);
        }
    }
    
    stbi_image_free(data);
    return image;
}

// Сохранение изображения с помощью stb_image_write
void saveImage(const std::string& filename, const Image& image) {
    if (image.empty() || image[0].empty()) return;
    
    int width = image[0].size();
    int height = image.size();
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            data[index] = image[y][x].r;
            data[index+1] = image[y][x].g;
            data[index+2] = image[y][x].b;
        }
    }
    
    if (!stbi_write_png(filename.c_str(), width, height, 3, data.data(), width * 3)) {
        throw std::runtime_error("Failed to save image: " + filename);
    }
}

// Функция для применения ядра размытия к одному пикселю
Color applyKernel(const Image& image, int x, int y, int kernelSize) {
    int halfKernel = kernelSize / 2;
    int sumR = 0, sumG = 0, sumB = 0;
    int count = 0;
    
    for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
        for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
            int nx = x + kx;
            int ny = y + ky;
            
            if (nx >= 0 && nx < image[0].size() && ny >= 0 && ny < image.size()) {
                sumR += image[ny][nx].r;
                sumG += image[ny][nx].g;
                sumB += image[ny][nx].b;
                count++;
            }
        }
    }
    
    if (count == 0) return image[y][x];
    return Color(sumR / count, sumG / count, sumB / count);
}

// Последовательное размытие изображения
Image sequentialBlur(const Image& input, int kernelSize) {
    if (input.empty()) return input;
    
    int height = input.size();
    int width = input[0].size();
    Image output(height, std::vector<Color>(width));
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            output[y][x] = applyKernel(input, x, y, kernelSize);
        }
    }
    
    return output;
}

// Параллельное размытие с использованием std::thread
Image parallelBlurThreads(const Image& input, int kernelSize, int numThreads) {
    if (input.empty()) return input;
    
    int height = input.size();
    int width = input[0].size();
    Image output(height, std::vector<Color>(width));
    
    std::vector<std::thread> threads;
    int rowsPerThread = height / numThreads;
    
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
        
        threads.emplace_back([&, startRow, endRow]() {
            for (int y = startRow; y < endRow; ++y) {
                for (int x = 0; x < width; ++x) {
                    output[y][x] = applyKernel(input, x, y, kernelSize);
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return output;
}

// Параллельное размытие с использованием OpenMP
Image parallelBlurOpenMP(const Image& input, int kernelSize) {
    if (input.empty()) return input;
    
    int height = input.size();
    int width = input[0].size();
    Image output(height, std::vector<Color>(width));
    
    #pragma omp parallel for
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            output[y][x] = applyKernel(input, x, y, kernelSize);
        }
    }
    
    return output;
}

// Функции для демонстрации атомарных операций
void incrementWithMutex(int& counter, std::mutex& mtx, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;
    }
}

void incrementWithAtomic(std::atomic<int>& counter, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        ++counter;
    }
}

void testAtomicOperations(int iterations, int numThreads) {
    std::cout << "\nTesting atomic operations with " << numThreads << " threads and " << iterations << " iterations per thread\n";
    
    // Тест с мьютексом
    {
        int counter = 0;
        std::mutex mtx;
        std::vector<std::thread> threads;
        
        Timer timer("Mutex version");
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(incrementWithMutex, std::ref(counter), std::ref(mtx), iterations);
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        timer.stop();
        
        std::cout << "Final counter value (mutex): " << counter << std::endl;
    }
    
    // Тест с atomic
    {
        std::atomic<int> counter(0);
        std::vector<std::thread> threads;
        
        Timer timer("Atomic version");
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(incrementWithAtomic, std::ref(counter), iterations);
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        timer.stop();
        
        std::cout << "Final counter value (atomic): " << counter << std::endl;
    }
}

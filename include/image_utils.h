#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>

// Простая структура для представления цвета пикселя
struct Color {
    unsigned char r, g, b;
    
    Color() : r(0), g(0), b(0) {}
    Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};

// Тип для представления изображения
using Image = std::vector<std::vector<Color>>;

// Функции для работы с изображениями
Image loadImage(const std::string& filename, int& width, int& height);
void saveImage(const std::string& filename, const Image& image);

// Функции размытия
Image sequentialBlur(const Image& input, int kernelSize = 3);
Image parallelBlurThreads(const Image& input, int kernelSize = 3, int numThreads = 4);
Image parallelBlurOpenMP(const Image& input, int kernelSize = 3);

// Функции для демонстрации атомарных операций
void incrementWithMutex(int& counter, std::mutex& mtx, int iterations);
void incrementWithAtomic(std::atomic<int>& counter, int iterations);
void testAtomicOperations(int iterations, int numThreads);

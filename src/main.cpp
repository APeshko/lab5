#include "image_utils.h"
#include "timer.h"
#include <iostream>
#include <string>

int main() {
    try {
        // Загрузка изображения
        int width, height;
        std::string inputFile = "input.png";
        std::cout << "Loading image..." << std::endl;
        Image image = loadImage(inputFile, width, height);
        std::cout << "Image loaded. Width: " << width << ", Height: " << height << std::endl;
        
        // Последовательное размытие
        std::cout << "\nRunning sequential blur..." << std::endl;
        Image seqResult;
        {
            Timer timer("Sequential blur");
            seqResult = sequentialBlur(image, 5); // Ядро 5x5
        }
        saveImage("sequential_blur.png", seqResult);
        
        // Параллельное размытие с std::thread
        std::cout << "\nRunning parallel blur with threads..." << std::endl;
        Image parResult;
        {
            Timer timer("Parallel blur (threads)");
            parResult = parallelBlurThreads(image, 5, 4); // 4 потока
        }
        saveImage("parallel_threads_blur.png", parResult);
        
        // Параллельное размытие с OpenMP (если доступно)
        #ifdef _OPENMP
        std::cout << "\nRunning parallel blur with OpenMP..." << std::endl;
        Image ompResult;
        {
            Timer timer("Parallel blur (OpenMP)");
            ompResult = parallelBlurOpenMP(image, 5);
        }
        saveImage("parallel_omp_blur.png", ompResult);
        #else
        std::cout << "\nOpenMP not available, skipping OpenMP version" << std::endl;
        #endif
        
        // Тестирование атомарных операций
        testAtomicOperations(1000000, 4);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

cmake_minimum_required(VERSION 3.10)
project(Lab5_Multithreading)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для OpenMP (опционально)
find_package(OpenMP)
if(OpenMP_FOUND)
    message(STATUS "OpenMP found, enabling support")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Для работы с изображениями (используем stb_image)
set(STB_IMAGE_HEADER "stb_image.h")
set(STB_IMAGE_WRITE_HEADER "stb_image_write.h")
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/${STB_IMAGE_HEADER})
    message(STATUS "Downloading stb_image.h...")
    file(DOWNLOAD 
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${STB_IMAGE_HEADER})
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/${STB_IMAGE_WRITE_HEADER})
    message(STATUS "Downloading stb_image_write.h...")
    file(DOWNLOAD 
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h"
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${STB_IMAGE_WRITE_HEADER})
endif()

include_directories(include)
add_executable(lab5 src/main.cpp src/image_utils.cpp src/timer.cpp)

# Для OpenMP (опционально)
if(OpenMP_FOUND)
    target_link_libraries(lab5 PUBLIC OpenMP::OpenMP_CXX)
endif()

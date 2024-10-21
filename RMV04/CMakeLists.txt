cmake_minimum_required(VERSION 3.0)
project(ArmorPlateDetection)

# 查找 OpenCV 库
find_package(OpenCV 4 REQUIRED)

# 添加库的头文件路径
include_directories(include)

# 添加库
add_library(ImageProcessingLib SHARED src/ImageProcessingLib.cpp)
add_library(ArmorPlateRecognitionLib SHARED src/ArmorPlateRecognitionLib.cpp)
add_library(enhancement SHARED src/enhancement.cpp)

# 添加主程序
add_executable(Main main.cpp)

# 链接 OpenCV 库和目标
target_link_libraries(ImageProcessingLib ${OpenCV_LIBS})
target_link_libraries(ArmorPlateRecognitionLib ${OpenCV_LIBS})
target_link_libraries(enhancement ${OpenCV_LIBS})

# 链接主程序和所有库
target_link_libraries(Main ImageProcessingLib ArmorPlateRecognitionLib enhancement ${OpenCV_LIBS})
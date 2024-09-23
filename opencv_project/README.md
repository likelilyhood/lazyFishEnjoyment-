# OpenCV 图像处理学习笔记

## 安装 OpenCV

## 一、安装 OpenCV 的方法

### （一）使用包管理器安装预编译版本
1. **优点**：简单快捷。
2. **缺点**：版本可能不是最新，可能缺少扩展模块。
3. **安装命令**：
   ```bash
   sudo apt update
   sudo apt install libopencv-dev
   ```

### （二）从源码编译安装
1. **优点**：可获得最新版本，能自定义安装选项。
2. **步骤如下**：
   - **安装依赖项**：
     ```bash
     sudo apt update
     sudo apt install -y build-essential cmake git pkg-config libgtk-3-dev \
     libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
     libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
     gfortran openexr libatlas-base-dev python3-dev python3-numpy \
     libtbb2 libtbb-dev libdc1394-22-dev libopenexr-dev \
     libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev
     ```
   - **下载 OpenCV 源码**：
     ```bash
     git clone https://github.com/opencv/opencv.git
     cd opencv
     git checkout [版本号] # 可以指定版本号，比如 4.5.0
     cd..
     git clone https://github.com/opencv/opencv_contrib.git
     cd opencv_contrib
     git checkout [版本号] # 确保与 opencv 版本号一致
     cd..
     ```
   - **编译安装**：
     ```bash
     cd opencv
     mkdir build
     cd build
     cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules..
     make -j$(nproc)
     sudo make install
     ```
   - **设置环境变量**：
     ```bash
     echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib" >> ~/.bashrc
     source ~/.bashrc
     ```
   - **验证安装**：
     ```bash
     pkg-config --modversion opencv4
     ```
   
## 二、验证 OpenCV 是否正确安装的方法

### （一）使用`pkg-config`验证
1. 若安装了 OpenCV 开发文件（如`libopencv-dev`等包），可使用`pkg-config`验证。
2. 命令`pkg-config --modversion opencv4`，对于某些版本可能需使用`opencv`代替`opencv4`。若安装正确，该命令将输出安装的 OpenCV 版本。

### （二）运行简单的 C++程序验证
1. 编写示例 C++程序，加载 OpenCV 高 GUI 模块并显示窗口。如：
   ```cpp
   #include <opencv2/opencv.hpp>
   #include <iostream>
   using namespace cv;
  
   int main() {
       std::cout << "OpenCV version: " << CV_VERSION << std::endl;

       cv::namedWindow("Test Window", cv::WINDOW_AUTOSIZE);
       cv::Mat image = cv::Mat::eye(200, 200, CV_8UC3);
       cv::imshow("Test Window", image);
       cv::waitKey(0);

       return 0;
   }
   ```
2. 保存为`test_opencv.cpp`，使用命令`g++ test_opencv.cpp -o test_opencv `pkg-config --cflags --libs opencv4`编译。
3. 运行编译好的程序`./test_opencv`，若正常，将打开一个窗口并显示 200x200 的灰度图像。

### （三）相对路径知识简要摘录
在计算机系统中，路径是用来指定文件或目录位置的一种方式。路径主要分为两种类型：绝对路径和相对路径。

**相对路径**：
- 相对路径是相对于当前工作目录的路径。
- 它依赖于当前所在的目录位置，不同的工作目录下，相同的相对路径可能指向不同的位置。
- 相对路径不以盘符或根目录的正斜杠开头。
- 可以使用 `.` 表示当前目录，`..` 表示当前目录的上一级目录。

例如，如果当前工作目录是 `/home/username/documents`：
- 绝对路径 `/home/username/documents/project` 总是指向用户目录下的 `documents` 目录中的 `project` 文件夹。
- 相对路径 `project` 也指向当前工作目录下的 `project` 文件夹。
- 相对路径 `../pictures` 则指向上一级目录 `/home/username/pictures`。

使用相对路径的好处是文件和目录的移动更加灵活，因为路径不会因文件位置的改变而失效。但缺点是，如果不知道当前的工作目录，可能会不清楚相对路径指向哪里。绝对路径则提供了一个明确无误的路径，无论当前工作目录在哪里。


# 三、CMake常见问题及解决方法

## （一）CMake 发现旧的`CMakeCache.txt`文件问题
1. **问题**：在新构建目录中运行 CMake 时出现错误，因为 CMake 发现旧的`CMakeCache.txt`文件，担心二进制文件被创建在错误位置。
2. **解决步骤**：
   - 删除旧的构建目录：`rm -rf /home/pqb/opencv_project/build`。
   - 创建新的构建目录：在源代码目录中执行`cd /home/pqb/opencv_project`，然后`mkdir build`，接着`cd build`。
   - 重新运行 CMake：在新构建目录中运行`cmake..`或包含特定参数运行。
   - 编译项目：若 CMake 配置成功，执行`make -j$(nproc)`。
   - 安装（如果需要）：编译成功后若要安装 OpenCV，使用`sudo make install`。
  ## CMake 创建同名可执行目标问题
1. **问题**：在配置 CMake 时遇到问题，具体是因为在 CMake 尝试创建一个名为"OpenCV_Project"的可执行目标时，发现已经存在一个同名的目标。通常发生在项目中有两个不同的 CMakeLists.txt 文件试图添加相同名称的可执行文件。
2. **解决步骤**：
   - 检查目标名称的唯一性：确保项目中所有`add_executable()`调用都有一个唯一的目标名称，尤其在多个 CMakeLists.txt 文件中。
   - 使用不同的目标名称：若在不同 CMakeLists.txt 文件中创建可执行文件，确保名称独特。
   - 清理构建目录：删除旧的构建缓存可能解决问题，执行`rm -rf /home/pqb/opencv_project/build`，然后`mkdir /home/pqb/opencv_project/build`，接着`cd /home/pqb/opencv_project/build`，最后`cmake..`。
   - 检查 CMakeLists.txt 文件：查看确保没有重复的`add_executable()`调用或目标名称唯一。
   - 查看 CMake 错误日志：查看`CMakeFiles/CMakeOutput.log`文件获取更多细节，如`cat /home/pqb/opencv_project/build/CMakeFiles/CMakeOutput.log`。
   - 重新运行 CMake：在清理构建目录并确保目标名称唯一后，重新运行`cmake..`。

  ## 编译时找不到 OpenCV 头文件问题
1. **问题**：编译`test_opencv.cpp`时找不到 OpenCV 的头文件`opencv2`，可能是 OpenCV 库未正确安装或头文件路径未被正确添加到编译器搜索路径中。
2. **解决步骤**：
   - 检查 OpenCV 是否正确安装：
     - 在基于 Debian 的系统上使用`dpkg -l | grep libopencv`检查。
     - 在基于 RPM 的系统上使用`rpm -qa | grep opencv`检查。
   - 检查头文件路径：
     - 在 CMakeLists.txt 中确保有`include_directories(${OpenCV_INCLUDE_DIRS})`命令添加头文件路径。
   - 检查 CMakeLists.txt 文件：
     - 确保包含正确指令找到和链接 OpenCV 库，如`find_package(OpenCV REQUIRED)`和`include_directories(${OpenCV_INCLUDE_DIRS})`。
   - 检查环境变量：
     - 确保`PATH`环境变量包含 OpenCV 的安装路径。
   - 清理并重新构建项目：
     - 在构建目录中执行`cd /home/pqb/opencv_project/build`，然后`rm -rf *`，接着`cmake..`，最后`make`。
   - 手动指定 OpenCV 路径：
     - 如果 CMake 仍然找不到 OpenCV，可手动设置`OpenCV_DIR`环境变量，如`export OpenCV_DIR=/path/to/opencv/build`，然后`cmake..`，替换实际构建目录。
   - 检查头文件包含语句：
     - 确保源代码中的头文件包含语句正确，如对于 OpenCV 4 使用`#include <opencv2/opencv.hpp>`而不是`#include <opencv2>`。

#### 包含头文件
```cpp
#include <opencv2/opencv.hpp>
```

## 读取图像

使用“cv::imread”函数可以读取图像文件：
```cpp
std::string image_path = "../resources/test_image.png";
cv::Mat image = cv::imread(image_path);
if (image.empty()) {
    std::cerr << "Could not read the image: " << image_path << std::endl;
    return 1;
}
```
如果图像路径不正确或者文件损坏了，“image.empty()”会返回“true”。这时候，需要检查一下图像路径或者文件是否完好。

## 显示图像

使用“cv::namedWindow”创建一个窗口，然后用“cv::imshow”在这个窗口中显示图像
```cpp
cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
cv::imshow("Display window", image);
```


## 转换为灰度图像

使用“cv::cvtColor”函数可以将图像转换为灰度图像
```cpp
cv::Mat grayImage;
cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
```
灰度图像在图像处理中非常有用哦，，虽然颜色变少了，但是却保留了大部分重要的视觉信息。有时候，简单中能突出关键的细节。

## 保存图像

使用“cv::imwrite”函数可以保存图像
```cpp
bool isSaved = cv::imwrite("../resources/gray_image.png", grayImage);
if (!isSaved) {
    std::cerr << "Failed to save the gray image." << std::endl;
    return 1;
}
```
如果保存成功，你就可以在指定的路径中找到保存的图像文件。
## 应用均值滤波

使用“cv::blur”函数可以应用均值滤波
```cpp
cv::Mat blurredImage;
cv::blur(image, blurredImage, cv::Size(9, 9));
```
均值滤波是一种简单而有效的图像平滑技术，可以减少图像中的噪声。

## 应用高斯滤波

“cv::GaussianBlur”函数则可以应用高斯滤波：
```cpp
cv::Mat gausfilteredImage;
cv::GaussianBlur(image, gausfilteredImage, cv::Size(kernelSize, kernelSize), sigmaX);
```
高斯滤波在减少噪声的同时，还能更好地保持图像的边缘信息。既能让图像变得平滑，又不会丢失重要的细节。

## 提取红色通道

通过设置颜色范围创建掩码，然后就可以提取红色通道啦，这就像是在彩色画卷中找出红色的部分：
```cpp
cv::Scalar lowerRed1(0, 100, 100);
cv::Scalar upperRed1(30, 255, 255);
cv::Mat mask1, mask2;
cv::inRange(hsvImage, lowerRed1, upperRed1, mask1);
cv::inRange(hsvImage, lowerRed2, upperRed2, mask2);
cv::Mat mask;
cv::bitwise_or(mask1, mask2, mask);
cv::Mat redchannelimage;
image.copyTo(redchannelimage, mask);
```
首先，设置两个红色范围的上下限。然后，使用“cv::inRange”函数创建两个掩码，分别对应两个红色范围。接着，用“cv::bitwise_or”函数合并这两个掩码。最后，将掩码应用到原始图像上，就得到了只包含红色通道的图像。
此处掩码的作用就在于确定具体的区域，运用掩码保护别的区域不受影响

## 边缘检测

使用“cv::Canny”函数可以进行边缘检测，就像在图像中找出隐藏的轮廓线：
```cpp
cv::Mat edges;
cv::Canny(contoursgrayImage, edges, lowerThreshold, upperThreshold);
```
边缘检测是图像处理中的一个重要步骤，它可以帮助我们识别图像中的结构边界。通过调整低阈值和高阈值，可以控制边缘检测的敏感度。找到边缘后，我们就可以进一步分析图像的形状和结构了。
之所以在此处调用canny函数，在于第一次边缘检测中，数据误差过于大，因此先对hsv图像中提取出红色部分，而后进行canny检测，进而获得较好数据，进一步运用在下一步的轮廓寻找中

## 寻找轮廓

“cv::findContours”函数可以帮助我们寻找图像中的轮廓，就像在神秘画卷中找出物体的轮廓：
```cpp
std::vector<std::vector<cv::Point>> contours;
cv::findContours(contoursbinaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
```
轮廓是图像中物体边界的表示，可以用于形状分析和对象识别。这个函数会返回一个包含所有轮廓的向量。每个轮廓都是由一系列点组成的，可以用不同的方式进行处理和分析。

## 绘制轮廓

使用“cv::drawContours”函数可以绘制轮廓，就像在魔法画卷上用彩色线条勾勒出物体的轮廓：
```cpp
cv::Mat contourImage;
image.copyTo(contourImage);
cv::drawContours(contourImage, contours, -1, cv::Scalar(0, 255, 0), 2);
```
首先，创建一个新的图像，然后将原始图像复制到这个新图像上。接着，使用“cv::drawContours”函数在新图像上绘制轮廓。可以指定轮廓的颜色和线条宽度，让轮廓更加清晰可见。

## 形态学操作

“cv::morphologyEx”函数可以进行形态学操作，比如膨胀和腐蚀，就像用魔法工具改变图像的形状和结构：
```cpp
cv::Mat morphImageOFhighlight;
cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
cv::morphologyEx(highlightBinaryImage, morphImageOFhighlight, cv::MORPH_CLOSE, kernel);
```
首先，使用“cv::getStructuringElement”函数创建一个结构元素（也称为核），它定义了操作的尺寸和形状。然后，使用“cv::morphologyEx”函数进行形态学操作。这里使用了闭运算（先膨胀后腐蚀），可以去除图像中的小噪点，同时保持物体的形状和大小。


### 颜色空间

图像可以在不同的颜色空间之间转换，就像在不同的魔法世界中穿梭一样。BGR（蓝色、绿色、红色）是图像在内存中的存储格式，而 HSV（色调、饱和度、亮度）更适合于人类视觉系统，因此在某些图像处理任务中更为方便。比如，在提取特定颜色的物体时，HSV 颜色空间可以让我们更容易地设置颜色范围。

```cpp
cv::Mat bgrImage = cv::imread("../resources/test_image.png");
cv::Mat hsvImage;
cv::cvtColor(bgrImage, hsvImage, cv::COLOR_BGR2HSV);
```

### 结构元素

在形态学操作中，结构元素（核）就像是魔法工具的形状和大小。它是一个二值矩阵，用于定义操作的范围和形状。可以使用不同的形状和大小的结构元素来实现不同的效果。

以下是一个创建圆形结构元素的代码示例：
```cpp
cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
```
这里创建了一个 5x5 的椭圆形结构元素。可以根据需要调整结构元素的大小和形状，以达到不同的图像处理效果。

### 轮廓分析

找到轮廓后，我们可以进行进一步的分析，就像在神秘画卷中深入研究物体的形状和特征一样。比如，可以计算轮廓的面积、长度、轮廓近似等。

以下是一个计算轮廓面积的代码示例：
```cpp
std::vector<std::vector<cv::Point>> contours;
cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
for (size_t i = 0; i < contours.size(); i++) {
    double area = cv::contourArea(contours[i]);
    std::cout << "Contour " << i << " Area: " << area << std::endl;
}
```
这段代码遍历所有找到的轮廓，计算每个轮廓的面积，并输出到控制台。可以根据轮廓的面积来筛选出特定大小的物体，或者进行其他形状分析。

## 常见图像处理问题的解决方案

### 图像噪声
- **均值滤波**：如前文所述，使用`cv::blur`函数对图像进行均值滤波，可以减少图像中的噪声。
- **高斯滤波**：`cv::GaussianBlur`函数进行高斯滤波，在减少噪声的同时能更好地保持图像的边缘信息。

### 图像模糊
- **提高分辨率**：如果图像本身分辨率较低，可以尝试使用插值算法提高图像的分辨率。
- **锐化处理**：通过增强图像的边缘和细节来改善模糊问题，可以使用拉普拉斯算子等进行锐化处理。

### 颜色失真
- **颜色校正**：根据具体情况，对图像进行颜色校正，例如调整色温、色调等。
- **白平衡调整**：如果图像的白平衡不准确，可以使用OpenCV提供的白平衡调整函数进行修正。

### 目标检测不准确
- **调整参数**：对于边缘检测、轮廓提取等操作，合理调整参数，如阈值、核大小等，以提高检测的准确性。
- **使用多种算法结合**：结合多种目标检测算法，如基于特征的检测、深度学习检测等，提高检测的可靠性。

#include <opencv2/opencv.hpp>
#include <iostream>

bool saveImage(const cv::Mat& image, const std::string& filePath);
void showImage(const std::string& windowName, const cv::Mat& image);

int main() {
    try {
        std::string inputImagePath = "../resources/test_image.png";

        // 使用 cv::imread() 函数读取图片
        cv::Mat image = cv::imread(inputImagePath);
        if (image.empty()) {
            throw std::runtime_error("Could not read the image: " + inputImagePath);
        }

        // 创建一个窗口来显示图片
        showImage("Display window", image);

        // 转换为灰度图片
        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
        if (!saveImage(grayImage, "../resources/gray_image.png")) {
            throw std::runtime_error("Failed to save the gray image.");
        }

        // 转换为 HSV 图片
        cv::Mat hsvImage;
        cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);
        if (!saveImage(hsvImage, "../resources/hsv_image.png")) {
            throw std::runtime_error("Failed to save the HSV image.");
        }

        // 应用均值滤波
        cv::Mat averageFilteredImage;
        cv::blur(image, averageFilteredImage, cv::Size(9, 9));
        if (!saveImage(averageFilteredImage, "../resources/filtered_image9*9.png")) {
            throw std::runtime_error("Failed to save the filtered image.");
        }

        // 应用高斯滤波
        int kernelSize = 13;
        double sigmaX = 6;
        cv::Mat gaussianFilteredImage;
        cv::GaussianBlur(image, gaussianFilteredImage, cv::Size(kernelSize, kernelSize), sigmaX);
        if (!saveImage(gaussianFilteredImage, "../resources/gausfiltered_image13.png")) {
            throw std::runtime_error("Failed to save the filtered image.");
        }

        cv::Scalar lowerRed1(0, 100, 100);
        cv::Scalar upperRed1(30, 255, 255);
        cv::Scalar lowerRed2(150, 100, 100);
        cv::Scalar upperRed2(180, 255, 255);

        // 创建掩码
        cv::Mat mask1, mask2;
        cv::inRange(hsvImage, lowerRed1, upperRed1, mask1);
        cv::inRange(hsvImage, lowerRed2, upperRed2, mask2);

        // 合并两个掩码
        cv::Mat mask;
        cv::bitwise_or(mask1, mask2, mask);

        // 将掩码应用到图像中
        cv::Mat redChannelImage;
        image.copyTo(redChannelImage, mask);
        if (!saveImage(redChannelImage, "../resources/red_channel.png")) {
            throw std::runtime_error("Failed to save the red channel image.");
        }

        // 转换为灰度图
        cv::Mat contoursGrayImage;
        cv::cvtColor(redChannelImage, contoursGrayImage, cv::COLOR_BGR2GRAY);

        // 采用 Canny 方法优化
        cv::Mat edges;
        double lowerThreshold = 50;
        double upperThreshold = 150;
        cv::Canny(contoursGrayImage, edges, lowerThreshold, upperThreshold);
        if (!saveImage(edges, "../resources/canny_edges.png")) {
            throw std::runtime_error("Failed to save the Canny edges image.");
        }

        // 二值化
        cv::Mat contoursBinaryImage;
        cv::threshold(edges, contoursBinaryImage, 100, 255, cv::THRESH_BINARY);

        // 寻找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(contoursBinaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // 绘制轮廓
        cv::Mat contourImage;
        image.copyTo(contourImage);
        cv::drawContours(contourImage, contours, -1, cv::Scalar(0, 255, 0), 2);
        if (!saveImage(contourImage, "../resources/contours.jpg")) {
            throw std::runtime_error("Failed to save the contours image.");
        }

        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            std::cout << "Contour " << i << " Area: " << area << std::endl;
        }

        // 绘制所有轮廓的外接矩形
        for (size_t i = 0; i < contours.size(); i++) {
            cv::Rect boundingRect = cv::boundingRect(contours[i]);
            cv::rectangle(contourImage, boundingRect, cv::Scalar(0, 255, 0), 2);
        }
        if (!saveImage(contourImage, "../resources/bounding_rectangles.png")) {
            throw std::runtime_error("Failed to save the bounding rectangles image.");
        }

        // 对灰度图像进行二值化操作并进行形态学操作
        cv::Mat highlightBinaryImage;
        cv::threshold(grayImage, highlightBinaryImage, 90, 255, cv::THRESH_BINARY_INV);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::Mat morphImageOFhighlight;
        cv::morphologyEx(highlightBinaryImage, morphImageOFhighlight, cv::MORPH_CLOSE, kernel);

        // 寻找轮廓
        std::vector<std::vector<cv::Point>> contoursofhighlight;
        cv::findContours(morphImageOFhighlight, contoursofhighlight, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // 绘制轮廓
        cv::Mat contourImageOFhighlight;
        image.copyTo(contourImageOFhighlight);
        cv::drawContours(contourImageOFhighlight, contoursofhighlight, -1, cv::Scalar(0, 255, 0), 2);
        if (!saveImage(contourImageOFhighlight, "../resources/high_light_areas_after_binnary_gray_eroded_dilated.jpg")) {
            throw std::runtime_error("Failed to save the high light areas image.");
        }

        // 膨胀操作
        cv::Mat dilatedImage;
        cv::dilate(morphImageOFhighlight, dilatedImage, kernel, cv::Point(-1, -1), 2);

        // 腐蚀操作
        cv::Mat erodedImage;
        cv::erode(dilatedImage, erodedImage, kernel, cv::Point(-1, -1), 2);
        if (!saveImage(dilatedImage, "../resources/dilated_image.jpg") ||!saveImage(erodedImage, "../resources/eroded_image.jpg")) {
            throw std::runtime_error("Failed to save the processed images.");
        }

        // 指定填充的起点
        cv::Point seedPoint(50, 50);

        // 指定填充的新颜色
        cv::Scalar newColor(155, 255, 55);

        // 指定填充的容差
        cv::Scalar loDiff(20, 20, 20), upDiff(20, 20, 20);

        // 创建一个矩形，用于接收填充区域的边界
        cv::Rect rect;

        // 进行漫水填充
        int num = cv::floodFill(erodedImage, seedPoint, newColor, &rect, loDiff, upDiff, 4);
        if (!saveImage(erodedImage, "../resources/floodfilled_image.jpg")) {
            throw std::runtime_error("Failed to save the flood filled image.");
        }

        // 获取旋转矩阵
        cv::Point2f center(image.cols / 2.0F, image.rows / 2.0F);
        double angle = 35;
        double scale = 1.0;
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, scale);

        // 进行仿射变换（旋转）
        cv::Mat rotatedImage;
        cv::warpAffine(image, rotatedImage, rotationMatrix, image.size());
        if (!saveImage(rotatedImage, "../resources/rotated_image.jpg")) {
            throw std::runtime_error("Failed to save the rotated image.");
        }

        // 获取图像尺寸
        int imageWidth = image.cols;
        int imageHeight = image.rows;

        // 定义裁剪区域为左上角 1/4
        int cropWidth = imageWidth / 2;
        int cropHeight = imageHeight / 2;
        cv::Rect roi(0, 0, cropWidth, cropHeight);

        // 裁剪图像
        cv::Mat croppedImage = image(roi);
        if (!saveImage(croppedImage, "../resources/cropped_image.jpg")) {
            throw std::runtime_error("Failed to save the cropped image.");
        }

        // 绘制圆形
        cv::circle(image, cv::Point(100, 100), 50, cv::Scalar(0, 255, 0), 2);

        // 绘制方形（矩形）
        cv::rectangle(image, cv::Point(150, 150), cv::Point(200, 200), cv::Scalar(0, 255, 0), 2);

        // 绘制文字
        cv::putText(image, "Hello, OpenCV!", cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

        // 绘制红色的外轮廓
        cv::Scalar contourColor(0, 0, 255);
        cv::drawContours(image, std::vector<std::vector<cv::Point>>{{cv::Point(50, 50), cv::Point(200, 50), cv::Point(200, 200), cv::Point(50, 200)}}, -1, contourColor, 2);

        // 绘制红色的 bounding box
        cv::Rect boundingBox(100, 100, 100, 100);
        cv::rectangle(image, boundingBox, cv::Scalar(0, 0, 255), 2);

        // 显示绘制后的图像
        showImage("Drawn Image", image);
        cv::waitKey(0);

        if (!saveImage(image, "../resources/drawn_image.jpg")) {
            throw std::runtime_error("Failed to save the drawn image.");
        }

    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

bool saveImage(const cv::Mat& image, const std::string& filePath) {
    return cv::imwrite(filePath, image);
}

void showImage(const std::string& windowName, const cv::Mat& image) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    cv::imshow(windowName, image);
}

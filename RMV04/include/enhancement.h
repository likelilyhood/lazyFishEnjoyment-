#pragma once

#include <opencv2/opencv.hpp>

class Enhancement {
public:
    // 透视变换函数
    cv::Mat performPerspectiveTransform(const cv::Mat& image, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints);

    // 亮度和对比度调整函数
    cv::Mat adjustBrightnessAndContrast(const cv::Mat& image, double alpha, double beta);
};

#pragma once

#include <opencv2/opencv.hpp>

// 枚举颜色选择
enum ColorChoice { RED, BLUE };

// 颜色分离函数
cv::Mat performColorSeparation(const cv::Mat& image, ColorChoice colorChoice);

// 降低曝光函数
cv::Mat reduceExposure(cv::Mat image, double factor);

// 图像二值化函数
cv::Mat performBinaryThresholding(cv::Mat image, int threshold);

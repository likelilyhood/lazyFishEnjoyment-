#include "../include/ImageProcessingLib.hpp"

cv::Mat performColorSeparation(const cv::Mat& image, ColorChoice colorChoice) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    cv::Mat grayImage;
    if (colorChoice == RED) {
        grayImage = channels[0] - channels[2];
    } else if (colorChoice == BLUE) {
        grayImage = channels[2] - channels[0];
    }
    return grayImage;
}

cv::Mat reduceExposure(cv::Mat image, double factor) {
    cv::Mat result = image.clone();
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            cv::Vec3b& pixel = result.at<cv::Vec3b>(i, j);
            for (int k = 0; k < 3; ++k) {
                pixel[k] = static_cast<uchar>(pixel[k] * factor);
            }
        }
    }
    return result;
}

cv::Mat performBinaryThresholding(cv::Mat image, int threshold) {
    cv::Mat binaryImage;
    cv::threshold(image, binaryImage, threshold, 255, cv::THRESH_BINARY);
    return binaryImage;
}

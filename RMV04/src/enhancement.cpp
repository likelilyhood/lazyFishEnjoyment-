#include "../include/enhancement.h"

cv::Mat Enhancement::performPerspectiveTransform(const cv::Mat& image, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) {
    cv::Mat transformMatrix = cv::getPerspectiveTransform(srcPoints, dstPoints);
    cv::Mat warpedImage;
    cv::warpPerspective(image, warpedImage, transformMatrix, cv::Size(dstPoints[3].x, dstPoints[3].y));
    return warpedImage;
}

cv::Mat Enhancement::adjustBrightnessAndContrast(const cv::Mat& image, double alpha, double beta) {
    cv::Mat adjustedImage = image.clone();
    for (int i = 0; i < adjustedImage.rows; ++i) {
        for (int j = 0; j < adjustedImage.cols; ++j) {
            cv::Vec3b& pixel = adjustedImage.at<cv::Vec3b>(i, j);
            for (int k = 0; k < 3; ++k) {
                pixel[k] = cv::saturate_cast<uchar>(alpha * pixel[k] + beta);
            }
        }
    }
    return adjustedImage;
}

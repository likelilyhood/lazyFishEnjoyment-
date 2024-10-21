#include <iostream>
#include <opencv2/opencv.hpp>
#include "include/ImageProcessingLib.hpp"
#include "include/ArmorPlateRecognitionLib.hpp"
#include "include/enhancement.h"

int main() {
    cv::VideoCapture videoCapture("../resourses/11.mp4");
    if (!videoCapture.isOpened()) {
        std::cerr << "无法打开视频文件。" << std::endl;
        return -1;
    }

    cv::VideoWriter videoWriter("output_video.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), videoCapture.get(cv::CAP_PROP_FPS), cv::Size(videoCapture.get(cv::CAP_PROP_FRAME_WIDTH), videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT)));

    cv::Mat frame;
    double exposureReductionFactor = 0.2;
    int binaryThresholdValue = 20;
    int brightnessThreshold = 200;

    // 用户输入颜色选择
    std::cout << "请选择颜色（0：蓝色，1：红色）：";
    int colorChoiceInput;
    std::cin >> colorChoiceInput;
    ColorChoice colorChoice = static_cast<ColorChoice>(colorChoiceInput);

    while (true) {
        bool readSuccess = videoCapture.read(frame);
        if (!readSuccess) {
            break;
        }

        // 使用新库进行透视变换和亮度对比度调整
        std::vector<cv::Point2f> srcPoints;
        // 设置源点
        srcPoints.push_back(cv::Point2f(100, 100));
        srcPoints.push_back(cv::Point2f(500, 100));
        srcPoints.push_back(cv::Point2f(500, 500));
        srcPoints.push_back(cv::Point2f(100, 500));

        std::vector<cv::Point2f> dstPoints;
        // 设置目标点
        dstPoints.push_back(cv::Point2f(0, 0));
        dstPoints.push_back(cv::Point2f(400, 0));
        dstPoints.push_back(cv::Point2f(400, 400));
        dstPoints.push_back(cv::Point2f(0, 400));

 

        // 降低曝光
        cv::Mat reducedExposureFrame = reduceExposure(frame, exposureReductionFactor);

        // 颜色分离
        cv::Mat separatedColorsFrame = performColorSeparation(reducedExposureFrame, colorChoice);

        // 二值化
        cv::Mat binaryFrame = performBinaryThresholding(separatedColorsFrame, binaryThresholdValue);

        // 再次阈值化和膨胀处理
        cv::Mat brightBinaryImage;
        cv::threshold(binaryFrame, brightBinaryImage, brightnessThreshold, 255, cv::THRESH_BINARY);
        cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::dilate(brightBinaryImage, brightBinaryImage, structuringElement);

        // 找轮廓
        std::vector<std::vector<cv::Point>> lightContours;
        cv::findContours(brightBinaryImage.clone(), lightContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // 过滤和处理轮廓
        ArmorParameters armorParams;
        std::vector<LightDescriptor> validLightDescriptors;
        for (const auto& contour : lightContours) {
            cv::RotatedRect rect = cv::minAreaRect(contour);
            double area = cv::contourArea(contour);
            double ratio = rect.size.width / rect.size.height;
            double solidity = area / rect.size.area();
            if (area >= armorParams.minLightArea &&
                std::abs(rect.angle) <= armorParams.maxLightAngle &&
                rect.size.width >= armorParams.minLightSize &&
                rect.size.height >= armorParams.minLightSize &&
                solidity >= armorParams.minLightContourSolidity &&
                ratio <= armorParams.maxLightRatio) {
                LightDescriptor lightDescriptor(rect);
                validLightDescriptors.push_back(lightDescriptor);
            }
        }

        // 识别装甲板
        std::vector<cv::RotatedRect> armorRectangles = detectArmorPlates(validLightDescriptors);

        // 绘制装甲板轮廓
        cv::Mat debugImage = frame.clone();
        for (const auto& rect : armorRectangles) {
            // 调整长度为原来的二倍
            cv::Size2f newSize(rect.size.width, rect.size.height * 2);
            cv::RotatedRect newRect(rect.center, newSize, rect.angle);

            cv::Point2f vertices[4];
            newRect.points(vertices);
            for (int k = 0; k < 4; ++k) {
                cv::line(debugImage, vertices[k], vertices[(k + 1) % 4], cv::Scalar(0, 225, 255), 2);
            }
            cv::Point2f topPoint, bottomPoint;
            topPoint.x = newRect.center.x;
            topPoint.y = newRect.center.y + 20;
            bottomPoint.x = newRect.center.x;
            bottomPoint.y = newRect.center.y - 20;

            cv::Point2f newCenter(applyFiltering(newRect.center.x, newRect.center.x, 10), applyFiltering(newRect.center.y, newRect.center.y, 10));
            cv::RotatedRect finalRect(newSize, newCenter, newRect.angle);
            cv::rectangle(debugImage, topPoint, bottomPoint, cv::Scalar(0, 120, 255), 2);
            cv::circle(debugImage, newCenter, 10, cv::Scalar(0, 120, 255));
        }

        // 显示图像
        cv::imshow("contours", debugImage);
        //cv::imshow("1", separatedColorsFrame);
       // cv::imshow("2", binaryFrame);
        // cv::imshow("3", brightBinaryImage);
        videoWriter.write(brightBinaryImage);

        if (cv::waitKey(30) >= 0) {
            break;
        }
    }

    videoCapture.release();
    videoWriter.release();
    cv::destroyAllWindows();

    return 0;
}

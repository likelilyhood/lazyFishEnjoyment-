#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class ArmorParameters {
public:
    int minLightArea = 700;
    double maxLightAngle = 45.0;
    double minLightSize = 10.0;
    double minLightContourSolidity = 0.5;
    double maxLightRatio = 0.4;
};

class LightDescriptor {
public:
    LightDescriptor();
    LightDescriptor(const cv::RotatedRect& light);
    const LightDescriptor& operator=(const LightDescriptor& ld);
    cv::Size2f getDoubleHeightSize() const;
    cv::RotatedRect getRect() const;

public:
    float width;
    float height;
    cv::Point2f center;
    float angle;
    float area;
};
int applyFiltering(int effectiveValue, int newValue, int maxDifference);

// 判断两个灯条是否可能组成装甲板
bool isArmorPlatePossible(const LightDescriptor& light1, const LightDescriptor& light2);

// 调整矩形方向辅助函数
cv::RotatedRect& adjustRectangle(cv::RotatedRect& rect, const int mode);

// 识别装甲板函数
std::vector<cv::RotatedRect> detectArmorPlates(const std::vector<LightDescriptor>& validLightDescriptors);

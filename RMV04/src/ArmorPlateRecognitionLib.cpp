#include "../include/ArmorPlateRecognitionLib.hpp"

LightDescriptor::LightDescriptor() {}

LightDescriptor::LightDescriptor(const cv::RotatedRect& light) {
    width = light.size.width;
    height = light.size.height;
    center = light.center;
    angle = light.angle;
    area = light.size.area();
}
   const int WIDTH_GREATER_THAN_HEIGHT = 1;
   const int ANGLE_TO_UP = 2;

const LightDescriptor& LightDescriptor::operator=(const LightDescriptor& ld) {
    this->width = ld.width;
    this->height = ld.height;
    this->center = ld.center;
    this->angle = ld.angle;
    this->area = ld.area;
    return *this;
}

cv::Size2f LightDescriptor::getDoubleHeightSize() const {
    return cv::Size2f(width, height * 2);
}

cv::RotatedRect LightDescriptor::getRect() const {
    return cv::RotatedRect(center, cv::Size2f(width, height), angle);
}

bool isArmorPlatePossible(const LightDescriptor& light1, const LightDescriptor& light2) {
    const double angleDiffThreshold = 100.0;
    const double distanceThreshold = 500.0;
    double angleDifference = std::abs(light1.angle - light2.angle);
    double distance = cv::norm(light1.center - light2.center);
    return angleDifference <= angleDiffThreshold && distance <= distanceThreshold;
}

cv::RotatedRect& adjustRectangle(cv::RotatedRect& rect, const int mode) {
    float& width = rect.size.width;
    float& height = rect.size.height;
    float& angle = rect.angle;
    if (mode == WIDTH_GREATER_THAN_HEIGHT) {
        if (width < height) {
            std::swap(width, height);
            angle += 90.0;
        }
    }
    while (angle >= 90.0) angle -= 180.0;
    while (angle < -90.0) angle += 180.0;
    if (mode == ANGLE_TO_UP) {
        if (angle >= 45.0) {
            std::swap(width, height);
            angle -= 90.0;
        } else if (angle < -45.0) {
            std::swap(width, height);
            angle += 90.0;
        }
    }
    return rect;
}

   int applyFiltering(int effectiveValue, int newValue, int maxDifference) {
       if ((newValue - effectiveValue > maxDifference) || (effectiveValue - newValue > maxDifference)) {
           newValue = effectiveValue;
           return effectiveValue;
       } else {
           newValue = effectiveValue;
           return newValue;
       }
   }

std::vector<cv::RotatedRect> detectArmorPlates(const std::vector<LightDescriptor>& validLightDescriptors) {
    std::vector<cv::RotatedRect> armorRectangles;
    for (size_t i = 0; i < validLightDescriptors.size(); ++i) {
        for (size_t j = i + 1; j < validLightDescriptors.size(); ++j) {
            float angleDifference = std::abs(validLightDescriptors[i].angle - validLightDescriptors[j].angle);
            std::cout<<1<<"     "<<angleDifference<<std::endl;
            if (angleDifference >= 7) continue;

            float lengthDifference1 = std::abs(validLightDescriptors[i].height - validLightDescriptors[j].height) / std::max(validLightDescriptors[i].height, validLightDescriptors[j].height);
            float lengthDifference2 = std::abs(validLightDescriptors[i].width - validLightDescriptors[j].width) / std::max(validLightDescriptors[i].width, validLightDescriptors[j].width);
            
            std::cout<<2<<" "<<lengthDifference1<<"     "<< lengthDifference2<<std::endl;
            if (lengthDifference1 > 0.10 || lengthDifference2 > 0.35) continue;
            
            cv::RotatedRect tempRect;
            tempRect.center.x = (validLightDescriptors[i].center.x + validLightDescriptors[j].center.x) / 2.0;
            tempRect.center.y = (validLightDescriptors[i].center.y + validLightDescriptors[j].center.y) / 2.0;
            tempRect.angle = (validLightDescriptors[i].angle + validLightDescriptors[j].angle) / 2.0;
            float halfHeightSum = (validLightDescriptors[i].height + validLightDescriptors[j].height) / 2.0;
            float distanceBetweenCenters = std::sqrt(std::pow(validLightDescriptors[i].center.x - validLightDescriptors[j].center.x, 2) + std::pow(validLightDescriptors[i].center.y - validLightDescriptors[j].center.y, 2));
            float ratio = distanceBetweenCenters / halfHeightSum;
            float xDifference = std::abs(validLightDescriptors[i].center.x - validLightDescriptors[j].center.x) / halfHeightSum;
            float yDifference = std::abs(validLightDescriptors[i].center.y - validLightDescriptors[j].center.y) / halfHeightSum;
            
            std::cout<<3<<" "<<ratio<<"     "<<xDifference<<"    "<<yDifference<<std::endl;
            if (ratio < 2.0 || ratio > 3.5 || xDifference < 0.6 || yDifference > 7.0) continue;
            tempRect.size.height = halfHeightSum;
            tempRect.size.width = distanceBetweenCenters;
            armorRectangles.push_back(tempRect);
            std::cout<<std::endl;
        }
    }
    return armorRectangles;
}

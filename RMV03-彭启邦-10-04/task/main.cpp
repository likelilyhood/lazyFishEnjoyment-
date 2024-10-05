#include "windmill.hpp"
#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <ceres/ceres.h>

// 定义风车速度表达式的函数
struct WindmillSpeedFunction {
    WindmillSpeedFunction(double t, double speed) : t(t), speed(speed) {}

    template <typename T>
    bool operator()(const T* const A, const T* const omega, const T* const phi, const T* const b, T* residual) const {
        residual[0] = speed - (A[0] * ceres::cos(omega[0] * t + phi[0]) + b[0]);
        return true;
    }

private:
    double t;
    double speed;
};

int main() {
    const double true_A = 0.785;
    const double true_omega = 1.884;
    const double true_phi = 1.65;
    const double true_b = 1.305;

    std::vector<double> timings;

    for (int run = 0; run < 10; run++) {
        std::chrono::milliseconds t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        WINDMILL::WindMill wm(t.count());
        cv::Mat src;
        cv::Point fispoint;
        cv::Point secpoint;
        bool foundFirstPoint = false;
        bool foundSecondPoint = false;

        cv::Point prev_fispoint;
        cv::Point prev_secpoint;
        cv::Point prev_frame_translation;
        double prev_frame_angle = 0.0;

        double A = 1.0;
        double omega = 1.0;
        double phi = 1.0;
        double b = 1.0;

        ceres::Problem problem;
        int frameCount = 0;
        int iterationCount = 0;

        std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

        while (true) {
            t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            src = wm.getMat((double)t.count() / 1000);

            //==========================代码区========================//

            cv::Mat filtered_src;
            cv::GaussianBlur(src, filtered_src, cv::Size(5, 5), 1.5);

            cv::Mat binary_filtered_src;
            cv::threshold(filtered_src, binary_filtered_src, 127, 255, cv::THRESH_BINARY);

            /*本段代码即是方法一，在图像标注时空间占用较大
        // 尝试识别“R”标和hammer
        cv::Mat template_R;
        cv::Mat template_Hammer;
        cv::Mat gray_template_R;
        cv::Mat gray_template_Hammer;
        template_R = cv::imread("../task/image/R.png", cv::IMREAD_GRAYSCALE);
        template_Hammer =cv::imread("../task/image/target.png",cv::IMREAD_GRAYSCALE);

        
        if (template_R.rows > binary_filtered_src.rows || template_R.cols > binary_filtered_src.cols)
        {
            double scale = std::min(static_cast<double>(binary_filtered_src.rows) / template_R.rows, static_cast<double>(binary_filtered_src.cols) / template_R.cols);
            cv::resize(template_R, template_R, cv::Size(), scale, scale);
        }

        cv::matchTemplate(gray_filtered_src, template_R, template_R, cv::TM_CCOEFF_NORMED);
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(template_R, &minVal, &maxVal, &minLoc, &maxLoc);
        if (maxVal > 0.9)
        {
          // 在“R”标位置绘制矩形进行标注
            int rectWidth = template_R.cols * 0.03; // 新的宽度，这里假设为原来的两倍
            int rectHeight = template_R.rows * 0.04; // 新的高度，这里假设为原来的两倍
            cv::rectangle(filtered_src, maxLoc, cv::Point(maxLoc.x + rectWidth, maxLoc.y + rectHeight), cv::Scalar(0, 255, 0), 2);
            cout<<"succeed！"<<endl;
        }
        else {cout<<"failed"<<endl;}

        */
            cv::Mat gray_filtered_src;
            cv::cvtColor(filtered_src, gray_filtered_src, cv::COLOR_BGR2GRAY);

            if (binary_filtered_src.channels() > 1) {
                cv::cvtColor(binary_filtered_src, binary_filtered_src, cv::COLOR_BGR2GRAY);
            }

            cv::Mat imageForContours = binary_filtered_src.clone();
            if (imageForContours.type()!= CV_8UC1) {
                imageForContours.convertTo(imageForContours, CV_8UC1);
            }

            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(imageForContours, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            cv::Point new_fispoint;
            cv::Point new_secpoint;

            for (size_t i = 0; i < contours.size(); i++) {
                double area = cv::contourArea(contours[i]);
                if (area > 4000 && area < 5000) {
                    cv::Rect boundingRect = cv::boundingRect(contours[i]);
                    cv::rectangle(filtered_src, boundingRect, cv::Scalar(0, 255, 0), 2);

                    cv::Moments moments = cv::moments(contours[i]);
                    int cX = static_cast<int>(moments.m10 / moments.m00);
                    int cY = static_cast<int>(moments.m01 / moments.m00);
                    new_fispoint = cv::Point(cX, cY);
                    cv::circle(filtered_src, cv::Point(cX, cY), 3, cv::Scalar(255, 0, 0), -1);
                }
                if (area < 170) {
                    cv::drawContours(filtered_src, contours, i, cv::Scalar(0, 255, 0), 1);
                    cv::Moments moments = cv::moments(contours[i]);
                    int cX = static_cast<int>(moments.m10 / moments.m00);
                    int cY = static_cast<int>(moments.m01 / moments.m00);
                    new_secpoint = cv::Point(cX, cY);
                    cv::circle(filtered_src, cv::Point(cX, cY), 3, cv::Scalar(255, 0, 0), -1);
                }
               /*中点标注 
               if (centerPoints.size() == 2)
        {
            cv::Point midPoint((centerPoints[0].x + centerPoints[1].x) / 2, (centerPoints[0].y + centerPoints[1].y) / 2);
            cv::circle(filtered_src, midPoint, 3, cv::Scalar(255, 255, 0), -1);
        }*/
            }

            // 如果有上一帧的点，则进行平移操作
            if (foundFirstPoint && foundSecondPoint) {
                cv::Point translation = new_fispoint - prev_fispoint;
                fispoint = prev_fispoint + translation;
                secpoint = prev_secpoint + translation;
            }

            // 计算两帧之间 fispoint 的角度变化，作为速度估计值
            if (foundFirstPoint && prev_fispoint!= cv::Point(0, 0)) {
                double dx = fispoint.x - prev_fispoint.x;
                double dy = fispoint.y - prev_fispoint.y;
                double current_angle = atan2(dy, dx);
                double angle_change = current_angle - prev_frame_angle;

                // 将角度变化作为速度估计值，传入 Ceres 进行拟合
                double current_time = t.count() / 1000.0;
                problem.AddResidualBlock(
                    new ceres::AutoDiffCostFunction<WindmillSpeedFunction, 1, 1, 1, 1, 1>(
                        new WindmillSpeedFunction(current_time, angle_change)),
                    nullptr,
                    &A, &omega, &phi, &b);
            }

            prev_fispoint = fispoint;
            prev_secpoint = secpoint;
            prev_frame_angle = atan2(fispoint.y - prev_fispoint.y, fispoint.x - prev_fispoint.x);

            foundFirstPoint = (new_fispoint!= cv::Point(0, 0));
            foundSecondPoint = (new_secpoint!= cv::Point(0, 0));

            fispoint = new_fispoint;
            secpoint = new_secpoint;

            //=======================================================//

            cv::waitKey(1);

            frameCount++;
            if (frameCount % 50 == 0) {
                ceres::Solver::Options options;
                options.minimizer_progress_to_stdout = false;
                ceres::Solver::Summary summary;
                ceres::Solve(options, &problem, &summary);

                // 计算误差
                double error = std::abs(A - true_A) + std::abs(omega - true_omega) + std::abs(phi - true_phi) + std::abs(b - true_b);
                if (error < (true_A + true_omega + true_phi + true_b) * 0.05) {
                    std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
                    std::chrono::duration<double> elapsedTime = endTime - startTime;
                    timings.push_back(elapsedTime.count());
                    break;
                }

                iterationCount++;
                if (iterationCount == 10) {
                    std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
                    std::chrono::duration<double> elapsedTime = endTime - startTime;
                    timings.push_back(elapsedTime.count());
                    break;
                }
            }
        }
    }

    double totalTime = 0.0;
    for (double timing : timings) {
        totalTime += timing;
    }
    double averageTime = totalTime / timings.size();

    std::cout << "Average timing: " << averageTime << " seconds." << std::endl;

    return 0;
}

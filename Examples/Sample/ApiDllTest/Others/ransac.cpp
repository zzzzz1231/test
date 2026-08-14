#include <iostream>
#include <optional>
#include <vector>
#include <random>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "ransac.hpp"

using namespace Ransac;

std::pair<int, int> Ransac::getRandomPoints(const std::vector<cv::Point2f> &points)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, points.size() - 1);

    // 随机选择两个索引
    int index1 = dist(gen);
    int index2 = dist(gen);

    // 确保两个点不同
    while (index2 == index1)
    {
        index2 = dist(gen);
    }

    return std::make_pair(index1, index2);
}

cv::Point3f Ransac::compute_lineparams(const std::vector<cv::Point2f> &center_pts, const double distanceThreshold, const int maxIterations)
{
    cv::Point3f bestLine; // 最佳拟合的直线
    int bestInliers = 0;  // 最佳内点数
    int numPoints = center_pts.size();
    if (numPoints < 2)
        return cv::Point3f(0, 0, 0);
    for (int iteration = 0; iteration < maxIterations; ++iteration)
    {
        // 随机选择两个点
        std::pair indexs = Ransac::getRandomPoints(center_pts);
        // std::cout << indexs.first << " " << indexs.second << std::endl;

        cv::Point2f point1 = center_pts[indexs.first];
        cv::Point2f point2 = center_pts[indexs.second];

        // 计算直线参数 (ax + by + c = 0)
        float a = point2.y - point1.y;
        float b = point1.x - point2.x;
        float c = point2.x * point1.y - point1.x * point2.y;

        // 计算距离并标记内点
        int inliers = 0;
        for (const cv::Point2f &point : center_pts)
        {
            float distance = std::abs(a * point.x + b * point.y + c) / std::sqrt(a * a + b * b);
            if (distance < distanceThreshold)
            {
                ++inliers;
            }
        }

        // 更新最佳模型
        if (inliers > bestInliers)
        {
            bestInliers = inliers;
            bestLine = cv::Point3f(a, b, c); // 这里只考虑二维平面上的直线
        }
    }

    return bestLine;
}

double Ransac::compute_error(const cv::Point3f &line, const std::vector<cv::Point2f> &center_pts)
{
    std::ofstream file("./data/output/distance.txt");
    double best_error = std::numeric_limits<double>::max();
    double error_sum = 0;
    for (size_t i = 0; i < center_pts.size(); ++i)
    {
        double dis = fabs(line.x * center_pts[i].x + line.y * center_pts[i].y + line.z) / sqrt(line.x * line.x + line.y * line.y);
        if (dis < best_error && dis != 0)
        {
            best_error = dis;
            error_sum += dis;
        }
    }
    std::cout << error_sum / center_pts.size() << std::endl;
    return best_error;
}
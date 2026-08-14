#pragma once

#include <iostream>
#include <optional>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace Unorded
{
    // 自定义哈希函数，用于将 Point 类型的对象映射到哈希表中
    struct PointHash
    {
        std::size_t operator()(const cv::Point2f &p) const
        {
            // 使用 std::hash 将 x 和 y 的哈希值进行组合
            return std::hash<double>()(p.x) ^ (std::hash<double>()(p.y) << 1);
        }
    };

    std::vector<cv::Point> removeDuplicates(std::vector<cv::Point> &points);
    std::vector<cv::Point> select_index(std::vector<cv::Point> &points, const cv::Mat &line_img, std::vector<int>& idx);

}
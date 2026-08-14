#pragma once

#include <iostream>
#include <algorithm>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2\core\core.hpp>

namespace MedianFilter
{
    std::vector<cv::Point> median_filter(const std::vector<cv::Point> &points, int kenersize);
    std::vector<cv::Point> avg_filter(const std::vector<cv::Point> &pts, int filter_size);
}

#pragma once

#include <iostream>
#include <optional>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace Ransac
{
    std::vector<cv::Point2f> select_index(const std::vector<cv::Point2f> &center_pts);
    std::pair<int, int> getRandomPoints(const std::vector<cv::Point2f> &points);
    cv::Point3f compute_lineparams(const std::vector<cv::Point2f> &center_pts, const double distanceThreshold, const int maxIterations);
    double compute_error(const cv::Point3f &line, const std::vector<cv::Point2f> &center_pts);
}
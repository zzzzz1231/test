#pragma once

#include <iostream>
#include <algorithm>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2\core\core.hpp>
#include <Eigen/Dense>

namespace Spline_center
{
    Eigen::VectorXf FitterLeastSquareMethod(std::vector<float>& X, std::vector<float>& Y, uint8_t orders);
    std::pair<std::vector<float>, std::vector<float>> InitData(std::vector<cv::Point> &new_center, const cv::Mat &Line_img);
}

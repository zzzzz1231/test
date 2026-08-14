#include <iostream>
#include <algorithm>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2\core\core.hpp>

#include "medianFilter.hpp"

std::vector<cv::Point> MedianFilter::avg_filter(const std::vector<cv::Point> &pts, int filter_size = 5)
{
    std::vector<cv::Point> points(pts);
    int k = filter_size / 2;
    for (int i = k; i < pts.size() - k; i++)
    {
        cv::Point p{0, 0};
        for (int j = i - k; j < i + k; j++)
        {
            p += pts[j];
        }
        points[i] = p / (k * 2);
    }
    return points;
}

std::vector<cv::Point> MedianFilter::median_filter(const std::vector<cv::Point> &points, int kenersize)
{
    std::vector<cv::Point> medianPoints(points);
    for (int i = kenersize; i < points.size() - kenersize; i++)
    {
        std::vector<float> temp;
        for (int j = i - kenersize; j < i + kenersize; j++)
        {
            temp.push_back(points[j].y);
        }
        std::sort(temp.begin(), temp.end());
        medianPoints[i].y = temp[kenersize];
    }
    return medianPoints;
}
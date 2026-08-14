#include <iostream>
#include <optional>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "unorded_points.hpp"

using namespace Unorded;

std::vector<cv::Point> Unorded::removeDuplicates(std::vector<cv::Point> &points)
{
    std::unordered_set<cv::Point2f, Unorded::PointHash> uniquePoints;
    for (const cv::Point2f &p : points)
    {
        uniquePoints.insert(p);
    }

    // 将去重后的点重新存储到向量中
    std::vector<cv::Point> result(uniquePoints.begin(), uniquePoints.end());
    return result;
}
std::vector<cv::Point> Unorded::select_index(std::vector<cv::Point> &points, const cv::Mat &line_img, std::vector<int>& idx)
{
    std::vector<cv::Point3d> pixelsvalue;
    for (int i = 0; i < points.size(); ++i)
    {
        cv::Point3d temp;
        temp.x = points[i].x;
        temp.y = points[i].y;
        // cv::Point rpt;
        // temp.x = static_cast<int>(std::round(points[i].x));
        // temp.y = static_cast<int>(std::round(points[i].y));
        temp.z = float(line_img.at<uchar>(temp.y, temp.x));
        pixelsvalue.push_back(temp);
    }
    std::unordered_map<int, int> maxYValue;

    // 遍历坐标
    for (const auto &point : pixelsvalue)
    {
        if (maxYValue.find(point.x) == maxYValue.end() || point.z > maxYValue[point.x])
        {
            // 如果当前 y 值未在哈希表中或者当前 value 大于已记录的最大 value，则更新
            maxYValue[point.x] = point.z;
        }
    }

    // 保留 x 相同但 value 最大的数据
    std::vector<cv::Point> resultPoints;
    idx.clear();
    //for (const cv::Point3f& p : pixelsvalue)
    for (int i=0; i<pixelsvalue.size(); ++i)
    {
        auto p = pixelsvalue[i];
        if (p.z == maxYValue[p.x])
        {
            cv::Point temp;
            // temp.x = static_cast<int>(std::round(p.x));
            // temp.y = static_cast<int>(std::round(p.y));
            temp.x = static_cast<float>(std::round(p.x));
            temp.y = static_cast<float>(std::round(p.y));
            resultPoints.push_back(temp);
            idx.push_back(i);
        }
    }

    return resultPoints;
}
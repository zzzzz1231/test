#pragma once
//#include <Eigen/Core>
//#include <Eigen/Dense>
#include <optional>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define IDX_NULL -1

namespace LaserCenterline
{
    class Centerline
    {
    public:
        Centerline() {}
        cv::Mat load_sick_data(const std::string &filename);
        static std::pair<float, cv::Mat> get_background(const cv::Mat &img);
        static std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> extract_laser_center(cv::Mat &Line_img, float &laser_width);
        static std::vector<cv::Point2f> maxi_pixels(cv::Mat &Line_img);
        static std::vector<cv::Point> slecet_center(std::vector<cv::Point> &center_pts, const cv::Mat &line_img, std::vector<int>& idx);

        struct laserExtracted
        {
            std::vector<cv::Point2f> centerPixel;
            std::vector<cv::Point2f> centerSubPixel;
            std::vector<cv::Point2f> centerEigenVector;
            std::vector<double>      centerT; // Eigen Distance
        };
        
        
        // 使用所有亮度超过阈值的点来经行处理
        static laserExtracted extract_laser_center(const cv::Mat& Line_img, const float& laser_width, const int& laser_threshold);
        
        // 仅使粗提取结果，求取附近的亚像素点
        static laserExtracted   extract_laser_center_xld(const cv::Mat& Line_img, const std::vector<int> Line_Y, const float& laser_width, const int& laser_threshold);
        
        //enum AlgoPreProcess
        //{
        //    Median,
        //};

        enum Algorithm
        {
            Max,
            Center_Of_Gravity,
            Center_Of_Range,
            First_Peak,
            Last_Peak,
        };
        static std::vector<int> extract_laser_center_px (const cv::Mat& Line_img, const Algorithm& Algo, const int& laser_threshold);

    };



}
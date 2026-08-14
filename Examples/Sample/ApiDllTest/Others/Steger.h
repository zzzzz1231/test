#pragma once

#include <fstream>
#include <iostream>
#include <cmath>
#include <ImageTable.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

//#include "centerline_extraction.hpp"
#define PI 3.1415926

cv::Mat /*LaserCenterline::Centerline::*/
load_sick_data(const std::string& filename);

std::pair<float, cv::Mat> /*LaserCenterline::Centerline::*/
get_background(const cv::Mat& img);

struct laserExtracted
{
	std::vector<cv::Point2f> centerPixel;
	std::vector<cv::Point2f> centerSubPixel;
	std::vector<cv::Point2f> centerEigenVector;
	std::vector<double>      centerT; // Eigen Distance
};

//std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> /*LaserCenterline::Centerline::*/
laserExtracted
extract_laser_center(const cv::Mat& Line_img, const float& laser_width, const int& laser_threshold=20);

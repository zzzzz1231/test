/**
*	@copyright	Copyright 2016-2023 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include <../CalibrationWrapper/CalibrationWrapper.h>
#include "InterfaceOperation.h"
#include <iostream>
#include <map>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <opencv2/opencv.hpp>


using namespace SickCam;
using namespace cv;


bool SICK_GTEST_LIBRARY Demo_R35_color_run15_fcc_cal()
{
	// 读取图像
	auto imgT = SickCam::ImgT();
	imgT.loadFromIconFile("F:\\20250109\\222\\image", 1, 1.0, false);

	////////////////////////////////////////////////////////////////////////////

	//// 应用高度图标定。仅需要首次调用，初始化一些静态资源
	int calibration_handle_id = 0;
	auto imgC = imgT.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, false);
	imgC.SaveCalibratedImagesToIconFile("D://caca", true);

	////////////////////////////////////////////////////////////////////////////


	// 应用彩色对齐标定。仅需要首次调用，初始化一些静态资源
	int color_handle_id = 0;

	// --> 测试多次扫描不完整后，需要首尾拼接多个图像的情况
	imgT.Append(imgT);
	imgT.Append(imgT);


	auto imgCal = imgT.ApplyCalibrationColor(
		color_handle_id,
		"F:\\20250109\\222\\Calibration2560up.json", 
		"F:\\20250109\\222\\2025-08-19-17-03-41_config.xml"
		, 200, // 假如工件需要 2 张或更多图才能扫描完整，为了对齐就扫 3 张，并且在这里剪除指定行数，即可获得完整的数据。
		false, 11, 1.2, 2560, 2560
	);

	cv::Mat o_ran = cv::Mat(imgCal.get_rows(), 2560, CV_32FC1, imgCal.getData(DN::RAN_CAL)).clone(); // deep copy
	cv::Mat o_red = cv::Mat(imgCal.get_rows(), 2560, CV_8UC1, imgCal.getData(DN::R)).clone(); // deep copy
	cv::Mat o_gre = cv::Mat(imgCal.get_rows(), 2560, CV_8UC1, imgCal.getData(DN::G)).clone(); // deep copy
	cv::Mat o_blu = cv::Mat(imgCal.get_rows(), 2560, CV_8UC1, imgCal.getData(DN::B)).clone(); // deep copy
	cv::Mat o_whi = cv::Mat(imgCal.get_rows(), 2560, CV_8UC1, imgCal.getData(DN::WHITE)).clone(); // deep copy

	cv::Mat o_RGB;
	cv::merge(std::vector<cv::Mat>{ o_blu,o_gre,o_red }, o_RGB);

	// 经过标定，3D线扫的 Range 和 2D线扫的 R G B 在物理空间中是对齐的。
	
	return true;
}




////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
	Demo_R35_color_run15_fcc_cal();

	// 释放静态资源
	SickCam::ImgT::DeleteCalibration(0);
	SickCam::ImgT::DeleteCalibrationColor(0);

	return 0;
}
#endif


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
#include "ColorImageRun.hpp"


using namespace color_image_run;
using namespace SickCam;
using namespace cv;


bool SICK_GTEST_LIBRARY grab_rgbw(const std::string& offlineImagePath)
{
	// 读取图像
	auto imgT = SickCam::ImgT();
	imgT.loadFromIconFile("F:\\TEST\\15kong", 1, 1.0, false);
	cv::Mat i_range       = cv::Mat(imgT.get_rows(), imgT.get_cols(), CV_16SC1, imgT.getData(DN::RAN)).clone(); // deep copy
	cv::Mat i_reflectance = cv::Mat(imgT.get_rows(), imgT.get_cols(), CV_8UC1, imgT.getData(DN::REF)).clone(); // deep copy
	//cv::Mat i_scatter     = cv::Mat();// cv::imread(offlineImagePath + "/scatter.tiff", cv::IMREAD_GRAYSCALE);
	cv::Mat i_red         = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::R)).clone();
	cv::Mat i_green       = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::G)).clone();
	cv::Mat i_blue        = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::B)).clone();
	cv::Mat i_white       = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::WHITE)).clone();


	// 放大 Range 和 Reflectance 图像方便后续对齐。
	// 也可以缩小 RGBW 

	cv::resize(i_reflectance, i_reflectance, i_red.size());
	cv::resize(i_range, i_range, i_red.size());
	i_range.convertTo(i_range, CV_16SC1);

	// 加载文件
	color_image_run::colorParam cop;
	cop = loadColorParam("F:\\TEST\\config.xml");

	// 获得 Red 图像的 Range 估计图像
	auto estimatedRange = color_image_run::process_estimate_range_component(i_range, cop.FactorXRed, cop.FactorYRed, true, 3);

	// 将 G B W 和 Red 对齐
	auto green = color_image_run::process_translate_move_image(i_green, cop.ShiftYG);
	auto blue  = color_image_run::process_translate_move_image(i_green, cop.ShiftYB);
	auto white = color_image_run::process_translate_move_image(i_green, cop.ShiftYW);

	cv::Mat color;
	cv::merge(std::vector<cv::Mat>({i_red, green, blue}), color);
	
	// 未标定的原始数据点云，xyz可以从 estimatedRange 中获取，rgb可以从 i_red,green,blue 中获取

	return 0;


	// 初始化标定
	auto rectMethod = SiCaliWrapper::RectMethod::Top;
	auto rangeAxis = RangeAxis::REVERSE; // 根据相机参数设定，或者是dat-xml的xml中，如果Z-scale是-0.0625，就是REVERSE。
	float aoiOffsetX = 0.0; // OffsetX_RegionSelector_Region1
	float scaleZ = rangeAxis == RangeAxis::STANDARD ? 0.0625f : -0.0625f;
	float aoiOffsetY = 0.0; // 根据相机参数设定 OffsetY_RegionSelector_Region1
	float aoiWidth = 2560;  // 根据相机参数设定 Width_RegionSelector_Region1
	float aoiHeight = 832;  // 根据相机参数设定 Height_RegionSelector_Region1
	float originZ = (float)(rangeAxis == RangeAxis::STANDARD ? aoiOffsetY : aoiOffsetY + aoiHeight);
	uint32_t scatterSize = 8;
	uint32_t rectificationWidth = 5120;

	SiCaliWrapper::CalibrationWrapper calibrationPtr_1;
	if (false == calibrationPtr_1.initCalibrationFilter(
		"F:\\TEST\\Calibration1.json", // 这是 range+reflectance 图像的标定文件
		i_range.cols, // image width
		i_range.rows, // image height
		aoiWidth, // aoi width
		aoiHeight,  // aoi height
		1.0,
		aoiOffsetX, // region1, offsetx
		scaleZ, originZ, scatterSize, rectMethod, rectificationWidth, MISSING_DATA,true,false,3.0
	)) {
		std::cout << "initCalibrationFilter failed!" << std::endl;
		return false;
	}

	// 标定
	auto pD = std::make_shared<SiCaliWrapper::calibratedData>(rectificationWidth, i_range.rows);
	if (false == calibrationPtr_1.calibrate_color(
			pD,	(uint8_t*)i_range.data, (uint8_t*)i_reflectance.data, nullptr, nullptr, nullptr, nullptr, nullptr, true)
		)
	{
		std::cerr << "calibrate failed!" << std::endl;
		return false;
	}

	// 初始化彩色图像的标定
	SiCaliWrapper::CalibrationWrapper calibrationPtr_2;
	if (false == calibrationPtr_2.initCalibrationFilter(
		"F:\\TEST\\Calibration2.json", // 这是 estimatedRange + red 图像的标定文件
		estimatedRange.cols, // image width，一般是 5120
		estimatedRange.rows, // image height
		aoiWidth, // aoi width
		aoiHeight,  // aoi height
		1.0,
		aoiOffsetX, // region1, offsetx
		scaleZ, originZ, scatterSize, rectMethod, rectificationWidth, MISSING_DATA, true, false, 3.0
	)) {
		std::cout << "initCalibrationFilter failed!" << std::endl;
		return false;
	}

	// 标定
	auto pD2 = std::make_shared<SiCaliWrapper::calibratedData>(rectificationWidth, i_range.rows);
	if (false == calibrationPtr_1.calibrate_color(
		pD, (uint8_t*)estimatedRange.data, (uint8_t*)i_red.data, nullptr, 
		(uint8_t*)i_red.data, (uint8_t*)green.data, (uint8_t*)blue.data, (uint8_t*)white.data, true)
		)
	{
		std::cerr << "calibrate failed!" << std::endl;
		return false;
	}

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
	grab_rgbw("F:\\TEST\\VL_Test\\FOV280\\raw\\test_004\\");
	return 0;
}
#endif


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




std::vector<ImgT> AlignColorImageArray(
	const std::vector<ImgT>& imageArray, const int& appendNumber/*必须大于1*/,
	const std::string& colorCalibrationJson,
	const std::string& colorConfigXML,
	const int& color_handle_id,
	bool  doFCC,
	const int& colorInterpolationStride,
	const double& rectificationSpread,
	const int& rawColoreResizedWidth,
	const int& rectificationWidth,
	const SiCaliWrapper::RectMethod& rectMethod
)
{
	std::vector<ImgT> alignedImages;
	if (appendNumber <= 1)
	{
		return alignedImages;
	}

	// ID = 1，比如 AppendNumber = 4，下面要合并的是 1 2 3 4 5 其中的 5 是重合冗余，后续去除。输出 1 2 3 4
	auto image1 = imageArray[0];
	for (int i = 1; i <= appendNumber; ++i)
	{
		image1 = image1.Append(imageArray[i]);
	}

	int imageHeight = imageArray[0].get_rows();

	auto imgCal = image1.ApplyCalibrationColor(
		color_handle_id,
		colorCalibrationJson,
		colorConfigXML, 
		imageHeight, // 去除最后一张
		doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod
	);
	alignedImages.push_back(image1);


	// 下面要合并的是 4 5 6 7 8 9 其中的 4 9 是重合冗余，后续去除。输出 5 6 7 8
	for (int lp = appendNumber; lp < imageArray.size(); lp += appendNumber)
	{
		image1 = imageArray[lp - 1];
		for (int i = lp; i <= lp + appendNumber; ++i)
		{
			image1 = image1.Append(imageArray[i]);
		}

		imgCal = image1.ApplyCalibrationColor(
			color_handle_id,
			colorCalibrationJson,
			colorConfigXML, 
			imageHeight, // 去除最后一张
			doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod,
			imageHeight // 去除头部一张
		);
		alignedImages.push_back(image1);
	}

	// 处理最后一张
	image1 = imageArray[imageArray.size() - 2];
	image1 = image1.Append(imageArray[imageArray.size() - 1]);
	
	imgCal = image1.ApplyCalibrationColor(
		color_handle_id,
		colorCalibrationJson,
		colorConfigXML,
		0, // 
		doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod,
		imageHeight // 去除头部一张
	);
	alignedImages.push_back(image1);

	return alignedImages;
}



bool grab_rgbw()
{
	// 读取图像
	auto imgT = SickCam::ImgT();
	//imgT.loadFromIconFile("F:\\20250109\\222\\image", 1, 1.0, false);
	//imgT.SaveRawImagesToIconFile("F:\\20250109\\222\\image22", true);

	//imgT.loadFromIconFile("F:\\111\\test", 1, 1.0, false);
	//imgT.SaveRawImagesToIconFile("F:\\111\\test22", true);
	
	imgT.loadFromIconFile("F:\\111\\Alignment_block", 1, 1.0, false);
	//imgT.SaveRawImagesToIconFile("F:\\111\\test33", true, false);
	cv::Mat www = imgT.has(DN::WHITE) ? cv::Mat(imgT.get_rows(), 5120, CV_8UC1, imgT.getData(DN::WHITE)).clone() : cv::Mat(); // deep copy

	////////////////////////////////////////////////////////////////////////////

	//// 应用高度图标定。仅需要首次调用，初始化一些静态资源
	//int calibration_handle_id = 0;
	//auto imgC = imgT.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, false);
	//auto imgC = imgT.ApplyCalibration(calibration_handle_id, "F:\\calibration.xml", 0, false);
	//imgC.SaveCalibratedImagesToIconFile("D://caca", true);

	////////////////////////////////////////////////////////////////////////////


	// 应用彩色对齐标定。仅需要首次调用，初始化一些静态资源
	int color_handle_id = 0;

	// --> 测试多次扫描不完整后，需要首尾拼接多个图像的情况
	//auto img_12  = imgT.Append(imgT);
	//auto img_123 = img_12.Append(imgT);

	auto imgCal = imgT.ApplyCalibrationColor(
		color_handle_id,
		"F:\\111\\Calibration.json",
		//"F:\\20250109\\222\\Calibration2560up.json",
		"F:\\111\\2025-12-11-14-33-00_config.xml"
		//"F:\\20250109\\222\\2025-08-19-17-03-41_config.xml"
		, 200, // 假如工件需要 2 张或更多图才能扫描完整，为了对齐就扫 3 张，并且在这里剪除指定行数，即可获得完整的数据。
		false, 11, 1.2, 5120, 5120
	);

	cv::Mat o_ran = cv::Mat(imgCal.get_rows(), 5120, CV_32FC1, imgCal.getData(DN::RAN_CAL)).clone(); // deep copy
	//cv::Mat o_red = imgCal.has(DN::R) ? cv::Mat(imgCal.get_rows(), 2560, CV_8UC1, imgCal.getData(DN::R)).clone() : cv::Mat(); // deep copy
	//cv::Mat o_gre = imgCal.has(DN::G) ? cv::Mat(imgCal.get_rows(), 2560, CV_8UC1, imgCal.getData(DN::G)).clone() : cv::Mat(); // deep copy
	//cv::Mat o_blu = imgCal.has(DN::B) ? cv::Mat(imgCal.get_rows(), 2560, CV_8UC1, imgCal.getData(DN::B)).clone() : cv::Mat(); // deep copy
	cv::Mat o_whi = imgCal.has(DN::WHITE) ? cv::Mat(imgCal.get_rows(), 5120, CV_8UC1, imgCal.getData(DN::WHITE)).clone() : cv::Mat(); // deep copy

	//cv::Mat o_RGB;
	//cv::merge(std::vector<cv::Mat>{ o_blu,o_gre,o_red }, o_RGB);

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
	grab_rgbw();

	// 释放静态资源
	SickCam::ImgT::DeleteCalibration(0);
	SickCam::ImgT::DeleteCalibrationColor(0);

	return 0;
}
#endif


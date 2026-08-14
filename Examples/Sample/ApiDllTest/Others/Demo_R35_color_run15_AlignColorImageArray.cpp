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
	const SiCaliWrapper::RectMethod& rectMethod,
	bool doCalibration
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
		//image1 = image1.Append(imageArray[i]);
		image1.Append(imageArray[i]);
	}

	int oneImageHeight = imageArray[0].get_rows();

	auto imgCal = image1.ApplyCalibrationColor(
		color_handle_id,
		colorCalibrationJson,
		colorConfigXML, 
		oneImageHeight, // 去除最后一张
		doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod, 0, doCalibration
	);
	alignedImages.push_back(imgCal);


	// 下面要合并的是 4 5 6 7 8 9 其中的 4 9 是重合冗余，后续去除。输出 5 6 7 8
	for (int lp = appendNumber; lp < imageArray.size(); lp += appendNumber)
	{
		image1 = imageArray[lp - 1];
		for (int i = lp; i <= lp + appendNumber && i < imageArray.size(); ++i)
		{
			//image1 = image1.Append(imageArray[i]);
			image1.Append(imageArray[i]);
		}

		imgCal = image1.ApplyCalibrationColor(
			color_handle_id,
			colorCalibrationJson,
			colorConfigXML, 
			oneImageHeight, // 去除最后一张
			doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod,
			oneImageHeight // 去除头部一张
			, doCalibration
		);
		alignedImages.push_back(imgCal);
	}

	// 处理最后一张
	image1 = imageArray[imageArray.size() - 2];
	//image1 = image1.Append(imageArray[imageArray.size() - 1]);
	image1.Append(imageArray[imageArray.size() - 1]);
	
	imgCal = image1.ApplyCalibrationColor(
		color_handle_id,
		colorCalibrationJson,
		colorConfigXML,
		0, // 
		doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod,
		oneImageHeight // 去除头部一张
		, doCalibration
	);
	alignedImages.push_back(imgCal);

	return alignedImages;
}



bool grab_rgbw()
{
	//ImgT resu;
	//resu.loadFromIconFile("F:\\111\\result\\res-all", 1.0);


	// 只有当图片非常多（扫描完整后，图像的行数超过 30,000 行），
	// 无法一次性对齐标定的情况下，才需要使用到这里的方法。否则直接append图像然后运行对齐就可以了。

	//ImgT imgT;
	//imgT.loadFromIconFile("F:\\111\\Alignment_block", 1, 1.0, false);
	//auto imagArray = imgT.Divide(20);
	//for (int i=0; i<imagArray.size(); ++i)
	//{
	//	imagArray[i].SaveRawImagesToIconFile("F:\\111\\data\\img-" + std::to_string(i), true);
	//}


	// 读取图像
	std::vector<ImgT> imageArray(0);
	for (int i=0; i<20; ++i)
	{
		ImgT img;
		img.loadFromIconFile("F:\\111\\data\\img-" + std::to_string(i), 1.0);
		imageArray.push_back(img);
	}

	int calibration_handle_id = 0;
	bool doCalibration = true;
	auto result = AlignColorImageArray(imageArray, 4, 
		"F:\\111\\Calibration.json", 
		"F:\\111\\2025-12-19-11-06-01_config.xml",
		calibration_handle_id, false, 3.0, 1.2, 5120, 5120, SiCaliWrapper::RectMethod::Top, doCalibration
	);

	if (doCalibration)
	{
		for (int i = 0; i < result.size(); ++i)
		{
			result[i].SaveCalibratedImagesToIconFile("F:\\111\\result\\res-" + std::to_string(i), true);
		}

		ImgT resultAll = result[0];
		for (int i = 1; i < result.size(); ++i)
		{
			//resultAll = resultAll.Append(result[i]);
			resultAll.Append(result[i]);
		}
		resultAll.SaveCalibratedImagesToIconFile("F:\\111\\result\\res-all2", true);
	}
	else
	{
		ImgT resultAll = result[0];
		for (int i = 1; i < result.size(); ++i)
		{
			//resultAll = resultAll.Append(result[i]);
			resultAll.Append(result[i]);
		}
		resultAll.SaveRawImagesToIconFile("F:\\111\\result\\res-all-raw2", true);
	}
	
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

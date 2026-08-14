/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>	
#include <../CalibrationWrapper/CalibrationWrapper.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using SickCam::Ranger3Shared;
using SickCam::Ranger3;
using SickCam::DataNames;
using SickCam::CAM_STATUS;

///////////////////////////////////////////////////////////////////////

bool SICK_GTEST_LIBRARY Demo_ImgT_calibration_offline_json_simple_method()
{
	std::string test_orig_image("E:\\TEST\\IconData\\");
	std::string test_save_to("E:\\TMP\\1_CAL2");
	std::string test_calibration_file("E:\\TEST\\IconData\\Calibration.json");
	//std::string test_calibration_file	("F:\\CalibrationResult.json");

	// --- Load image
	SickCam::ImgT img;
	img.loadFromIconFile(test_orig_image, 1.0);
	if(img.isEmpty())
	{
		std::cout << "loadFromIconFile from " << test_orig_image << "  failed!" << std::endl;
		return false;
	}


	// 可以设置为
	int cal_handle = 15;

	SickCam::ImgT imgOut = img.ApplyCalibration(cal_handle, test_calibration_file, 0, true);


	//std::vector<Point2D> vpt = img.ApplyMapPoints(cal_handle, test_calibration_file, );











	std::cout
		<< "Save, return " 
		//<< imgOut.SaveToIconFile(test_save_to, std::vector<DataNames>({ DataNames::RAN_CAL, DataNames::REF_CAL, DataNames::SCA_8_CAL, DataNames::SCA_16_CAL, DataNames::MAR }), true)
		<< imgOut.SaveCalibratedImagesToIconFile(test_save_to, false, true) // 新的 API
		<< std::endl;

	{
		SickCam::ImgT img;
		img.loadFromIconFile(test_save_to, 0.04);

		cv::Mat range, reflectance, scatter16, scatter8, mark;
		if (img.has(SickCam::DataNames::RAN_CAL))
			cv::imshow("Range",			range		= cv::Mat(img.get_rows(), img.get_cols(), CV_32FC1, (uint8_t*)img.getData(SickCam::DataNames::RAN_CAL)));
		
		if (img.has(SickCam::DataNames::REF_CAL))
			cv::imshow("reflectance",	reflectance = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1,	(uint8_t*)img.getData(SickCam::DataNames::REF_CAL)));
		/*
		if (img.has(SickCam::DataNames::SCA_8_CAL))
			cv::imshow("scatter8",		scatter8	= cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1,	(uint8_t*)img.getData(SickCam::DataNames::SCA_8_CAL)));

		if (img.has(SickCam::DataNames::SCA_16_CAL)) // todo : Known issue, will fix in next version
			cv::imshow("scatter",		scatter16	= cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::SCA_16_CAL)));

		if (img.has(SickCam::DataNames::MAR))
			cv::imshow("Mark",			mark		= cv::Mat(img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img.getData(SickCam::DataNames::MAR)));
		*/
		cv::waitKey(0);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_ImgT_calibration_offline_json_simple_method();
	return 0;
}
#endif

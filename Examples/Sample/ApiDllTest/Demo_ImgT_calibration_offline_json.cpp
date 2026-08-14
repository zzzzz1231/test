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

bool SICK_GTEST_LIBRARY Demo_ImgT_calibration_offline_json()
{
	std::string test_orig_image			("E:\\TEST\\IconData\\FreeRunning"); 
	std::string test_save_to			("E:\\TMP\\1_CAL2");
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

	// -- check Range
	uint8_t * pRange = nullptr;
	if (img.has(DataNames::RAN))
		pRange = (uint8_t*)img.getData(DataNames::RAN);
	
	// -- check Reflectance
	uint8_t * pReflectance = nullptr;
	if (img.has(DataNames::REF))
		pReflectance = (uint8_t*)img.getData(DataNames::REF);

	// -- check Scatter
	auto
	scatterSize = img.has(DataNames::SCA_8 ) ? 8  : 0;
	scatterSize = img.has(DataNames::SCA_16) ? 16 : scatterSize;

	uint8_t * pScatter = nullptr;
	if (scatterSize == 16)
		pScatter = (uint8_t*)img.getData(DataNames::SCA_16);
	else if (scatterSize == 8)
		pScatter = (uint8_t*)img.getData(DataNames::SCA_8);

	// --- Rectification Spread
	double rectificationSpread = 1.2; // 可以获得较少的 missing data

	// --- Init Calibration Wrapper
	SiCaliWrapper::CalibrationWrapper calibrationWrapper;
	uint32_t rectificationWidth = 4096;
	auto rangeAxis   = RangeAxis::REVERSED;
	float aoiOffsetX = 0.0; // OffsetX_RegionSelector_Region1
	float scaleZ     = rangeAxis == RangeAxis::STANDARD ? 0.0625f : -0.0625f;
	float aoiOffsetY = 0.0; // OffsetY_RegionSelector_Region1
	float aoiWidth   = 2560;  // Width_RegionSelector_Region1
	float aoiHeight  = 832;  // Height_RegionSelector_Region1
	float originZ    = (float)(rangeAxis == RangeAxis::STANDARD ? aoiOffsetY : aoiOffsetY + aoiHeight);
	//uint32_t scatterSize = 8;
	if (false == calibrationWrapper.initCalibrationFilter(
		test_calibration_file, // json file recommended
		img.get_cols(), // image width
		img.get_rows(), // image height (rows)
		aoiWidth, // aoi width
		aoiHeight,  // aoi height
		1.0,
		aoiOffsetX, // region1, offsetx
		scaleZ,
		originZ,
		scatterSize,
		SiCaliWrapper::RectMethod::Top,
		rectificationWidth,
		MISSING_DATA,
		true,
		false,
		3.0
	)){
		std::cout << "initCalibrationFilter failed!" << std::endl;
		return false;
	}

	
	// --- Calibrate
	auto pD = std::make_shared<SiCaliWrapper::calibratedData>(rectificationWidth, img.get_rows());
#ifndef __arm__
	// 标定的线程数，建议设定为 CPU 核数的一半，为了不让 CPU 使用率达到 100% 。
	int numberOfThread = 4;
	//calibrationWrapper.set_NumberOfThreads_jsonOnly_X64(numberOfThread);
#endif // !__arm__
	if (false == calibrationWrapper.calibrate(pD, pRange, pReflectance, pScatter, true))
	{
		std::cout << "calibrate failed!" << std::endl;
		return false;
	}

	// --- Get Calibrated Data
	SickCam::ImgT imgOut;
	double offsetY(0.0), offsetZ(pD->lower_bound_r), scaleY(0.044);
	double X_cal_Offset = pD->lower_bound_x;
	double Z_cal_Offset = pD->lower_bound_r;
	double X_cal_Scale = (pD->upper_bound_x - pD->lower_bound_x) / pD->Width; // 通过
	double Z_cal_Scale = 0.001; // 需要从 Ranger3Studio 的参数中得到 
	imgOut.setCaliInfo(	pD->Width, 
						pD->Height, 
						pD->offsetX, offsetY, 0.0,
						pD->scaleX, scaleY, 0.0,
						pD->lower_bound_x, pD->upper_bound_x, 
						pD->lower_bound_r, pD->upper_bound_r,
						X_cal_Offset, Z_cal_Offset, X_cal_Scale, Z_cal_Scale
	);
	
	imgOut.insertDataCopy(DataNames::RAN_CAL, (uint8_t*)pD->getData(SiCaliWrapper::RAN_C), 0);
	imgOut.insertDataCopy(DataNames::REF_CAL, (uint8_t*)pD->getData(SiCaliWrapper::REF_C), 0);

	if(scatterSize == 16)
		imgOut.insertDataCopy(DataNames::SCA_16_CAL, (uint8_t*)pD->getData(SiCaliWrapper::SCA_16), 0);
	else if(scatterSize == 8)
		imgOut.insertDataCopy(DataNames::SCA_8_CAL,  (uint8_t*)pD->getData(SiCaliWrapper::SCA_8),  0);

	if (img.has(DataNames::MAR)) // 若有编码器信息，不参与标定，直接拷贝到输出图像中。
		imgOut.insertDataCopy(DataNames::MAR, (uint8_t*)img.getData(DataNames::MAR), 0);


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
		//cv::waitKey(0);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_ImgT_calibration_offline_json();
	return 0;
}
#endif

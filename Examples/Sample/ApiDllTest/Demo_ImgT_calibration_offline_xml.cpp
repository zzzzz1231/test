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

bool SICK_GTEST_LIBRARY calibration_offline_xml()
{
	std::string test_orign_image("E:\\TEST\\IconData\\");
	std::string test_save_to("E:\\TMP\\1_CAL2");
	std::string test_calibration_file("E:\\TEST\\IconData\\Calibration.json");



	// --- Load image
	SickCam::ImgT img;
	img.loadFromIconFile(test_orign_image, 1.0);
	if(img.isEmpty())
	{
		std::cout << "loadFromIconFile from " << test_orign_image << "  failed!" << std::endl;
		return false;
	}

	// -- check Range
	uint8_t * pRange = nullptr;
	if (img.has(DataNames::RAN))
		pRange = (uint8_t*)img.getData(DataNames::RAN);
	
	// -- check Reflectance (Scatter is not supported)
	uint8_t * pReflectance = nullptr;
	if (img.has(DataNames::REF))
		pReflectance = (uint8_t*)img.getData(DataNames::REF);

	// --- Init Calibration Wrapper
	CaliWrapper Cali;
	if (false == Cali.initCalibrationFilter(	"..\\..\\scatter\\SN19020016_Calibration.xml",
												img.get_cols(),
												img.get_rows(),
												img.getR_aoiWidth(),
												img.getR_aoiHeight(),
												img.getRange_info().xml_scale_x,
												img.getRange_info().xml_origin_x,
												img.getRange_info().xml_scale_z,
												img.getRange_info().xml_origin_z,
												16, /* default value for scatter, but scatter is not suppprted in xml calibration */
												RectMethod::Top, 
												img.getR_aoiWidth(), 
												MISSING_DATA, 
												true))
	{
		std::cout << "initCalibrationFilter failed!" << std::endl;
		return false;
	}
	
	// --- Calibrate
	auto pD = std::make_shared<SiCaliWrapper::calibratedData>(img.get_cols(), img.get_rows());
	if (false == Cali.calibrate(pD, pRange, pReflectance, nullptr, true))
	{
		std::cout << "calibrate failed!" << std::endl;
		return false;
	}

	// --- Get Calibrated Data
	SickCam::ImgT imgOut;

	imgOut.setCaliInfo(	pD->Width, 
						pD->Height, 
						pD->offsetX, 0, 0,
						pD->scaleX, 0.044, 1.0,
						pD->lower_bound_x, pD->upper_bound_x, 
						pD->lower_bound_r, pD->upper_bound_r, 
						1.0/*X 分辨率*/, pD->lower_bound_x, 1.0/*Z 分辨率*/, pD->lower_bound_r); // 分辨率多可以在Ranger3Studio中读到
	
	imgOut.insertDataCopy(DataNames::RAN_CAL, (uint8_t*)pD->getData(SiCaliWrapper::RAN_C), 0);
	imgOut.insertDataCopy(DataNames::REF_CAL, (uint8_t*)pD->getData(SiCaliWrapper::REF_C), 0);

	if (img.has(DataNames::MAR)) // 若有编码器信息，不参与标定，直接拷贝到输出图像中。
		imgOut.insertDataCopy(DataNames::MAR, (uint8_t*)img.getData(DataNames::MAR), 0);


	std::cout
		<< "Save, return " 
		<< imgOut.SaveCalibratedImagesToIconFile(test_save_to + "\\FFF2", true)
		<< std::endl;

	{
		SickCam::ImgT img;
		img.loadFromIconFile(test_save_to + "\\FFF2", 0.04);

		cv::Mat range, reflectance, mark;
		if (img.has(SickCam::DataNames::RAN_CAL))
			cv::imshow("Range",			range		= cv::Mat(img.get_rows(), img.get_cols(), CV_32FC1, (uint8_t*)img.getData(SickCam::DataNames::RAN_CAL)));
		
		if (img.has(SickCam::DataNames::REF_CAL))
			cv::imshow("reflectance",	reflectance = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1,	(uint8_t*)img.getData(SickCam::DataNames::REF_CAL)));

		if (img.has(SickCam::DataNames::MAR))
			cv::imshow("Mark",			mark		= cv::Mat(img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img.getData(SickCam::DataNames::MAR)));

		//cv::waitKey(0);
	}

	return true;
}


#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	calibration_offline_xml();
	return 0;
}
}
#endif
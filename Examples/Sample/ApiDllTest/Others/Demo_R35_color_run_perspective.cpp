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

std::string out_path = "";
std::string josn_path = "";
color_image_run::colorParam cop;
uint32_t rectificationWidth = 5120;
cv::Mat xMap;

using namespace SickCam;
using namespace cv;

bool SICK_GTEST_LIBRARY grab_rgbw(const std::string& offlineImagePath)
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;


	// --- Declare Calibration Wrapper
	SiCaliWrapper::CalibrationWrapper calibrationWrapper;


		cv::Mat i_range       ;
		cv::Mat i_reflectance ;
		cv::Mat i_scatter     ;
		cv::Mat i_red         ;
		cv::Mat i_green       ;
		cv::Mat i_blue        ;
		cv::Mat i_white       ;
		auto imgT = SickCam::ImgT();
		//imgT.loadFromIconFile("F:\\TEST\\VL_Test\\FOV280\\Dots_Resize\\Dots_Resize", 1, 1.0, false);
		//imgT.loadFromIconFile("F:\\TEST\\VL_Test\\FOV280\\Dots\\img-2", 1, 1.0, false);
		imgT.loadFromIconFile("F:\\TEST\\VL_Test\\FOV280\\raw\\test_004\\img-2", 1, 1.0, false);
	

		if (imgT.isEmpty())
		{
			i_range = cv::imread(offlineImagePath + "/range.tiff", cv::IMREAD_UNCHANGED);
			i_reflectance = cv::imread(offlineImagePath + "/reflectance.tiff", cv::IMREAD_GRAYSCALE);
			i_scatter = cv::Mat();// cv::imread(offlineImagePath + "/scatter.tiff", cv::IMREAD_GRAYSCALE);
			i_red = cv::imread(offlineImagePath + "/r.tiff", cv::IMREAD_GRAYSCALE);
			i_green = cv::imread(offlineImagePath + "/g.tiff", cv::IMREAD_GRAYSCALE);
			i_blue = cv::imread(offlineImagePath + "/b.tiff", cv::IMREAD_GRAYSCALE);
			i_white = cv::imread(offlineImagePath + "/w.tiff", cv::IMREAD_GRAYSCALE);
		}
		else
		{
			i_range       = cv::Mat(imgT.get_rows(), imgT.get_cols(), CV_16SC1, imgT.getData(DN::RAN)).clone(); // deep copy
			i_reflectance = cv::Mat(imgT.get_rows(), imgT.get_cols(), CV_8UC1, imgT.getData(DN::REF)).clone(); // deep copy
			i_scatter     = cv::Mat();// cv::imread(offlineImagePath + "/scatter.tiff", cv::IMREAD_GRAYSCALE);
			
			//i_red         = cv::Mat();
			//i_green       = cv::Mat();
			//i_blue        = cv::Mat();
			//i_white       = cv::Mat();
			i_red         = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::R)).clone();
			i_green       = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::G)).clone();
			i_blue        = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::B)).clone();
			i_white       = cv::Mat(imgT.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, imgT.getData(DN::WHITE)).clone();

		}


		// --- Load image
		//cv::Mat i_range       = cv::imread(offlineImagePath + "/range.tiff", cv::IMREAD_UNCHANGED);
		//cv::Mat i_reflectance = cv::imread(offlineImagePath + "/reflectance.tiff", cv::IMREAD_GRAYSCALE);
		//cv::Mat i_scatter     = cv::Mat();// cv::imread(offlineImagePath + "/scatter.tiff", cv::IMREAD_GRAYSCALE);
		//cv::Mat i_red         = cv::imread(offlineImagePath + "/r.tiff", cv::IMREAD_GRAYSCALE);
		//cv::Mat i_green       = cv::imread(offlineImagePath + "/g.tiff", cv::IMREAD_GRAYSCALE);
		//cv::Mat i_blue        = cv::imread(offlineImagePath + "/b.tiff", cv::IMREAD_GRAYSCALE);
		//cv::Mat i_white       = cv::imread(offlineImagePath + "/w.tiff", cv::IMREAD_GRAYSCALE);


		// 放大 Range 和 Reflectance 图像方便后续对齐。
		cv::resize(i_reflectance, i_reflectance, i_red.size());
		cv::resize(i_range, i_range, i_red.size());
		i_range.convertTo(i_range, CV_16SC1);



		// --- Process (Aligned)
		cv::Mat adj_range;
		cv::Mat adj_reflectance;
		cv::Mat adj_scatter = cv::Mat();
		cv::Mat adj_red;
		cv::Mat adj_green;
		cv::Mat adj_blue;
		cv::Mat adj_color;
		cv::Mat adj_white;


		//auto timebeg = cv::getTickCount();
		color_image_run::process_rawData_adjustOrdinary_align(cop,
			i_range,
			i_reflectance,
			i_scatter,
			i_red,
			i_green,
			i_blue,
			i_white,
			adj_range      ,
			adj_reflectance,
			adj_scatter    ,
			adj_red        ,
			adj_green      ,
			adj_blue       ,
			adj_color      ,
			adj_white,
			false,
			"",
			"",
			false
		);


		// Save Calibrated Data (Aligned)
		std::stringstream ss;
		ss << out_path;
		cv::imwrite(ss.str() + "_raw_range.tiff"      , adj_range);
		cv::imwrite(ss.str() + "_raw_reflectance.tiff", adj_reflectance);
		//cv::imwrite(ss.str() + "_raw_color.tiff"      , adj_color);
		//cv::imwrite(ss.str() + "_raw_white.tiff"      , adj_white);

		// --- Init Calibration Wrapper
		auto rectMethod = SiCaliWrapper::RectMethod::Top;
		//auto rangeAxis = imgT.getRange_info().m_RangeAxis;
		auto rangeAxis = RangeAxis::REVERSE; // from xml
		float aoiOffsetX = 0.0; // OffsetX_RegionSelector_Region1
		float scaleZ = rangeAxis == RangeAxis::STANDARD ? 0.0625f : -0.0625f;
		float aoiOffsetY = 0.0; // OffsetY_RegionSelector_Region1
		float aoiWidth = 2560;  // Width_RegionSelector_Region1
		float aoiHeight = 832;  // Height_RegionSelector_Region1
		float originZ = (float)(rangeAxis == RangeAxis::STANDARD ? aoiOffsetY : aoiOffsetY + aoiHeight);
		uint32_t scatterSize = 8;
		if (false == calibrationWrapper.initCalibrationFilter(
			josn_path, // json file recommended
			adj_range.cols, // image width
			adj_range.rows, // image height
			aoiWidth, // aoi width
			aoiHeight,  // aoi height
			1.0,
			aoiOffsetX, // region1, offsetx
			scaleZ,
			originZ,
			scatterSize,
			rectMethod,
			rectificationWidth,
			MISSING_DATA,
			true,
			false,
			3.0
		)) {
			std::cout << "initCalibrationFilter failed!" << std::endl;
			return false;
		}



		// Caibrate
		//timebeg = cv::getTickCount();
		auto pD = std::make_shared<SiCaliWrapper::calibratedData>(rectificationWidth, adj_range.rows);
		if (false == calibrationWrapper.calibrate_color(pD, 
			(uint8_t*)adj_range.data, (uint8_t*)adj_reflectance.data, (uint8_t*)adj_scatter.data,
			(uint8_t*)adj_red.data  , (uint8_t*)adj_green.data      , (uint8_t*)adj_blue.data, (uint8_t*)adj_white.data, true))
		{
			std::cerr << "calibrate failed!" << std::endl;
			return false;
		}

		//timeend = cv::getTickCount();
		//std::cout << "calibrate, time(0.24)=" << (timeend - timebeg) / cv::getTickFrequency() << std::endl;


		// Get Calibrated Data (Aligned)
		cv::Mat cal_range = cv::Mat(adj_range.rows, rectificationWidth, CV_32FC1, pD->getData(SiCaliWrapper::PtrN::RAN_C));
		cv::Mat cal_refec = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1, pD->getData(SiCaliWrapper::PtrN::REF_C));
		cv::Mat cal_red   = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1, pD->getData(SiCaliWrapper::PtrN::RED_C));
		cv::Mat cal_green = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1, pD->getData(SiCaliWrapper::PtrN::GRE_C));
		cv::Mat cal_blue  = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1, pD->getData(SiCaliWrapper::PtrN::BLU_C));
		cv::Mat cal_white = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1, pD->getData(SiCaliWrapper::PtrN::WHI_C));

		cv::Mat cal_color;
		cv::merge(std::vector<cv::Mat>{cal_blue, cal_green, cal_red}, cal_color);


		double offsetX = pD->offsetX;
		double scaleX  = pD->scaleX;

		std::cout << offsetX << "\n";
		std::cout << scaleX <<"\n"; 


		// Save Calibrated Data (Aligned)
		std::stringstream ss1;
		ss1 << out_path;
		cv::imwrite(ss1.str() + "_cal_range.tiff", cal_range);
		cv::imwrite(ss1.str() + "_cal_reflectance.tiff", cal_refec);
		cv::imwrite(ss1.str() + "_cal_color.tiff", cal_color);
		cv::imwrite(ss1.str() + "_cal_white.tiff", cal_white);


	print_process_ok("All done! ");
	return true;
}




////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
	cop  = loadColorParam("F:\\TEST\\VL_Test\\FOV280\\out/config.xml");
	out_path = "F:\\TEST\\VL_Test\\FOV280\\raw\\test_004\\";
	josn_path = "F:\\TEST\\VL_Test\\FOV280\\Dots\\Calibration2.json";
	grab_rgbw("F:\\TEST\\VL_Test\\FOV280\\raw\\test_004\\");

	return 0;
}
#endif


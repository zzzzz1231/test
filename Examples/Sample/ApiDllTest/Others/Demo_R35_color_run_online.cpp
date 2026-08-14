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
#define NEED_CALIBRATION


using namespace color_image_run;

std::string out_path = "";
std::string josn_path = "";
color_image_run::colorParam cop;
uint32_t rectificationWidth = 5120;
cv::Mat xMap;

using namespace SickCam;
using namespace cv;

void SICK_CALLBACK
on_color_image(ImgT* pImg, void* any)
{
	std::cout << pImg->collectInfo() << std::endl;
	
	Mat i_range       = Mat(pImg->get_rows(), pImg->get_cols(), CV_16UC1, pImg->getData(DataNames::RAN));
	Mat i_reflectance 
		          = pImg->has(DataNames::REF)   ? Mat() : Mat(pImg->get_rows(), pImg->get_cols() , CV_8UC1, pImg->getData(DataNames::REF));
	Mat i_scatter = pImg->has(DataNames::SCA_8) ? Mat() : Mat(pImg->get_rows(), pImg->get_cols() , CV_8UC1, pImg->getData(DataNames::SCA_8));
	Mat i_red     = pImg->has(DataNames::R)     ? Mat() : Mat(pImg->get_rows(), HIRES_IMG_WIDTH, CV_8UC1, pImg->getData(DataNames::R));
	Mat i_green   = pImg->has(DataNames::G)     ? Mat() : Mat(pImg->get_rows(), HIRES_IMG_WIDTH, CV_8UC1, pImg->getData(DataNames::G));
	Mat i_blue    = pImg->has(DataNames::B)     ? Mat() : Mat(pImg->get_rows(), HIRES_IMG_WIDTH, CV_8UC1, pImg->getData(DataNames::B));
	Mat i_white   = pImg->has(DataNames::WHITE) ? Mat() : Mat(pImg->get_rows(), HIRES_IMG_WIDTH, CV_8UC1, pImg->getData(DataNames::WHITE));

	// 放大 Range 和 Reflectance 图像方便后续对齐。
	cv::resize(i_reflectance, i_reflectance, i_red.size());
	cv::resize(i_range, i_range, i_red.size());
	//cv::resize(i_scatter, i_scatter, i_red.size());


	// Process (Aligned)
	cv::Mat adj_range;
	cv::Mat adj_reflectance;
	cv::Mat adj_scatter;
	cv::Mat adj_red;
	cv::Mat adj_green;
	cv::Mat adj_blue;
	cv::Mat adj_color;
	cv::Mat adj_white;
	process_rawData_adjustOrdinary_align(cop,
		i_range,
		i_reflectance,
		i_scatter,
		i_red,
		i_green,
		i_blue,
		i_white,
		adj_range,
		adj_reflectance,
		adj_scatter,
		adj_red,
		adj_green,
		adj_blue,
		adj_color,
		adj_white,
		false,
		""
	);

	// Crop
	//adj_color.rowRange(0, 624) = cv::Scalar(0, 0, 0); // top
	//adj_color.rowRange(adj_color.rows - 10, adj_color.rows) = cv::Scalar(0, 0, 0); // bottom

	// Save Raw (Aligned)
	std::stringstream ss;
	ss << out_path << pImg->get_ID();
	cv::imwrite(ss.str() + "_adj_range.tiff"  , adj_range);
	cv::imwrite(ss.str() + "_adj_reflectance.tiff", adj_reflectance);
	cv::imwrite(ss.str() + "_adj_color.tiff", adj_color);
	cv::imwrite(ss.str() + "_adj_white.tiff", adj_white);

#ifdef NEED_CALIBRATION
	// Caibrate
	SiCaliWrapper::CalibrationWrapper* calibrationWrapper = (SiCaliWrapper::CalibrationWrapper*)any;
	auto pD = std::make_shared<SiCaliWrapper::calibratedData>(rectificationWidth, adj_range.rows);
	if (false == calibrationWrapper->calibrate_color(pD,
		(uint8_t*)adj_range.data, (uint8_t*)adj_reflectance.data, (uint8_t*)adj_scatter.data,
		(uint8_t*)adj_red.data, (uint8_t*)adj_green.data, (uint8_t*)adj_blue.data, (uint8_t*)adj_white.data, true))
	{
		std::cerr << "calibrate failed!" << std::endl;
		return;
	}

	// Get Calibrated Data (Aligned)
	cv::Mat cal_range = cv::Mat(adj_range.rows, rectificationWidth, CV_32FC1, pD->getData(SiCaliWrapper::PtrN::RAN_C));
	cv::Mat cal_refec = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1 , pD->getData(SiCaliWrapper::PtrN::REF_C));
	cv::Mat cal_red   = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1 , pD->getData(SiCaliWrapper::PtrN::RED_C));
	cv::Mat cal_green = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1 , pD->getData(SiCaliWrapper::PtrN::GRE_C));
	cv::Mat cal_blue  = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1 , pD->getData(SiCaliWrapper::PtrN::BLU_C));
	cv::Mat cal_white = cv::Mat(adj_range.rows, rectificationWidth, CV_8UC1 , pD->getData(SiCaliWrapper::PtrN::WHI_C));

	cv::Mat cal_color;
	cv::merge(std::vector<cv::Mat>{cal_blue, cal_green, cal_red}, cal_color);


	// Save Calibrated Data (Aligned)
	cv::imwrite(ss.str() + "_cal_range.tiff", cal_range);
	cv::imwrite(ss.str() + "_cal_reflectance.tiff", cal_refec);
	cv::imwrite(ss.str() + "_cal_color.tiff", cal_color);
	cv::imwrite(ss.str() + "_cal_white.tiff", cal_white);
#endif
}

bool SICK_GTEST_LIBRARY grab_rgbw()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;


	// --- Declare Calibration Wrapper
	SiCaliWrapper::CalibrationWrapper calibrationWrapper;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::CameraShared>(
		"",											/* 日志文件路径 */
		SickCam::CameraShared::getPathToRanger3Producer(),	/* cti 文件 */
		true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);

	pR3S->scanDevice();

	// --- Check connected device
	std::vector<std::string> ip(0);
	if (pR3S->isAvaliable()) {
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices) {
			ip.push_back(sub.first);
			std::cout << "---- " << sub.first << ", sn=" << sub.second->getSN() << std::endl;
		}
	}
	else {
		print_process_ng("Error: scan camera error. \n");
		return false;
	}

	//  --- Init Ranger3
	std::shared_ptr<SickCam::Ranger3> pCam;
	if (ip.size() > 0) {
		pCam = std::make_shared<SickCam::Ranger3>(pR3S, ip[0], true);
	}


	// --- Connect Ranger3
	auto err = pCam->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng(ip[0] + " Connect failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}
	pCam->setTimeOut(1);


	// --- Start Ranger3
	auto err = pCam->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng(ip[0] + "Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
	}

	// 当前（2024/10/31）只能使用 in-PC 标定
	pCam->setCalibration_No();

	err = pCam->startCamera(on_color_image, &calibrationWrapper); // pass variable to callback function
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng(ip[0] + "Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}


	// --- Hold on main thread
	getchar();


	// --- Stop Ranger3
	auto err = pCam->stopCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		print_process_ng(ip[0] + "Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}

	// --- Disconnect Ranger3
	auto err = pCam->disconnectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		print_process_ng(ip[0] + "Disconnect camera! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
	}
	


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
	grab_rgbw();

	return 0;
}
#endif


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


struct colorParam
{
	cv::Mat mat_r;
	cv::Mat mat_g;
	cv::Mat mat_b;
	cv::Mat mat_w;
	int ResizeFactor = 1.0;
	int ColorTopBlackRows = 0;
	int ColorEndBlackRows = 0;

	double RedGain = 1.0;
	double GreenGain = 1.0;
	double BlueGain = 1.0;

	double ReflectanceOrdinaryScale = 1.0;
	double ReflectanceOrdinaryOffset = 0.0;

	double RedOrdinaryScale = 1.0;
	double RedOrdinaryOffset = 0.0;
};


std::string out_path = "";
std::string josn_path = "";
colorParam cop;
uint32_t rectificationWidth = 5120;
cv::Mat xMap;


void process_rawData_adjustOrdinary_align(
	const colorParam& cop,
	const cv::Mat& i_ran,
	const cv::Mat& i_ref,
	const cv::Mat& i_scatter,
	const cv::Mat& i_r,
	const cv::Mat& i_g,
	const cv::Mat& i_b,
	const cv::Mat& i_w,
	cv::Mat& dst_ran,
	cv::Mat& dst_ref,
	cv::Mat& dst_scatter,
	cv::Mat& dst_red,
	cv::Mat& dst_green,
	cv::Mat& dst_blue,
	cv::Mat& dst_color,
	cv::Mat& dst_white,
	bool is_debug = false,
	const std::string& debug_img_save_to = ""
);

colorParam loadColorParam(const std::string xmlpath);

using namespace SickCam;
using namespace cv;

void SICK_CALLBACK
on_color_image(ImgT* pImg, void* any)
{
	std::cout << pImg->collectInfo() << std::endl;
	
	Mat i_range       = Mat(pImg->get_rows(), pImg->get_cols(), CV_16UC1, pImg->getData(DataNames::RAN));
	Mat i_reflectance 
		          = pImg->has(DataNames::REF)   ? Mat() : Mat(pImg->get_rows(), pImg->get_cols()    , CV_8UC1, pImg->getData(DataNames::REF));
	Mat i_scatter = pImg->has(DataNames::SCA_8) ? Mat() : Mat(pImg->get_rows(), pImg->get_cols()    , CV_8UC1, pImg->getData(DataNames::SCA_8));
	Mat i_red     = pImg->has(DataNames::R)     ? Mat() : Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(DataNames::R));
	Mat i_green   = pImg->has(DataNames::G)     ? Mat() : Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(DataNames::G));
	Mat i_blue    = pImg->has(DataNames::B)     ? Mat() : Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(DataNames::B));
	Mat i_white   = pImg->has(DataNames::WHITE) ? Mat() : Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(DataNames::WHITE));

	// 放大 Range 和 Reflectance 图像方便后续对齐。
	cv::resize(i_reflectance, i_reflectance, i_red.size());
	cv::resize(i_range, i_range, i_red.size());


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
		adj_white
	);

	// Save Raw (Aligned)
	std::stringstream ss;
	ss << out_path << pImg->get_ID();
	cv::imwrite(ss.str() + "_adj_range.tiff"  , adj_range);
	cv::imwrite(ss.str() + "_adj_reflectance.tiff"  , adj_reflectance);
	cv::imwrite(ss.str() + "_adj_color.tiff", adj_color);
	cv::imwrite(ss.str() + "_adj_white.tiff", adj_white);

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
}

bool SICK_GTEST_LIBRARY grab_rgbw(const std::string& offlineImagePath, bool calibrteOfflineImages)
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;

	// --- Init Calibration Wrapper
	SiCaliWrapper::CalibrationWrapper calibrationWrapper;
	auto rangeAxis = SiCaliWrapper::RectMethod::Top;
	float aoiOffsetX = 0.0; // OffsetX_RegionSelector_Region1
	float scaleZ = rangeAxis ? 0.0625f : -0.0625f;
	float aoiOffsetY = 0.0; // OffsetY_RegionSelector_Region1
	float aoiWidth = 2560;  // Width_RegionSelector_Region1
	float aoiHeight = 832;  // Height_RegionSelector_Region1
	float originZ = (float)(rangeAxis ? aoiOffsetY : aoiOffsetY + aoiHeight);
	uint32_t scatterSize = 8;
	if (false == calibrationWrapper.initCalibrationFilter(
		josn_path, // json file recommended
		5120, // image width
		1000, // image height
		aoiWidth, // aoi width
		aoiHeight,  // aoi height
		1.0,
		aoiOffsetX, // region1, offsetx
		scaleZ,
		originZ,
		scatterSize,
		rangeAxis,
		rectificationWidth,
		MISSING_DATA,
		true,
		false,
		3.0
	)) {
		std::cout << "initCalibrationFilter failed!" << std::endl;
		return false;
	}


	if (calibrteOfflineImages)
	{
		// --- Load image
		cv::Mat i_range = cv::imread(offlineImagePath + "/range.tiff", cv::IMREAD_UNCHANGED);
		cv::Mat i_reflectance = cv::imread(offlineImagePath + "/reflectance.tiff", cv::IMREAD_GRAYSCALE);
		cv::Mat i_scatter = cv::Mat();// cv::imread(offlineImagePath + "/scatter.tiff", cv::IMREAD_GRAYSCALE);
		cv::Mat i_red   = cv::imread(offlineImagePath + "/red.tiff", cv::IMREAD_GRAYSCALE);
		cv::Mat i_green = cv::imread(offlineImagePath + "/green.tiff", cv::IMREAD_GRAYSCALE);
		cv::Mat i_blue  = cv::imread(offlineImagePath + "/blue.tiff", cv::IMREAD_GRAYSCALE);
		cv::Mat i_white = cv::imread(offlineImagePath + "/white.tiff", cv::IMREAD_GRAYSCALE);


		// 放大 Range 和 Reflectance 图像方便后续对齐。
		cv::resize(i_reflectance, i_reflectance, i_red.size());
		cv::resize(i_range, i_range, i_red.size());



		// --- Process (Aligned)
		cv::Mat adj_range;
		cv::Mat adj_reflectance;
		cv::Mat adj_scatter = cv::Mat();
		cv::Mat adj_red;
		cv::Mat adj_green;
		cv::Mat adj_blue;
		cv::Mat adj_color;
		cv::Mat adj_white;


		// Transpose
		i_range       = i_range      .t();
		i_reflectance = i_reflectance.t();
		i_scatter     = i_scatter    .t();
		i_red         = i_red        .t();
		i_green       = i_green      .t();
		i_blue        = i_blue       .t();
		i_white       = i_white      .t();

		auto timebeg = cv::getTickCount();

		process_rawData_adjustOrdinary_align(cop,
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
			adj_white
		);

		auto timeend = cv::getTickCount();


		// Transpose
		adj_range       = adj_range      .t();
		adj_reflectance = adj_reflectance.t();
		adj_scatter     = adj_scatter    .t();
		adj_red         = adj_red        .t();
		adj_green       = adj_green      .t();
		adj_blue        = adj_blue       .t();
		//adj_color       = adj_color      .t();
		adj_white	    = adj_white		 .t();

		std::cout << "process_rawData_adjustOrdinary_align, time(0.49)=" << (timeend-timebeg)/cv::getTickFrequency() << std::endl;

		// Caibrate
		timebeg = cv::getTickCount();
		auto pD = std::make_shared<SiCaliWrapper::calibratedData>(rectificationWidth, adj_range.rows);
		if (false == calibrationWrapper.calibrate_color(pD, 
			(uint8_t*)adj_range.data, (uint8_t*)adj_reflectance.data, (uint8_t*)adj_scatter.data,
			(uint8_t*)adj_red.data  , (uint8_t*)adj_green.data      , (uint8_t*)adj_blue.data, (uint8_t*)adj_white.data, true))
		{
			std::cerr << "calibrate failed!" << std::endl;
			return false;
		}

		timeend = cv::getTickCount();
		std::cout << "calibrate, time(0.24)=" << (timeend - timebeg) / cv::getTickFrequency() << std::endl;


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


		// Save Calibrated Data (Aligned)
		std::stringstream ss;
		ss << out_path;
		cv::imwrite(ss.str() + "_cal_range.tiff", cal_range);
		cv::imwrite(ss.str() + "_cal_reflectance.tiff", cal_refec);
		cv::imwrite(ss.str() + "_cal_color.tiff", cal_color);
		cv::imwrite(ss.str() + "_cal_white.tiff", cal_white);

	}
	else
	{
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
		std::vector<std::shared_ptr<SickCam::Ranger3>> pCams(0);
		for (int i = 0; i < ip.size(); ++i) {
			pCams.push_back(std::make_shared<SickCam::Ranger3>(pR3S, ip[0], true));
		}


		// --- Connect Ranger3
		for (int i = 0; i < ip.size(); ++i) {
			auto err = pCams[i]->connectCamera();
			if (SickCam::CAM_STATUS::All_OK != err) {
				print_process_ng(ip[0] + " Connect failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
				print_process_ng("The camera may be connected by other progress!");
				return false;
			}
			pCams[i]->setTimeOut(1);
		}


		// --- Start Ranger3
		for (int i = 0; i < ip.size(); ++i) {
			auto err = pCams[i]->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
			if (SickCam::CAM_STATUS::All_OK != err) {
				print_process_ng(ip[0] + "Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
				return false;
			}

			// 当前（2024/08/31）只能使用 in-PC 标定
			pCams[i]->setCalibration_No();

			err = pCams[i]->startCamera(on_color_image, &calibrationWrapper); // pass variable to callback function
			if (SickCam::CAM_STATUS::All_OK != err) {
				print_process_ng(ip[0] + "Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
				print_process_ng("The camera may be connected by other progress!");
				return false;
			}
		}


		// --- Hold on main thread
		getchar();


		// --- Stop Ranger3
		for (int i = 0; i < ip.size(); ++i)
		{
			auto err = pCams[i]->stopCamera();
			if (SickCam::CAM_STATUS::All_OK != err)
			{
				print_process_ng(ip[0] + "Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
				print_process_ng("The camera may be connected by other progress!");
				return false;
			}
		}

		// --- Disconnect Ranger3
		for (int i = 0; i < ip.size(); ++i)
		{
			auto err = pCams[i]->disconnectCamera();
			if (SickCam::CAM_STATUS::All_OK != err)
			{
				print_process_ng(ip[0] + "Disconnect camera! ErrorCode: " + SickCam::CAM_STATUS_str(err));
				return false;
			}
		}
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
	cop  = loadColorParam("D:/color/config.xml");
	out_path = "D:/color/color-1/";
	josn_path = "D:/color/calibration.json";
	grab_rgbw("D:/color/data/", true);

	return 0;
}
#endif



//////////////////////////////////////////////////////




colorParam loadColorParam(const std::string xmlpath)
{
	colorParam cop;

	//
	cv::FileStorage fs(xmlpath, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::cerr << "Error: Could not open XML file. Path=" << xmlpath << std::endl;
		return colorParam();
	}


	fs["mat_r"] >> cop.mat_r;
	fs["mat_g"] >> cop.mat_g;
	fs["mat_b"] >> cop.mat_b;
	fs["mat_w"] >> cop.mat_w;
	fs["ResizeFactor"] >> cop.ResizeFactor;
	fs["ColorTopBlackRows"] >> cop.ColorTopBlackRows;
	fs["ColorEndBlackRows"] >> cop.ColorEndBlackRows;
	fs["RedGain"] >> cop.RedGain;
	fs["GreenGain"] >> cop.GreenGain;
	fs["BlueGain"] >> cop.BlueGain;
	fs["ReflectanceOrdinaryScale"] >> cop.ReflectanceOrdinaryScale;
	fs["ReflectanceOrdinaryOffset"] >> cop.ReflectanceOrdinaryOffset;
	fs["RedOrdinaryScale"] >> cop.RedOrdinaryScale;
	fs["RedOrdinaryOffset"] >> cop.RedOrdinaryOffset;

	fs.release();
	return cop;
}


//void process_range_reflectance(
//	const colorParam& cop,
//	const cv::Mat& i_ran, const cv::Mat& i_ref, 
//	cv::Mat& dst_ran, cv::Mat& dst_ref
//)
//{
//	//
//	dst_ran = cv::Mat::zeros(i_ran.size(), i_ran.type());
//	dst_ref = cv::Mat::zeros(i_ref.size(), i_ref.type());
//	cv::Mat mark = cv::Mat::zeros(i_ran.size(), CV_8UC1);
//
//	//
//	for (int j = 0; j < i_ran.rows; ++j)
//	{
//		auto ptr_ran = i_ran.ptr<uint16_t>(j);
//		auto ptr_ref = i_ref.ptr<uint8_t>(j);
//
//		for (int i = 0; i < i_ran.cols; ++i)
//		{
//			if (ptr_ran[i] == 0)
//				continue;
//
//
//			auto deltY = ptr_ran[i] * cop.ReflectanceOrdinaryScale + cop.ReflectanceOrdinaryOffset;
//			int y = j - cvRound(deltY);
//
//			if (y < 0 || y >= i_ran.rows)
//				continue;
//
//			// add to mark
//			mark.at<uint8_t>(y, i) = 255; // 255 点不需要插值
//
//			// fill it
//			dst_ran.at<uint16_t>(y, i) = ptr_ran[i];
//			dst_ref.at<uint8_t>(y, i) = ptr_ref[i];
//
//			// fill adjcent pts
//			int yt = y - 1;
//			if (yt < 0 || yt >= i_ran.rows)
//				continue;
//			else
//			{
//				if (dst_ran.at<uint16_t>(yt, i) == 0)
//					dst_ran.at<uint16_t>(yt, i) = ptr_ran[i];
//				else
//				{
//					if (mark.at<uint8_t>(y, i) == 0)
//						dst_ran.at<uint16_t>(yt, i) = (dst_ran.at<uint16_t>(yt, i) + ptr_ran[i]) / 2;
//				}
//
//				if (dst_ref.at<uint8_t>(yt, i) == 0)
//					dst_ref.at<uint8_t>(yt, i) = ptr_ref[i];
//				else
//				{
//					if (mark.at<uint8_t>(y, i) == 0)
//						dst_ref.at<uint8_t>(yt, i) = (dst_ref.at<uint8_t>(yt, i) + ptr_ref[i]) / 2;
//				}
//
//			}
//
//		}
//	}
//}



void adj_ordinary_setup_t(
	const double& scale, const double& offset,
	const cv::Mat& i_ran, const cv::Mat& i_src, cv::Mat& dst_ran, cv::Mat& dst_src,
	bool processRange = false, bool processSrc = false)
{
	//
	if (processRange)
		dst_ran = cv::Mat::zeros(i_ran.size(), i_ran.type());

	if (processSrc)
		dst_src = cv::Mat::zeros(i_src.size(), i_src.type());

	if (!processRange && !processSrc)
	{
		std::cerr << "adj_ordinary_setup: Logic error!" << std::endl;
		return;
	}

	cv::Mat mark = cv::Mat::zeros(i_ran.size(), CV_8UC1);

	//
	for (int j = 0; j < i_ran.rows; ++j)
	{
		auto ptr_ran = i_ran.ptr<uint16_t>(j);
		auto ptr_src = i_src.ptr<uint8_t>(j);

		for (int i = 0; i < i_ran.cols; ++i)
		{
			if (ptr_ran[i] == 0)
				continue;


			auto deltY = ptr_ran[i] * scale + offset;
			int y = j - cvRound(deltY);

			if (y < 0 || y >= i_ran.rows)
				continue;

			// add to mark
			mark.at<uint8_t>(y, i) = 255; // 255 点不需要插值

			// fill it
			if (processRange)
				dst_ran.at<uint16_t>(y, i) = ptr_ran[i];
			
			if (processSrc)
				dst_src.at<uint8_t>(y, i) = ptr_src[i];

			// fill adjcent pts
			int yt = y - 1;
			if (yt < 0 || yt >= i_ran.rows)
				continue;
			else
			{
				if (processRange)
				{
					if (dst_ran.at<uint16_t>(yt, i) == 0)
						dst_ran.at<uint16_t>(yt, i) = ptr_ran[i];
					else
					{
						if (mark.at<uint8_t>(yt, i) == 0)
							dst_ran.at<uint16_t>(yt, i) = (dst_ran.at<uint16_t>(yt, i) + ptr_ran[i]) / 2;
					}
				}

				if (processSrc)
				{
					if (dst_src.at<uint8_t>(yt, i) == 0)
						dst_src.at<uint8_t>(yt, i) = ptr_src[i];
					else
					{
						if (mark.at<uint8_t>(yt, i) == 0)
							dst_src.at<uint8_t>(yt, i) = (dst_src.at<uint8_t>(yt, i) + ptr_src[i]) / 2;
					}
				}
			}

		}
	}
}


// you need to transpose input Mat
void adj_ordinary_setup(
	const double& scale, const double& offset,
	const cv::Mat& i_ran, const cv::Mat& i_src, cv::Mat& dst_ran, cv::Mat& dst_src,
	bool processRange = false, bool processSrc = false)
{
	//
	if (processRange)
		dst_ran = cv::Mat::zeros(i_ran.size(), i_ran.type());

	if (processSrc)
		dst_src = cv::Mat::zeros(i_src.size(), i_src.type());

	if (!processRange && !processSrc)
	{
		std::cerr << "adj_ordinary_setup_t: Logic error!" << std::endl;
		return;
	}

	cv::Mat mark = cv::Mat::zeros(i_ran.size(), CV_8UC1);

	//
	for (int j = 0; j < i_ran.rows; ++j)
	{
		auto ptr_ran = i_ran.ptr<uint16_t>(j);
		auto ptr_src = i_src.ptr<uint8_t>(j);
		
		auto ptr_mark = mark.ptr<uint8_t>(j);
		uint16_t* ptr_dst_ran = nullptr;
		uint8_t*  ptr_dst_src = nullptr;
		if (processRange)
			ptr_dst_ran = dst_ran.ptr<uint16_t>(j);

		if (processSrc)
			ptr_dst_src = dst_src.ptr<uint8_t>(j);
	
		for (int i = 0; i < i_ran.cols; ++i)
		{
			if (ptr_ran[i] == 0)
				continue;

			// Image is transposed, Y is i
			auto deltY = ptr_ran[i] * scale + offset;
			int y = i - cvRound(deltY);

			if (y < 0 || y >= i_ran.cols)
				continue;

			// add to mark
			//mark.at<uint8_t>(y, i) = 255; // 255 点不需要插值
			ptr_mark[y] = 255;

			// fill it
			if (processRange)
				ptr_dst_ran[y] = ptr_ran[i];

			if (processSrc)
				ptr_dst_src[y] = ptr_src[i];

			// fill adjcent pts
			int yt = y - 1;
			if (yt < 0 || yt >= i_ran.cols)
				continue;
			else
			{
				if (processRange)
				{
					if (ptr_dst_ran[yt] == 0)
						ptr_dst_ran[yt] = ptr_ran[i];
					else
					{
						if (ptr_mark[yt] == 0)
							ptr_dst_ran[yt] = (ptr_dst_ran[yt] + ptr_ran[i]) / 2;
					}
				}

				if (processSrc)
				{
					if (ptr_dst_src[yt] == 0)
						ptr_dst_src[yt] = ptr_src[i];
					else
					{
						if (ptr_mark[yt] == 0)
							ptr_dst_src[yt] = (ptr_dst_src[yt] + ptr_src[i]) / 2;
					}
				}
			}

		}
	}
}



// affine(rgbw), adj(all), crop(all),

// i_ran 和 i_ref 的宽度需要是 5120 
void process_rawData_adjustOrdinary_align(
	const colorParam& cop,
	const cv::Mat& i_ran,
	const cv::Mat& i_ref,
	const cv::Mat& i_scatter,
	const cv::Mat& i_r,
	const cv::Mat& i_g,
	const cv::Mat& i_b,
	const cv::Mat& i_w,
	cv::Mat& dst_ran,
	cv::Mat& dst_ref,
	cv::Mat& dst_scatter,
	cv::Mat& dst_red,
	cv::Mat& dst_green,
	cv::Mat& dst_blue,
	cv::Mat& dst_color,
	cv::Mat& dst_white,
	bool is_debug,
	const std::string& debug_img_save_to
)
{
	////
	//cv::resize(i_ran, i_ran, cv::Size(), cop.ResizeFactor, 1.0);
	//cv::resize(i_ref, i_ref, cv::Size(), cop.ResizeFactor, 1.0);


	if (i_ran.empty())
	{
		std::cerr << "Range must provide!" << std::endl;
		return;
	}
	// Crop parameters
	int top1, top2, end1, end2;
	top1 = 0;
	top2 = cop.ColorTopBlackRows;
	end1 = i_ran.rows - cop.ColorEndBlackRows;
	end2 = i_ran.rows;


	// Range and reflectance
	
	adj_ordinary_setup(cop.ReflectanceOrdinaryScale, cop.ReflectanceOrdinaryOffset, i_ran, i_ref, dst_ran, dst_ref, true, !i_ref.empty());
	if (is_debug)
		cv::imwrite(debug_img_save_to + "/adj_range.tiff", dst_ran);

	/// Crop range
	dst_ran.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
	dst_ran.rowRange(end1, end2) = cv::Scalar(0, 0, 0);
	if (is_debug)
		cv::imwrite(debug_img_save_to + "/adj_range_crop.tiff", dst_ran);

	// Crop Reflectance
	if (!i_ref.empty())
	{
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_reflectance.tiff", dst_ref);

		dst_ref.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_ref.rowRange(end1, end2) = cv::Scalar(0, 0, 0);
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_reflectance_crop.tiff", dst_ref);
	}

	// Scatter
	if (!i_scatter.empty())
	{
		// adj_ordinary_setup
		cv::Mat tmp;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_scatter, tmp, dst_scatter, true, !i_scatter.empty());
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_scatter.tiff", dst_scatter);

		// Crop
		dst_scatter.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_scatter.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_scatter_crop.tiff", dst_white);
	}

	// RGB
	if (!i_r.empty() && !i_g.empty() && !i_b.empty())
	{
		// Gain
		cv::Mat i_r2;
		cv::Mat i_g2;
		cv::Mat i_b2;
		i_r2 = i_r * cop.RedGain;
		i_g2 = i_g * cop.GreenGain;
		i_b2 = i_b * cop.BlueGain;
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_r2_gain.tiff", i_r2);
			cv::imwrite(debug_img_save_to + "/adj_g2_gain.tiff", i_g2);
			cv::imwrite(debug_img_save_to + "/adj_b2_gain.tiff", i_b2);
		}

		// Affine
		cv::Mat i_r3;
		cv::Mat i_g3;
		cv::Mat i_b3;
		cv::warpAffine(i_r2, i_r3, cop.mat_r, i_r.size());
		cv::warpAffine(i_g2, i_g3, cop.mat_g, i_g.size());
		cv::warpAffine(i_b2, i_b3, cop.mat_b, i_b.size());
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_r3_warpAffine.tiff", i_r3);
			cv::imwrite(debug_img_save_to + "/adj_g3_warpAffine.tiff", i_g3);
			cv::imwrite(debug_img_save_to + "/adj_b3_warpAffine.tiff", i_b3);
		}

		// adj_ordinary_setup
		cv::Mat tmp;

		cv::Mat tmp_range_red;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_r3, tmp_range_red, dst_red, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_red, i_r3, tmp, dst_red, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_red.tiff", tmp_range_red);
		}

		cv::Mat tmp_range_green;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_g3, tmp_range_green, dst_green, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_green, i_g3, tmp, dst_green, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_green.tiff", tmp_range_green);
		}

		cv::Mat tmp_range_blue;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_b3, tmp_range_blue, dst_blue, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_blue, i_b3, tmp, dst_blue, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_blue.tiff", tmp_range_blue);
		}


		// Merge color
		cv::merge(std::vector<cv::Mat>{dst_blue, dst_green, dst_red}, dst_color);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_color.tiff", dst_color);
			cv::imwrite(debug_img_save_to + "/adj_r.tiff", dst_red);
			cv::imwrite(debug_img_save_to + "/adj_g.tiff", dst_green);
			cv::imwrite(debug_img_save_to + "/adj_b.tiff", dst_blue);
		}

		// Crop 
		dst_color.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_color.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		dst_red.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_red.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		dst_green.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_green.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		dst_blue.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_blue.rowRange(end1, end2) = cv::Scalar(0, 0, 0);
	
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_color_crop.tiff", dst_color);
			cv::imwrite(debug_img_save_to + "/adj_r_crop.tiff", dst_red);
			cv::imwrite(debug_img_save_to + "/adj_g_crop.tiff", dst_green);
			cv::imwrite(debug_img_save_to + "/adj_b_crop.tiff", dst_blue);
		}

	}

	// White
	if (!i_w.empty())
	{
		// Affine
		cv::Mat i_w3;
		cv::warpAffine(i_w, i_w3, cop.mat_w, i_w.size());

		// adj_ordinary_setup
		cv::Mat tmp_range_white, tmp;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_w3, tmp_range_white, dst_white, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_white, i_w3, tmp, dst_white, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_white.tiff", tmp_range_white);
		}

		// Crop
		dst_white.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_white.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_white.tiff", dst_white);
		}

	}


}





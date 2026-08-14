/**
*	@copyright	Copyright 2016-2023 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include "InterfaceOperation.h"
#include <iostream>
#include <map>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <opencv2/opencv.hpp>

std::string path = "";

void SICK_CALLBACK
on_color_image(SickCam::ImgT* pImg, void* any)
{
	SickCam::Ranger3* pC = (SickCam::Ranger3*)any;
	std::cout << pC->get_DeviceIP() << ": " << pImg->collectInfo() << std::endl;
	std::stringstream ss;
	ss << path << pImg->get_ID();


	cv::Mat ref = cv::Mat(pImg->get_rows(), pImg->get_cols(), CV_8UC1, pImg->getData(SickCam::DataNames::REF));
	cv::Mat ran = cv::Mat(pImg->get_rows(), pImg->get_cols(), CV_16UC1, pImg->getData(SickCam::DataNames::RAN));
	
	cv::Mat scatter = cv::Mat(pImg->get_rows(), pImg->get_cols(), CV_8UC1, pImg->getData(SickCam::DataNames::SCA_8));

	cv::Mat white = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::WHITE));
	cv::Mat R = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::R));
	cv::Mat G = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::G));
	cv::Mat B = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::B));

	//cv::Mat color;
	//cv::merge(std::vector<cv::Mat>{B,G,R}, color);

	// 放大 Range 和 Reflectance 图像方便后续对齐。
	cv::resize(ref, ref, R.size());
	cv::resize(ran, ran, R.size());
	cv::resize(scatter, scatter, R.size());

	cv::imwrite(ss.str() + "_ref.tiff", ref);
	cv::imwrite(ss.str() + "_ran.tiff", ran);
	cv::imwrite(ss.str() + "_scatter.tiff", scatter);
	cv::imwrite(ss.str() + "_white.tiff", white);
	//cv::imwrite(ss.str() + "_color.tiff", color);
	cv::imwrite(ss.str() + "_cr.tiff", R);
	cv::imwrite(ss.str() + "_cg.tiff", G);
	cv::imwrite(ss.str() + "_cb.tiff", B);
}

bool SICK_GTEST_LIBRARY init_save_rgb(const std::string& saveto)
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;
	path = saveto;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::CameraShared>(
		"",											/* 日志文件 */
		SickCam::CameraShared::getPathToRanger3Producer(),	/* cti 文件 */
		true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);
	pR3S->scanDevice();


	// --- Check connected device
	std::vector<std::string> ip(0);
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			ip.push_back(sub.first);
			std::cout << "---- " << sub.first << ", sn=" << sub.second->getSN() << std::endl;
		}
	}
	else
	{
		print_process_ng("Error: scan camera error. \n");
		return false;
	}

	//  --- Init Ranger3
	std::vector<std::shared_ptr<SickCam::Ranger3>> pCams(0);
	for (int i = 0; i < ip.size(); ++i)
	{
		pCams.push_back(std::make_shared<SickCam::Ranger3>(pR3S, ip[0], true));
	}


	// --- Connect Ranger3
	for (int i = 0; i < ip.size(); ++i)
	{
		auto err = pCams[i]->connectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng(ip[0] + " Connect failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			print_process_ng("The camera may be connected by other progress!");
			return false;
		}

		pCams[i]->setTimeOut(1);
		std::cout << "\n\n\nBasic Information of " << ip[i] << ":\n" << pCams[i]->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;
	}


	// --- Start Ranger3
	for (int i = 0; i < ip.size(); ++i)
	{
		auto err = pCams[i]->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng(ip[0] + "Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			return false;
		}

		pCams[i]->setCalibration_No();

		err = pCams[i]->startCamera(on_color_image, pCams[i].get()); // pass variable to callback function
		if (SickCam::CAM_STATUS::All_OK != err)
		{
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


	print_process_ok("All done! ");
	return true;
}


//void image_append(const std::string& path, const std::string saveTo)
//{
//	for ()
//	{
//	}
//
//
//}




////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	init_save_rgb("D://1012/dot/");

	return 0;
}
#endif


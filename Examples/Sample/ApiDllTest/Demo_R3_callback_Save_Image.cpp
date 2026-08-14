/**
*	@copyright	Copyright 2016-2021 SICK AG. All rights reserved.
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
#include <Windows.h>
#include <mutex>

std::fstream file;
uint64_t callback_number = 0;
uint64_t total_number = 0;
std::mutex object;

void SICK_CALLBACK
on_get_image_Demo_R3_callback_Save_Image(SickCam::ImgT * pImg, void * any)
{
	{
		std::unique_ptr<std::mutex>(object);
		++callback_number;
		++total_number;
	}


	uint64_t number = pImg->get_ID();
	std::cout << ((SickCam::Ranger3*)any)->getDeviceName() << ": Get Image : " << number << std::endl;
	std::cout << "callback_number = " << callback_number << std::endl;
	file << number << std::endl;
	pImg->SaveCalibratedImagesToIconFile("E:/123/" + std::to_string(number), false);
	
	{
		std::unique_ptr<std::mutex>(object);
		--callback_number;
	}
}

bool Demo_R3_callback_Save_Image()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
		"",											/* 日志文件 */
		SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
		true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);

	pR3S->scanDevice();

	// --- Check connected device
	std::vector<std::shared_ptr<SickCam::Ranger3>> pCams;
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			std::cout << "---- " << sub.first << std::endl;
			//  --- Init Ranger3
			pCams.push_back(std::make_shared<SickCam::Ranger3>(pR3S, sub.first, true));
		}
	}
	else
	{
		print_process_ng("Error: scan camera error. \n");
		return false;
	}

	for (auto pCam : pCams)
	{
		// --- Connect Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->connectCamera());

		int linerate = 2000, nRows = 1000;
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("DeviceScanType", "Linescan3D")); // sensor image -> Areascan; 3D Range-> Linescan3D
																									   //CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("AcquisitionLineRate", std::to_string(linerate)));
																									   //CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("Height_RegionSelector_Scan3dExtraction1", std::to_string(nRows)));
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("ChunkModeActive", "0"));
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->setBufferCount(20));

		std::cout << "\n\n\nBasic Information:\n" << pCam->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;

		//int n_thread = 4; // the number of threads used in in-PC calibraition. Keep it low if you want to lower the cpu using rate.
		//pCam->setCalibration_inPC("D:\\CalibrationResult_0.json", true, 1536, n_thread, 0);

		pCam->setCalibration_inDevice(2048);
	//	pCam->setCalibration_No();
		// --- Start Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->startCamera(on_get_image_Demo_R3_callback_Save_Image, pCam.get()));
	}

	// --- Hold on main thread
	while (true) // 
	{
		//auto key = getchar();
		//if (key == 'q' || key == 'Q')
		//	break;

		if (total_number > 3000)
		{
			break;
		}
	}

	for (auto pCam : pCams)
	{
		// --- Stop Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->stopCamera());

		// --- Disconnect Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->disconnectCamera());
	}

	while (true)
	{
		if (callback_number <= 0)
			break;

		std::cout << "callback_number = " << callback_number << std::endl;
		__sleep1MS(1000);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int Demo_R3_callback_Save_Image(int argc, char* argv[])
{
	if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
		return false;

	//file.open("E:/123/log.txt", std::ios::app);
	Demo_R3_callback_Save_Image();

	return 0;
}
#endif

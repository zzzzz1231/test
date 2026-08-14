/**
*	@copyright	Copyright 2016-2023 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)



#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽

#include <Ranger3.h>			// 此头文件封装了 API 的主体。
//#include "InterfaceOperation.h"
#include <iostream>
#include <map>
#ifdef _WIN32
#include <Windows.h>
#endif


int i = 0;

void
on_callback_function_Demo_R3_01_quick_start_callback(SickCam::ImgT* pImg, void* any)
{
	SickCam::Ranger3* pC = (SickCam::Ranger3*)any;
	std::cout << pImg->get_ID() << " : exp= " << ++i << std::endl;

	if (1 == pImg->get_ID())
	{
		pImg->SaveCalibratedImagesToIconFile("D://fff", true);
	}
}

bool SICK_GTEST_LIBRARY Demo_R3_01_quick_start_callback()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;


	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::CameraShared>(
		"D:/",											/* 日志文件 */
		SickCam::CameraShared::getPathToRanger3Producer(),	/* cti 文件 */
		true,										/* 是否打印信息到屏幕 */
		true										/* 是否打印信息到文件 */
		);
	pR3S->scanDevice();


	// --- Check connected device
	std::vector<std::string> ip(0);
	auto devices = pR3S->getConDevListIP();
	if (pR3S->isAvaliable() && !devices.empty())
	{
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			ip.push_back(sub.first);
			std::cout << "---- " << sub.first << ", sn=" << sub.second->getSN() << std::endl;
		}
	}
	else
	{
		std::cout << "Error: scan camera error. \n";
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
			std::cout << (ip[0] + " Connect failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			std::cout << ("The camera may be connected by other progress!");
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
			std::cout << (ip[0] + "Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			return false;
		}
		//pCams[i]->setParameterValue("TriggerMode_TriggerSelector_LineStart", "Off");
		//pCams[i]->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off");
		//pCams[i]->setParameterValue("AcquisitionLineRate", "3000");
		//pCams[i]->setParameterValue("Height_RegionSelector_Scan3dExtraction1", "1000");

		
		//pCams[i]->enableCallbackThreadsInPool(20);

		//pCams[i]->setCalibration_inDevice(2560);
		pCams[i]->setBufferCount(5);
		err = pCams[i]->startCamera(on_callback_function_Demo_R3_01_quick_start_callback, pCams[i].get()); // pass variable to callback function
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			std::cout << (ip[0] + "Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			std::cout << ("The camera may be connected by other progress!");
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
			std::cout << (ip[0] + "Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			std::cout << ("The camera may be connected by other progress!");
			return false;
		}
	}

	// --- Disconnect Ranger3
	for (int i = 0; i < ip.size(); ++i)
	{
		auto err = pCams[i]->disconnectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			std::cout << (ip[0] + "Disconnect camera! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			return false;
		}
	}


	std::cout << ("All done! ");
	return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	Demo_R3_01_quick_start_callback();
	return 0;
}
#endif
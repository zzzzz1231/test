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

void SICK_CALLBACK
on_callback_function_Demo_R3_enable_CorrectRawImageEdgeEffect(SickCam::ImgT * pImg, void * any)
{
	SickCam::Ranger3* pC = (SickCam::Ranger3*)any;
	std::cout << pC->get_DeviceIP()<< ": " << pImg->collectInfo() << std::endl;

	if (pImg->get_ID() == 1)
	{
		pImg->SaveRawImagesToIconFile("D://ori", false);
	}
}

bool Demo_R3_enable_CorrectRawImageEdgeEffect()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	std::cout << SickCam::VER::__version__() << std::endl;


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
	for (int i=0; i<ip.size(); ++i)
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
		std::cout << "\n\n\nBasic Information of "<< ip[i]<< ":\n" << pCams[i]->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;
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
		pCams[i]->enableCorrectRawImageEdgeEffect(false);

		err = pCams[i]->startCamera(on_callback_function_Demo_R3_enable_CorrectRawImageEdgeEffect, pCams[i].get()); // pass variable to callback function
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

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	Demo_R3_enable_CorrectRawImageEdgeEffect();
	return 0;
}
#endif


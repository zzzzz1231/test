/**	
*	@copyright	Copyright 2016-2024 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/
#ifdef _WIN32
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Trispector.h>	
#include <iostream>


using SickCam::TrispectorShared;
using SickCam::Trispector;

////////////////////////////////////////////////////////////////////////////////
void __stdcall onCallback_tri(SickCam::ImgT * pImg, void * any)
{
	std::cout << "onCallback(), get image - " << pImg->get_ID() << std::endl;
}

bool SICK_GTEST_LIBRARY Demo_Trispector_01_quick_start_callback()
{
	// --- Init Trispector environment
	auto pShared = std::make_shared<TrispectorShared>("",
		TrispectorShared::getPathToTrispectorProducer(),
		true, false);

	// --- Check connected device
	std::string sn = "";
	if (pShared->isAvaliable())
	{
		auto devices = pShared->getConDevListSN();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		sn = devices.begin()->first;
	}
	else
	{
		std::cout << "Error: no camera found. \n" << std::endl;
		std::cout << "有可能是防火墙导致无法发现相机。" << std::endl;
		std::cout << "在 Windows 中，你可以选择关闭“Windows 防火墙”。" << std::endl;
		std::cout << "如果你希望更加安全的运行，可以如下设置，只允许指定的程序可以通过防火墙。" << std::endl;
		std::cout << "在 Windows 中，你可以通过“Windows 防火墙”设置来添加规则：" << std::endl;
		std::cout << "打开“控制面板” > “系统和安全” > “Windows 防火墙” > “允许应用或功能通过防火墙” > “选择你的应用程序” "<< std::endl;

		return false;
	}

	//  --- Init Trispector
	auto pCam = std::make_shared<Trispector>(pShared, sn);

	// --- Connect Trispector
	auto err = pCam->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)	{
		std::cerr << "Connect camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	// --- Start Trispector by callback mode.
	int parameter_pass_to_callback = 10; // class and struct are supported also.
	err = pCam->startCamera(onCallback_tri, & parameter_pass_to_callback);

	if (SickCam::CAM_STATUS::All_OK != err)	{
		std::cerr << "Start camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	// --- Grab images
	std::cout << "Press any key to quit" << std::endl;
	getchar();

	// --- Stop Trispector
	err = pCam->stopCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		std::cerr << "Stop camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	// --- Disconnect Trispector
	err = pCam->disconnectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		std::cerr << "Disconnect camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	return true;
}


#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	std::cout << SickCam::VER::__version_Trispector().c_str() << std::endl;
	std::cout << "请设置程序，可以通过防护墙。设置成功后，回车继续执行。" << std::endl;
	getchar();

	Demo_Trispector_01_quick_start_callback();

	return 0;
}
#endif

#endif
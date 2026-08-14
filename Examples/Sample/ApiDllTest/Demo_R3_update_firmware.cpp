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
#ifdef _WIN32
#include <Windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////

bool SICK_GTEST_LIBRARY Demo_R3_update_firmware(const std::string firmwarePath)
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
		"D:/log",											/* 日志文件 */
		SickCam::Ranger3Shared::getPathToRanger3Producer(),
		true,										/* 是否打印信息到屏幕 */
		true										/* 是否打印信息到文件 */
		);

	if (SickCam::CAM_STATUS::All_OK != pR3S->scanDevice())
	{
		return false;
	}

	// --- Check connected device
	std::vector<std::string> ip(0);
	if (pR3S->isAvaliable()) {
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			ip.push_back(sub.first);
			std::cout << "---- " << sub.first << std::endl;
		}
	}
	else
	{
		print_process_ng("Error: scan camera error. \n");
		return false;
	}

	//  --- Init Ranger3
	auto pCam1 = std::make_shared<SickCam::Ranger3>(pR3S, ip[0], true);

	// --- Connect Ranger3
	auto err = pCam1->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}

	//print_process_ng("Do you want to update firmware? firmware path: " + firmwarePath + "\n\n\n");

	if (pCam1->updateFirmware(firmwarePath))
		print_process_ok("Updating firmware OK!");
	else
		print_process_ng("Updating firmware Failed!");

	// DO NOT CALL DISCONNECT FUNCTION


	// re-scan 
	ip = std::vector<std::string>(0);
	while (ip.empty())
	{
		pR3S->scanDevice(); // scan until find device
		if (pR3S->isAvaliable()) {
			auto devices = pR3S->getConDevListIP();
			for (auto sub : devices)
			{
				ip.push_back(sub.first);
			}
		}
	}

	pCam1 = std::make_shared<SickCam::Ranger3>(pR3S, ip[0], true);
	// --- Connect Ranger3
	err = pCam1->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err) {
		return false;
	}

	// --- Disconnect Ranger3
	if (SickCam::CAM_STATUS::All_OK != pCam1->disconnectCamera()) {
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	Demo_R3_update_firmware("D:/Program Files/SICK_GCN/Ranger3Studio/Ranger3Studio-3.1.0.12065/Firmware/update-all-3.1.0.12065.tgz.aes"); // test valid path

	return 0;
}
#endif


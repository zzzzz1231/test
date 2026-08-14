/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include "Control.h"			// 要启用 OpenCV 请在 "Control.h" 中启用宏 ENABLE_OPENCV
#include <Ranger3.h>			// 此头文件封装了，API 的主体。 
#include "InterfaceOperation.h"
#include <iostream>
#include <map>



bool Demo_R3_get_device_information()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>("",
         SickCam::Ranger3Shared::getPathToRanger3Producer(),
		false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else
	{
		print_process_ng("Error: scan camera error. \n");
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<SickCam::Ranger3>(pR3S, ip, true);

	// --- Connect Ranger3
	auto err = pCam->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}

	//  --- Get parameters
	std::vector<std::string> parameter_readOnly = {
		"DeviceVendorName",
		"DeviceModelName",
		"DeviceVersion",
		"DeviceFirmwareVersion",
		"DeviceSerialNumber",
		"DeviceUserID",
		"DeviceSFNCVersionMajor",
		"DeviceSFNCVersionMinor",
		"DeviceSFNCVersionSubMinor",
		"DeviceTLType",
		"DeviceTLVersionMajor",
		"DeviceTLVersionMinor",
		"DeviceLinkThroughputCurrent",
		"DeviceLinkThroughputLimit",
		"DeviceLinkHeartbeatTimeout",
		"DeviceTemperature",
		"DeviceTemperatureMin",
		"DeviceTemperatureMax",
	};

	for (auto sub : parameter_readOnly)
	{
		std::cout << sub << " = " << pCam->getParameterValue(sub) << std::endl;
	}

	// --- Disconnect Ranger3
	err = pCam->disconnectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		print_process_ng("Disconnect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_R3_get_device_information();
	return 0;
}
#endif


/**
*	@copyright	Copyright 2018-2023 SICK GCN. All rights reserved.
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

bool SICK_GTEST_LIBRARY Demo_R3_simplest_demo()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>("",
        SickCam::Ranger3Shared::getPathToRanger3Producer(),
		true, false);

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

	//  --- Set parameters
	pCam->setParametPath("");	// json / xml 标定文件
	pCam->setCalibraPath("");	// csv 参数文件
	pCam->setScaleY(0.04);		// Y 方向分辨率，根据硬件计算后输入。
	err = pCam->setParameterValue("DeviceScanType", "Areascan"); // sensor image. 若要 3D Range，请设置为 Linescan3D.
	std::cout << "Set grabbing mode to Sensor, return " << SickCam::CAM_STATUS_str(err) << std::endl;

	// --- Start Ranger3 by callback mode or grabbing mode.
	int parameter_pass_to_callback = 10; // class and struct are supported also.
	err = pCam->startCamera();
	if (SickCam::CAM_STATUS::All_OK != err)	{
		print_process_ng("Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
	}

	// --- Grab images
	int n(10);
	//std::cout << "How many images do you want to grab?" << std::endl;
	//std::cout << "请输入要采集的图像数量：" << std::endl;
	//std::cin >> n;

	for (int i = 0; i < n; )
	{
		SickCam::ImgT imgTable;
		try {
			if(SickCam::CAM_STATUS::All_OK == pCam->getImageData(imgTable))
			    std::cout << ++i << " Image : " << imgTable.collectInfo() << std::endl;
		}
		catch (...) { continue; }
	}

	// --- Stop Ranger3
	err = pCam->stopCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		print_process_ng("Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
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
	// 打印相机 SDK 的版本信息
	std::cout << SickCam::VER::__version__().c_str() << std::endl;

	// 示例。建议查看 callback 示例。 
	Demo_R3_simplest_demo();

	return 0;
}
#endif


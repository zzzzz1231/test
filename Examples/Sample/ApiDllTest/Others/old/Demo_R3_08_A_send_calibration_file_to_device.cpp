/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	3.0.0.2
*	@date		2021.02.01
*/

#ifdef _WIN32
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Control.h"			// 要启用 OpenCV 请在 "Control.h" 中启用宏 ENABLE_OPENCV
#include "InterfaceOpenCV.h"	// 此头文件封装了，OpenCV 的部分函数，仅用于示例程序中的图像显示。
#include <Ranger3.h>			// 此头文件封装了，API 的主体。 

#include <iostream>
#include <map>


template<typename T> using SPtr = std::shared_ptr<T>;
template<typename T> using Uptr = std::unique_ptr<T>;

using SickCam::Ranger3Shared;
using SickCam::Ranger3;


bool test_simple()
{
	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", 
		Ranger3Shared::getPathToRanger3Producer(),
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
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);

	// --- Connect Ranger3
	auto err = pCam->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)	{
		std::cerr << "Connect camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	// --- Send calibration file to Ranger3
	err = pCam->sendFileToCamera("..\\..\\scatter\\SN19020016_CalibrationResult.json");
	if (SickCam::CAM_STATUS::All_OK != err) {
		std::cerr << "Send file to Ranger3 failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}
	else
	{
		std::cout << "\n\nSend ok!\n\n" << std::endl;
	}

	// --- [CHECK] : Get calibration file from Ranger3
	auto fileString = pCam->retrieveFileFromCamera();
	if (fileString.empty()) {
		std::cerr << "Retrieve file from Ranger3 failed! " << std::endl;
		return false;
	}
	else
	{
		std::cout << "The file you just sent is : \n\n" << fileString << std::endl;
		std::cout << "\n\nProcess ok!\n\n" << std::endl;
	}

	// --- Disconnect Ranger3
	err = pCam->disconnectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		std::cerr << "Disconnect camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	/**************************************************************\
	* 发送标定文件到相机
	* 
	* 注意：【 相机固件版本必须为 2.3 以上，推荐使用 2.4.0.5459 】
	* 注意：只能使用 json 格式的文件
	\**************************************************************/
	test_simple();

	return 0;
}

#endif
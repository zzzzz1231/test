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

#include "Control.h"
#include "InterfaceOpenCV.h"
#include <Ranger3.h>			

#include <iostream>
#include <map>


template<typename T> using SPtr = std::shared_ptr<T>;
template<typename T> using Uptr = std::unique_ptr<T>;

using SickCam::Ranger3Shared;
using SickCam::Ranger3;


void __stdcall
onCallbackCalibration(SickCam::ImgT * pImg, void * any)
{
	std::cout << "Press any key to quit" << std::endl;
	std::cout << "Get image - " << pImg->collectInfo() << std::endl;
	pImg->SaveToIconFile("D:\\calibrated_image_from_ranger3", { SickCam::DataNames::RAN_CAL, SickCam::DataNames::REF_CAL }, true);
}

bool test_simple()
{
	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", 
		Ranger3Shared::getPathToRanger3Producer(),
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

	// --- Init calibration from camera.
	err = pCam->setCalibrationByString(pCam->retrieveFileFromCamera());
	if (SickCam::CAM_STATUS::All_OK != err) {
		std::cerr << "Send file to Ranger3 failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	//  --- Set parameters
	err = pCam->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image. If you want 3D Range, set it as Linescan3D.

	// --- Start Ranger3 by callback mode or grabbing mode.
	err = pCam->startCamera(onCallbackCalibration);
	if (SickCam::CAM_STATUS::All_OK != err)	{
		std::cerr << "Start camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	// --- Hold on main thread
	std::cout << "Press any key to quit" << std::endl;
	system("pause");

	// --- Stop Ranger3
	err = pCam->stopCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		std::cerr << "Stop camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
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
	* ONLY in 2.4.0.5459
	\**************************************************************/

	test_simple();

	return 0;
}
#endif

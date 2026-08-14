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

void SICK_CALLBACK
on_get_image_Demo_R3_callback_calibration_in_device_saveImage_Float(SickCam::ImgT * pImg, void * any)
{
	//std::cout << ((SickCam::Ranger3*)any)->getDeviceName() << ": Get Image : " << pImg->collectInfo() << std::endl;
	auto id = pImg->get_ID();
	std::stringstream ss;
	ss << ss.str() << id << "";
	pImg->SaveCalibratedImagesToIconFile(ss.str(), false);
}

bool Demo_R3_callback_calibration_in_device_saveImage_Float()
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
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("DeviceScanType", "Linescan3D")); // sensor image -> Areascan; 3D Range-> Linescan3D
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("AcquisitionLineRate", std::to_string(linerate)));
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("Height_RegionSelector_Scan3dExtraction1", std::to_string(nRows)));
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->setParameterValue("ChunkModeActive", "0"));
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->setBufferCount(linerate / nRows * 2));
		
		std::cout << "\n\n\nBasic Information:\n" << pCam->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;

		int n_thread = 4; // the number of threads used in in-PC calibraition. Keep it low if you want to lower the cpu using rate.
		pCam->setCalibration_inDevice(2560, false);
		// --- Start Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->startCamera(on_get_image_Demo_R3_callback_calibration_in_device_saveImage_Float, pCam.get()));
	}

	// --- Hold on main thread
	//while (true) // 
	//{
	//	auto key = getchar();
	//	if (key == 'q' || key == 'Q')
	//		break;
	//}

	//getchar();
	for (int i = 0; i < 10; ++i)
	{
		__sleep1MS(1000); // 1 sec
	}

	for (auto pCam : pCams)
	{
		// --- Stop Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->stopCamera());

		// --- Disconnect Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam->disconnectCamera());
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_R3_callback_calibration_in_device_saveImage_Float();
	return 0;
}
#endif


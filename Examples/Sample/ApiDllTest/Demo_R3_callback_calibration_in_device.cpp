/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
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
on_callback_function_Demo_R3_callback_calibration_in_device(SickCam::ImgT * pImg, void * any)
{
	std::cout << pImg->collectInfo() << std::endl;
}

bool Demo_R3_callback_calibration_in_device()
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
		return false;
	}

	std::cout << "\n\n\n\n\n" << pCam1->getDeviceInfoAsStructureString() << "\n\n\n\n\n" << std::endl;
	std::cout << "Please make sure device firmwawre version is higher than 2.6.0.0! \n\nPress any kep to continue, or close this window to quit.\n\n\n\n" << std::endl;
	getchar();

    err = pCam1->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        return false;
    }

	int linerate = 2000, nRows = 3000;
	CHECK(SickCam::CAM_STATUS::All_OK == pCam1->setParameterValue("DeviceScanType", "Linescan3D")); // sensor image -> Areascan; 3D Range-> Linescan3D
	CHECK(SickCam::CAM_STATUS::All_OK == pCam1->setParameterValue("AcquisitionLineRate", std::to_string(linerate)));
	CHECK(SickCam::CAM_STATUS::All_OK == pCam1->setParameterValue("Height_RegionSelector_Scan3dExtraction1", std::to_string(nRows)));
	CHECK(SickCam::CAM_STATUS::All_OK == pCam1->setParameterValue("TriggerMode_TriggerSelector_LineStart", "Off"));
	CHECK(SickCam::CAM_STATUS::All_OK == pCam1->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off"));
	CHECK(SickCam::CAM_STATUS::All_OK == pCam1->setParameterValue("ChunkModeActive", "0"));
	CHECK(SickCam::CAM_STATUS::All_OK == pCam1->setBufferCount(20));

	// --- Activate in-device Calibration
	err = pCam1->setCalibration_inDevice(2560);
	//err = pCam1->setCalibration_No();
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng("setCalibration_inDevice failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
	}

	// --- Start Ranger3
    err = pCam1->startCamera(on_callback_function_Demo_R3_callback_calibration_in_device);
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        return false;
    }

	// --- Hold on main thread
	//getchar();
	for (int i = 0; i < 10; ++i)
	{
		__sleep1MS(1000); // 1 sec
	}

    // --- Stop Ranger3
    err = pCam1->stopCamera();
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        print_process_ng("The camera may be connected by other progress!");
        return false;
    }

	// --- Disconnect Ranger3
	if (SickCam::CAM_STATUS::All_OK != pCam1->disconnectCamera()){
		print_process_ng("Disconnect camera! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
		return -1;
#endif

	Demo_R3_callback_calibration_in_device();
	return 0;
}
#endif

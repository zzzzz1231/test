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

uint64_t last_id2=0;
int error_time2 = 0;

void SICK_CALLBACK
on_callback_function_Demo_R3_callback_highspeed(SickCam::ImgT * pImg, void * any)
{
	uint64_t c_id = pImg->get_ID();
	if (c_id - last_id2 == 1)
		std::cout << (last_id2 = c_id) << std::endl;
	else
	{
		std::stringstream ss;
		ss << "c_id=" << c_id << ", last_id=" << last_id2;
		print_process_ng(ss.str());
		++error_time2;
	}

	if (error_time2>10)
	{
		exit(-99);
	}

	__sleep1MS(100);
}

bool Demo_R3_callback_highspeed()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::CameraShared>(
		"",											/* 日志文件 */
		SickCam::CameraShared::getPathToRanger3Producer(),	/* cti 文件 */
        true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);


	//pR3S->scanDevice();


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

	std::cout << "\n\n\nBasic Information:\n" << pCam1->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;

    // --- Start Ranger3
	err = pCam1->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
	err = pCam1->setParameterValue("AcquisitionLineRate", "1000"); // 
	err = pCam1->setParameterValue("Height_RegionSelector_Scan3dExtraction1", "2"); 
	if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        return false;
    }
	
	//int n_thread = 4; // the number of threads used in in-PC calibraition. Keep it low if you want to lower the cpu using rate.
	//pCam1->setCalibration_inPC("D:\\UserFile.json", true, 10, n_thread);
	//pCam1->setCalibration_No();
	auto aa = pCam1->setCalibration_inDevice(3200);
	pCam1->setBufferCount(400);
	pCam1->enableCallbackThreadsInPool(400, true);
    err = pCam1->startCamera(on_callback_function_Demo_R3_callback_highspeed, nullptr, 0);
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        print_process_ng("The camera may be connected by other progress!");
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
	Demo_R3_callback_highspeed();
	return 0;
}
#endif


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

void SICK_CALLBACK
on_callback_function_Demo_R3_CalibratedAC(SickCam::ImgT * pImg, void * any)
{
	//std::cout << pImg->get_ID() << std::endl;
	std::cout << pImg->collectInfo() << std::endl;
	if(pImg->get_ID() == 1)
		pImg->SaveCalibratedImagesToIconFile("D://SDK-TEST-AC", true);

	//if (pImg->get_ID() == 1)
	//	pImg->SaveRawImagesToIconFile("D://SDK-TEST_imageR-AC-16", true);
}

bool SICK_GTEST_LIBRARY Demo_R3_CalibratedAC()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;
	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
		"",											/* 日志文件 */
		SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
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
	if (SickCam::CAM_STATUS::All_OK != err) 
	{
		print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}
	//pCam1->setTimeOut(1);
	//pCam1->setBufferCount(50);
	
	std::cout << "\n\n\nBasic Information:\n" << pCam1->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;

    // --- Start Ranger3
	//err = pCam1->setParameter("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
	//err = pCam1->setParameter("Height_RegionSelector_Scan3dExtraction1", "1000"); // 
    if (SickCam::CAM_STATUS::All_OK != err)
	{
        print_process_ng("Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        return false;
    }
	
	pCam1->setCalibration_inDevice_CalibrateAC(true, true, false);
	//pCam1->setCalibration_inDevice_CalibrateAC(false, false);
	//pCam1->setCalibration_inDevice(2560);
	//pCam1->setCalibration_inDevice(3200, true);

	err = pCam1->startCamera(on_callback_function_Demo_R3_CalibratedAC);
    if (SickCam::CAM_STATUS::All_OK != err) 
	{
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
    if (SickCam::CAM_STATUS::All_OK != err) 
	{
        print_process_ng("Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        print_process_ng("The camera may be connected by other progress!");
        return false;
    }

	// --- Disconnect Ranger3
	if (SickCam::CAM_STATUS::All_OK != pCam1->disconnectCamera())
	{
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
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	//Demo of loading image with RangeA
	//SickCam::ImgT imgT;
	//imgT.loadFromIconFile("D:/aaa", 0.1, 1.0, false);
	//imgT.SaveCalibratedImagesToIconFile("D:/aaa4", true, false);

	Demo_R3_CalibratedAC();
	return 0;
}
#endif



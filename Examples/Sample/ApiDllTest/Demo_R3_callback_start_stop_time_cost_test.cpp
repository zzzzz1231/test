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
#include <chrono>

uint64_t last_id=0;

void SICK_CALLBACK
on_callback_function_Demo_R3_callback_start_stop_time_cost_test(SickCam::ImgT * pImg, void * any)
{
    std::cout << "image id = " << pImg->get_ID() << std::endl;
	//uint64_t c_id = pImg->get_ID();
	//if (c_id - last_id == 1)
	//	std::cout << (last_id = c_id) << std::endl;
	//else
	//	print_process_ng(c_id);

}

bool Demo_R3_callback_start_stop_time_cost_test()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
		"",											/* 日志文件 */
		SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
        false,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);


	pR3S->scanDevice();


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
	CHECK(pCam1->connectCamera() == SickCam::CAM_STATUS::All_OK);
	//CHECK(pCam1->setCalibration_inPC("D:\\2.json", true, 2560) == SickCam::CAM_STATUS::All_OK);
	//CHECK(pCam1->setCalibration_inPC("D:\\111.json", true, 2560, 2) == SickCam::CAM_STATUS::All_OK);


	std::cout << "\n\n\nBasic Information:\n" << pCam1->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;
	// sensor image -> Areascan; 3D Range-> Linescan3D
	CHECK_Set_GrabSensorImage(pCam1);
	auto d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	CHECK_Set_GrabRangeImage(pCam1);
	auto end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::cout << "@@@@@@@@@@@@@@@   swith 2D to 3D : " << end - d << " microseconds. \n";

	CHECK_Set_RangeImage_LineRate(pCam1, 4000);
	CHECK_Set_RangeImage_nRows(pCam1, 160);
	CHECK(pCam1->setTimeOut(1) == SickCam::CAM_STATUS::All_OK);

	for (int i=0; i<3; ++i)
	{
		// --- Start Ranger3
		auto d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		//CHECK(pCam1->startCamera(/*on_callback_function, nullptr, 0*/) == SickCam::CAM_STATUS::All_OK);
		CHECK(pCam1->startCamera(on_callback_function_Demo_R3_callback_start_stop_time_cost_test, nullptr, 0) == SickCam::CAM_STATUS::All_OK);
		auto end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		std::cout << "@@@@@@@@@@@@@@@   startCamera : " << end - d << " microseconds. \n";
	
		__sleep1MS(1000);

		//SickCam::ImgT img;
		//while(pCam1->getImageData(img) != SickCam::CAM_STATUS::All_OK)
		//{
		//	//__sleep1MS(500);
		//	if ('q' == getchar())
		//		break;
		//}
		//std::cout << "Img id = " << img.get_ID() << std::endl;

		// --- Stop Ranger3
		d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		CHECK(pCam1->stopCamera() == SickCam::CAM_STATUS::All_OK);
		end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		std::cout << "@@@@@@@@@@@@@@@   endCamera : " << end - d << " microseconds. \n";
		__sleep1MS(1000);
	}

    
	// --- Disconnect Ranger3
	CHECK(pCam1->disconnectCamera() == SickCam::CAM_STATUS::All_OK);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_R3_callback_start_stop_time_cost_test();
	return 0;
}
#endif

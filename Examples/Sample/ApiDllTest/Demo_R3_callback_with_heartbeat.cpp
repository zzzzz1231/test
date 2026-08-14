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


std::set<uint64_t> IMG_IDs; // Mark how many callback functions is on;

void SICK_CALLBACK
on_get_image_Demo_R3_callback_with_heartbeat(SickCam::ImgT * pImg, void * any)
{
    IMG_IDs.insert(pImg->get_ID());
	std::cout << "Get Image : " << pImg->get_ID() << std::endl;
    IMG_IDs.erase(pImg->get_ID());

}

void SICK_CALLBACK
on_lost_device_Demo_R3_callback_with_heartbeat(std::string* name, std::string* ip, std::string* mac, std::string* msg, void * pR3)
{
    std::cout << *msg << std::endl;

    while(!IMG_IDs.empty()) // wait until all on_get_image()s finished.
        __sleep1MS(50);

    auto pCam = (SickCam::Ranger3*)pR3;
    while (true)
    {
        auto ec = pCam->reconnectCamera();
        std::cout << "on_lost_device : " << SickCam::CAM_STATUS_str(ec) << std::endl;
        if(SickCam::CAM_STATUS::All_OK == ec)
        {
            std::cout << "on_lost_device : reconnect device ok." << std::endl;

            // start
            auto err = pCam->startCamera(on_get_image_Demo_R3_callback_with_heartbeat);
            if (SickCam::CAM_STATUS::All_OK != err) {
                print_process_ng("Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
            }

            break;
        }
        std::cout << "on_lost_device : Lost device, reconnect after 1000 ms." << std::endl;
        __sleep1MS(1000);
    }

}

bool Demo_R3_callback_with_heartbeat()
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
	auto err = pCam1->connectCamera(on_lost_device_Demo_R3_callback_with_heartbeat, 1000, pCam1.get());
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}

    // --- Start Ranger3
    err = pCam1->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
    pCam1->setParameterValue("ChunkModeActive", "1");
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        return false;
    }
    err = pCam1->startCamera(on_get_image_Demo_R3_callback_with_heartbeat);
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
    Demo_R3_callback_with_heartbeat();
	return 0;
}
#endif

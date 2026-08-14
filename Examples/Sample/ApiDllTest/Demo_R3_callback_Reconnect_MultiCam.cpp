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

const int try_n_time = 5;
std::map<std::string, int> try_n_times_when_lost;

std::mutex lock_pCams;
std::map<std::string, std::shared_ptr<SickCam::Ranger3>> pCams; // ip, ptr_r3

#ifdef _WIN32
const std::string json_path = "D:\\111.json";
#endif
#ifdef __linux__
const std::string json_path = "/home/alex/111.json";
#endif


void SICK_CALLBACK
on_get_image_Demo_R3_callback_Reconnect_MultiCam(SickCam::ImgT * pImg, void * any)
{
	std::cout <<"@@@@@@@@@@ "<< ((SickCam::Ranger3*)any)->getDeviceName() << ": Get Image : " << pImg->get_ID() << (pImg->has(SickCam::DataNames::RAN_CAL) ? ", CAL" : "")<< std::endl;
	//__sleep1MS(10000);
	//std::cout << "@@@@@@@@@@ " << ((SickCam::Ranger3*)any)->getDeviceName() << ": Get Image : " << pImg->get_ID() << " Done. "<< std::endl;
}

void SICK_CALLBACK
on_lost_device_Demo_R3_callback_Reconnect_MultiCam(std::string* _name, std::string* _ip, std::string* _mac, std::string* _msg, void * pR3)
{
	std::string name(*_name), ip(*_ip), mac(*_mac), msg(*_msg);

    std::cout << msg << std::endl;
    auto pCam = (SickCam::Ranger3*)pR3;
	try_n_times_when_lost.insert({mac, try_n_time});
    while (try_n_times_when_lost[mac] > 0)
    {
        auto ec = pCam->reconnectCamera();
        if(SickCam::CAM_STATUS::All_OK == ec)
        {
            std::cout << "@@@@@@@@@@ on_lost_device [ " << name << "]: reconnect device ok." << std::endl;
			if (SickCam::CAM_STATUS::All_OK == pCam->startCamera(on_get_image_Demo_R3_callback_Reconnect_MultiCam, pCam)) // start
			{
				print_process_ok("@@@@@@@@@@ on_lost_device [ " + name + "]: re-found device!");
				try_n_times_when_lost.erase(mac);
				return;
			}
        }

		try_n_times_when_lost[mac]--;
		print_process_ng("@@@@@@@@@@ on_lost_device [ " +  name + "]: Lost device, reconnect after 1000 ms. It will still try " + std::to_string(try_n_times_when_lost[mac]) + " times.");
        __sleep1MS(1000);
    }

	print_process_ng("@@@@@@@@@@ on_lost_device [ " + name + "]: Lost device forever! Free this device pointer!!");
	try_n_times_when_lost.erase(mac);

	std::unique_lock<std::mutex> lock(lock_pCams);
	pCams.erase(ip);
	auto a = pCams.size();
	print_process_ng("@@@@@@@@@@ on_lost_device [ " + name + "]: deleted " + ip + " from pCams!");
	print_process_ok("@@@@@@@@@@ 3 pCams.size() = " + std::to_string(pCams.size()));

}

bool Demo_R3_callback_Reconnect_MultiCam()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "@@@@@@@@@@ sdk version number: " << SickCam::VER::version_number << std::endl;
	std::cout << "@@@@@@@@@@ sdk version time: " << SickCam::VER::version_time << std::endl;
	
	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
		"",											/* 日志文件 */
		SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
		true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);
	pR3S->scanDevice();

	// --- Check connected device

	if (pR3S->isAvaliable())
	{
        auto devices = pR3S->getConDevListIP();
		print_process_ok("@@@@@@@@@@ Found: " );
        for (auto sub : devices)
		{
			print_process_ok("@@@@@@@@@@     ---- " + sub.first + ", " + sub.second->mDeviceName);
			//  --- Init Ranger3
			
			std::unique_lock<std::mutex> lock(lock_pCams);
			pCams.insert({ sub.first, std::make_shared<SickCam::Ranger3>(pR3S, sub.first, true) });
        }
	}
	else
	{
		print_process_ng("@@@@@@@@@@ Error: scan camera error. \n");
		return false;
	}

	for (auto pCam : pCams)
	{
		// --- Connect Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->connectCamera(on_lost_device_Demo_R3_callback_Reconnect_MultiCam, 2000, pCam.second.get()));

		int linerate = 2000, nRows = 3000;
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setParameterValue("DeviceScanType", "Linescan3D")); // sensor image -> Areascan; 3D Range-> Linescan3D
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setParameterValue("AcquisitionLineRate", std::to_string(linerate)));
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setParameterValue("Height_RegionSelector_Scan3dExtraction1", std::to_string(nRows)));
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setParameterValue("TriggerMode_TriggerSelector_LineStart", "Off"));
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off"));
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setParameterValue("ChunkModeActive", "0"));
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setBufferCount(20));
		//CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->setCalibration_inPC(json_path, true, 3000, 2));

		// --- Start Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->startCamera(on_get_image_Demo_R3_callback_Reconnect_MultiCam, pCam.second.get()));
	}

	print_process_ok("@@@@@@@@@@ 4 pCams.size() = " + std::to_string(pCams.size()));

	// --- Hold on main thread
	while (true)
	{
		std::string aline = "";
		std::getline(std::cin, aline);
		auto key = aline[0];
		if (key == 'q' || key =='Q')
			break;

		if (key == 's' || key == 'S')
		{
			for (auto pCam : pCams)
			{
				// --- Start Ranger3
				if (pCam.second->getStatus() == SickCam::CAM_STATUS::CAM_IS_STARTED)
				{
					CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->stopCamera());
				}
				else
				{
					CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->startCamera(on_get_image_Demo_R3_callback_Reconnect_MultiCam, pCam.second.get()));
				}
			}
			continue;
		}

		if (key == 'd' || key == 'D')
		{
			for (auto pCam : pCams)
			{
				// --- Disconnect Ranger3
				if (pCam.second->getStatus() == SickCam::CAM_STATUS::CAM_IS_CONNECTED || pCam.second->getStatus() == SickCam::CAM_STATUS::CAM_IS_STOPPED)
				{
					CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->disconnectCamera());
				}
				
			}
			pCams.clear();
			continue;
		}


		// simulate scan-device button event.
		if (SickCam::CAM_STATUS::All_OK == pR3S->scanDevice())
		{
			auto devices = pR3S->getConDevListIP(true); // Note: sometimes, different device's IPs can be the same. If so, use device name or macaddresss instead.
			for (auto candidate : devices) // devices: include old and new devices
			{
				//  --- Init Ranger3
				auto ptr = std::make_shared<SickCam::Ranger3>(pR3S, candidate.first, true);

				// --- Connect Ranger3
				CHECK(SickCam::CAM_STATUS::All_OK == ptr->connectCamera(on_lost_device_Demo_R3_callback_Reconnect_MultiCam, 2000, ptr.get()));

				int linerate = 2000, nRows = 1000;
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setParameterValue("DeviceScanType", "Linescan3D")); // sensor image -> Areascan; 3D Range-> Linescan3D
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setParameterValue("AcquisitionLineRate", std::to_string(linerate)));
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setParameterValue("Height_RegionSelector_Scan3dExtraction1", std::to_string(nRows)));
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setParameterValue("TriggerMode_TriggerSelector_LineStart", "Off"));
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off"));
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setParameterValue("ChunkModeActive", "0"));
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setCalibration_inDevice(2560));
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setCalibration_inPC(json_path,true, 3000, 12));
				
				//CHECK(SickCam::CAM_STATUS::All_OK == ptr->setBufferCount(20));
				CHECK(SickCam::CAM_STATUS::All_OK == ptr->startCamera(on_get_image_Demo_R3_callback_Reconnect_MultiCam, ptr.get()));
				
				std::unique_lock<std::mutex> lock(lock_pCams);
				if (pCams.count(candidate.first) != 0)
					pCams.erase(candidate.first);

				pCams.insert({ candidate.first, ptr });
			}
		}
	}

	for (auto pCam : pCams)
	{
		// --- Stop Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->stopCamera());

		// --- Disconnect Ranger3
		CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->disconnectCamera());
	}
	pCams.clear();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_R3_callback_Reconnect_MultiCam();
	return 0;
}
#endif

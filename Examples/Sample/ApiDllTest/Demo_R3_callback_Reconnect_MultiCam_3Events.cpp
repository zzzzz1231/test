/**
*	@copyright	Copyright 2018-2026 SICK GCN. All rights reserved.
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

namespace Demo_R3_callback_Reconnect_MultiCam_3Events
{

	std::map<std::string, std::shared_ptr<SickCam::Ranger3>> pCams; // ip, ptr_r3

#ifdef _WIN32
	const std::string json_path = "D:\\111.json";
#endif
#ifdef __linux__
	const std::string json_path = "/home/alex/111.json";
#endif


	void SICK_CALLBACK
		on_get_image(SickCam::ImgT* pImg, void* any)
	{
		std::cout << ((SickCam::Ranger3*)any)->get_DeviceIP() << ": Get Image : " << pImg->get_ID() << std::endl;
	}

	void SICK_CALLBACK
		on_lost(std::string* _name, std::string* _ip, std::string* _mac, std::string* _msg, void* pR3)
	{
		std::string name(*_name), ip(*_ip), mac(*_mac), msg(*_msg);
		print_process_ng(ip + " is lost!");
		print_process_ng(msg);
	}

	void SICK_CALLBACK
		on_lost_connect_ok(std::string* _name, std::string* _ip, std::string* _mac, std::string* _msg, void* pR3)
	{
		print_process_ng(*_ip + " is connect OK!");
		auto r3 = (SickCam::Ranger3*)(pR3);
		r3->startCamera(on_get_image);
	}

	void SICK_CALLBACK
		on_lost_connect_failed(std::string* _name, std::string* _ip, std::string* _mac, std::string* _msg, void* pR3)
	{
		print_process_ng(*_ip + " is connect failed!");
	}


	bool SICK_GTEST_LIBRARY Demo_R3_callback_Reconnect_MultiCam_3E()
	{
		// --- Init Ranger3 environment
		auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
			"D://",											/* 日志文件 */
			SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
			true,										/* 是否打印信息到屏幕 */
			false										/* 是否打印信息到文件 */
			);

		pR3S->scanDevice();

		// --- Check connected device

		if (pR3S->isAvaliable())
		{
			auto devices = pR3S->getConDevListIP();
			for (auto sub : devices)
			{
				print_process_ok("@@@@@@@@@@     ---- " + sub.first + ", " + sub.second->mDeviceName);
				//  --- Init Ranger3
				pCams[sub.first] = std::make_shared<SickCam::Ranger3>(pR3S, sub.first, true);
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
			CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->connectCamera(120, 1000, 900,
				on_lost, on_lost_connect_ok, on_lost_connect_failed));

			CHECK(pCam.second->E2U_3DMode());
			CHECK(pCam.second->E2U_GrabBy_FreeRunning());
			// --- 设置图像采集的线程池
			CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->enableCallbackThreadsInPool(20, true));
			// --- Start Ranger3
			CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->startCamera(on_get_image));
		}

		print_process_ok("@@@@@@@@@@ 4 pCams.size() = " + std::to_string(pCams.size()));

		// --- Hold on main thread
		while (true)
		{
			std::string aline = "";
			std::getline(std::cin, aline);
			auto key = aline[0];
			if (key == 'q' || key == 'Q')
				break;

			if (key == 's' || key == 'S')
			{
				for (auto pCam : pCams)
				{
					if (pCam.second->getStatus() == SickCam::CAM_STATUS::CAM_IS_STARTED)
					{
						CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->stopCamera());
					}
					else
					{
						CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->startCamera(on_get_image, pCam.second.get()));
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
				// Note: sometimes, different device's IPs can be the same. If so, use device name or macaddresss instead.
				auto devices = pR3S->getConDevListIP(true);

				for (auto candidate : devices) // new devices
				{
					//  --- Init Ranger3
					auto ptr = std::make_shared<SickCam::Ranger3>(pR3S, candidate.first, true);

					// --- Connect Ranger3
					CHECK(SickCam::CAM_STATUS::All_OK == ptr->connectCamera(120, 1000, 900,
						on_lost, on_lost_connect_ok, on_lost_connect_failed));

					CHECK(ptr->E2U_3DMode());
					CHECK(ptr->E2U_GrabBy_FreeRunning());

					CHECK(SickCam::CAM_STATUS::All_OK == ptr->startCamera(on_get_image));

					pCams[candidate.first] = ptr;
				}
			}
		}

		for (auto pCam : pCams)
		{
			CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->stopCamera());
			CHECK(SickCam::CAM_STATUS::All_OK == pCam.second->disconnectCamera());
		}
		pCams.clear();
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
	int main(int argc, char* argv[])
	{
		Demo_R3_callback_Reconnect_MultiCam_3E();
		return 0;
	}
#endif

}
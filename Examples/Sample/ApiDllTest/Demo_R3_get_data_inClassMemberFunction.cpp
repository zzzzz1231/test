/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include "Control.h"			// 要启用 OpenCV 请在 "Control.h" 中启用宏 ENABLE_OPENCV
#include <Ranger3.h>			// 此头文件封装了，API 的主体。 
#include "InterfaceOperation.h"
#include <iostream>
#include <map>


namespace Demo_R3_get_data_inClassMemberF
{

	class aaa
	{
	public:
		aaa() {}

		void start()
		{
			// 使用匿名函数来避免静态成员，作为回调函数
			auto trampoline = [](SickCam::ImgT* img, void* pCam)
			{
				std::cout << img->get_ID() << std::endl;
			};

			pCam->startCamera(trampoline, this);
		}

		std::shared_ptr<SickCam::Ranger3Shared> pR3S;
		std::shared_ptr<SickCam::Ranger3> pCam;
	};



	bool SICK_GTEST_LIBRARY Demo_R3_get_data_inClassMemberFunction()
	{
		std::cout << "Demo Function Name: " << __func__ << std::endl;
		std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
		std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

		aaa A;

		// --- Init Ranger3 environment
		A.pR3S = std::make_shared<SickCam::Ranger3Shared>("",
			SickCam::Ranger3Shared::getPathToRanger3Producer(),
			false, false);

		// --- Check connected device
		std::string ip = "";
		if (A.pR3S->isAvaliable())
		{
			auto devices = A.pR3S->getConDevListIP();
			std::cout << "Found: " << std::endl;
			for (auto sub : devices)
				std::cout << "---- " << sub.first.c_str() << std::endl;
			ip = devices.begin()->first;
		}
		else
		{
			print_process_ng("Error: scan camera error. \n");
			return false;
		}

		//  --- Init Ranger3
		A.pCam = std::make_shared<SickCam::Ranger3>(A.pR3S, ip, true);

		// --- Connect Ranger3
		auto err = A.pCam->connectCamera();
		if (SickCam::CAM_STATUS::All_OK != err) {
			print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			print_process_ng("The camera may be connected by other progress!");
			return false;
		}

		//A.pCam->startCamera(iii, A.pCam.get(), 0);
		A.start();

		Sleep(2000);

		A.pCam->stopCamera();

		// --- Disconnect Ranger3
		err = A.pCam->disconnectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng("Disconnect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			return false;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
	int main(int argc, char* argv[])
	{
		Demo_R3_get_data_inClassMemberFunction();
		return 0;
	}
#endif

}
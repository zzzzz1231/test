/**	
*	@copyright	Copyright 2016-2021 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/
#ifdef _WIN32
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Trispector.h>	
#include <iostream>


using SickCam::TrispectorShared;
using SickCam::Trispector;

////////////////////////////////////////////////////////////////////////////////

bool SICK_GTEST_LIBRARY Demo_Trispector_01_quick_start_single_thread()
{
	// --- Init Trispector environment
	auto pShared = std::make_shared<TrispectorShared>("",
		TrispectorShared::getPathToTrispectorProducer(),
		true, false);

	// --- Check connected device
	std::string sn = "";
	if (pShared->isAvaliable())
	{
		auto devices = pShared->getConDevListSN();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		sn = devices.begin()->first;
	}
	else
	{
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Trispector
	auto pCam = std::make_shared<Trispector>(pShared, sn);

	// --- Connect Trispector
	auto err = pCam->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)	{
		std::cerr << "Connect camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}



	system("pause");


	// --- Start Trispector by grabbing mode.
	err = pCam->startCamera();

	if (SickCam::CAM_STATUS::All_OK != err)	{
		std::cerr << "Start camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	std::cout << "start!" << std::endl;

	// --- Grab images
	for (int i = 0; i < 5;)
	{
		SickCam::ImgT imgTable;
		try {
			err = pCam->getImageData(imgTable);
			if (err == SickCam::CAM_STATUS::All_OK)
				std::cout << i++ << ": Image id = " << imgTable.get_ID() << std::endl;
				
		}
		catch (...) { continue; }
		Sleep(0);
	}

	// --- Stop Trispector
	err = pCam->stopCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		std::cerr << "Stop camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}



	system("pause");


	// --- Disconnect Trispector
	err = pCam->disconnectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		std::cerr << "Disconnect camera failed! ErrorCode: " << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}

	return true;
}

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	std::cout << SickCam::VER::__version__().c_str() << std::endl;

	Demo_Trispector_01_quick_start_single_thread();

	return 0;
}


#endif
#endif

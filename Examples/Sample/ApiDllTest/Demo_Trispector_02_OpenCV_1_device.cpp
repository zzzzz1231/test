/**	
*	@copyright	Copyright 2016-2021 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/
#ifdef _WIN32
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include "InterfaceOpenCV.h"	// 
#include "InterfaceOperation.h"	// 
#include <Trispector.h>			// 

#include <opencv2/opencv.hpp>  // 
#include <iostream>
#include <map>


using SickCam::TrispectorShared;
using SickCam::Trispector;

////////////////////////////////////////////////////////////////////////////////

bool SICK_GTEST_LIBRARY Demo_Trispector_02_OpenCV_1_device(const std::vector<TRISPECTOR>& Cams)
{
	if (Cams.empty()) {
		std::cout << "The Cams(=" << Cams.size() << ") is empty !!!" << std::endl;
		return false;
	}

	// --- Init Trispector environment.
	auto pShared = std::make_shared<TrispectorShared>("",
		TrispectorShared::getPathToTrispectorProducer(),
		true, false);

	// --- Check
	if (pShared->isAvaliable()) {
		auto devices = pShared->getConDevListSN();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
	}
	else
	{
		std::cout << "Error: scan camera error. \n" << std::endl;	return 0;
	}

	//  --- Init Trispector.
	std::map<std::string, TRISPECTOR> mapCams; /// key=IP
	for (auto cam : Cams)
	{
		auto pCam = std::make_shared<Trispector>(pShared, cam.SN);
		if (Ecode::All_OK == pCam->isReady())
		{
			auto element = std::pair<std::string, TRISPECTOR>{ pCam->getDeviceSN(), cam };
			element.second.inner_window_name = pCam->getDeviceName();
			element.second.inner_pCam = pCam;
			element.second.inner_pMP = std::make_shared<mouse_para>();
			element.second.inner_pMP->winName_ran = element.second.inner_window_name;
			element.second.inner_pMP->text = {
				std::string("Press 'Q' or 'q' to quit!"),
				std::string("Press 'C' or 'c' to switch sensor/range!"),
				std::string("Press 'S' or 's' to stop / start acquisition!"),
				std::string("Placeholder"),
				std::string("Placeholder"),
				std::string("Placeholder"),
				std::string("Placeholder"),
				std::string(" = = = = = = = = = = = = = = = = = = = "),
				std::string("Image ID = "), // 8 Do not change the order
				std::string("Position = "), // 9 Do not change the order
				std::string(" = = = = = = = = = = = = = = = = = = = "),
				std::string("Placeholder"), // 11 Do not change the order
				std::string("Placeholder"), // 12 Do not change the order

			};

			cv::namedWindow(element.second.inner_pMP->winName_ran, CV_WINDOW_KEEPRATIO);
			cv::setMouseCallback(element.second.inner_pMP->winName_ran, &(on_mouse_trispector), element.second.inner_pMP.get());

			mapCams.insert(element);
		}
		else
			std::cerr << "Init camera(" << cam.SN << ") fail" << "! \n\nThe recommended IP format is \" 192.168.0.1\";\nThe recommended MAC format is \"00:06:77:0b:d6:e6\"." << endl;
	}

	for (auto subCam : mapCams)
	{
		// --- Connect all Trispector
		auto err = subCam.second.inner_pCam->connectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			std::cerr << "Connect camera(" << subCam.first << ") fail. Error code: " << SickCam::CAM_STATUS_str(err) << std::endl;		return false;
		}

		// --- Set parameters 
		subCam.second.inner_pCam->setTimeOut(4000);
		subCam.second.inner_pCam->setScaleY(0.08);
		subCam.second.inner_pCam->setBufferCount(20);
	}

	bool is_started(false);

	// --- Start all Ranger3 with callback function
	DEMO_TRI::__start_all(is_started, mapCams);

	while (true)
	{
		// --- Image operation is processed by DEMO_TRI::onCallback_2;
		// --- Deal with commands.
		char key = cv::waitKey(10);

		// Start or Stop
		if (key == 's' || key == 'S') {
			if (is_started)				DEMO_TRI::__stop_all(is_started, mapCams);
			else						DEMO_TRI::__start_all(is_started, mapCams);
		}
		else if (key == 'c' || key == 'C')			DEMO_TRI::__switch_mode(is_started, mapCams);
		else if (key == 'q' || key == 'Q')
		{
			DEMO_TRI::__stop_all(is_started, mapCams);
			for (auto subCam : mapCams)// Quit
			{
				subCam.second.inner_pCam->disconnectCamera();
				while (subCam.second.inner_pCam->getStatus() != SickCam::CAM_STATUS::CAM_IS_DISCONNECTED);
				std::cout << "Disconnect " << subCam.first << std::endl;
			}
			break;
		}
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	std::cout << SickCam::VER::__version__().c_str() << std::endl;

	Demo_Trispector_02_OpenCV_1_device(
		{
			TRISPECTOR("18480056")
		}
	);
	return 0;
}
#endif
#endif

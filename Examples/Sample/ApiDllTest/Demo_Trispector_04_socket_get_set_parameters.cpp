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
bool SICK_GTEST_LIBRARY Demo_Trispector_04_socket_get_set_parameters()
{
	// --- Init Trispector environment
	auto pShared = std::make_shared<TrispectorShared>("",
		TrispectorShared::getPathToTrispectorProducer(),
		false, false);

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

	// --- Init Socket
	pCam->socketInit("192.168.1.70", 2115);
	//trispector_socket_example socket_example;
	//socket_example.initSocket(sn, "192.168.1.70", 2115);

	// --- Get / Set Parameters. All availiable commands can be found in /documents/TrispectorDoc/socket_data_fromat_1|2.pdf
	std::cout << "get exposure"		<< " = " << pCam->socketSendCommand("get exposure") << std::endl;
	std::cout << "set exposure 50"	<< " = " << pCam->socketSendCommand("set exposure 50") << std::endl;
	std::cout << "get exposure"		<< " = " << pCam->socketSendCommand("get exposure") << std::endl;

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
	Demo_Trispector_04_socket_get_set_parameters();

	return 0;
}

#endif
#endif
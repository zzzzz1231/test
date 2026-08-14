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
#include <omp.h>


using SickCam::TrispectorShared;
using SickCam::Trispector;

////////////////////////////////////////////////////////////////////////////////

bool SICK_GTEST_LIBRARY Demo_Trispector_05_4_devices()
{
	// --- Init Trispector environment.
	auto time_beg2 = cv::getTickCount();
	auto pShared = std::make_shared<TrispectorShared>("", TrispectorShared::getPathToTrispectorProducer(), false, false);
	auto time_end2 = cv::getTickCount();
	std::cout << "Init environment(include scan()) time: " << (time_end2 - time_beg2) / cv::getTickFrequency() << " sec" << std::endl;

	// --- Check
	std::vector<std::string> SNs;
	if (pShared->isAvaliable())
	{
		auto devices = pShared->getConDevListSN();
		for (auto sub : devices) 
			SNs.push_back(sub.first);
	}
	else
	{
		std::cerr << "Error: scan camera error. \n" << std::endl;
		return 0;
	}

	//  --- Init Trispector.
	auto devices = pShared->getConDevListSN();
	auto device_ptr = std::vector<std::shared_ptr<Trispector>>(devices.size());
	{
		auto time_beg = cv::getTickCount();
#pragma omp parallel for
		for (int i = 0; i < SNs.size(); ++i)
		{
			auto time_beg1 = cv::getTickCount();
			auto pCam = std::make_shared<Trispector>(pShared, SNs[i]);
			auto time_end1 = cv::getTickCount();
			std::cout << SNs[i] <<  ", init time: " << (time_end1 - time_beg1) / cv::getTickFrequency() << " sec" << std::endl;

			if (Ecode::All_OK == pCam->isReady())
				device_ptr[i]=pCam;
			else
				std::cerr << "Init camera(" << SNs[i] << ") fail" << "!" << endl;
		}
		auto time_end = cv::getTickCount();
		std::cout << "Init total time: " << (time_end - time_beg) / cv::getTickFrequency() << " sec" << std::endl;
	}


	//  --- Connect Trispector.
	{
		auto time_beg = cv::getTickCount();
#pragma omp parallel for
		for (int i = 0; i < device_ptr.size(); ++i)
		{
			auto time_beg1 = cv::getTickCount();
			if (Ecode::All_OK != device_ptr[i]->connectCamera())
				continue;
			auto time_end1 = cv::getTickCount();
			std::cout << SNs[i] << ", connect time: " << (time_end1 - time_beg1) / cv::getTickFrequency() << " sec" << std::endl;
		}
		auto time_end = cv::getTickCount();
		std::cout << "Connect total time: " << (time_end - time_beg) / cv::getTickFrequency() << " sec" << std::endl;
	}


	//  --- Start Trispector.
	{
		auto time_beg = cv::getTickCount();
#pragma omp parallel for
		for (int i = 0; i < device_ptr.size(); ++i)
		{
			auto time_beg1 = cv::getTickCount();
			if (Ecode::All_OK != device_ptr[i]->startCamera())
				continue;
			auto time_end1 = cv::getTickCount();
			std::cout << SNs[i] << ", start time: " << (time_end1 - time_beg1) / cv::getTickFrequency() << " sec" << std::endl;
		}
		auto time_end = cv::getTickCount();
		std::cout << "Start total time: " << (time_end - time_beg) / cv::getTickFrequency() << " sec" << std::endl;
	}


	//  --- Stop Trispector.
	{
		auto time_beg = cv::getTickCount();
#pragma omp parallel for
		for (int i = 0; i < device_ptr.size(); ++i)
		{
			auto time_beg1 = cv::getTickCount();
			if (Ecode::All_OK != device_ptr[i]->stopCamera())
				continue;
			auto time_end1 = cv::getTickCount();
			std::cout << SNs[i] << ", stop time: " << (time_end1 - time_beg1) / cv::getTickFrequency() << " sec" << std::endl;
		}
		auto time_end = cv::getTickCount();
		std::cout << "Stop total time: " << (time_end - time_beg) / cv::getTickFrequency() << " sec" << std::endl;
	}

	//  --- Disconnect Trispector.
	{
		auto time_beg = cv::getTickCount();
#pragma omp parallel for
		for (int i = 0; i < device_ptr.size(); ++i)
		{
			auto time_beg1 = cv::getTickCount();
			if (Ecode::All_OK != device_ptr[i]->disconnectCamera())
				continue;
			auto time_end1 = cv::getTickCount();
			std::cout << SNs[i] << ", disconnect time: " << (time_end1 - time_beg1) / cv::getTickFrequency() << " sec" << std::endl;
		}
		auto time_end = cv::getTickCount();
		std::cout << "Disconnect total time: " << (time_end - time_beg) / cv::getTickFrequency() << " sec" << std::endl;
	}


	return true;
}
////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	std::cout << SickCam::VER::__version__().c_str() << std::endl;

	Demo_Trispector_05_4_devices();
	return 0;
}
#endif
#endif

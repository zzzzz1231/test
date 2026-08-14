/**
*	@copyright	Copyright 2018-2023 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/


#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include "ImageGrabber.h"	 // 此头文件封装了，轮询采集图像的相关函数，用于展示采图过程。
#include "InterfaceOpenCV.h" // 此头文件封装了，OpenCV 的部分函数，仅用于示例程序中的图像显示。
#include <Ranger3.h>		 // 此头文件封装了，API 的主体。 

#include <opencv2/opencv.hpp> // OpenCV 仅作为本示例程序中的显示之用，API 并不依赖该库。
#include <iostream>

using STR = std::string;
using SickCam::Ranger3Shared;
using SickCam::Ranger3;
using SickCam::CAM_STATUS;
using SickCam::DataNames;
using SickCam::ImgT;

////////////////////////////////////////////////////////////////////////////////

bool SICK_GTEST_LIBRARY
Demo_R3_thread_grabbing(
	const bool		usingIP,
	const std::string& IP_MAC,
	const SickCam::DataNames& imgType,
	const int& nImg,
	const std::string& PARAM_CSV_PATH,
	const std::string& CALIB_XML_PATH
	)
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment.
	auto pR3S = std::make_shared<Ranger3Shared>("", SickCam::CameraShared::getPathToRanger3Producer(), false, false);

	// --- Check
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevList();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			std::cout << "---- " << sub.first.c_str() << std::endl;
		}
	}
	else
	{
		printf("Error: scan camera error. \n");
		return 0;
	}

	//  --- Init ImageGrabber for one ranger3. // 使用 mac 地址来识别相机前，请确保相机和电脑的 IP 符合要求。
	auto pIG = std::make_unique<ImageGrabber>(pR3S, IP_MAC, usingIP);
	if (Ecode::All_OK != pIG->getR3()->isReady())
	{
		printf("Error: init camera failed! \n");
		return false;
	}

	/// 操作 OpenCV 相关函数的指针，仅作为显示之用。
	auto pIF = std::make_unique<InterfaceOpenCV>();

	if (false == pIG->connect()) 
	{
		std::cerr << "Init camera fail." << std::endl;
		return false;
	}

	//  --- Init ImageGrabber for one ranger3.
	pIG->setUserParameter(0.02, PARAM_CSV_PATH, CALIB_XML_PATH, SiCaliWrapper::RectMethod::Top, 4000, NAN);
	pIG->getR3()->setBufferCount(20);
	bool ok(false);
	bool to_get_sensor(imgType == DN::SEN);
	
	if (to_get_sensor)
	{
		/// 设置获得 激光线（Sensor）图像
		ok = pIG->setParameter("DeviceScanType", "Areascan"); printf("%d, \n", ok); // Sensor image
	}
	else
	{
		/// 设置获得 3D 高度（Range）数据。
		ok = pIG->setParameter("DeviceScanType", "Linescan3D"); printf("%d, \n", ok); // 3D Range image
		
		// --- Set parameter
		/// 使能 Reflectance 图像。Range 图像是默认一定会输出的。
		ok = pIG->setParameter("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "1");	printf("%d, \n", ok);
		
		/// 设置采集 3000 个剖面。
		ok = pIG->setParameter("Height_RegionSelector_Scan3dExtraction1", "300");	printf("%d, \n", ok);

		// DEBUG 
		ok = pIG->setParameter("Width_RegionSelector_Region1", "2000");	printf("%d, \n", ok);

	}

	//  --- Start a thread for one ranger3.
	std::thread* thread_pImgGabber = new std::thread(&ImageGrabber::run, pIG.get());
	thread_pImgGabber->detach();
	delete thread_pImgGabber;

	// --- Prepare to show image
	mouse_para mp;

	mp.winName_ran = to_get_sensor ? "Sensor" : (CALIB_XML_PATH.empty()? "Range" : "Range_calibrated");
#if (CV_VERSION_MAJOR==3)
    cv::namedWindow(mp.winName_ran, CV_WINDOW_KEEPRATIO); // opencv 349
#endif
#if (CV_VERSION_MAJOR==4)
    cv::namedWindow(mp.winName_ran, cv::WINDOW_FREERATIO); // opencv 430
#endif
	cv::setMouseCallback(mp.winName_ran, &(on_mouse_ran), &mp);
	std::cout << "Press 'Q' or 'q' to quit! " << std::endl;
	std::cout << "Press 'C' or 'c' to switch sensor/range! " << std::endl;
	std::cout << "Press 'S' or 's' to start/stop acquisition! " << std::endl;

	if (false == pIG->requireStartAcquition())
	{
		std::cerr << "Start camera fail." << std::endl;
		return false;
	}
	while (!pIG->isStartAcquition());

	int img_n(1);
	bool is_setParameter(false);
	int number = nImg < 1 ? 100 : nImg;

	auto begin = cv::getTickCount();
	while (number)
	{
		char key = cv::waitKey(10);
		if (key == 's' || key == 'S')
		{
			if (pIG->isStartAcquition())
			{
				pIG->requireStopAcquition();
				while (pIG->isStartAcquition());
				std::cout << "Stopped!" << std::endl;
			}
			else
			{
				pIG->requireStartAcquition();
				while (!pIG->isStartAcquition());
				std::cout << "Started!" << std::endl;
			}
		}

		if (key == 'q' || key == 'Q')
		{
			pIG->requireStopAcquition();
			while (pIG->isStartAcquition());
			std::cout << "Stopped!" << std::endl;

			pIG->requireDisconnect();
			while (!pIG->isDisconnected());
			std::cout << "Disconnected!" << std::endl;
			break;
		}

		if (key == 'c' || key == 'C')
		{
			pIG->requireChangeOutputMode();
			continue;
		}

		if (!pIG->isGrabbed())		continue;

		// --- Get Image.
		mp.pImgD = std::make_unique<SickCam::ImgT>(pIG->getImgT());

		if (mp.pImgD->isEmpty())		continue;

		bool hasRef = mp.pImgD->has(DN::REF);
		bool isSens = mp.pImgD->has(DN::SEN);
		bool hasChu = mp.pImgD->has_ChunkInfo();

		DN RefName = isSens ? DN::SEN : DN::REF;

		uint8_t * pRef = (uint8_t *)mp.pImgD->getData(RefName);

		mp.setI(pIF->uint8_tPtrToMat(pRef, mp.pImgD->get_rows(RefName), mp.pImgD->get_cols(RefName)));

		mp.setR(pIF->uint16_tPtrToMat(
			(uint16_t *)mp.pImgD->getData(DN::RAN),
			mp.pImgD->get_rows(DN::RAN), mp.pImgD->get_cols(DN::RAN)));

		mp.setCR(pIF->floatPtrToMat(
			(float *)mp.pImgD->getData(DN::RAN_CAL),
			mp.pImgD->get_rows(DN::RAN_CAL), mp.pImgD->get_cols(DN::RAN_CAL)));

		if (!isSens)
		{
			if (!mp.org_ran.empty())
			{
				// Prepare to show.
				cv::Mat src = mp.cal_ran.empty() ? mp.org_ran.clone() : mp.cal_ran.clone();
				if (!mp.cal_ran.empty())
					pIF->removeNAN(src);

				cv::normalize(src, src, 0.0, 255.0, cv::NORM_MINMAX);
				src.convertTo(src, CV_8UC1);
				cv::imshow(mp.winName_ran, src);
			}
		}
		else
		{
			if (!mp.org_int.empty())
				cv::imshow(mp.winName_ran, mp.org_int);
		}

		nImg < 1 ? number : --number;
	}

	if (number == 0)
	{
		pIG->requireStopAcquition();
		while (pIG->isStartAcquition());
		std::cout << "Stopped!" << std::endl;

		pIG->requireDisconnect();
		while (!pIG->isDisconnected());
		std::cout << "Disconnected!" << std::endl;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	std::cout << SickCam::VER::__version__().c_str() << std::endl;

	// 相机网段必须和 PC 相同，输出 range 图像，取 10 张图像。
	Demo_R3_thread_grabbing(
		true, 
		"192.168.1.21",
		DN::SEN, 
		-1, 
		"", /* 参数文件 csv */
		"..\\..\\scatter\\SN19020016_CalibrationResult.json");

	return 0;
}
#endif


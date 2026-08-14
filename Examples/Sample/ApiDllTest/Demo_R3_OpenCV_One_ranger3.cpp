/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include "InterfaceOpenCV.h"	// 此头文件封装了，OpenCV 的部分函数，仅用于示例程序中的图像显示。
#include "InterfaceOperation.h"	// 此头文件封装了，demo 中使用到的部分函数，仅用于示例程序中。
#include <Ranger3.h>			// 此头文件封装了，API 的主体。 

#include <iostream>
#include <map>

////////////////////////////////////////////////////////////////////////////////
bool SICK_GTEST_LIBRARY Demo_R3_OpenCV_One_ranger3(std::vector<RANGER3>& Cams)
{
	if (Cams.empty())	{
		std::cout << "The Cams(=" << Cams.size() << ") is empty !!!" << std::endl;
		return false;
	}

	// --- Init Ranger3 environment.
    auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
            "",											/* 日志文件 */
            SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
            true,										/* 是否打印信息到屏幕 */
            false										/* 是否打印信息到文件 */
    );

	// --- Check
	if (pR3S->isAvaliable())	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;

        if(Cams[0].IP_MAC.empty())
            Cams[0].IP_MAC = devices.begin()->first; // 取第一个找到的相机的 ip
	}
	else
	{
		print_process_ng("Error: scan camera error. \n");
		return false;
	}

	//  --- Init ranger3.
	std::map<std::string, RANGER3> mapCams; /// key=IP
	for (auto cam : Cams)
	{
		auto pCam = std::make_shared<SickCam::Ranger3>(pR3S, cam.IP_MAC, cam.is_using_IP);
		if (Ecode::All_OK == pCam->isReady())
		{
			auto element = std::pair<std::string, RANGER3>{ pCam->getIp(), cam };
			element.second.inner_ip				= pCam->getIp();
			element.second.inner_mac			= pCam->getMac();
			element.second.inner_window_name	= element.second.inner_ip + " - " + pCam->getDeviceName();
			element.second.inner_pCam			= pCam;
			element.second.inner_pMP			= std::make_shared<mouse_para>();
			element.second.inner_pMP->winName_ran = element.second.inner_window_name;
			element.second.inner_pMP->text		= {
				std::string("Press 'Q' or 'q' to quit!"),
				std::string("Press 'C' or 'c' to switch sensor/range!"),
				std::string("Press 'S' or 's' to stop / start acquisition!"),
				std::string("Press 'P' or 'p' to set parameters!"),
				std::string("Press 'U' or 'u' to init UesrSet!"),
				std::string("Press 'F' or 'f' to test File!"),
				std::string("Press 'T' or 't' to test UesrSet!"),
				std::string(" = = = = = = = = = = = = = = = = = = = "),
				std::string("Image ID = "),									// 8 Do not change the order
				std::string("Position = "),									// 9 Do not change the order
				std::string(" = = = = = = = = = = = = = = = = = = = "),
				std::string("ExposureTime_RegionSelector_Region0 = "),		// 11 Do not change the order
				std::string("UesrSet = "),									// 12 Do not change the order
				std::string("Press 'W' or 'w' to save image!")				// 13
			};
#ifdef __linux__ // opencv4.3.1 in this testing environment
            cv::namedWindow		(element.second.inner_pMP->winName_ran, cv::WINDOW_KEEPRATIO);
#else
            cv::namedWindow		(element.second.inner_pMP->winName_ran, CV_WINDOW_KEEPRATIO);
#endif
			cv::setMouseCallback(element.second.inner_pMP->winName_ran, &(on_mouse), element.second.inner_pMP.get());

			mapCams.insert(element);
		}
		else
			print_process_ng("Init camera(" + cam.IP_MAC + ") fail!" );
	}
	
	for (auto subCam : mapCams)
	{
		// --- Connect all Ranger3
		auto err = subCam.second.inner_pCam->connectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng("Connect camera(" + subCam.first + ") fail. Error code: " + SickCam::CAM_STATUS_str(err));
			return false;
		}

		// --- Set parameters 
		subCam.second.inner_pCam->setParametPath (subCam.second.parameter_File_Path);	// 若使用相机自带默认参数，	请使用 parameter_File_Path="";
		subCam.second.inner_pCam->setRectMethos	 (SiCaliWrapper::RectMethod::Top);		// 执行 rectifcation 的方法，	仅用于标定后的图像。
		subCam.second.inner_pCam->setMissingData (MISSING_DATA);						// 用于填充无效区域的值，		仅用于标定后的图像。
		subCam.second.inner_pCam->setScaleY		 (0.08);								// 由外部计算得到，			仅用于标定后的图像。
		subCam.second.inner_pCam->setRectSpread	 (1.2);									// 可以获得比较少的 missing data 仅用于标定后的图像。

		// --- Activate in-device Calibration
		auto e = subCam.second.inner_pCam->setCalibration_inDevice(2560); // 若不需要获得标定后的图像，	请使用 calibration_File_Path="";
		//auto e = subCam.second.inner_pCam->setCalibration_inPC(subCam.second.calibration_File_Path, true, 2560); // 若不需要获得标定后的图像，	请使用 calibration_File_Path="";
		std::cout
			<< "set Scan3dOutputMode_Scan3dExtractionSelector_Scan3dExtraction1 : "
			<< SickCam::CAM_STATUS_str(e)
			<< "\n";
	}

	bool is_started(false);

	// --- Start all Ranger3 with callback function
	DEMO::__start_all_by_callback(is_started, mapCams);
	if (!is_started)
		return false;

	while (true)
	{
		// --- Image operation is processed by DEMO::onCallback_2;
		// --- Deal with commands.
		char key = cv::waitKey(10);

		// Start or Stop
		if (key == 's' || key == 'S') {
			if (is_started)							DEMO::__stop_all			(is_started, mapCams);
			else									DEMO::__start_all			(is_started, mapCams);
		}
		else if (key == 'p' || key == 'P')			DEMO::__parameter_setting	(is_started, mapCams);
#ifdef _WIN32
		else if (key == 'u' || key == 'U')			DEMO::__userSet_init		(is_started, mapCams);
		else if (key == 't' || key == 'T')			DEMO::__userSet_testing		(is_started, mapCams);
		else if (key == 'f' || key == 'F')			DEMO::__file_operation		(is_started, mapCams);
#endif
		else if (key == 'c' || key == 'C')			DEMO::__switch_mode			(is_started, mapCams);
		else if (key == 'w' || key == 'W')			DEMO::__g_save_image		= !(DEMO::__g_save_image);
		else if (key == 'q' || key == 'Q')
		{
			DEMO::__stop_all(is_started, mapCams);
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
	// 打印相机 SDK 的版本信息
	std::cout << SickCam::VER::__version__().c_str() << std::endl;

	
    // 使用找到的第一个 IP 打开相机，相机 IP 网段必须和 PC 相同。
#if 1
    std::vector<RANGER3> devs = {
            RANGER3(
                    true,			/* 是否是使用 IP */
                    "",             /* IP 地址 */
                    "",				/* 参数文件路径 csv */
				//	""
				"D:/UserFile66.json"				/* 标定文件路径 xml/json */
            )
    };
	Demo_R3_OpenCV_One_ranger3(devs);
#endif // 0


	// 使用指定 IP 打开相机，相机 IP 网段必须和 PC 相同。
#if 0
    std::vector<RANGER3> devs = {
        RANGER3(
            true,			/* 是否是使用 IP */
            "192.168.1.21", /* IP 地址 */
            "",				/* 参数文件路径 csv */
            ""				/* 标定文件路径 xml/json */
        )
    };
	test_Ranger3s_callback(devs);
#endif // 0


	// 使用指定 MAC 打开相机，相机 IP 网段必须和 PC 相同。
#if 0
	test_Ranger3s_callback(
		{ RANGER3(false, "00:06:77:0B:d6:E8", "", "") }
	);
#endif // 0

	return 0;
}

#endif
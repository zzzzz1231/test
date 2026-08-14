/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include <opencv2/opencv.hpp>
#include "InterfaceOpenCV.h"
#include "InterfaceOperation.h"
#include <iostream>
#include <map>
#include <fstream>

using SickCam::Ranger3Shared;
using SickCam::Ranger3;


bool Demo_R3_get_ResetEncoder(int height, int number)
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>(
		"",											/* 日志文件 */
		Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
        true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);

	// --- Check connected device
	std::vector<std::string> ip(0);
	if (pR3S->isAvaliable()) {
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
	auto pCam1 = std::make_shared<Ranger3>(pR3S, ip[0], true);

	// --- Connect Ranger3
	auto err = pCam1->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}

	err = pCam1->setParameter("EncoderReset", "");
	if (SickCam::CAM_STATUS::All_OK != err)
	{
		print_process_ng("EncoderReset failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		return false;
	}

	// --- Disconnect Ranger3
    err = pCam1->disconnectCamera();
	if (SickCam::CAM_STATUS::All_OK != err)
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
	Demo_R3_get_ResetEncoder(7200, 50);
	return 0;
}
#endif


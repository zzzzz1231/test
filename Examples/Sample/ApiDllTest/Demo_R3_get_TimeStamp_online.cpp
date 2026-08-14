/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include <opencv2/opencv.hpp>
#include "InterfaceOpenCV.h"
#include "InterfaceOperation.h"
#include <iostream>
#include <map>
#include <fstream>

using SickCam::Ranger3Shared;
using SickCam::Ranger3;

void SICK_CALLBACK
on_callback_function2(SickCam::ImgT * pImg, void * any)
{
	// Metold 1, by get_ChunkInfo();
	auto chunk_data_mark = pImg->get_ChunkInfo();

	// Metold 2, by SickCam::DataNames::MAR;
	auto chunk_data_mark_way2 = InterfaceOpenCV::getMat(*pImg, SickCam::DataNames::MAR);

	// Checkout result
	if (chunk_data_mark[0].timestamp != combine_uint32_uint32(chunk_data_mark_way2.at<uint32_t>(2), chunk_data_mark_way2.at<uint32_t>(3)))
	{
		std::stringstream ss;
		ss << "Error. " << pImg->get_ID() << ", " << chunk_data_mark[0].timestamp << ", " << combine_uint32_uint32(chunk_data_mark_way2.at<uint32_t>(2), chunk_data_mark_way2.at<uint32_t>(3)) <<  std::endl;
		print_process_ng(ss.str());
	}
	else
		cout << "OK. " << pImg->get_ID() << ", " << chunk_data_mark[0].timestamp << std::endl;

	// save image
	///std::stringstream ss;
	///ss << "D:\\support\\test2_bc200\\img_";
	///pImg->SaveToIconFile(ss.str() + std::to_string(pImg->get_ID()), {DN::RAN, DN::MAR});
}

bool test_simple(int height, int number)
{
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
	//err = pCam1->disconnectCamera();
	//err = pCam1->connectCamera();
	if (SickCam::CAM_STATUS::All_OK != err) {
		print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
		print_process_ng("The camera may be connected by other progress!");
		return false;
	}

	pCam1->setParametPath("/home/arm/SICK/K/Camera3D.csv");

	// --- Hold on main thread
    pCam1->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image. 若要 3D Range，请设置为 Linescan3D. Areascan
    pCam1->setParameterValue("ChunkModeActive", "1");
	pCam1->startCamera(on_callback_function2);

	getchar();
	
    pCam1->stopCamera();

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

int main(int argc, char* argv[])
{
	test_simple(7200, 50);
	return 0;
}


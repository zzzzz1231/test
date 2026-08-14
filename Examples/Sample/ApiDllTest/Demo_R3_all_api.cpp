/**
*	测试所有的 API
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


#ifdef _WIN32
const std::string json_path = "D:\\111.json";
#endif
#ifdef __linux__
const std::string json_path = "/home/alex/111.json";
#endif


void SICK_CALLBACK
on_get_image_2(SickCam::ImgT * pImg, void * any)
{
	std::cout << ((SickCam::Ranger3*)any)->get_DeviceIP() << ": Get Image : " << pImg->get_ID() << std::endl;
}

void SICK_CALLBACK
on_lost_2(std::string* _name, std::string* _ip, std::string* _mac, std::string* _msg, void* pR3)
{
	std::string name(*_name), ip(*_ip), mac(*_mac), msg(*_msg);
	print_process_ng(ip + " is lost!");
	print_process_ng(msg);
}

void SICK_CALLBACK
on_lost_connect_ok_2(std::string* _name, std::string* _ip, std::string* _mac, std::string* _msg, void* pR3)
{
	print_process_ng(*_ip + " is connect OK!");
	auto r3 = (SickCam::Ranger3*)(pR3);
	r3->startCamera(on_get_image_2);
}

void SICK_CALLBACK
on_lost_connect_failed_2(std::string* _name, std::string* _ip, std::string* _mac, std::string* _msg, void* pR3)
{
	print_process_ng(*_ip + " is connect failed!");
}


bool SICK_GTEST_LIBRARY Demo_R3_All_API()
{
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
		"D://",											/* 日志文件 */
		SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
		true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);

	pR3S->loadCtiFile(pR3S->getPathToRanger3Producer());

	pR3S->isAvaliable();

	pR3S->isCtiFound();

	pR3S->isDevFound();

	pR3S->isLogOutput();

	pR3S->getVersion();

	pR3S->getVersionTime();

	pR3S->getPathToTrispectorProducer();

	pR3S->scanDevice();

	int n_scanDevice = 0;
	pR3S->scanDevice(n_scanDevice);

	pR3S->getConDevList();
	pR3S->getConDevListIP();
	pR3S->getConDevListMAC();
	pR3S->getConDevListSN();



	// 
	auto devices = pR3S->getConDevListIP();
	assert(devices.size() > 0);

	auto r3 = std::make_shared<SickCam::Ranger3>(pR3S, devices.begin()->first, true);
	r3->isReady();


	// 只能在 CS 部分才能测试
	// r3->InitDevice(pR3S, devices.begin()->first, true);

	r3->connectCamera(120, 1000, 900, on_lost_2, on_lost_connect_ok_2, on_lost_connect_failed_2);

	r3->setScaleY(0.9);
	r3->setCalibration_inDevice();
	r3->setCalibration_inDevice_CalibrateAC();
	r3->setCalibration_No();
	r3->exportConfiguretionFile("E:/TMP/tt.csv");
	r3->importConfiguretionFile("E:/TMP/tt.csv");

	r3->setTimeOut(100);
	r3->setBufferCount(30);

	r3->E2U_2DMode();
	r3->E2U_3DMode();
	r3->E2U_3DMode(false);
	r3->E2U_GrabBy_Encoder();
	r3->E2U_GrabBy_FreeRunning();
	r3->E2U_LaserFlash();

	auto _setIp_arg1 = r3->get_DeviceIP();
	std::string _setIp_arg2 = _setIp_arg1;
	std::string _setIp_arg3 = _setIp_arg1;
	
	_setIp_arg2[_setIp_arg2.size() - 1] += 1;
	_setIp_arg3[_setIp_arg3.size() - 1] += 2;

	r3->setIp(_setIp_arg2, true);
	r3->setIp(_setIp_arg3, false);


	r3->setParameterValue("DeviceScanType", "Linescan3D");
	r3->setParameter("DeviceScanType", "Linescan3D");

	r3->getIp();
	r3->getIp(false);
	r3->getIp(true);

	r3->getSubNet();
	r3->getSubNet(false);
	r3->getSubNet(true);

	r3->getMac();

	r3->getTemperature();
	r3->getParameter("DeviceScanType");
	r3->getParameterValue("DeviceScanType");

	SickCam::AllParams Info;
	r3->getAllParameterInfo(Info);
	r3->getAllParameterInfo(Info, false);
	r3->getAllParameterInfo(Info, true);

	SickCam::Categories cate;
	SickCam::Params para;
	r3->getAllParameterInfo(cate, para);
	r3->getAllParameterInfo(cate, para, false);
	r3->getAllParameterInfo(cate, para, true);


	r3->getParametersAsStructureString();
	r3->getDeviceInfoAsStructureString();

	r3->getStatus();
	r3->getHeartBeat();

	r3->getImgID();

	std::string ss = r3->retrieveFileFromCamera();

	// 会修改相机重要配置，先不测
	//r3->sendFileToCamera(ss);


	r3->getUserSet(UserSetId::DEFAULT);
	SickCam::CAM_STATUS _getUserSet_a1;
	r3->getUserSet(UserSetId::USER_SET_1, _getUserSet_a1);




	r3->enableCorrectRawImageEdgeEffect(true);
	r3->enableCorrectRawImageEdgeEffect(false);

	r3->updateParamtersInfo();
	r3->paramCategoryList();
	r3->paramFullNameOfCategory("DeviceControl");
	r3->paramFullNameList();
	r3->paramCurrentValue("TriggerMode_TriggerSelector_LineStart");

	int _paramOptionalValues_a1;
	r3->paramOptionalValues("TriggerActivation_TriggerSelector_FrameStart",
		_paramOptionalValues_a1);



	r3->E2U_GrabBy_FreeRunning();
	r3->E2U_LaserFlash();


	// indevice - raw
	r3->setCalibration_No();
	r3->startCamera(on_get_image_2);
	int delay_count = 26; // 60 秒
	while (delay_count>0)
	{
		Sleep(1000);
		delay_count--;
	}
	r3->stopCamera();

	// indevice - cal
	r3->setCalibration_inDevice();
	r3->startCamera(on_get_image_2);
	delay_count = 30; // 60 秒
	while (delay_count > 0)
	{
		Sleep(1000);
		delay_count--;
	}
	r3->stopCamera();


	// indevice - cal
	r3->setCalibration_inDevice_CalibrateAC();
	r3->startCamera(on_get_image_2);
	delay_count = 25; // 60 秒
	while (delay_count > 0)
	{
		Sleep(1000);
		delay_count--;
	}
	r3->stopCamera();


	// inPC - cal
	r3->setCalibration_inPC("E:/TEST/IconData/Calibration.json");
	r3->startCamera(on_get_image_2);
	delay_count = 27; // 60 秒
	while (delay_count > 0)
	{
		Sleep(1000);
		delay_count--;
	}
	r3->stopCamera();


	r3->disconnectCamera();


	// Image

	auto img1 = SickCam::ImgT();
	auto img2 = SickCam::ImgT("E:/TEST/IconData/FreeRunning", 1.0);
	auto img3 = SickCam::ImgT(img1);


	auto img = img1;
	auto _img_equl = img == img1;


	SickCam::ImgT::DN2Str(SickCam::DataNames::REF);
	SickCam::ImgT::Str2DN("ComponentSelector_Range_RegionSelector_Scan3dExtraction1");


	img.getData(SickCam::DataNames::RAN);
	img.getData(SickCam::DataNames::WHITE);
	img.getData();


	img.getAvalibleDataNames();

	img.get_cols();
	img.get_rows();

	img.isEmpty();
	img.get_ChunkInfo();

	img.has_ChunkInfo();

	img.has(SickCam::DataNames::RAN);

	cv::Mat dst = cv::Mat(
		img.get_rows(),
		img.get_cols(),
		CV_16SC1, 
		img.getData(SickCam::DataNames::RAN));



	img.insertDataCopy(SickCam::DataNames::RAN2, dst.data, 1);

	img.printInfo();
	img.collectInfo();

	return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_R3_All_API();
	return 0;
}
#endif

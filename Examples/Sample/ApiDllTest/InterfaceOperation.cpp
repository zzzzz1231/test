/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	3.0.0.2
*	@date		2021.02.01
*/

#include "InterfaceOperation.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void print_process_ok(const std::string & log)
{
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	std::cout << log << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	std::cout << "\033[32m" << log << "\033[0m" << std::endl;
#endif
}
void print_process_ng(const std::string & log)
{
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	std::cerr << log << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	std::cerr << "\033[31m" << log << "\033[0m" << std::endl;
#endif
}

void print_process_ok(const char*  log)
{
	return print_process_ok(std::string(log));
}


void print_process_ng(const int64_t & id)
{
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	std::cerr << id << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	std::cerr << "\033[31m" << id << "\033[0m" << std::endl;
#endif
}

void print_process_ng(const char*  log)
{
	return print_process_ng(std::string(log));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CHECK_Set_GrabSensorImage(SPtr<SickCam::Ranger3> pR3)
{
	CHECK(pR3->setParameterValue("DeviceScanType", "Areascan") == SickCam::CAM_STATUS::All_OK);
}
void CHECK_Set_GrabRangeImage(SPtr<SickCam::Ranger3> pR3)
{
	CHECK(pR3->setParameterValue("DeviceScanType", "Linescan3D") == SickCam::CAM_STATUS::All_OK);
}
void CHECK_Set_RangeImage_nRows(SPtr<SickCam::Ranger3> pR3, const int& nRows)
{
	CHECK(pR3->setParameterValue("Height_RegionSelector_Scan3dExtraction1", std::to_string(nRows)) == SickCam::CAM_STATUS::All_OK);
}
void CHECK_Set_RangeImage_LineRate(SPtr<SickCam::Ranger3> pR3, const int& rate)
{
	CHECK(pR3->setParameterValue("AcquisitionLineRate", std::to_string(rate)) == SickCam::CAM_STATUS::All_OK);
}
void CHECK_Set_Mode_FreeRunning(SPtr<SickCam::Ranger3> pR3)
{
	CHECK(pR3->setParameterValue("TriggerMode_TriggerSelector_LineStart", "Off") == SickCam::CAM_STATUS::All_OK);
	CHECK(pR3->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off") == SickCam::CAM_STATUS::All_OK);
}
void CHECK_Set_Mode_Encoder(SPtr<SickCam::Ranger3> pR3)
{
	CHECK(pR3->setParameterValue("TriggerMode_TriggerSelector_LineStart", "On") == SickCam::CAM_STATUS::All_OK);
	CHECK(pR3->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off") == SickCam::CAM_STATUS::All_OK);
}
void CHECK_Set_Mode_FrameTrigger(SPtr<SickCam::Ranger3> pR3)
{
	CHECK(pR3->setParameterValue("TriggerMode_TriggerSelector_LineStart", "On") == SickCam::CAM_STATUS::All_OK);
	CHECK(pR3->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "On") == SickCam::CAM_STATUS::All_OK);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void
#ifndef __linux__
__stdcall
#endif
onCallback(SickCam::ImgT * pImg, void * any)
{
	std::cout << "onCallback(), get image - " << pImg->get_ID() << std::endl;
}


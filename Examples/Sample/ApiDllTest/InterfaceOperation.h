/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	3.0.0.2
*	@date		2021.02.01
*/

#pragma once

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Control.h"
#include <Ranger3.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include <conio.h>
#endif

// Only in windows it will show text color.
#define CHECK(x) \
	do{if(x) \
		print_process_ok(#x);\
	else\
	{	print_process_ng(#x);\
		exit(0);}\
	}while(0);

void print_process_ok(const std::string & log);
void print_process_ok(const char*  log);
void print_process_ng(const std::string & log);
void print_process_ng(const int64_t & log);
void print_process_ng(const char*  log);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 【仅用于 SDK 代码调试！】请参考这里的演示代码，查看相应模式的设置参数。在您正式的工程中，直接设置相机参数即可。Please see the demo code and set parameter directly. 
void CHECK_Set_GrabSensorImage(SPtr<SickCam::Ranger3> pR3); 
// 【仅用于 SDK 代码调试！】请参考这里的演示代码，查看相应模式的设置参数。在您正式的工程中，直接设置相机参数即可。Please see the demo code and set parameter directly. 
void CHECK_Set_GrabRangeImage(SPtr<SickCam::Ranger3> pR3);
// 【仅用于 SDK 代码调试！】请参考这里的演示代码，查看相应模式的设置参数。在您正式的工程中，直接设置相机参数即可。Please see the demo code and set parameter directly. 
void CHECK_Set_RangeImage_nRows(SPtr<SickCam::Ranger3> pR3, const int& nRows);
// 【仅用于 SDK 代码调试！】请参考这里的演示代码，查看相应模式的设置参数。在您正式的工程中，直接设置相机参数即可。Please see the demo code and set parameter directly. 
void CHECK_Set_RangeImage_LineRate(SPtr<SickCam::Ranger3> pR3, const int& rate);
// 【仅用于 SDK 代码调试！】请参考这里的演示代码，查看相应模式的设置参数。在您正式的工程中，直接设置相机参数即可。Please see the demo code and set parameter directly. 
void CHECK_Set_Mode_FreeRunning(SPtr<SickCam::Ranger3> pR3);
// 【仅用于 SDK 代码调试！】请参考这里的演示代码，查看相应模式的设置参数。在您正式的工程中，直接设置相机参数即可。Please see the demo code and set parameter directly. 
void CHECK_Set_Mode_Encoder(SPtr<SickCam::Ranger3> pR3);
// 【仅用于 SDK 代码调试！】请参考这里的演示代码，查看相应模式的设置参数。在您正式的工程中，直接设置相机参数即可。Please see the demo code and set parameter directly. 
void CHECK_Set_Mode_FrameTrigger(SPtr<SickCam::Ranger3> pR3);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void
#ifndef __linux__
__stdcall
#endif
onCallback(SickCam::ImgT* pImg, void* any);
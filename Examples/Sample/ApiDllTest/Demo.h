/**
*	@copyright	Copyright 2018-2023 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma once

#ifdef SICK_GTEST
#   define SICK_GTEST_LIBRARY   __declspec(dllexport)
#else
#   define SICK_GTEST_LIBRARY   
#endif

#include <string>
#include <Ranger3.h>

bool SICK_GTEST_LIBRARY Demo_ImgT_calibration_offline_json();
bool SICK_GTEST_LIBRARY Demo_ImgT_calibration_offline_json_simple_method();
bool SICK_GTEST_LIBRARY calibration_offline_xml();
bool SICK_GTEST_LIBRARY calibration_offline_xml_CalibratedAC();
bool SICK_GTEST_LIBRARY Demo_R3_All_API();

bool SICK_GTEST_LIBRARY Demo_R3_01_quick_start_callback();
bool SICK_GTEST_LIBRARY Demo_R3_01_callback_by_thread_pool();
bool SICK_GTEST_LIBRARY Demo_R3_CalibratedAC();
bool SICK_GTEST_LIBRARY Demo_R3_callback_calibration_in_device();
bool SICK_GTEST_LIBRARY Demo_R3_callback_calibration_in_device_saveImage_Float();
bool SICK_GTEST_LIBRARY Demo_R3_callback_calibration_in_device_saveImage_Word16();
bool SICK_GTEST_LIBRARY Demo_R3_callback_calibration_in_PC();
bool SICK_GTEST_LIBRARY Demo_R3_callback_ChangingRows_deliveredLineCount();
bool SICK_GTEST_LIBRARY Demo_R3_callback_highspeed();
bool SICK_GTEST_LIBRARY Demo_R3_callback_Reconnect_MultiCam();
bool SICK_GTEST_LIBRARY Demo_R3_callback_Save_Image();
bool SICK_GTEST_LIBRARY Demo_R3_callback_start_stop_time_cost_test();
bool SICK_GTEST_LIBRARY Demo_R3_callback_with_heartbeat();
bool SICK_GTEST_LIBRARY Demo_R3_enable_CorrectRawImageEdgeEffect();
bool SICK_GTEST_LIBRARY Demo_R3_get_device_information();
bool SICK_GTEST_LIBRARY Demo_R3_get_ResetEncoder(int height, int number);
bool SICK_GTEST_LIBRARY Demo_R3_get_set_parameters();
bool SICK_GTEST_LIBRARY method_1_Demo_R3_get_TimeStamp_from_offline_data(const std::string& path_folder);
bool SICK_GTEST_LIBRARY method_2_Demo_R3_get_TimeStamp_from_offline_data(const std::string& path_folder);
bool SICK_GTEST_LIBRARY Demo_R3_save_MarkData_ChunkData_online();
bool SICK_GTEST_LIBRARY Demo_R3_simplest_demo();
bool SICK_GTEST_LIBRARY Demo_R3_thread_grabbing(
	const bool		usingIP,
	const std::string& IP_MAC,
	const SickCam::DataNames& imgType,
	const int& nImg,
	const std::string& PARAM_CSV_PATH,
	const std::string& CALIB_XML_PATH
);
bool SICK_GTEST_LIBRARY Demo_R3_update_firmware(const std::string firmwarePath);










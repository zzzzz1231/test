/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	3.0.0.2
*	@date		2021.02.01
*/

#pragma once

#include "Control.h"
#include <Ranger3.h>			// 此头文件封装了，API 的主体。 

#ifdef UNIT_TEST

using SickCam::Ranger3Shared;
using SickCam::Ranger3;


//////////////////////////////////////////////////////////
bool protected_unit_test_status_unreachable();
bool protected_unit_test_status_occupied();
bool protected_unit_test_ip_setting();
bool protected_unit_test_ip_many_times();
bool protected_unit_test_grabbing_rate();
bool protected_unit_test_chunk_data();
bool protected_unit_test_in_PC_calibration();
bool protected_unit_test_in_device_calibration();
#endif // UNIT_TEST


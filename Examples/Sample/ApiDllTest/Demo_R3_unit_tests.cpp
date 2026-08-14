/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Control.h"
#include "UnitTests.h"
#include <Ranger3.h>			// 此头文件封装了，API 的主体。 

////////////////////////////////////////////////////////////////////////////////

int Demo_R3_unit_tests(int argc, char* argv[])
{
#ifdef UNIT_TEST
	protected_unit_test_status_unreachable();
	protected_unit_test_status_occupied();
	protected_unit_test_ip_setting();
	protected_unit_test_ip_many_times();
	protected_unit_test_grabbing_rate();
	protected_unit_test_chunk_data();
	protected_unit_test_in_PC_calibration();
	protected_unit_test_in_device_calibration();
	//protected_unit_test_laser_strobe();

#endif
	
	return 0;
}


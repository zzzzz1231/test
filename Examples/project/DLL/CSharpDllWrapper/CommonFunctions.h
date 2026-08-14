/**	@file	SampleApiDllCSharpWrapper.h
*
*	@brief	C# Wrapper Class of Ranger3.
*
*	@attention
*	This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
*
*
*	@copyright	Copyright 2016-2026 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*
*/


#pragma once

#include <Ranger3.h>
#include <Trispector.h>

#include <cliext\list>
#include <cliext\map>
#include <vcclr.h>

#include <string>
#include <sstream>

#include <windows.h>

using namespace System;

namespace SICK3D
{

std::wstring string2wstring(const std::string & str);
std::string wstring2string(const std::wstring & wstr);
std::string wchart2string(const wchar_t* pwc);
std::string StringRef2string(const String^ strRef);


template<class T>
using SPtr = std::shared_ptr<T>;

template<class T>
using UPtr = std::unique_ptr<T>;


////////////////////////////////////////////////////////////////////////////////


public enum class CAM_STATUS
{
	All_OK = 1,
	CAM_IS_CONNECTED,
	CAM_IS_STARTED,
	CAM_IS_STOPPED,
	CAM_IS_DISCONNECTED,
	ERROR_OPEN_CAM,
	ERROR_SCAN_EMPTY_DEVICE,
	ERROR_CTI_NOT_FOUND,
	ERROR_OPEN_CONSUMER,
	ERROR_OPEN_TL_HANDLE,
	ERROR_EMPTY_IF_ID,
	ERROR_EMPTY_IF_HANDLE,
	ERROR_CSV_PATH,
	ERROR_CALIBRATION_PATH,
	ERROR_EMPTY_PATH,
	ERROR_EMPTY_IMG,
	ERROR_SET_CAM,
	ERROR_CLO_CAM,
	ERROR_GET_IMG,
	ERROR_GET_SEN_IMG,
	ERROR_RST_CAM,
	ERROR_UPDATE_PARA,
	ERROR_STOP_ACQUISITION,
	ERROR_START_ACQUISITION,
	ERROR_PARAMETER_FORMAT_DISMATCH,
	ERROR_PARAMETER_VALUE_DISMATCH,
	ERROR_PARAMETER_READ_ONLY,
	ERROR_PARAMETER_INVALID,
	ERROR_PARAMETER_VALUE_INVALID,
	ERROR_PARAMETERS_EMPTY,
	ERROR_SCAN_PARAMETERS,
	ERROR_COMMUNICATION,
	ERROR_SET_IP,
	ERROR_SET_SUBNET,
	ERROR_NULL_PTR_DEV,
	ERROR_NULL_DEV_HANDLE,
	ERROR_NULL_DS_HANDLE,
	ERROR_OPEN_DATASTREAM,
	ERROR_CAM_NOT_CONNECT,
	ERROR_CAM_NOT_START,
	ERROR_CAM_NOT_STOP,
	ERROR_CAM_NOT_DISCONNECT,
	ERROR_CAM_IS_CONNECTED,
	ERROR_CAM_IS_STARTED,
	ERROR_CAM_IS_STOPPED,
	ERROR_CAM_IS_DISCONNECTED,
	ERROR_CAM_IS_UNREACHABLE,
	ERROR_CAM_IS_OCCUPIED,
	ERROR_CAM_IS_LOST,
	ERROR_IMAGE_NOT_READY,
	ERROR_USER_FILE_NOT_EXIST,
	ERROR_USER_FILE_SEND_FAILED,
	ERROR_USER_FILE_NOT_FOUND_IN_PC,
	ERROR_USER_SET_NOT_FOUND,
	ERROR_OPERATION_NOT_ALLOW,
	WARN_IMAGE_LOST,
	TIME_OUT,
	NO_CHANGE_IN_NETWORK,
	UNKNOWN,
	DEFAULT
};

public enum class RMethod
{
	Top,
	Mean,
	Bottom
};






}

/**	@file	VersionCSharpWrapper.cpp
*
*	@brief	C# Wrapper Class of Trispector.
*
*	@attention
*	This is the C# API wrapper of C++ file version.cpp|h.
*
*
*	@copyright	Copyright 2016-2021 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	3.5.0.8
*	@date		2020.09.21
*
*/

#include "stdafx.h"
#include "VersionCSharpWrapper.h"

namespace SICK3D
{

String ^
VER::__Version__()
{
	auto str = SickCam::VER::__version__();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}


}

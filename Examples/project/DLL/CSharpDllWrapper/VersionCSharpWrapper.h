/// <summary>
/// C# Wrapper Class of Version.
/// </summary>
/// <remarks>
/// This is the C# API wrapper of C++ file version.cpp|h.
/// Copyright 2016-2021 SICK AG. All rights reserved.
/// Author: Vision Lab, SICK GCN
/// Version: 3.4.3.66
/// Date: 2020.09.21
/// </remarks>

#pragma once

#include "ImgTCSharpWrapper.h"
#include "CommonFunctions.h"
#include <Trispector.h>
#include <cliext\list>
#include <cliext\map>
#include <vcclr.h>
#include <msclr\lock.h>

#include <string>
#include <sstream>

#include <windows.h>

using namespace System;
using namespace System::Threading;
using namespace msclr;

//#define _CALLBACK_ON

namespace SICK3D
{

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 显示 API 的版本号和更新。 Show the version number and updates of APIs.
/// </summary>
/// <remarks>
/// <code>
/// + "### 2.4.0.5459-6 (Ranger3) ### \n\n"
/// + "[特性] 支持使用 Callback 采集图像。\n\n"
/// + "[特性] 支持使用线程轮询采集图像。\n\n"
/// + "[特性] 支持使用 xml 格式的标定文件。\n\n"
/// + "[特性] 支持使用 json 格式的标定文件。\n\n"
/// + "[特性] 支持从相机加载标定文件。\n\n"
/// + "\n\n"
/// + "[语言] 支持 C++。\n\n"
/// + "[语言] 支持 C#。\n\n"
/// + "\n\n"
/// + "[环境] Windows 7  - X64 架构，Visual Studio 2013 - 2019。\n\n"
/// + "[环境] Windows 10 - X64 架构，Visual Studio 2013 - 2019。\n\n"
/// + "[环境] Ubuntu 18  - X64 架构，CMake 2.8及以上，C++11 标准。\n\n"
/// + "[环境] Ubuntu 18  - ARM64 架构，CMake 2.8及以上，C++11 标准。\n\n"
/// + "\n\n"
/// + "[示例] 控制台：保存 / 读取 icon 图像格式的示例代码。\n\n"
/// + "[示例] 控制台：在线 / 离线 标定图像。\n\n"
/// + "[示例] 控制台：转化成点云，并保存为 txt 文件。\n\n"
/// + "[示例] 控制台：结合 OpenCV 显示图像。\n\n"
/// + "[示例] 控制台：以 callback 方式采集图像。\n\n"
/// + "[示例] 控制台：以线程轮询方式采集图像。\n\n"
/// + "[示例] 控制台：解析编码器值、时间戳等信息。\n\n"
/// + "[示例] 控制台：读取相机的 SN 、温度、厂家等信息。\n\n"
/// + "[示例] 控制台：从相机上读取标定参数文本（固件版本：2.4.0.5459，并且要事先放文件到相机内。）。\n\n"
/// + "[示例] GUI：MFC 示例程序，提供完整功能。\n\n"
/// + "[示例] GUI：C# Winform 示例程序，提供完整功能。\n\n"
/// + "\n\n"
/// + "\n\n"
/// + "---------- \n\n"
/// + "### 2.4.0.5459-6 (Trispector) ### \n\n"
/// + "[特性] 支持使用 Callback 采集图像。\n\n"
/// + "[特性] 参数设置须通过 Sopas 或者 Socket 通讯实现。\n\n"
/// + "\n\n"
/// + "[语言] 支持 C++。\n\n"
/// + "[语言] 支持 C#。\n\n"
/// + "\n\n"
/// + "[环境] Windows 7  - X64 架构，Visual Studio 2013 - 2019。\n\n"
/// + "[环境] Windows 10 - X64 架构，Visual Studio 2013 - 2019。\n\n"
/// + "\n\n"
/// + "[示例] 控制台：结合 OpenCV 显示图像。\n\n"
/// + "[示例] 控制台：以 callback 方式采集图像。\n\n"
/// + "[示例] 控制台：使用 socket 和相机通讯示例。\n\n"
/// + "---------- \n\n"
/// </code>
/// </remarks>
public ref class VER
{
public:
	VER() {};
	~VER() {};

	/// <summary>
	/// 【警告：旧接口。请优先使用 SwigRanger3 】
	/// 以字符串形式返回版本信息。 Return version number and updates as string.
	/// </summary>
	/// <returns>版本信息字符串</returns>
	String^ __Version__();

};




}

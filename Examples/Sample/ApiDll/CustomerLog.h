/**	@file	CustomerLog.h
*
*	@brief	Class of output log.
*
*	@attention
*	Log and printf control
*
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*
*/


#pragma once

#include "Typedef.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>





namespace SickCam
{

/*
* @brief 日志系统
* 
* @note
* 可以在日志路径下，创建一个 “log.sick.ignore.ini”，来配置需要显示和记录的日志。
* - 文件示例（请保持和真实的代码引用一致）

# 注释行以 # 开头
ShowFuntionNameLine = 0

CameraSharedBase = 1
CameraSharedSingleton = 0
CameraSharedScan = 0
CameraSharedScanL1 = 0
CameraSharedScanL2 = 0

Ranger3Base = 0
Ranger3GetImage = 0
Ranger3GetImageL1 = 0
Ranger3HeartBeats = 1
Ranger3HeartBeatsL1 = 1
Ranger3HeartBeatsCount = 0
Ranger3Reconnect = 1
Ranger3ReconnectL1 = 1


* 
* 
*/
class CustomerLog
{
public:
	CustomerLog(const std::string path, const bool enShow = true, const bool enWrite = true, const int keepNMessage = 10);
	~CustomerLog();

	EXPORT_TO_DLL
	static std::string time();


	EXPORT_TO_DLL
	std::string getLastLog();

	// Overloading << 
	template<typename T>
	friend
		CustomerLog &
		operator << (CustomerLog & ths, const T & ss);


	/// this is the type of std::cout
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

	/// this is the function signature of std::endl
	typedef CoutType& (*StandardEndLine)(CoutType&);

	/// define an operator<< to take in std::endl
	CustomerLog &
		operator << (StandardEndLine manip);



	bool checkKeyword(const std::string& keyword);

	// 读取ini配置，存入 map<string, bool>
	bool loadIniConfig(const std::string& filePath)
	{
		std::ifstream fin(filePath);
		if (!fin.is_open())
		{
			//std::cerr << "打开 Ranger3 SDK 的日志的显示等级配置文件失败。文件路径：" << filePath << "。使用默认配置输出基础日志。" << std::endl;
			std::cout << "Open Ranger3 SDK log output base level log ." << std::endl;
			return false;
		}

		config.clear();

		std::string line;
		while (std::getline(fin, line))
		{
			line = trim(line);
			// 跳过空行、注释行
			if (line.empty() || line.front() == '#')
				continue;

			// 按 = 分割 key value
			size_t eqPos = line.find('=');
			if (eqPos == std::string::npos)
				continue;

			std::string key = trim(line.substr(0, eqPos));
			std::string val = trim(line.substr(eqPos + 1));

			// 转为 bool
			bool bVal = (val == "true" || val == "1");
			config[key] = bVal;
		}
		fin.close();
		return true;
	}


protected:
	CustomerLog() = delete;
	void _open();
	void _close();


	std::map<std::string, bool> config;

	// 去除字符串首尾空格
	std::string trim(std::string s)
	{
		// 去掉首部空格
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(static_cast<int>(ch));
			}));

		// 去掉尾部空格
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(static_cast<int>(ch));
			}).base(), s.end());

		return s;
	}



private:

	std::ofstream	mFile;
	std::string		mPath;
	bool mEnShow;
	bool mEnWrite;

	std::vector<std::string> mLast; // size = keepNMessage
	int mLastId;
	int mKeepNMessage;
};


/////////////////////////////////////////////////////////////


template<typename T>
inline CustomerLog &
operator << (CustomerLog & ths, const T & ss)
{
	if (ths.mEnShow)
	{
		std::cout << ss;
	}

	if (ths.mEnWrite)
	{
		ths.mFile << ss;
	}

	return ths;
}

template < >
inline CustomerLog &
operator << (CustomerLog & ths, const std::stringstream & ss)
{
	if (ths.mEnShow)
	{
		std::cout << ss.str().c_str();
	}

	if (ths.mEnWrite)
	{
		ths.mFile << ss.str().c_str();
	}

	return ths;
}







}
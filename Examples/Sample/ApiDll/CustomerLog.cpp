#include "CustomerLog.h"


namespace SickCam 
{

CustomerLog::CustomerLog(const std::string path, const bool enShow, const bool enWrite, const int keepNMessage)
	: mEnShow(enShow)
	, mEnWrite(enWrite)
	, mLast(10)
	, mLastId(0)
	, mKeepNMessage(keepNMessage > 1 ? keepNMessage : 1)
{
	if (mEnWrite && !path.empty())
	{
		mPath = path;
		mFile.open(mPath, std::fstream::app);
	}
	else
	{
		mEnWrite = false;
	}

	// 增加屏蔽词机制
//	loadIniConfig(path + "/log.sick.ignore.ini");
}

CustomerLog::~CustomerLog()
{
	if (mEnWrite)
		mFile.close();
}

std::string
CustomerLog::time()
{
	// 获取当前时间点
	auto now = std::chrono::system_clock::now();

	// 转成 time_t（秒级）
	time_t tt = std::chrono::system_clock::to_time_t(now);

	// 获取毫秒部分
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	// 格式化时间
	struct tm ltm = { 0 };
#ifdef __linux__
	localtime_r(&tt, &ltm);
#else
	localtime_s(&ltm, &tt);
#endif

	std::stringstream stm;
	stm << std::setfill('0');

	// 日期 + 时间（时:分:秒）
	stm << std::setw(4) << (ltm.tm_year + 1900);
	stm << std::setw(2) << (ltm.tm_mon + 1);
	stm << std::setw(2) << ltm.tm_mday << " ";
	stm << std::setw(2) << ltm.tm_hour << ":";
	stm << std::setw(2) << ltm.tm_min << ":";
	stm << std::setw(2) << ltm.tm_sec;

	// 追加 毫秒（3位，固定宽度）
	stm << "." << std::setw(3) << milliseconds.count();

	return stm.str();
}

std::string 
CustomerLog::getLastLog()
{
	std::stringstream ss;
	for (int i = mLastId; i < mKeepNMessage; ++i)
		ss << mLast[i];

	for (int i = 0; i < mLastId; ++i)
		ss << mLast[i];

	return ss.str();
}


void
CustomerLog::_open()
{
	if (mEnWrite)
	{
		if (!mFile.is_open())
			mFile.open(mPath, std::fstream::app);
	}
}

void
CustomerLog::_close()
{
	if (mEnWrite)
	{
		if (mFile.is_open())
			mFile.close();
	}
}

CustomerLog &
CustomerLog::operator << (StandardEndLine manip)
{
	//if (manip == static_cast<StandardEndLine>(endl))
	//{
	//	if (mEnShow)
	//	{
	//		cout << "\n\n\n" << "----- "<< time() <<" -----\n" << endl;
	//	}

	//	if (mEnWrite)
	//	{
	//		_open();
	//		mFile << "\n\n\n" << "----- " << time() << " -----\n" << endl;
	//		_close();
	//	}

	//	std::stringstream ss;
	//	ss << "\n\n\n" << "----- " << time() << " -----\n" << endl;
	//	mLast[mLastId++] = ss.str();
	//	mLastId %= mKeepNMessage;
	//}


	/// call the function, but we cannot return it's value
	manip(cout);
	return *this;
}


bool 
CustomerLog::checkKeyword(const std::string& keyword)
{
	// 用户没有设定的时候，默认输出 Base 相关的日志
	if (config.empty())
	{
		if (keyword == "CameraSharedBase" || keyword == "Ranger3Base")
			return true;
		else
			return false;
	}
	else
	{
		return config[keyword];
	}
}




}


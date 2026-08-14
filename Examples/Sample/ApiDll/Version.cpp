#include "Version.h"

namespace SickCam
{
std::string 
VER::version_number = std::to_string(R3_VERSION_1) + "." 
		+ std::to_string(R3_VERSION_2) + "." 
		+ std::to_string(R3_VERSION_3) + "." 
		+ std::to_string(R3_VERSION_4);

std::string VER::version_time = R3_VERSION_TIME;

std::string VER::m_version_linux_aarch64 = "SICK GenICam SDK CPP " + version_number + ", " + version_time + ", Linux, aarch64";

std::string VER::m_version_linux_x64 = "SICK GenICam SDK CPP " + version_number + ", " + version_time + ", Linux, x64";

std::string VER::m_version_trispector = "SICK GenICam SDK CPP " + version_number + ", " + version_time + ", Windows 10, x64";

std::string VER::m_version = "SICK GenICam SDK CPP " + version_number +", "+ version_time + ", Windows 10, x64";



#ifdef __linux__
#ifdef __arm__
std::string VER::version_platform = "Linux, aarch64";
#else
std::string VER::version_platform = "Linux, x64";
#endif // __arm__
#endif // __linux__

#ifdef _WIN32
std::string VER::version_platform = "Win10, x64";
#endif // _WIN32


std::string VER::version_language = "C++";
std::string VER::version_name = "SICK GenICam SDK";





void mark_Obsolete(SPtr<CustomerLog> plog, const std::string& deviceName, const std::string& fname_old, const std::string& fname_new)
{
	*plog << CustomerLog::time() << "[" << deviceName << "][" << fname_old << "]: 不建议再继续使用 / Obsolete function! 请使用代替函数 / Replaceed by: " << fname_new << "\n";
	*plog << CustomerLog::time() << "[" << deviceName << "][" << fname_old << "]: 不建议再继续使用 / Obsolete function! 请使用代替函数 / Replaceed by: " << fname_new << "\n";
	*plog << CustomerLog::time() << "[" << deviceName << "][" << fname_old << "]: 不建议再继续使用 / Obsolete function! 请使用代替函数 / Replaceed by: " << fname_new << "\n";
}

bool isEnvironmentVariableExisted(const std::string& name)
{
	char* value = getenv(name.c_str());
	if (value == nullptr)
	{
		return false;
	}
	return true;
}



}

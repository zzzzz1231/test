/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include <opencv2/opencv.hpp>
#include "InterfaceOpenCV.h"
#include "InterfaceOperation.h"
#include <iostream>
#include <map>
#include <fstream>

using SickCam::Ranger3Shared;
using SickCam::Ranger3;

////////////////////////////////////////////////////////////////////////////////

bool method_1_Demo_R3_get_TimeStamp_from_offline_data(const std::string& path_folder)
{
    std::cout << "Demo Function Name: " << __func__ << std::endl;
    std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
    std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

    SickCam::ImgT img;
    img.loadFromIconFile(path_folder, 1.0);
    auto mark = InterfaceOpenCV::getMat(img, SickCam::DataNames::MAR);

    std::vector<uint64_t> timestamp = getTimestamp_fromMat(mark);

    for (int j = 0; j< timestamp.size(); ++j)
    {
        std::cout << j << ": " << timestamp[j] << std::endl;
    }
    return true;
}

bool method_2_Demo_R3_get_TimeStamp_from_offline_data(const std::string& path_folder)
{
    std::cout << "Demo Function Name: " << __func__ << std::endl;
    std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
    std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

    SickCam::ImgT img;
    img.loadFromIconFile(path_folder, 1.0);
    auto mark = img.get_ChunkInfo();

    for (int j = 0; j< mark.size(); ++j)
    {
        std::cout << j << ": " << mark[j].timestamp << std::endl;
    }
    return true;
}

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
    std::string path_folder = "D:/delete/1";
#endif
#ifdef __linux__
    std::string path_folder = "/home/alex/delete/1";
#endif

    method_1_Demo_R3_get_TimeStamp_from_offline_data(path_folder);
    method_2_Demo_R3_get_TimeStamp_from_offline_data(path_folder);

	return 0;
}
#endif


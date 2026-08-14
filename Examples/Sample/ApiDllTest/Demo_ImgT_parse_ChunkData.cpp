/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>
#include <opencv2/opencv.hpp>
#include <bitset>

using SickCam::DataNames;

int SICK_GTEST_LIBRARY parse_ChunkData(int argc, char* argv[])
{
	/*************************************************\
	* 若要获得编码器信息、时间戳信息等，需要将参数 ChunkDataEnable 设置为 1。
	* 标定后图像和标定前图像，均可以包含相同的 Mark 数据，也称 ChunkData。
	* ChunkData 保存在 dat|xml 文件中
	\*************************************************/

#ifdef _WIN32
    std::string path_folder = "D:/delete/1";
#endif
#ifdef __linux__
    std::string path_folder = "/home/alex/delete/1";
#endif

	SickCam::ImgT img;
	img.loadFromIconFile(path_folder, 1.0);
    auto chunk_data_mark = img.get_ChunkInfo();
    std::ofstream file(path_folder + "_parse.txt", std::ios::app);
    file
            << "timestamp          "<<", "
            << "encoderValue       "<<", "
            << "overtriggerCount)  "<<", "
            << "frameTriggerActive "<<", "
            << "lineTriggerActive  "<<", "
            << "encoderResetActive "<<", "
            << "encoderA           "<<", "
            << "encoderB           "<<", \n";
    for(auto sub : chunk_data_mark)
    {
        file
                << sub.timestamp<<", "
                << sub.encoderValue<<", "
                << uint16_t(sub.overtriggerCount)<<", "
                << (sub.frameTriggerActive   ? ("true"):("false")) <<", "
                << (sub.lineTriggerActive    ? ("true"):("false")) <<", "
                << (sub.encoderResetActive   ? ("true"):("false")) <<", "
                << (sub.encoderA             ? ("true"):("false")) <<", "
                << (sub.encoderB             ? ("true"):("false")) <<", \n";
    }
    file.close();
	return 0;
}

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
    parse_ChunkData();
    return 0;
}
#endif


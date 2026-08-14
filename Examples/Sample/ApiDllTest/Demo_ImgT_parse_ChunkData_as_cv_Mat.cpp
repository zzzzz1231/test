/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <ImageTable.h>
#include <opencv2/opencv.hpp>
#include "InterfaceOperation.h"
#include "InterfaceOpenCV.h"
#include <bitset>

using SickCam::DataNames;

int SICK_GTEST_LIBRARY parse_ChunkData_as_cv_Mat()
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


	cv::Mat mark;
	if (img.has(SickCam::DataNames::MAR))
		mark = cv::Mat(img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img.getData(SickCam::DataNames::MAR));

    //for(int j=0; j < mark.rows; ++j)
    for(int j=0; j < 1; ++j) // show the first profile chunkdata only
    {
        auto pdata = mark.ptr<uint32_t>(j);
        std::cout
            << "timestamp          =" << combine_uint32_uint32(pdata[2], pdata[3])                                      << ", \n"
            << "encoderValue       =" << pdata[0]                                                                       << ", \n"
            << "overtriggerCount   =" << static_cast<uint32_t>((pdata[1] >> SickCam::STATUS_BIT_OVERTRIG)        &0xff) << ", \n"
            << "frameTriggerActive =" << static_cast<bool>    ((pdata[1] >> SickCam::STATUS_BIT_ENABLE)          &1)    << ", \n"
            << "lineTriggerActive  =" << static_cast<bool>    ((pdata[1] >> SickCam::STATUS_BIT_LINE_TRIGGER)    &1)    << ", \n"
            << "encoderResetActive =" << static_cast<bool>    ((pdata[1] >> SickCam::STATUS_BIT_ENCODER_RESET)   &1)    << ", \n"
            << "encoderA           =" << static_cast<bool>    ((pdata[1] >> SickCam::STATUS_BIT_ENCODER_A)       &1)    << ", \n"
            << "encoderB           =" << static_cast<bool>    ((pdata[1] >> SickCam::STATUS_BIT_ENCODER_B)       &1)    << ", \n";
    }

	return 0;
}

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
    parse_ChunkData_as_cv_Mat();
    return 0;
}
#endif
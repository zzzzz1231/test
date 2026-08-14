/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <ImageTable.h>
#include <opencv2/opencv.hpp>

using SickCam::DataNames;

bool SICK_GTEST_LIBRARY Demo_ImgT_load_color_image_offline()
{
	// --- 读取原始图像
	{
		SickCam::ImgT img;
		img.loadFromIconFile("F:\\TEST\\20241014-Ranger3-80\\1017\\taijie\\img-0", 1.0);

		cv::Mat range, reflectance, scatter, mark, red, green, blue, hiresgray;
		if (img.has(SickCam::DataNames::RAN))
			range		= cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::RAN));

		if (img.has(SickCam::DataNames::REF))
			reflectance	= cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::REF));

		if (img.has(SickCam::DataNames::SCA_16))
			scatter		= cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::SCA_16));

		if (img.has(SickCam::DataNames::MAR))
			mark		= cv::Mat(img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img.getData(SickCam::DataNames::MAR)); // 编码器信息

		if (img.has(SickCam::DataNames::R))
			red = cv::Mat(img.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::R));

		if (img.has(SickCam::DataNames::G))
			green = cv::Mat(img.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::G));

		if (img.has(SickCam::DataNames::B))
			blue = cv::Mat(img.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::B));

		if (img.has(SickCam::DataNames::WHITE))
			hiresgray = cv::Mat(img.get_rows(), HIRES_IMG_WIDTH, CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::WHITE));

	}



	return true;
}

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	Demo_ImgT_load_color_image_offline();
	return 0;
}
#endif
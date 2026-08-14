/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <ImageTable.h>
#include <opencv2/opencv.hpp>

using SickCam::DataNames;

int SICK_GTEST_LIBRARY load_save_image_offline()
{
	// --- 读取激光图像
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\sensor\\sensor", 1.0);
		cv::Mat sensor;
		if (img.has(SickCam::DataNames::SEN))
			sensor = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::SEN));
	}

	// --- 读取原始图像
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\scater\\RAW", 1.0);

		cv::Mat range, reflectance, scatter, mark;
		if (img.has(SickCam::DataNames::RAN))
			range		= cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::RAN));

		if (img.has(SickCam::DataNames::REF))
			reflectance	= cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::REF));

		if (img.has(SickCam::DataNames::SCA_16))
			scatter		= cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::SCA_16));

		if (img.has(SickCam::DataNames::MAR))
			mark		= cv::Mat(img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img.getData(SickCam::DataNames::MAR)); // 编码器信息
	}

	// --- 读取标定后图像
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\scater\\CAL", 0.04);

		cv::Mat range, reflectance, scatter, mark;
		if (img.has(SickCam::DataNames::RAN_CAL))
			range		= cv::Mat(img.get_rows(), img.get_cols(), CV_32FC1, (uint8_t*)img.getData(SickCam::DataNames::RAN_CAL));

		if (img.has(SickCam::DataNames::REF_CAL))
			reflectance = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::REF_CAL));

		if (img.has(SickCam::DataNames::SCA_16_CAL))
			scatter		= cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::SCA_16_CAL));

		if (img.has(SickCam::DataNames::MAR))
			mark		= cv::Mat(img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img.getData(SickCam::DataNames::MAR)); // 编码器信息
	}

	// --- 保存激光图像
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\sensor\\sensor", 1.0);
		img.SaveSensorImageToIconFile("D:\\support\\DATA\\ICON\\sensor\\sensor_test");
	}

	// --- 保存原始图像
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\scater\\RAW", 1.0);

		img.SaveRawImagesToIconFile("D:\\support\\DATA\\ICON\\scater\\RAW_test", false);
	}

	// --- 保存标定后图像
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\scater\\CAL", 0.04);

		img.SaveCalibratedImagesToIconFile("D:\\support\\DATA\\ICON\\scater\\CAL_test2", false);
	}

	// --- 保存为点云
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\battery\\fill", 0.008);
		img.SaveCalibratedDataToPointCloud("D:\\support\\DATA\\ICON\\battery\\fill_test.txt");
	}

	return 0;
}

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	load_save_image_offline();
	return 0;
}
#endif
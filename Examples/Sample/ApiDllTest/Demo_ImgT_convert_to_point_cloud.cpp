/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <ImageTable.h>
#include <opencv2/opencv.hpp>

using SickCam::DataNames;



int SICK_GTEST_LIBRARY convert_to_point_cloud()
{
	// --- 保存为点云文件
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\battery\\fill", 0.008);
		img.SaveCalibratedDataToPointCloud("D:\\support\\DATA\\ICON\\battery\\fill_test.txt");
	}

	// --- 按照 float 读取，转化成点云的计算方法（Ranger3 图像是左手系）
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\battery\\fill", 0.008); // 按照 float 读取
		size_t s = img.get_cols() * img.get_rows();
		size_t cols = img.get_cols();

		std::vector<float> xmap(s), ymap(s), zmap(s);
		if (img.has(SickCam::DN::RAN_CAL))
		{
			double
				xResolution(img.getCalibration_info().scaleX),
				yResolution(img.getCalibration_info().scaleY),
				xOffset(img.getCalibration_info().offsetX),
				yOffset(img.getCalibration_info().offsetY);

			float* pd = (float*)img.getData(SickCam::DN::RAN_CAL);
			for (uint32_t j = 0; j < img.get_rows(); ++j)
			{
				for (uint32_t i = 0; i < img.get_cols(); ++i)
				{
					auto ID = i + j * cols;
					if (pd[ID] == -1.0e6f)	continue;
					if (pd[ID] == 0.f)		continue;
					if (std::isnan(pd[ID]))	continue;

					// 点云值
					xmap[ID] = i * xResolution + xOffset;
					ymap[ID] = j * yResolution + yOffset;
					zmap[ID] = pd[ID];
				}
			}
		}
	}

	// --- 按照 uint16 读取，转化成点云的计算方法（Ranger3 图像是左手系）
	{
		SickCam::ImgT img;
		img.loadFromIconFile("D:\\support\\DATA\\ICON\\battery\\fill", 0.008, 1.0, false); // 按照 uint16 读取
		size_t s = img.get_cols() * img.get_rows();
		size_t cols = img.get_cols();

		std::vector<float> xmap(s), ymap(s), zmap(s);
		if (img.has(SickCam::DN::RAN_CAL_16))
		{
			double
				xResolution(img.getCalibration_info().scaleX),
				yResolution(img.getCalibration_info().scaleY),
				zResolution(img.getCalibration_info().scaleZ),
				xOffset(img.getCalibration_info().offsetX),
				yOffset(img.getCalibration_info().offsetY),
				zOffset(img.getCalibration_info().offsetZ);

			uint16_t* pd = (uint16_t*)img.getData(SickCam::DN::RAN_CAL_16);
			for (uint32_t j = 0; j < img.get_rows(); ++j)
			{
				for (uint32_t i = 0; i < img.get_cols(); ++i)
				{
					auto ID = i + j * cols;
					if (pd[ID] == 0)		continue;

					// 点云值
					xmap[ID] = i * xResolution + xOffset;
					ymap[ID] = j * yResolution + yOffset;
					zmap[ID] = pd[ID] * zResolution + zOffset;
				}
			}
		}
	}

	return 0;
}

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	convert_to_point_cloud();
	return 0;
}
#endif
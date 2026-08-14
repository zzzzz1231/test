/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <ImageTable.h>
#include <opencv2/opencv.hpp>

using SickCam::DataNames;


int SICK_GTEST_LIBRARY _convert_to_cv_Mat()
{
	SickCam::ImgT img;
	img.loadFromIconFile("E:\\TEST\\IconData\\Calibrated", 1.0, 1.0);
	cv::Mat src1, src2;
	if (img.has(SickCam::DataNames::RAN_CAL))
	{
		src1 = cv::Mat(img.get_rows(), img.get_cols(), CV_32FC1, (uint8_t*)img.getData(SickCam::DataNames::RAN_CAL));
		cv::imwrite("E:\\TMP\\tR1.tiff", src1);

		// 演示 float 到 uint8 的数据压缩
		double low_range = img.getC_lower_bound_r();
		double up_range = img.getC_upper_bound_r();
		double scale = (up_range - low_range) / 256.0;
		cv::Mat dst8 = ((src1 - low_range) / scale);
		dst8.convertTo(dst8, CV_8UC1);
		
		cv::imwrite("E:\\TMP\\tR1.8b.bmp", dst8);


		src2 = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::REF_CAL));
		cv::imwrite("E:\\TMP\\tI1.bmp", src2);
	}
	return 0;
}


#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	_convert_to_cv_Mat();
	return 0;
}
#endif
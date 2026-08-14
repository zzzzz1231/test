/**
*	@copyright	Copyright 2016-2023 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include "InterfaceOperation.h"
#include <iostream>
#include <map>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <opencv2/opencv.hpp>



double calculateMean(const cv::Mat& img, int height_start, int height_end, const std::vector<int>& middle_of_image) 
{
	double sum = 0;
	int count = 0;
	// 遍历每列，计算总和
	for (int i = 0; i < middle_of_image.size(); ++i) {
		int col = middle_of_image[i];
		for (int row = height_start; row < height_end; ++row) {
			sum += img.at<uchar>(row, col);
			++count;
		}
	}

	// 计算平均值
	double mean = sum / count;
	return mean;
}

int DARK_COLS_WIDTH = 256;
int HIRES_WIDTH = 5120;


void SICK_GTEST_LIBRARY Demo_R35_color_Init_1_white_balance()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
	std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::Ranger3Shared>("",
		SickCam::Ranger3Shared::getPathToRanger3Producer(),
		false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else
	{
		print_process_ng("Error: scan camera error. \n");
		return;
	}

	// --- Init Ranger3
	auto pCam = std::make_shared<SickCam::Ranger3>(pR3S, ip, true);

	// --- Connect Ranger3
	auto err = pCam->connectCamera();

	//  --- Set parameters
	err = pCam->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image. 若要 3D Range，请设置为 Linescan3D.
	pCam->setCalibration_No();

	// return values
	int	exp_time_prev = 0;
	double red_gain = 1.0;
	double green_gain = 1.0;
	double blue_gain = 1.0;


	SickCam::ImgT imgTable;
	double gain_threshold = 240 * 0.8;
	int height_of_image = 20;
	int exp_time_low = 600; // 4;
	double exp = exp_time_low;

	int nr_of_pixel_in_each_line = 1000;
	std::vector<int>middle_of_image = {
		DARK_COLS_WIDTH + HIRES_WIDTH / 2 - nr_of_pixel_in_each_line / 2,
		DARK_COLS_WIDTH + HIRES_WIDTH / 2 + nr_of_pixel_in_each_line / 2,
	};

	cv::namedWindow("test", cv::WindowFlags::WINDOW_AUTOSIZE);


	for (int i = 0; i < 500; )
	{
		// 设置曝光
		err = pCam->stopCamera();
		err = pCam->setParameter("ExposureTime_RegionSelector_RegionHiresColor", std::to_string(exp));
		err = pCam->startCamera();

		int count = 0;
		while (SickCam::CAM_STATUS::All_OK != pCam->getImageData(imgTable))
		{
			std::cout << ".";
			Sleep(100);
			if (++count > 50)
				break;
		};

		// 获得图像
		cv::Mat R = cv::Mat(imgTable.get_rows(), imgTable.get_cols() * 2, CV_8UC1, imgTable.getData(SickCam::DataNames::R));
		cv::Mat G = cv::Mat(imgTable.get_rows(), imgTable.get_cols() * 2, CV_8UC1, imgTable.getData(SickCam::DataNames::G));
		cv::Mat B = cv::Mat(imgTable.get_rows(), imgTable.get_cols() * 2, CV_8UC1, imgTable.getData(SickCam::DataNames::B));


		double red_mean = calculateMean(R, 2, height_of_image - 2, middle_of_image);
		double green_mean = calculateMean(G, 2, height_of_image - 2, middle_of_image);
		double blue_mean = calculateMean(B, 2, height_of_image - 2, middle_of_image);


		// merge b g r
		cv::Mat color;
		cv::merge(std::vector<cv::Mat>{B, G, R}, color);
		cv::imshow("test", color);
		cv::waitKey(1);

		double biggest = std::max({ red_mean, green_mean, blue_mean });

		std::cout << "Exp:" << exp << ", red mean: " << red_mean
			<< ", green mean: " << green_mean << ", blue mean: " << blue_mean << std::endl;

		if (biggest < gain_threshold)
		{
			exp_time_prev = exp;
			if (biggest == red_mean)
			{
				red_gain = 1;
				green_gain = red_mean / green_mean;
				blue_gain = red_mean / blue_mean;
			}
			else if (biggest == green_mean)
			{
				red_gain = green_mean / red_mean;
				green_gain = 1;
				blue_gain = green_mean / blue_mean;
			}
			else if (biggest == blue_mean)
			{

				red_gain = blue_mean / red_mean;
				green_gain = blue_mean / green_mean;
				blue_gain = 1;
			}
		}

		// Increment exposure time by 20%
		exp *= 1.2;

		if (biggest > gain_threshold)
		{
			std::cout << "Done!" << std::endl;
			//std::cout << "Exp:" << exp << ", red mean: " << red_mean
			//	<< ", green mean: " << green_mean << ", blue mean: " << blue_mean << std::endl;

			std::cout << "Exp(us):" << exp << "red gain: " << red_gain
				<< ", green gain: " << green_gain << ", blue gain: " << blue_gain << std::endl;

			break;
		}

	}


	err = pCam->stopCamera();

	err = pCam->disconnectCamera();

}


////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	std::cout << "Done!" << std::endl;
	std::cout << "Exp(us):" << 120 << ", red gain: " << 1.0
		<< ", green gain: " << 1.12 << ", blue gain: " << 0.98 << std::endl;

	return 0;



#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
	
	Demo_R35_color_Init_1_white_balance();

	return 0;

}
#endif


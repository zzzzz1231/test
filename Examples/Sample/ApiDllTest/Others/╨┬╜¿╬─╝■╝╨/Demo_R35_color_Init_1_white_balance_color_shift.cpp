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



// 参数加载
// 不同的架设，对应着这里的不同的参数。需要初始化过程确定。
struct color_params
{
	// 成像原理决定了R G B三通道存在Y向的偏差。这里设置的是：彩色 R G B 三通道的沿着 Y 方向的偏移量
	int red_shift = 0;     // red 是基准，所以无需偏移。
	int green_shift = -9;  // green 图像在Y方向上，相对 red 图像偏移。一般负数表示向Y的下方偏移。
	int blue_shift = -18; // green 图像在Y方向上，相对 red 图像偏移。

	// 经过 Y 向偏移调整后的图像，在头尾会出现重影。这里设置的是：在彩色图像的Y方向头尾，需要剪除的行的数量。参见文档，附录1-1
	int remove_rows_Y_begin = 18; // 一般来说，可以取 green_shift 和 blue_shif t的绝对值的最大值
	int remove_rows_Y_end = 0;

	// 
	double global_scale = 1.0;
	double red_scale = 1.16019;
	double green_scale = 1.01414;
	double blue_scale = 1;
	double dc_level = 0;

};
color_params g_color_params;


cv::Mat shiftImage(const cv::Mat& img, int shift) {
	// 检查图像是否为空
	if (img.empty()) {
		throw std::runtime_error("Input image is empty.");
	}

	// 检查图像是否为单通道
	if (img.channels() != 1) {
		throw std::runtime_error("Input image must be single-channel.");
	}

	int rows = img.rows;
	int cols = img.cols;
	cv::Mat shiftedImg(img.size(), img.type(), cv::Scalar(0));

	// 复制图像到新图像上
	for (int i = 0; i < rows; ++i) {
		int shiftedRow = (i + shift + rows) % rows; // 计算当前行在原始图像中的位置
		//shiftedImg.row(i) = img.row(shiftedRow).clone();
		img.row(shiftedRow).copyTo(shiftedImg.row(i));
	}

	return shiftedImg;
}

cv::Mat color_shift(const cv::Mat& R, const cv::Mat& G, const cv::Mat& B,
	const int& green_shifts,
	const int& blue_shifts,
	const int& max_nr_color_shifts = 25,
	const double& red_scale = 1.0,
	const double& green_scale = 1.0,
	const double& blue_scale = 1.0,
	const double& global_scale = 1.0,
	const double& dc_level = 1.0
)
{
	// process green, blue

	cv::Mat BShift = shiftImage(B, blue_shifts);
	cv::Mat GShift = shiftImage(G, green_shifts);
	cv::Mat RShift = R; // shiftImage(R, 0);

	// merge b g r
	cv::Mat color;
	cv::merge(std::vector<cv::Mat>{BShift, GShift, RShift}, color);

	return color;
}


void init_white_balance()
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



	int n(500);

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


	for (int i = 0; i < n; )
	{
		// 设置曝光
		err = pCam->stopCamera();
		err = pCam->setParameter("ExposureTime_RegionSelector_RegionHiresColor", std::to_string(exp));
		//std::cout << SickCam::CAM_STATUS_str(err) << std::endl;
		err = pCam->startCamera();

		int count = 0;
		while (SickCam::CAM_STATUS::All_OK != pCam->getImageData(imgTable))
		{
			std::cout << ".";
			Sleep(100);
			if (++count > 50)
				break;
		};
		//std::cout << ++i << " Image : " << imgTable.collectInfo() << std::endl;

		// 获得图像
		cv::Mat R = cv::Mat(imgTable.get_rows(), imgTable.get_cols() * 2, CV_8UC1, imgTable.getData(SickCam::DataNames::R));
		cv::Mat G = cv::Mat(imgTable.get_rows(), imgTable.get_cols() * 2, CV_8UC1, imgTable.getData(SickCam::DataNames::G));
		cv::Mat B = cv::Mat(imgTable.get_rows(), imgTable.get_cols() * 2, CV_8UC1, imgTable.getData(SickCam::DataNames::B));


		double red_mean = calculateMean(R, 2, height_of_image - 2, middle_of_image);
		double green_mean = calculateMean(G, 2, height_of_image - 2, middle_of_image);
		double blue_mean = calculateMean(B, 2, height_of_image - 2, middle_of_image);


		// 偏移
		int green_s = -9, blue_s = -18; // 举例：这里的 -9 -18 就是手动查找到的最佳值
		cv::Mat color = color_shift(R, G, B, green_s, blue_s);

		// 裁剪
		cv::Mat colorCrop = color.rowRange(g_color_params.remove_rows_Y_begin, color.rows - g_color_params.remove_rows_Y_end);
		cv::resize(colorCrop, colorCrop, cv::Size(), 0.3, 0.3);
		cv::imshow("test", colorCrop);
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
			std::cout << "Exp:" << exp << ", red mean: " << red_mean
				<< ", green mean: " << green_mean << ", blue mean: " << blue_mean << std::endl;

			std::cout << "red gain: " << red_gain
				<< ", green gain: " << green_gain << ", blue gain: " << blue_gain << std::endl;

			break;
		}

	}


	err = pCam->stopCamera();

	err = pCam->disconnectCamera();

}



void check_rgb_shift_balance()
{
	cv::Mat r = cv::imread("D:\\123\\1_cr.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat g = cv::imread("D:\\123\\1_cg.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat b = cv::imread("D:\\123\\1_cb.tiff", cv::IMREAD_UNCHANGED);


	// 最终检查，参数是否合适

	// 白平衡
	cv::Mat red_balanced   = r * g_color_params.red_scale * g_color_params.global_scale - g_color_params.dc_level;
	cv::Mat green_balanced = g * g_color_params.green_scale * g_color_params.global_scale - g_color_params.dc_level;
	cv::Mat blue_balanced  = b * g_color_params.blue_scale * g_color_params.global_scale - g_color_params.dc_level;


	// 偏移
	int green_s = -9, blue_s = -18; // 举例：这里的 -9 -18 就是手动查找到的最佳值
	cv::Mat color = color_shift(red_balanced, green_balanced, blue_balanced, green_s, blue_s);


	// 裁剪
	cv::Mat colorCrop = color.rowRange(g_color_params.remove_rows_Y_begin, color.rows - g_color_params.remove_rows_Y_end);



	// 保存
	std::stringstream ss;
	ss << "D:\\123\\1_colorAdj_g" << green_s << "_b" << blue_s
		<< "Y" << g_color_params.remove_rows_Y_begin << "_" << g_color_params.remove_rows_Y_end << "_wbalanced.tiff";



	cv::imwrite(ss.str(), colorCrop);

}


////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	//cv::Mat ones = cv::Mat::ones(10, 10, CV_8UC1);
	//std::cout << calculateMean(ones, 2, 5, { 1, 3 }) << std::endl;
	//return 0;

#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
	
	//init_white_balance();
	
	check_rgb_shift_balance();

	return 0;

}
#endif


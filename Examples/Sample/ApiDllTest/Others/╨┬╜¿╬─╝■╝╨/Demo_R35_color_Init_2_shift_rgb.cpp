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


cv::Mat sobelConversion(const cv::Mat& src) {
	cv::Mat sobelX, sobelY;
	cv::Sobel(src, sobelX, CV_64F, 1, 0);
	cv::Sobel(src, sobelY, CV_64F, 0, 1);
	cv::Mat sobel;
	cv::absdiff(sobelX, sobelY, sobel);
	cv::Mat sobelNorm;
	cv::normalize(sobel, sobelNorm, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	//src = sobelNorm.clone();
	return sobelNorm;
}

cv::Point findColorShifts(const cv::Mat& red, const cv::Mat& green, const cv::Mat& blue, int numShifts) {
	// Ensure the input images are valid and have the same size
	if (red.empty() || green.empty() || blue.empty() || red.size() != green.size() || green.size() != blue.size()) {
		std::cerr << "Invalid input images." << std::endl;
		return cv::Point(0, 0);
	}

	// Apply Sobel filter to each color channel
	cv::Mat redSobel, greenSobel, blueSobel;
	redSobel = sobelConversion(red);
	greenSobel = sobelConversion(green);
	blueSobel = sobelConversion(blue);

	// Initialize shift arrays
	std::vector<double> rbShifts(numShifts * 2, 0.0);
	std::vector<double> rgShifts(numShifts * 2, 0.0);

	// Calculate shifts
	for (int i = -numShifts; i < numShifts; ++i) {
		cv::Mat blueShifted = blueSobel.clone();
		cv::Mat greenShifted = greenSobel.clone();
		cv::Mat redCropped = redSobel(cv::Rect(numShifts, 0, red.cols - 2 * numShifts, red.rows));

		// Shift images
		cv::Mat shiftMatrix = (cv::Mat_<double>(2, 3) << 1, 0, i, 0, 1, 0);
		cv::warpAffine(blueShifted, blueShifted, shiftMatrix, blueSobel.size());
		cv::warpAffine(greenShifted, greenShifted, shiftMatrix, greenSobel.size());

		cv::Mat blueCropped = blueShifted(cv::Rect(numShifts, 0, blue.cols - 2 * numShifts, blue.rows));
		cv::Mat greenCropped = greenShifted(cv::Rect(numShifts, 0, green.cols - 2 * numShifts, green.rows));

		// Calculate absolute difference and mean
		cv::Mat rbDiff, rgDiff;
		cv::absdiff(redCropped, blueCropped, rbDiff);
		cv::absdiff(redCropped, greenCropped, rgDiff);
		double rbMean = cv::mean(rbDiff)[0];
		double rgMean = cv::mean(rgDiff)[0];

		// Store the results
		rbShifts[i + numShifts] = rbMean;
		rgShifts[i + numShifts] = rgMean;
	}

	// Find the index of the minimum shift
	int greenShiftIndex = std::distance(rgShifts.begin(), std::min_element(rgShifts.begin(), rgShifts.end()));
	int blueShiftIndex = std::distance(rbShifts.begin(), std::min_element(rbShifts.begin(), rbShifts.end()));

	return cv::Point(greenShiftIndex - numShifts, blueShiftIndex - numShifts);
}


//这个函数首先检查输入图像是否有效，然后应用水平偏移，计算左侧暗列的平均值，并将其应用为校正图像。最后，它从偏移后的图像中减去校正图像的值，但确保结果不小于0。
cv::Mat HFPN_correction(const cv::Mat& img, const int& offset, const int& DARK_COLS_WIDTH = 10) {
	// 检查图像是否为空或不是单通道
	if (img.empty() || img.channels() != 1) {
		throw std::runtime_error("Input image must be non-empty and single-channel.");
	}

	int h = img.rows;
	int w = img.cols;

	// 创建一个与输入图像相同大小和类型的图像，用于存储偏移后的图像
	cv::Mat img_offseted = img.clone();

	// 应用水平偏移
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			img_offseted.at<uchar>(i, j) = std::min(255, img.at<uchar>(i, j) + offset);
		}
	}

	// 计算左侧暗列的平均值
	std::vector<double> dark_cols(h, 0);
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < std::min(DARK_COLS_WIDTH, w); ++j) {
			dark_cols[i] += img.at<uchar>(i, j);
		}
		if (DARK_COLS_WIDTH > 0) {
			dark_cols[i] /= std::min(DARK_COLS_WIDTH, w);
		}
	}

	// 将暗列平均值转换为图像
	cv::Mat correction_image(h, 1, img.type());
	for (int i = 0; i < h; ++i) {
		cv::Scalar scalar(static_cast<uchar>(std::min(static_cast<uchar>(255), static_cast<uchar>(dark_cols[i]))));
		correction_image.row(i) = scalar;
	}
	correction_image = correction_image.reshape(1, w); // 扩展为单列图像

	// 应用校正
	cv::Mat corrected_img = img_offseted.clone();
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			corrected_img.at<uchar>(i, j) = std::max(0, img_offseted.at<uchar>(i, j) - correction_image.at<uchar>(i, j));
		}
	}

	return corrected_img;
}

cv::Mat VFPN_correction(const cv::Mat& img, const cv::Mat& ref_img, const int& hfpn_offset, const int& vfpn_offset) {
	// 检查图像是否为空或不是单通道
	if (img.empty() || img.channels() != 1 || ref_img.empty() || ref_img.channels() != 1) {
		throw std::runtime_error("Input images must be non-empty and single-channel.");
	}

	int h = img.rows;
	int w = img.cols;

	// 创建一个与输入图像相同大小和类型的图像，用于存储垂直偏移后的图像
	cv::Mat img_offseted = img.clone();

	// 应用垂直偏移
	std::vector<double> vfpn_offset_vals(h, 0);
	for (int i = 0; i < h; ++i) {
		vfpn_offset_vals[i] = vfpn_offset;
	}
	cv::Mat vfpn_offset_arr(h, 1, CV_64F, vfpn_offset_vals.data());
	vfpn_offset_arr.convertTo(vfpn_offset_arr, CV_8U);

	img_offseted = img_offseted + vfpn_offset_arr;

	// 计算校正图像
	cv::Mat correction_img = HFPN_correction(ref_img, hfpn_offset);
	cv::Mat correction_img_mean = correction_img.reshape(1, h); // 转换为一维向量
	cv::reduce(correction_img_mean, correction_img_mean, 0, cv::REDUCE_AVG, CV_8U); // 计算平均值

	// 应用校正
	cv::Mat corrected_img = img_offseted.clone();
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			int val = std::min(img.at<uchar>(i, j), static_cast<uchar>(correction_img_mean.at<uchar>(i, 0)));
			corrected_img.at<uchar>(i, j) = std::max(0, img_offseted.at<uchar>(i, j) - val);
		}
	}

	return corrected_img;
}

cv::Mat remove_dark_cols(const cv::Mat& image, const int& DARK_COLS_WIDTH = 10) {
	// 检查图像是否为空或不是单通道
	if (image.empty() || image.channels() != 1) {
		throw std::runtime_error("Input image must be non-empty and single-channel.");
	}

	// 获取暗列的宽度

	// 使用colRange方法裁剪图像
	cv::Mat croppedImage = image.colRange(DARK_COLS_WIDTH, image.cols);

	return croppedImage;
}

cv::Mat fpnCorrectWhite(const cv::Mat& white, const cv::Mat& darkImg, int hfpn_offset, int vfpn_offset) {
	// 检查图像是否为空或不是单通道
	if (white.empty() || white.channels() != 1 || darkImg.empty() || darkImg.channels() != 1) {
		throw std::runtime_error("Input images must be non-empty and single-channel.");
	}

	// 进行水平FPN校正
	cv::Mat HFPN_corrected = HFPN_correction(white, hfpn_offset);

	// 从dark_img中提取第一个通道作为参考图像
	cv::Mat darkImgChannel;
	cv::extractChannel(darkImg, darkImgChannel, 0);

	// 进行垂直FPN校正
	cv::Mat VFPN_corrected = VFPN_correction(HFPN_corrected, darkImgChannel, hfpn_offset, vfpn_offset);

	// 去除暗列
	VFPN_corrected = remove_dark_cols(VFPN_corrected);

	return VFPN_corrected;
}

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

	// 计算滚动后图像的开始和结束行
	//int startRow = (rows + shift + rows) % rows; // 确保滚动后的行数在范围内
	//int endRow = startRow + rows;

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
	// find green, blue shift
	//cv::Point shifts = findColorShifts(R, G, B, max_nr_color_shifts);
	//std::cout << "Green shift: " << shifts.x << ", Blue shift: " << shifts.y << std::endl;



	// process green, blue
	//cv::Mat GShift = shiftImage(G, shifts.x);
	//cv::Mat BShift = shiftImage(B, shifts.y);

	cv::Mat BShift = shiftImage(B, blue_shifts);
	cv::Mat GShift = shiftImage(G, green_shifts);

	cv::Mat RShift = R; // shiftImage(R, 0);


	// white balance  这里的参数需要通过白平衡操作中计算得到
	//cv::Mat RBalance = RShift * red_scale   * global_scale - dc_level;
	//cv::Mat GBalance = GShift * green_scale * global_scale - dc_level;
	//cv::Mat BBalance = BShift * blue_scale  * global_scale - dc_level;



	// merge b g r
	cv::Mat color;
	cv::merge(std::vector<cv::Mat>{BShift, GShift, RShift}, color);
	//cv::merge(std::vector<cv::Mat>{RBalance, GBalance, RBalance}, color);

	return color;
}



// 参数加载
// 不同的架设，对应着这里的不同的参数。需要初始化过程确定。
struct color_params
{
	// 成像原理决定了R G B三通道存在Y向的偏差。这里设置的是：彩色 R G B 三通道的沿着 Y 方向的偏移量
	int red_shift = 0;     // red 是基准，所以无需偏移。
	int green_shift = -9;  // green 图像在Y方向上，相对 red 图像偏移。一般负数表示向Y的下方偏移。
	int blue_shift  = -18; // green 图像在Y方向上，相对 red 图像偏移。

	// 经过 Y 向偏移调整后的图像，在头尾会出现重影。这里设置的是：在彩色图像的Y方向头尾，需要剪除的行的数量。参见文档，附录1-1
	int remove_rows_Y_begin = 18; // 一般来说，可以取 green_shift 和 blue_shif t的绝对值的最大值
	int remove_rows_Y_end   = 0;

};

color_params g_color_params;


bool find_rgb_shift()
{
	cv::Mat r = cv::imread("D:\\123\\1_cr.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat g = cv::imread("D:\\123\\1_cg.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat b = cv::imread("D:\\123\\1_cb.tiff", cv::IMREAD_UNCHANGED);

	// Python 中，算法直接找的rgb偏移不准

	///////////////////////////////////// 手动查找

	cv::Mat back = cv::Mat::zeros(b.size(), b.type());

	// 设定green分量的范围
	for (int gs = -15; gs < 15; ++gs)
	{
		cv::Mat color = color_shift(r, g, back, gs, 0);

		std::stringstream ss;
		ss << "D:\\123\\findshift_rg\\1_colorAdj_g" << gs << ".tiff";
		cv::imwrite(ss.str(), color);

		std::cout << ss.str() << std::endl;
	}

	// 设定blue分量的范围
	for (int bs = -20; bs < -14; ++bs)
	{
		cv::Mat color = color_shift(r, back, b, 0, bs);

		std::stringstream ss;
		ss << "D:\\123\\findshift_rb\\1_colorAdj_b" << bs << ".tiff";
		cv::imwrite(ss.str(), color);

		std::cout << ss.str() << std::endl;
	}

	// 保存数据（best gs，best bs）到纸上。后续正式运行时候，需要加载这些参数
	g_color_params.green_shift = -9; // 举例：这里的 -9 -18 就是手动查找到的最佳值
	g_color_params.blue_shift = -18;
	g_color_params.remove_rows_Y_begin = 18;
	g_color_params.remove_rows_Y_end = 0;


	return true;
}

bool check_rgb_shift()
{
	cv::Mat r = cv::imread("D:\\123\\1_cr.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat g = cv::imread("D:\\123\\1_cg.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat b = cv::imread("D:\\123\\1_cb.tiff", cv::IMREAD_UNCHANGED);


	// 最终检查，参数是否合适

	// 偏移
	int green_s = -9, blue_s = -18; // 举例：这里的 -9 -18 就是手动查找到的最佳值
	cv::Mat color = color_shift(r, g, b, green_s, blue_s);

	// 裁剪
	cv::Mat colorCrop = color.rowRange(g_color_params.remove_rows_Y_begin, color.rows - g_color_params.remove_rows_Y_end);

	// 保存
	std::stringstream ss;
	ss  << "D:\\123\\1_colorAdj_g" << green_s << "_b" << blue_s 
		<< "Y" << g_color_params.remove_rows_Y_begin << "_"<< g_color_params.remove_rows_Y_end << ".tiff";
	cv::imwrite(ss.str(), colorCrop);

	return true;
}



////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	// 遍历输出图像，查找最合适的偏移值
	//find_rgb_shift();

	// 检查参数是否合适
	check_rgb_shift();

	return 0;
}
#endif


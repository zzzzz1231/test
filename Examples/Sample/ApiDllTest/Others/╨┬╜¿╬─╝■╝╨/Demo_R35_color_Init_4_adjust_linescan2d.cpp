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
#include <sstream>

#include <numeric> // 用于 std::accumulate


// TODO:
void findOverlapRegion()
{}

cv::Mat overlapImages(const cv::Mat& base, const cv::Mat& src, const int px, const int py, const double alpha = 0.5) 
{
	// 检查图像是否为空
	if (base.empty() || src.empty()) {
		throw std::runtime_error("One or both input images are empty.");
	}

	// 检查图像类型是否相同
	if (base.type() != src.type()) {
		throw std::runtime_error("Images have different types.");
	}


	cv::Mat base_red;
	cv::Mat src_green;
	cv::Mat zeros = cv::Mat::zeros(base.size(), base.type());
	cv::Mat result_red;

	cv::merge(std::vector<cv::Mat>({ zeros, zeros, base }), base_red);
	cv::merge(std::vector<cv::Mat>({ zeros, src,  zeros }), src_green);
	base_red.copyTo(result_red);

	cv::addWeighted(
		src_green(cv::Rect(0, 0, src.cols - px, src.rows - py)), alpha,
		result_red(cv::Rect(px, py, src.cols - px, src.rows - py)), 1 - alpha,
		0, result_red(cv::Rect(px, py, src.cols - px, src.rows - py)));


	return result_red;
}




// Ref 和 linescan2d 的交际部分保留，以ref为基准，对齐。其他区域填0。
cv::Mat cropImage(const cv::Mat& linescan2d, const int remove_heads, const int remove_tails)
{
	return cv::Mat();
}


// 视野大小调整
cv::Mat alignFOV(const cv::Mat& linescan2d, const double scale, const double offset)
{
	return cv::Mat();
}


void adjustYByRangeConfigure(const std::vector<int>& deltaRanges, const std::vector<int>& deltaYs, double& scale, double& offset) 
{
	if (deltaRanges.size() != deltaYs.size() || deltaRanges.empty()) 
	{
		throw std::invalid_argument("The size of deltaRanges and deltaYs must be the same and not empty.");
	}

	// 1. 计算平均值
	double meanDeltaRange = std::accumulate(deltaRanges.begin(), deltaRanges.end(), 0.0) / deltaRanges.size();
	double meanDeltaY = std::accumulate(deltaYs.begin(), deltaYs.end(), 0.0) / deltaYs.size();

	// 2. 中心化数据
	std::vector<double> centeredDeltaRanges(deltaRanges.size());
	std::vector<double> centeredDeltaYs(deltaYs.size());
	for (size_t i = 0; i < deltaRanges.size(); ++i) 
	{
		centeredDeltaRanges[i] = deltaRanges[i] - meanDeltaRange;
		centeredDeltaYs[i] = deltaYs[i] - meanDeltaY;
	}

	// 3. 计算斜率（scale）
	double numerator = 0.0;
	double denominator = 0.0;
	for (size_t i = 0; i < centeredDeltaRanges.size(); ++i) 
	{
		numerator += centeredDeltaRanges[i] * centeredDeltaYs[i];
		denominator += centeredDeltaRanges[i] * centeredDeltaRanges[i];
	}
	scale = numerator / denominator;

	// 4. 计算截距（offset）
	offset = meanDeltaY - (scale * meanDeltaRange);

	// 如果需要，可以在这里添加对 scale 和 offset 的进一步处理
	// 例如，确保 scale 不为 0，或者应用其他业务逻辑
}

// 按照 Range 重新调整 Y 值。   这里的问题是，linescane2d 没有 Range 值。
cv::Mat adjustYByRange(const cv::Mat& linescan2d, const double scale, const double offset)
{
	return cv::Mat();
}


// debug
void appendImages(bool debug)
{
	int imageNumber = 16;

	// 读取两个图像
	std::string mark = "ran";

	std::string path = "D:\\123\\test2\\";
	std::string name = "test2";

	cv::Mat image1 = cv::imread(path + name + "1_" + mark + ".tiff", cv::IMREAD_GRAYSCALE);
	int rows = imageNumber * image1.rows;
	int cols = image1.cols;

	cv::Mat result;
	result.create(cv::Size(cols, rows), image1.type());

	for (int i = 1; i <= imageNumber; ++i)
	{
		std::stringstream ss;
		ss << path + name << i << "_" + mark + ".tiff";

		cv::Mat image = cv::imread(ss.str(), cv::IMREAD_GRAYSCALE);
		image.copyTo(result(cv::Rect(0, (i - 1) * image.rows, image.cols, image.rows)));
	}


	// 保存结果图像
	cv::resize(result, result, cv::Size(0, 0), 1.0, 0.04);// 0.04-2560   0.08-5120
	//cv::resize(result, result, cv::Size(2560, 960));// 0.04 0.08
	cv::imwrite(path + "1" + mark + ".tiff", result);
}




std::string windowName = "Reflectance";
cv::Mat displayedImage;
double scaleFactor = 1.0;
int baseWidth = 0;
int baseHeight = 0;
int mouseX = 0;        // 鼠标在图像上的x坐标
cv::Point2f offset(0,0);    // 图像平移量
cv::Rect rect;
void on_mouse_scale(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_MOUSEWHEEL) 
	{
		// 获取窗口尺寸
		cv::Rect windowRect = cv::getWindowImageRect(windowName);
		if (windowRect.width == -1 || windowRect.height == -1) {
			std::cout << "Could not get window size." << std::endl;
			return;
		}

		// 计算缩放比例
		double scaleX = static_cast<double>(windowRect.width) / displayedImage.cols;
		double scaleY = static_cast<double>(windowRect.height) / displayedImage.rows;




		double step = 0.01;
		std::cout << x << "," << y << std::endl;
		std::cout << scaleFactor << std::endl;
		
		int x_px = x / scaleFactor + rect.x;
		int y_px = y / scaleFactor + rect.y;

		scaleFactor += (flags > 0) ? step : -step;
		scaleFactor = std::max(1.0, scaleFactor); // 限制最小缩放比例
		//scaleFactor = std::min(0.5, scaleFactor); // 限制最大缩放比例

		int x_now = scaleFactor * x_px;
		int y_now = scaleFactor * y_px;

		rect.x = x_now - scaleFactor * (x_px - rect.x);
		rect.y = y_now - scaleFactor * (y_px - rect.y);

		
		if (rect.x + rect.width > displayedImage.cols)
		{
			rect.width = displayedImage.cols - rect.x;
		}
		else
			rect.width = baseWidth;

		if (rect.y + rect.height > displayedImage.rows)
		{
			rect.height = displayedImage.rows - rect.y;
		}
		else
			rect.height = baseHeight;





		//cv::Mat dst;
		//cv::resize(displayedImage(rect), dst, cv::Size(0, 0), scaleFactor, scaleFactor);
		//cv::merge(std::vector<cv::Mat>{dst, dst, dst}, dst);



		cv::Mat dst = displayedImage.clone();
		cv::merge(std::vector<cv::Mat>{dst, dst, dst}, dst);

		cv::drawMarker(dst, cv::Point(x_px, y_px), cv::Scalar(0,0,255));
		cv::rectangle(dst, rect, cv::Scalar(255, 0, 0));

		//cv::imshow(windowName, dst);
		cv::resizeWindow(windowName, cv::Size(baseWidth, baseHeight));
		cv::imshow(windowName, dst);
	}
}
void alignRanger35FOV(const cv::Mat& baseReflectance, const cv::Mat& red, const cv::Mat& green, const cv::Mat& blue, const cv::Mat& white)
{
	// resize ref
	cv::Mat baseRef;
	cv::resize(baseReflectance, baseRef, cv::Size(0, 0), 2.0, 2.0);

	displayedImage = baseRef;
	windowName = "Reflectance";
	cv::namedWindow(windowName, cv::WINDOW_KEEPRATIO);
	//cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
	cv::resizeWindow(windowName, cv::Size(baseWidth, baseHeight));
	//cv::resizeWindow(windowName, cv::Size(displayedImage.cols/5, displayedImage.rows/5));
	baseWidth = displayedImage.cols / 2;
	baseHeight = displayedImage.rows / 2;
	rect = cv::Rect(0,0, displayedImage.cols, displayedImage.rows);
	scaleFactor = 1.0;
	cv::imshow(windowName, displayedImage);
	cv::setMouseCallback(windowName, on_mouse_scale);





	// 等待键盘事件
	cv::waitKey(0);




}



////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	//appendImages(true);

	/* 【视野对齐】 */

	cv::Mat imageRef = cv::imread("D:\\123\\test2\\1ref.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat imageRed = cv::imread("D:\\123\\test2\\1cr.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat imageGreen = cv::imread("D:\\123\\test2\\1cg.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat imageBlue = cv::imread("D:\\123\\test2\\1cb.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat imageWhite = cv::imread("D:\\123\\test2\\1white.tiff", cv::IMREAD_UNCHANGED);
	
	//cv::resize(image2, image2, cv::Size(2560, 960));//0.04 0.08

	// find
	alignRanger35FOV(imageRef, imageRed, imageGreen, imageBlue, imageWhite);


	//overlapImages(image1, image2, 0, 500); // 验证


	/* 给定一个标志点，查找到图像的对其位置 */


	/* 图像做适当的截取和补充 */


	/* 【架设校正】根据Range，调整Y值 */

	// 数据可以从Range图像上获得
	//cv::Mat range = cv::imread("D:\\123\\test2\\1ran.tiff", cv::IMREAD_UNCHANGED);

	//std::vector<int> deltaRs = {

	//};
	//std::vector<int> deltaYs;
	//double scale = 0.0; 
	//double offset = 0.0;
	//adjustYByRangeConfigure(deltaRs, deltaYs, scale, offset);




	/*  */






	return 0;

}
#endif


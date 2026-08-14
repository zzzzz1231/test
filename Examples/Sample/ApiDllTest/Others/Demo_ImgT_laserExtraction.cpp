/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include <ImageTable.h>
#include <opencv2/opencv.hpp>
#include <omp.h>

using SickCam::DataNames;

std::vector<int> extractProfile(const cv::Mat& src, const int& maxThreshold, const int& step = 4, const int& missingData=-1)
{
	if (src.empty())
		return std::vector<int>();

	std::vector<int> profile(src.cols);
	cv::Mat imgTp;
	cv::transpose(src, imgTp); // traverse faster

#pragma omp parallel for 
	for (int j = 0; j < profile.size(); ++j)
	{
		auto pd = imgTp.ptr(j);
		int up = -1, down = imgTp.cols;
		// start from 0, 间隔 step 查找
		for (int c = 0; c < imgTp.cols; c += step)
		{
			if (pd[c] > maxThreshold)
			{
				up = c;
				if (c > step)
				{
					// 再逐个查找
					for (int c2 = c - step + 1; c2 < c; ++c2)
					{
						if (pd[c2] > maxThreshold)
							up = c2;
					}
				}
			}
		}
		if (up == -1)
		{
			// missing data
			profile[j] = missingData;
			continue;
		}

		// start from 832, 间隔 step 查找
		for (int c = imgTp.cols - 1; c >= 0; c -= step)
		{
			if (pd[c] > maxThreshold)
			{
				down = c;
				if (c < imgTp.cols - 1 - step)
				{
					// 再逐个查找
					for (int c2 = c + step - 1; c2 > c; --c2)
					{
						if (pd[c2] > maxThreshold)
							down = c2;
					}
				}
			}
		}
		if (down == imgTp.cols)
		{
			// missing data
			profile[j] = missingData;
			continue;
		}

		// get up and down average vale
		profile[j] = (up + down)/2; 
	}
	return profile;
}


std::vector<int> g_pro;
extern "C" __declspec(dllexport)
int* extractProfileC(const char * psrc, const int w, const int h, const int maxThreshold, const int step, const int missingData)
{
	g_pro = extractProfile(
			cv::Mat(h, w, CV_8UC1, (void*)psrc), maxThreshold, step, missingData
		);
	return g_pro.data();
}

extern "C" __declspec(dllexport)
int add(int a, int b)
{
	return a + b;
}


int main(int argc, char* argv[])
{
	// laser images
	SickCam::ImgT img;
	img.loadFromIconFile("D:\\support\\LA\\sensor", 1.0, 1.0);
	cv::Mat src = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::SEN));
	cv::threshold(src, src, 120, 255, CV_THRESH_TOZERO);
	cv::Mat src3;
	cv::merge(std::vector<cv::Mat>{ src, src, src }, src3);

	{
		for (int i = 0; i < 2; ++i)
		{
			auto beg = cv::getTickCount();

			img.test_extractProfile_malloc("sick_use_only_1596*#3@6!16");
			auto profile = img.test_extractProfile("sick_use_only_1596*#3@6!16", 100, 1, -1);

			auto end = cv::getTickCount();
			std::cout << "time1 = " << (end - beg) / cv::getTickFrequency() << std::endl;
			for (int i = 0; i < 2560; ++i)
			{
				if (profile[i] == -1)
					continue;
				cv::circle(src3, cv::Point(i, profile[i]), 0, cv::Scalar(0, 0, 255), -1);
			}
			img.test_extractProfile_free("sick_use_only_1596*#3@6!16");
		}

		return 0;
	}

	/// ////////////////////////////////////////////////////

	{
		auto beg = cv::getTickCount();
		auto profile = extractProfile(src, 120, 4);


		auto end = cv::getTickCount();
		std::cout << "time1 = " << (end - beg) / cv::getTickFrequency() << std::endl;

		for (int i = 0; i < profile.size(); ++i)
		{
			if (profile[i] == -1)
				continue;
			cv::circle(src3, cv::Point(i, profile[i]), 0, cv::Scalar(0, 0, 255), -1);
		}
		return 0;
	}

	{
		int64_t count = 0;
		auto beg = cv::getTickCount();
		for (int j = 0; j < src.rows; ++j)
		{
			auto pd = src.ptr<uchar>(j);
			for (int i = 0; i < src.cols; ++i)
			{
				count += pd[i];
			}
		}
		auto end = cv::getTickCount();
		std::cout << "pd[i]; time1 = " << (end - beg) / cv::getTickFrequency() << ", count = " << count << std::endl;
	}



	
	{
		int64_t count = 0;
		auto beg = cv::getTickCount();
		for (int i = 0; i < src.cols; ++i)
		{
			auto pd = src.col(i);
			for (int j = 0; j < src.rows; ++j)
			{
				count += pd.at<uchar>(j);
			}
		}
		auto end = cv::getTickCount();
		std::cout << "pd.at<uchar>(j); time = " << (end - beg) / cv::getTickFrequency() << ", count = " << count << std::endl;
	}

	{
		int64_t count = 0;
		auto beg = cv::getTickCount();
		for (int j = 0; j < src.rows; ++j)
		{
			auto pd = src.row(j);
			for (int i = 0; i < src.cols; ++i)
			{
				count += pd.at<uchar>(i);
			}
		}
		auto end = cv::getTickCount();
		std::cout << "pd[i]; time1 = " << (end - beg) / cv::getTickFrequency() << ", count = " << count << std::endl;
	}

	{
		int64_t count = 0;
		auto beg = cv::getTickCount();
		cv::transpose(src, src);
		for (int j = 0; j < src.rows; ++j)
		{
			auto pd = src.ptr<uchar>(j);
			for (int i = 0; i < src.cols; ++i)
			{
				count += pd[i];
			}
		}
		auto end = cv::getTickCount();
		std::cout << "cv::transpose(src, src); pd[i]; time1 = " << (end - beg) / cv::getTickFrequency() << ", count = " << count << std::endl;
	}


	return 0;
}


/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include <ImageTable.h>
#include <opencv2/opencv.hpp>

using SickCam::DataNames;


float getZ(const double& a0, const double& a1, const double& a2, const double& a3, const uint16_t& y)
{
	return static_cast<float>(a0 + a1 * y + a2 * y * y + a3 * y * y * y);
}

float getXs(const double& a0, const double& a1, const double& a2, const double& a3, const uint16_t& y)
{
	return static_cast<float>(a0 + a1 * y + a2 * y * y + a3 * y * y * y);
}

//float getZ(const std::string& dataStr, const uint16_t& y)
//{
//	float a0=0, a1=0, a2=0, a3=0;
//	std::vector<float> a(3, 0.f);
//	for (int i=0; i<dataStr.size(); ++i)
//	{
//		auto c = dataStr[i];
//		if (c == '[')
//		{
//			++i; // remove "[ "
//			continue;
//		}
//		std::string numStr = "";
//		if (c == ' ')
//		{
//			a.push_back(atof(numStr.c_str()));
//		}
//		else
//		{
//			numStr.push_back(c);
//		}
//	}
//
//
//
//	return getZ(a0 + a1 * y + a2 * y * y + a3 * y * y * y);
//}
//
//float getXs(const double& a0, const double& a1, const double& a2, const double& a3, const uint16_t& y)
//{
//	return static_cast<float>(a0 + a1 * y + a2 * y * y + a3 * y * y * y);
//}

int main(int argc, char* argv[])
{
	SickCam::ImgT img;
	img.loadFromIconFile("F:\\gitlab\\TriangulationLaserCam\\data\\SN22290001\\Calibration\\DotImage", 1.0, 1.0);
	cv::Mat ran, ref;
	ran = cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::RAN));
	ref = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::REF));


	//  相机标定的参数
	//	x 系数
	//	[ 2.0976878e-17 -4.7144051e-14  4.4485668e-10 -3.2378912e-14]
	//  z 系数
	//	[ 7.6594560e-14 -1.7214084e-10  1.6243408e-06 -1.4176893e-10]

	auto XMiddle = 1080;
	cv::Mat ranCal(ran.size(), CV_32FC1, cv::Scalar(-1.0e6f));
	cv::Mat refCal(ran.size(), CV_8UC1, cv::Scalar(0));
	for (int j=0;j<ran.rows; ++j)
	{
		auto pd = ran.ptr<uint16_t>(j);
		auto pdCal = ranCal.ptr<float>(j);
		for (int i = 0; i < ran.cols; ++i)
		{
			if (pd[i] == 0)
				continue;
			else
			{
				// calc X
				auto XScale = getXs(1.0585793e+00, 6.0213160e-06, 0, 0, pd[i]);
				auto XOffset = XMiddle - XMiddle * XScale;
				//auto XOffset = 0; // XMiddle - XMiddle * XScale;
				auto x = cvRound(i*XScale + XOffset);
				auto y = j;


				// calc Z
				if (0 <= x && x < ran.cols && 0 <= y && y < ran.rows)
				{
					ranCal.at<float>(y, x) = getZ(5.7773876e+01, -4.6641333e-03, 0, 0, pd[i]);
				}
			}
		}
	}


	return 0;
}


/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#define HAS_CALCON 0


#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边

#include <ImageTable.h>
#include <opencv2/opencv.hpp>

#if HAS_CALCON
#include <halconcpp/HalconCpp.h>

// Missing data 是-1.0e6f
HalconCpp::HObject Icon2Halcon_Range(SickCam::ImgT& img)
{
	if (!img.has(SickCam::DataNames::RAN_CAL))
		return HalconCpp::HObject();

	auto pF = img.getData(SickCam::DataNames::RAN_CAL);
	HalconCpp::HObject dst;
	HalconCpp::GenImage1(&dst, "real", (int)img.get_cols(), (int)img.get_rows(), (Hlong)pF);

	return dst;
}


// Missing data 是 0
HalconCpp::HObject Icon2Halcon_Reflectance(SickCam::ImgT& img)
{
	if (!img.has(SickCam::DataNames::REF_CAL))
		return HalconCpp::HObject();

	auto pF = img.getData(SickCam::DataNames::REF_CAL);
	HalconCpp::HObject dst;
	HalconCpp::GenImage1(&dst, "byte", (int)img.get_cols(), (int)img.get_rows(), (Hlong)pF);

	return dst;
}
#endif

int SICK_GTEST_LIBRARY convert_to_Halcon_CPP()
{
#if HAS_CALCON
	// --- 读取标定后图像
	SickCam::ImgT img;
	img.loadFromIconFile("D:\\90", 0.04);
	assert(!img.isEmpty());

	
	// convert to HImage
	auto dst1 = Icon2Halcon_Range(img); //
	auto dst2 = Icon2Halcon_Reflectance(img);

	// 如果想替换 missing data
	{
		// 方法1
		HalconCpp::HObject reg;
		HalconCpp::Threshold(dst1, &reg, -1000, 1000); // 这里的阈值需要根据实际情况调整。默认的 missingdata 是-1.0e6f, 所以阈值的下限要大于它即可。推荐-1000；
		HalconCpp::ReduceDomain(dst1, reg, &dst1);
	}

	//{
	//	// 方法2
	//	HalconCpp::HObject reg;
	//	HalconCpp::Threshold(dst1, &reg, -10000000, -1000); //找到 missing data
	//	HalconCpp::PaintRegion(reg, dst1, &dst1, 0, "fill"); // 用 0 替换 missing data
	//}


	// sve as tiff
	HalconCpp::HTuple max, min, w, h;
	HalconCpp::HObject domain;
	HalconCpp::HTuple gray, gray_dst;

	HalconCpp::GetDomain(dst1, &domain);
	HalconCpp::GetRegionPoints(domain, &w, &h);

	HalconCpp::GetGrayval(dst1, w, h, &gray);
	HalconCpp::TupleMax(gray, &max);
	HalconCpp::TupleMin(gray, &min);
	HalconCpp::HTuple scale = (max - min) / 65535.0;
	gray_dst = (gray / scale).TupleRound();

	HalconCpp::SetGrayval(dst1, w, h, gray_dst);

	HalconCpp::WriteImage(dst1, "tiff", 0, "D:\\img2Htuple.tiff");
	HalconCpp::WriteImage(dst2, "tiff", 0, "D:\\img2Htuple_intensity.tiff");

#else
	std::cout << "No Halcon found!!!" << std::endl;
#endif


	return 0;
}




#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
	convert_to_Halcon_CPP();
	return 0;
}
#endif

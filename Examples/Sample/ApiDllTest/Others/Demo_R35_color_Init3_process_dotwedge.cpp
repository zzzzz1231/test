/**
*	@copyright	Copyright 2016-2023 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include <../CalibrationWrapper/CalibrationWrapper.h>
#include "InterfaceOperation.h"
#include <iostream>
#include <map>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <opencv2/opencv.hpp>


struct colorParam
{
	cv::Mat mat_r;
	cv::Mat mat_g;
	cv::Mat mat_b;
	cv::Mat mat_w;
	int ResizeFactor = 1.0;
	int ColorTopBlackRows = 0;
	int ColorEndBlackRows = 0;

	double RedGain = 1.0;
	double GreenGain = 1.0;
	double BlueGain = 1.0;

	double ReflectanceOrdinaryScale = 1.0;
	double ReflectanceOrdinaryOffset = 0.0;

	double RedOrdinaryScale = 1.0;
	double RedOrdinaryOffset = 0.0;
};

colorParam cop;

void process_rawData_adjustOrdinary_align(
	const colorParam& cop,
	const cv::Mat& i_ran,
	const cv::Mat& i_ref,
	const cv::Mat& i_scatter,
	const cv::Mat& i_r,
	const cv::Mat& i_g,
	const cv::Mat& i_b,
	const cv::Mat& i_w,
	cv::Mat& dst_ran,
	cv::Mat& dst_ref,
	cv::Mat& dst_scatter,
	cv::Mat& dst_red,
	cv::Mat& dst_green,
	cv::Mat& dst_blue,
	cv::Mat& dst_color,
	cv::Mat& dst_white,
	bool is_debug = false,
	const std::string& debug_img_save_to = ""
);

colorParam loadColorParam(const std::string xmlpath);

using namespace SickCam;
using namespace cv;


bool SICK_GTEST_LIBRARY 
process_dotwedge()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;

	// --- 读取原始图像
	SickCam::ImgT img;
	img.loadFromIconFile("D:\\uncalibrated\\DotImage", 1.0);

	cv::Mat range, reflectance, mark;
	if (img.has(SickCam::DataNames::RAN))
		range = cv::Mat(img.get_rows(), img.get_cols(), CV_16SC1, (uint8_t*)img.getData(SickCam::DataNames::RAN));

	if (img.has(SickCam::DataNames::REF))
		reflectance = cv::Mat(img.get_rows(), img.get_cols(), CV_8UC1, (uint8_t*)img.getData(SickCam::DataNames::REF));

	// 放大 Range 和 Reflectance 图像方便后续对齐。
	cv::resize(range, range, cv::Size(5120, range.rows));
	cv::resize(reflectance, reflectance, cv::Size(5120, range.rows));


	// --- Process (Aligned)
	cv::Mat adj_range;
	cv::Mat adj_reflectance;
	cv::Mat adj_scatter = cv::Mat();
	cv::Mat adj_red;
	cv::Mat adj_green;
	cv::Mat adj_blue;
	cv::Mat adj_color;
	cv::Mat adj_white;

	process_rawData_adjustOrdinary_align(cop,
		range,
		reflectance,
		cv::Mat(),
		cv::Mat(),
		cv::Mat(),
		cv::Mat(),
		cv::Mat(),
		adj_range      ,
		adj_reflectance,
		adj_scatter    ,
		adj_red        ,
		adj_green      ,
		adj_blue       ,
		adj_color      ,
		adj_white
	);

	// --- Save
	img.setR_cols(5120);
	img.insertDataCopy(DN::RAN, (uint8_t*)(adj_range.data), 1);
	img.insertDataCopy(DN::REF, (uint8_t*)(adj_reflectance.data), 1);

	img.SaveRawImagesToIconFile("D:\\uncalibrated\\adjDotImage", img.has(SickCam::DataNames::MAR));

	print_process_ok("All done! ");
	return true;
}


////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif
	cop  = loadColorParam("D:/color/config.xml");
	process_dotwedge();

	return 0;
}
#endif



//////////////////////////////////////////////////////




colorParam loadColorParam(const std::string xmlpath)
{
	colorParam cop;

	//
	cv::FileStorage fs(xmlpath, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::cerr << "Error: Could not open XML file. Path=" << xmlpath << std::endl;
		return colorParam();
	}


	fs["mat_r"] >> cop.mat_r;
	fs["mat_g"] >> cop.mat_g;
	fs["mat_b"] >> cop.mat_b;
	fs["mat_w"] >> cop.mat_w;
	fs["ResizeFactor"] >> cop.ResizeFactor;
	fs["ColorTopBlackRows"] >> cop.ColorTopBlackRows;
	fs["ColorEndBlackRows"] >> cop.ColorEndBlackRows;
	fs["RedGain"] >> cop.RedGain;
	fs["GreenGain"] >> cop.GreenGain;
	fs["BlueGain"] >> cop.BlueGain;
	fs["ReflectanceOrdinaryScale"] >> cop.ReflectanceOrdinaryScale;
	fs["ReflectanceOrdinaryOffset"] >> cop.ReflectanceOrdinaryOffset;
	fs["RedOrdinaryScale"] >> cop.RedOrdinaryScale;
	fs["RedOrdinaryOffset"] >> cop.RedOrdinaryOffset;

	fs.release();
	return cop;
}


void adj_ordinary_setup(
	const double& scale, const double& offset,
	const cv::Mat& i_ran, const cv::Mat& i_src, cv::Mat& dst_ran, cv::Mat& dst_src,
	bool processRange = false, bool processSrc = false)
{
	//
	if (processRange)
		dst_ran = cv::Mat::zeros(i_ran.size(), i_ran.type());

	if (processSrc)
		dst_src = cv::Mat::zeros(i_src.size(), i_src.type());

	if (!processRange && !processSrc)
	{
		std::cerr << "adj_ordinary_setup: Logic error!" << std::endl;
		return;
	}

	cv::Mat mark = cv::Mat::zeros(i_ran.size(), CV_8UC1);

	//
	for (int j = 0; j < i_ran.rows; ++j)
	{
		auto ptr_ran = i_ran.ptr<uint16_t>(j);
		auto ptr_src = i_src.ptr<uint8_t>(j);

		for (int i = 0; i < i_ran.cols; ++i)
		{
			if (ptr_ran[i] == 0)
				continue;


			auto deltY = ptr_ran[i] * scale + offset;
			int y = j - cvRound(deltY);

			if (y < 0 || y >= i_ran.rows)
				continue;

			// add to mark
			mark.at<uint8_t>(y, i) = 255; // 255 点不需要插值

			// fill it
			if (processRange)
				dst_ran.at<uint16_t>(y, i) = ptr_ran[i];
			
			if (processSrc)
				dst_src.at<uint8_t>(y, i) = ptr_src[i];

			// fill adjcent pts
			int yt = y - 1;
			if (yt < 0 || yt >= i_ran.rows)
				continue;
			else
			{
				if (processRange)
				{
					if (dst_ran.at<uint16_t>(yt, i) == 0)
						dst_ran.at<uint16_t>(yt, i) = ptr_ran[i];
					else
					{
						if (mark.at<uint8_t>(yt, i) == 0)
							dst_ran.at<uint16_t>(yt, i) = (dst_ran.at<uint16_t>(yt, i) + ptr_ran[i]) / 2;
					}
				}

				if (processSrc)
				{
					if (dst_src.at<uint8_t>(yt, i) == 0)
						dst_src.at<uint8_t>(yt, i) = ptr_src[i];
					else
					{
						if (mark.at<uint8_t>(yt, i) == 0)
							dst_src.at<uint8_t>(yt, i) = (dst_src.at<uint8_t>(yt, i) + ptr_src[i]) / 2;
					}
				}
			}

		}
	}
}


// you need to transpose input Mat
void adj_ordinary_setup_t(
	const double& scale, const double& offset,
	const cv::Mat& i_ran, const cv::Mat& i_src, cv::Mat& dst_ran, cv::Mat& dst_src,
	bool processRange = false, bool processSrc = false)
{
	//
	if (processRange)
		dst_ran = cv::Mat::zeros(i_ran.size(), i_ran.type());

	if (processSrc)
		dst_src = cv::Mat::zeros(i_src.size(), i_src.type());

	if (!processRange && !processSrc)
	{
		std::cerr << "adj_ordinary_setup_t: Logic error!" << std::endl;
		return;
	}

	cv::Mat mark = cv::Mat::zeros(i_ran.size(), CV_8UC1);

	//
	for (int j = 0; j < i_ran.rows; ++j)
	{
		auto ptr_ran = i_ran.ptr<uint16_t>(j);
		auto ptr_src = i_src.ptr<uint8_t>(j);
		
		auto ptr_mark = mark.ptr<uint8_t>(j);
		uint16_t* ptr_dst_ran = nullptr;
		uint8_t*  ptr_dst_src = nullptr;
		if (processRange)
			ptr_dst_ran = dst_ran.ptr<uint16_t>(j);

		if (processSrc)
			ptr_dst_src = dst_src.ptr<uint8_t>(j);
	
		for (int i = 0; i < i_ran.cols; ++i)
		{
			if (ptr_ran[i] == 0)
				continue;

			// Image is transposed, Y is i
			auto deltY = ptr_ran[i] * scale + offset;
			int y = i - cvRound(deltY);

			if (y < 0 || y >= i_ran.cols)
				continue;

			// add to mark
			//mark.at<uint8_t>(y, i) = 255; // 255 点不需要插值
			ptr_mark[y] = 255;

			// fill it
			if (processRange)
				ptr_dst_ran[y] = ptr_ran[i];

			if (processSrc)
				ptr_dst_src[y] = ptr_src[i];

			// fill adjcent pts
			int yt = y - 1;
			if (yt < 0 || yt >= i_ran.cols)
				continue;
			else
			{
				if (processRange)
				{
					if (ptr_dst_ran[yt] == 0)
						ptr_dst_ran[yt] = ptr_ran[i];
					else
					{
						if (ptr_mark[yt] == 0)
							ptr_dst_ran[yt] = (ptr_dst_ran[yt] + ptr_ran[i]) / 2;
					}
				}

				if (processSrc)
				{
					if (ptr_dst_src[yt] == 0)
						ptr_dst_src[yt] = ptr_src[i];
					else
					{
						if (ptr_mark[yt] == 0)
							ptr_dst_src[yt] = (ptr_dst_src[yt] + ptr_src[i]) / 2;
					}
				}
			}

		}
	}
}

// affine(rgbw), adj(all), crop(all),

// i_ran 和 i_ref 的宽度需要是 5120 
void process_rawData_adjustOrdinary_align(
	const colorParam& cop,
	const cv::Mat& i_ran,
	const cv::Mat& i_ref,
	const cv::Mat& i_scatter,
	const cv::Mat& i_r,
	const cv::Mat& i_g,
	const cv::Mat& i_b,
	const cv::Mat& i_w,
	cv::Mat& dst_ran,
	cv::Mat& dst_ref,
	cv::Mat& dst_scatter,
	cv::Mat& dst_red,
	cv::Mat& dst_green,
	cv::Mat& dst_blue,
	cv::Mat& dst_color,
	cv::Mat& dst_white,
	bool is_debug,
	const std::string& debug_img_save_to
)
{
	////
	//cv::resize(i_ran, i_ran, cv::Size(), cop.ResizeFactor, 1.0);
	//cv::resize(i_ref, i_ref, cv::Size(), cop.ResizeFactor, 1.0);


	if (i_ran.empty())
	{
		std::cerr << "Range must provide!" << std::endl;
		return;
	}
	// Crop parameters
	int top1, top2, end1, end2;
	top1 = 0;
	top2 = cop.ColorTopBlackRows;
	end1 = i_ran.rows - cop.ColorEndBlackRows;
	end2 = i_ran.rows;


	// Range and reflectance
	
	adj_ordinary_setup(cop.ReflectanceOrdinaryScale, cop.ReflectanceOrdinaryOffset, i_ran, i_ref, dst_ran, dst_ref, true, !i_ref.empty());
	if (is_debug)
		cv::imwrite(debug_img_save_to + "/adj_range.tiff", dst_ran);

	/// Crop range
	dst_ran.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
	dst_ran.rowRange(end1, end2) = cv::Scalar(0, 0, 0);
	if (is_debug)
		cv::imwrite(debug_img_save_to + "/adj_range_crop.tiff", dst_ran);

	// Crop Reflectance
	if (!i_ref.empty())
	{
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_reflectance.tiff", dst_ref);

		dst_ref.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_ref.rowRange(end1, end2) = cv::Scalar(0, 0, 0);
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_reflectance_crop.tiff", dst_ref);
	}

	// Scatter
	if (!i_scatter.empty())
	{
		// adj_ordinary_setup
		cv::Mat tmp;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_scatter, tmp, dst_scatter, true, !i_scatter.empty());
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_scatter.tiff", dst_scatter);

		// Crop
		dst_scatter.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_scatter.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		if (is_debug)
			cv::imwrite(debug_img_save_to + "/adj_scatter_crop.tiff", dst_white);
	}

	// RGB
	if (!i_r.empty() && !i_g.empty() && !i_b.empty())
	{
		// Gain
		cv::Mat i_r2;
		cv::Mat i_g2;
		cv::Mat i_b2;
		i_r2 = i_r * cop.RedGain;
		i_g2 = i_g * cop.GreenGain;
		i_b2 = i_b * cop.BlueGain;
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_r2_gain.tiff", i_r2);
			cv::imwrite(debug_img_save_to + "/adj_g2_gain.tiff", i_g2);
			cv::imwrite(debug_img_save_to + "/adj_b2_gain.tiff", i_b2);
		}

		// Affine
		cv::Mat i_r3;
		cv::Mat i_g3;
		cv::Mat i_b3;
		cv::warpAffine(i_r2, i_r3, cop.mat_r, i_r.size());
		cv::warpAffine(i_g2, i_g3, cop.mat_g, i_g.size());
		cv::warpAffine(i_b2, i_b3, cop.mat_b, i_b.size());
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_r3_warpAffine.tiff", i_r3);
			cv::imwrite(debug_img_save_to + "/adj_g3_warpAffine.tiff", i_g3);
			cv::imwrite(debug_img_save_to + "/adj_b3_warpAffine.tiff", i_b3);
		}

		// adj_ordinary_setup
		cv::Mat tmp;

		cv::Mat tmp_range_red;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_r3, tmp_range_red, dst_red, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_red, i_r3, tmp, dst_red, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_red.tiff", tmp_range_red);
		}

		cv::Mat tmp_range_green;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_g3, tmp_range_green, dst_green, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_green, i_g3, tmp, dst_green, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_green.tiff", tmp_range_green);
		}

		cv::Mat tmp_range_blue;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_b3, tmp_range_blue, dst_blue, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_blue, i_b3, tmp, dst_blue, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_blue.tiff", tmp_range_blue);
		}


		// Merge color
		cv::merge(std::vector<cv::Mat>{dst_blue, dst_green, dst_red}, dst_color);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_color.tiff", dst_color);
			cv::imwrite(debug_img_save_to + "/adj_r.tiff", dst_red);
			cv::imwrite(debug_img_save_to + "/adj_g.tiff", dst_green);
			cv::imwrite(debug_img_save_to + "/adj_b.tiff", dst_blue);
		}

		// Crop 
		dst_color.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_color.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		dst_red.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_red.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		dst_green.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_green.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		dst_blue.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_blue.rowRange(end1, end2) = cv::Scalar(0, 0, 0);
	
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_color_crop.tiff", dst_color);
			cv::imwrite(debug_img_save_to + "/adj_r_crop.tiff", dst_red);
			cv::imwrite(debug_img_save_to + "/adj_g_crop.tiff", dst_green);
			cv::imwrite(debug_img_save_to + "/adj_b_crop.tiff", dst_blue);
		}

	}

	// White
	if (!i_w.empty())
	{
		// Affine
		cv::Mat i_w3;
		cv::warpAffine(i_w, i_w3, cop.mat_w, i_w.size());

		// adj_ordinary_setup
		cv::Mat tmp_range_white, tmp;
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_w3, tmp_range_white, dst_white, true, true);
		adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_white, i_w3, tmp, dst_white, false, true);
		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/tmp_range_white.tiff", tmp_range_white);
		}

		// Crop
		dst_white.rowRange(top1, top2) = cv::Scalar(0, 0, 0);
		dst_white.rowRange(end1, end2) = cv::Scalar(0, 0, 0);

		if (is_debug)
		{
			cv::imwrite(debug_img_save_to + "/adj_white.tiff", dst_white);
		}

	}


}





/**
*	@copyright	Copyright 2016-2024 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*   @version    1.0.0
*/

#pragma once
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include <iostream>
#include <map>
#ifdef _WIN32
#include <Windows.h>
#endif

#include <opencv2/opencv.hpp>

namespace color_image_run {

	struct colorParam
	{
		cv::Mat mat_r;
		cv::Mat mat_g;
		cv::Mat mat_b;
		cv::Mat mat_w;
		int ResizeFactor = 1.0;

		double RedGain = 1.0;
		double GreenGain = 1.0;
		double BlueGain = 1.0;

		double ReflectanceOrdinaryScale = 1.0;
		double ReflectanceOrdinaryOffset = 0.0;

		double RedOrdinaryScale = 1.0;
		double RedOrdinaryOffset = 0.0;


		// refine
		cv::Mat mat_r_refine;
		cv::Mat mat_g_refine;
		cv::Mat mat_b_refine;
		cv::Mat mat_w_refine;

	};


	inline colorParam loadColorParam(const std::string& xmlpath)
	{
		color_image_run::colorParam cop;

		//
		cv::FileStorage fs(xmlpath, cv::FileStorage::READ);
		if (!fs.isOpened()) {
			std::cerr << "Error: Could not open XML file. Path=" << xmlpath << std::endl;
			return color_image_run::colorParam();
		}


		fs["mat_r"] >> cop.mat_r;
		fs["mat_g"] >> cop.mat_g;
		fs["mat_b"] >> cop.mat_b;
		fs["mat_w"] >> cop.mat_w;
		fs["ResizeFactor"] >> cop.ResizeFactor;
		fs["RedGain"] >> cop.RedGain;
		fs["GreenGain"] >> cop.GreenGain;
		fs["BlueGain"] >> cop.BlueGain;
		fs["ReflectanceOrdinaryScale"] >> cop.ReflectanceOrdinaryScale;
		fs["ReflectanceOrdinaryOffset"] >> cop.ReflectanceOrdinaryOffset;
		fs["RedOrdinaryScale"] >> cop.RedOrdinaryScale;
		fs["RedOrdinaryOffset"] >> cop.RedOrdinaryOffset;


		fs["mat_r_refine"] >> cop.mat_r_refine;
		fs["mat_g_refine"] >> cop.mat_g_refine;
		fs["mat_b_refine"] >> cop.mat_b_refine;
		fs["mat_w_refine"] >> cop.mat_w_refine;


		fs.release();

		return cop;
	}


	inline void adj_ordinary_setup(
		const double& scale, const double& offset,
		const cv::Mat& i_ran, const cv::Mat& i_src, cv::Mat& dst_ran, cv::Mat& dst_src,
		bool processRange = false, bool processSrc = false)
	{
		if (scale == 0.0 && offset == 0.0)
		{
			dst_ran = i_ran;
			dst_src = i_src;
			return;
		}

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
	inline void adj_ordinary_setup_t(
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
			uint8_t* ptr_dst_src = nullptr;
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

	// i_ran 和 i_ref 的宽度需要是 5120 
	inline void process_rawData_adjustOrdinary_align(
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
		const std::string& debug_img_save_to,
		const std::string& debug_img_save_name = "",
		bool to_adj_ordinary_setup = true
	)
	{
		if (i_ran.empty())
		{
			std::cerr << "Range must provide!" << std::endl;
			return;
		}

		// Range and reflectance
		adj_ordinary_setup(cop.ReflectanceOrdinaryScale, cop.ReflectanceOrdinaryOffset, i_ran, i_ref, dst_ran, dst_ref, true, !i_ref.empty());
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_range.tiff", dst_ran);

		// Crop Reflectance
		if (!i_ref.empty())
		{
			if (is_debug)
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_reflectance.tiff", dst_ref);

		}

		// Scatter
		if (!i_scatter.empty())
		{
			// adj_ordinary_setup
			cv::Mat tmp;
			adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_scatter, tmp, dst_scatter, true, !i_scatter.empty());
			if (is_debug)
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_scatter.tiff", dst_scatter);
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
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_r2_gain.tiff", i_r2);
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_g2_gain.tiff", i_g2);
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_b2_gain.tiff", i_b2);
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
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_r3_warpAffine.tiff", i_r3);
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_g3_warpAffine.tiff", i_g3);
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_b3_warpAffine.tiff", i_b3);
			}

			// adj_ordinary_setup

			if (to_adj_ordinary_setup)
			{
				cv::Mat tmp;

				cv::Mat tmp_range_red;
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_r3, tmp_range_red, dst_red, true, true);
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_red, i_r3, tmp, dst_red, false, true);


				cv::Mat tmp_range_green;
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_g3, tmp_range_green, dst_green, true, true);
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_green, i_g3, tmp, dst_green, false, true);


				cv::Mat tmp_range_blue;
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_b3, tmp_range_blue, dst_blue, true, true);
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_blue, i_b3, tmp, dst_blue, false, true);

			}
			else
			{
				dst_blue = i_b3;
				dst_green = i_g3;
				dst_red = i_r3;
			}



			// Merge color
			cv::merge(std::vector<cv::Mat>{dst_blue, dst_green, dst_red}, dst_color);
			if (is_debug)
			{
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_color.tiff", dst_color);
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_r.tiff", dst_red);
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_g.tiff", dst_green);
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_b.tiff", dst_blue);
			}

			// refine 
			if (!cop.mat_r_refine.empty() && !cop.mat_g_refine.empty() && !cop.mat_b_refine.empty())
			{
				cv::warpAffine(dst_red, dst_red, cop.mat_r_refine, dst_red.size());
				cv::warpAffine(dst_green, dst_green, cop.mat_g_refine, dst_green.size());
				cv::warpAffine(dst_blue, dst_blue, cop.mat_b_refine, dst_blue.size());
				cv::merge(std::vector<cv::Mat>{dst_blue, dst_green, dst_red}, dst_color);

				if (is_debug)
				{
					cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_refined_r.tiff", dst_red);
					cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_refined_g.tiff", dst_green);
					cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_refined_b.tiff", dst_blue);
					cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_refined_c.tiff", dst_color);
				}
			}
		}

		// White
		if (!i_w.empty())
		{
			// Affine
			cv::Mat i_w3;
			cv::warpAffine(i_w, i_w3, cop.mat_w, i_w.size());

			// adj_ordinary_setup
			if (to_adj_ordinary_setup)
			{
				cv::Mat tmp_range_white, tmp;
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, i_ran, i_w3, tmp_range_white, dst_white, true, true);
				adj_ordinary_setup(cop.RedOrdinaryScale, cop.RedOrdinaryOffset, tmp_range_white, i_w3, tmp, dst_white, false, true);
			}
			else
			{
				dst_white = i_w3;
			}

			if (is_debug)
			{
				cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_white.tiff", dst_white);
			}


			// Refine
			if (!cop.mat_w_refine.empty())
			{
				cv::warpAffine(dst_white, dst_white, cop.mat_w_refine, dst_white.size());
				if (is_debug)
				{
					cv::imwrite(debug_img_save_to + "/" + debug_img_save_name + "_adj_refined_white.tiff", dst_white);
				}
			}

		}
	}


	inline void process_rawData_warp_perspective(
		const colorParam& cop,
		const cv::Mat& i_r,
		const cv::Mat& i_g,
		const cv::Mat& i_b,
		const cv::Mat& i_w,
		const cv::Mat& matrix_r,
		const cv::Mat& matrix_g,
		const cv::Mat& matrix_b,
		const cv::Mat& matrix_w,
		cv::Mat& dst_red,
		cv::Mat& dst_green,
		cv::Mat& dst_blue,
		cv::Mat& dst_color,
		cv::Mat& dst_white,
		bool is_debug,
		const std::string& debug_img_save_to,
		const std::string& debug_img_save_name = "",
		bool to_adj_ordinary_setup = true
	)
	{
		cv::Mat range = m_pEzR->ImageBank2cvMatShallowCopy(m_Image[0]);
		cv::Mat refle = m_pEzR->ImageBank2cvMatShallowCopy(m_Image[0], false);

		cv::Mat perspectiveMatrix = m_pEzR->vector2cvMat<double>(m_PerspectiveTransform, 3, 3);
		perspectiveMatrix.convertTo(perspectiveMatrix, CV_32FC1);

		// 创建目标图像
		cv::Mat dstRange = range;
		//cv::warpPerspective(range, dstRange, perspectiveMatrix, range.size());


		cv::Mat dstReflec;
		if (!refle.empty())
		{
			cv::warpPerspective(refle, dstReflec, perspectiveMatrix, refle.size());

			m_ImageOut = m_pEzR->cvMat2ImageBank(dstRange, dstReflec, m_Image[0].Info);
			return ErrorCode::ALL_OK;
		}
		else
		{
			m_ImageOut = m_pEzR->cvMat2ImageBank(dstRange, cv::Mat(), m_Image[0].Info);
			return ErrorCode::ALL_OK;
		}
	}

}//namespace, color_image_run




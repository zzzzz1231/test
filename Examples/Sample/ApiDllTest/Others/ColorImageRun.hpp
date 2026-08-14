/**
*	@copyright	Copyright 2016-2024 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*   @version    3.0.2
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
	//version 3
	//Time count <=65ms
//	inline cv::Mat FlatFieldingCorrection(cv::Mat& input_img, cv::Mat factorMatrix)
//	{
//		// 确保输入矩阵和校正因子矩阵的大小匹配
//		CV_Assert(input_img.cols == factorMatrix.cols); // 确保列数相同
//
//		cv::Mat correctedImage = cv::Mat::zeros(input_img.size(), CV_8U);
//
//		//factorMatrix.convertTo(factorMatrix, CV_8U);
//		// 遍历每列，进行逐元素乘法
//#pragma omp parallel for
//// 遍历每列，进行逐元素乘法
//		for (int col = 0; col < input_img.cols; ++col) {
//			for (int row = 0; row < input_img.rows; ++row) {
//				// 使用 at 方法进行逐元素操作
//				correctedImage.at<uchar>(row, col) = input_img.at<uchar>(row, col) * factorMatrix.at<uchar>(col);
//			}
//		}
//
//		//correctedImage.convertTo(correctedImage, CV_8U);
//		return correctedImage;
//	}


//	//v2:指针耗时 <=57ms
//	inline cv::Mat FlatFieldingCorrection(cv::Mat& input_img, cv::Mat factorMatrix)
//	{
//		// 确保输入矩阵和校正因子矩阵的大小匹配
//		CV_Assert(input_img.cols == factorMatrix.cols); // 确保列数相同
//
//		cv::Mat correctedImage = cv::Mat::zeros(input_img.size(), CV_8U);
//
//		//factorMatrix.convertTo(factorMatrix, CV_8U);
//	
//			// 获取 factorMatrix 的指针
//		uchar* factor_ptr = factorMatrix.ptr<uchar>(0);
//#pragma omp parallel for
//		for (int col = 0; col < input_img.cols; ++col) {
//			for (int row = 0; row < input_img.rows; ++row) {
//				// 使用指针方式进行逐元素操作
//				auto input_ptr = input_img.ptr<uchar>(row);
//				auto corrected_ptr = correctedImage.ptr<uchar>(row);
//		
//
//				corrected_ptr[col] = input_ptr[col] * factor_ptr[col];
//			}
//		}
//
//		//correctedImage.convertTo(correctedImage, CV_8U);
//		return correctedImage;
//	}
	//v3 <=25ms
	inline cv::Mat FlatFieldingCorrection(const cv::Mat& input_img, const cv::Mat factorMatrix)
	{
		// 确保输入矩阵和校正因子矩阵的大小匹配,确保类型一致
		CV_Assert(input_img.cols == factorMatrix.cols && input_img.type() == factorMatrix.type());
		// 
		// 进行逐元素矩阵乘法
		cv::Mat correctedImage = input_img.mul(factorMatrix);
		correctedImage.convertTo(correctedImage, CV_8U);
		return correctedImage;
	}



	// version 3
	struct colorParam
	{

		// Perspective Transformation
		//cv::Mat matPer_r;
		//cv::Mat matPer_g;
		//cv::Mat matPer_b;
		//cv::Mat matPer_w;

		//// [No Need] 
		cv::Mat mat_r; // 2 rows, 3 cols
		cv::Mat mat_g;
		cv::Mat mat_b;
		cv::Mat mat_w;

		double RedGain = 1.0;
		double GreenGain = 1.0;
		double BlueGain = 1.0;

		double ReflectanceOrdinaryScale = 1.0;
		double ReflectanceOrdinaryOffset = 0.0;

		double RedOrdinaryScale = 1.0;
		double RedOrdinaryOffset = 0.0;


		// [No Need] refine
		cv::Mat mat_r_refine; // can be empty
		cv::Mat mat_g_refine; // can be empty
		cv::Mat mat_b_refine; // can be empty
		cv::Mat mat_w_refine; // can be empty


		// version 3 add
		int version = -1;
		cv::Mat FactorXRed;
		cv::Mat FactorYRed;
		cv::Mat FactorXGreen; // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项没用。
		cv::Mat FactorYGreen; // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项没用。
		cv::Mat FactorXBlue;  // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项没用。
		cv::Mat FactorYBlue;  // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项没用。
		cv::Mat FactorXWhite; // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项没用。
		cv::Mat FactorYWhite; // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项没用。
		int ShiftYG; // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项有用。
		int ShiftYB; // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项有用。
		int ShiftYW; // 如果 GBW 选择和Red对齐，使用Red的标定文件来标定，则此项有用。
		//version 3 add FlatCorrectionFactors
		cv::Mat RedCorrectionFactors; //平场矫正系数矩阵
		cv::Mat	GreenCorrectionFactors; //平场矫正系数矩阵
		cv::Mat	BlueCorrectionFactors; //平场矫正系数矩阵
		cv::Mat GrayCorrectionFactors; //平场矫正系数矩阵

	};

	// version 3
	inline colorParam loadColorParam(const std::string& xmlpath)
	{
		color_image_run::colorParam cop;

		cv::FileStorage fs(xmlpath, cv::FileStorage::READ);
		if (!fs.isOpened()) {
			std::cerr << "Error: Could not open XML file. Path=" << xmlpath << std::endl;
			return color_image_run::colorParam();
		}

		cv::FileNode node = fs["Version"];
		if (node.empty()) 
		{
			// version 1
			fs["mat_r"] >> cop.mat_r;
			fs["mat_g"] >> cop.mat_g;
			fs["mat_b"] >> cop.mat_b;
			fs["mat_w"] >> cop.mat_w;
			//fs["ResizeFactor"] >> cop.ResizeFactor;
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

		}
		else
		{
			// version 3, 15 holes
			node >> cop.version;
			fs["FactorXRed"  ] >> cop.FactorXRed;
			fs["FactorYRed"  ] >> cop.FactorYRed;

			cv::FileNode node_green = fs["FactorXGreen"];
			if (node_green.empty())
			{
				// 这是推荐的方案，GBW 选择和Red对齐，使用Red的标定文件来标定
				fs["ShiftYG"] >> cop.ShiftYG;
				fs["ShiftYB"] >> cop.ShiftYB;
				fs["ShiftYW"] >> cop.ShiftYW;

			}
			else
			{
				// 这是不推荐的方案，要对RGBW分别做标定，比较麻烦。
				fs["FactorXGreen"] >> cop.FactorXGreen;
				fs["FactorYGreen"] >> cop.FactorYGreen;
				fs["FactorXBlue"] >> cop.FactorXBlue;
				fs["FactorYBlue"] >> cop.FactorYBlue;
				fs["FactorXWhite"] >> cop.FactorXWhite;
				fs["FactorYWhite"] >> cop.FactorYWhite;
			}
			fs["RedCorrectionFactors"] >> cop.RedCorrectionFactors;
			fs["GreenCorrectionFactors"] >> cop.GreenCorrectionFactors;
			fs["BlueCorrectionFactors"] >> cop.BlueCorrectionFactors;
			fs["GrayCorrectionFactors"] >> cop.GrayCorrectionFactors;

		}


		fs.release();

		return cop;
	}


	// version 3
	inline cv::Mat process_estimate_range_component(
		const cv::Mat& i_ran,
		const float& xk1, const float& xk2, const float& xb1, const float& xb2,
		const float& yk1, const float& yk2, const float& yb1, const float& yb2,
		const bool& doInterpolation = true,
		const int& size = 3
	)
	{
		// EZR 将数据*0.0625，这里需要另外处理
#ifdef EZR_EXPORTS
		auto o_img = cv::Mat(i_ran.size(), CV_32FC1, cv::Scalar(EZR_NAN));
		for (int j = 0; j < i_ran.rows; ++j)
		{
			auto pf = i_ran.ptr<float>(j);


			for (int i = 0; i < i_ran.cols; ++i)
			{
				if (pf[i] == -1.0e6f)
					continue;

				auto v = pf[i];
				auto x = i * (v * xk1 + xb1) + (v * xk2 + xb2);
				auto y = j * (v * yk1 + yb1) + (v * yk2 + yb2);

				if (x < 0 || i_ran.cols <= x)
					continue;

				if (y < 0 || i_ran.rows <= y)
					continue;


				o_img.at<float>(y, x) = v;

			}
		}
#else
		auto o_img = cv::Mat(i_ran.size(), CV_16UC1, cv::Scalar(0));
		for (int j = 0; j < i_ran.rows; ++j)
		{
			auto pf = i_ran.ptr<uint16_t>(j);


			for (int i = 0; i < i_ran.cols; ++i)
			{
				if (pf[i] == -1.0e6f)
					continue;

				auto v = pf[i];
				auto x = i * (v * xk1 + xb1) + (v * xk2 + xb2);
				auto y = j * (v * yk1 + yb1) + (v * yk2 + yb2);

				if (x < 0 || i_ran.cols <= x)
					continue;

				if (y < 0 || i_ran.rows <= y)
					continue;


				o_img.at<uint16_t>(y, x) = v;

			}
		}
#endif




		// interpolation
		if (doInterpolation) // close_morph3
		{
			// 创建结构元素
			cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(size, size));

			// 进行闭运算
			//cv::Mat closedImage;
			cv::morphologyEx(o_img, o_img, cv::MORPH_CLOSE, kernel);
		}

		return o_img;
	}

	inline cv::Mat process_estimate_range_component(
		const cv::Mat& i_ran,
		const cv::Mat& FactorX,
		const cv::Mat& FactorY,
		const bool& doInterpolation = true,
		const int& size = 3
	)
	{
		float xk1, xk2, xb1, xb2;
		xk1 = FactorX.at<double>(0, 0);
		xk2 = FactorX.at<double>(1, 0);
		xb1 = FactorX.at<double>(2, 0);
		xb2 = FactorX.at<double>(3, 0);

		float yk1, yk2, yb1, yb2;
		yk1 = FactorY.at<double>(0, 0);
		yk2 = FactorY.at<double>(1, 0);
		yb1 = FactorY.at<double>(2, 0);
		yb2 = FactorY.at<double>(3, 0);

		return process_estimate_range_component(i_ran,
			xk1, xk2, xb1, xb2,
			yk1, yk2, yb1, yb2, doInterpolation, size
		);

	}

	inline cv::Mat process_translate_move_image(const cv::Mat& image_uint8, int yShift)
	{
		// 获取图像的尺寸
		int rows = image_uint8.rows;
		int cols = image_uint8.cols;

		// 创建平移矩阵
		cv::Mat translationMatrix = cv::Mat::zeros(2, 3, CV_32F);
		translationMatrix.at<float>(0, 0) = 1.0;  // x方向的缩放因子
		translationMatrix.at<float>(1, 1) = 1.0;  // y方向的缩放因子
		translationMatrix.at<float>(0, 2) = 0.0;  // x方向的平移量
		translationMatrix.at<float>(1, 2) = -yShift;   // y方向的平移量（向上平移）

		// 应用平移变换
		cv::Mat result;
		cv::warpAffine(image_uint8, result, translationMatrix, cv::Size(cols, rows));
		return result;
	}


	// version 2
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

	// Version 1，  [No Need] i_ran 和 i_ref 的宽度需要是 5120 
	inline void process_rawData_adjustOrdinary_align(
		const colorParam& cop,
		const cv::Mat& i_ran_,
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
		const std::string& debug_img_save_name="",
		bool to_adj_ordinary_setup = true
	)
	{
		cv::Mat i_ran = i_ran_;
		if (i_ran.empty())
		{
			std::cerr << "i_ran_ is empty!" << std::endl;
			return;
		}

		if (i_ran.type() == CV_32FC1)
			i_ran.convertTo(i_ran, CV_16SC1);

		// Range and reflectance
		adj_ordinary_setup(cop.ReflectanceOrdinaryScale, cop.ReflectanceOrdinaryOffset, i_ran, i_ref, dst_ran, dst_ref, true, !i_ref.empty());
		if (is_debug)
			cv::imwrite(debug_img_save_to + "/"+ debug_img_save_name+"_adj_range.tiff", dst_ran);

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
				dst_green= i_g3;
				dst_red  = i_r3;
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


	// version 2
	inline void run_ordinary_setup_fix(
		const double& scale, const double& offset,
		const cv::Mat& inRange, const cv::Mat& inImg2D, cv::Mat& outRange, cv::Mat& outImg2D,
		bool processRange = false, bool processSrc = false)
	{
		if (scale == 0.0 && offset == 0.0)
		{
			outRange = inRange;
			outImg2D = inImg2D;
			return;
		}

		//
		if (processRange)
			outRange = cv::Mat::zeros(inRange.size(), inRange.type());

		if (processSrc)
			outImg2D = cv::Mat::zeros(inImg2D.size(), inImg2D.type());

		if (!processRange && !processSrc)
		{
			std::cerr << "adj_ordinary_setup: Logic error!" << std::endl;
			return;
		}

		cv::Mat mark = cv::Mat::zeros(inRange.size(), CV_8UC1);

		//
		for (int j = 0; j < inRange.rows; ++j)
		{
			auto ptr_ran = inRange.ptr<uint16_t>(j);
			auto ptr_src = inImg2D.ptr<uint8_t>(j);

			for (int i = 0; i < inRange.cols; ++i)
			{
				if (ptr_ran[i] == 0)
					continue;


				auto deltY = ptr_ran[i] * scale + offset;
				int y = j - cvRound(deltY);

				if (y < 0 || y >= inRange.rows)
					continue;

				// add to mark
				mark.at<uint8_t>(y, i) = 255; // 255 点不需要插值

				// fill it
				if (processRange)
					outRange.at<uint16_t>(y, i) = ptr_ran[i];

				if (processSrc)
					outImg2D.at<uint8_t>(y, i) = ptr_src[i];

				// fill adjcent pts
				int yt = y - 1;
				if (yt < 0 || yt >= inRange.rows)
					continue;
				else
				{
					if (processRange)
					{
						if (outRange.at<uint16_t>(yt, i) == 0)
							outRange.at<uint16_t>(yt, i) = ptr_ran[i];
						else
						{
							if (mark.at<uint8_t>(yt, i) == 0)
								outRange.at<uint16_t>(yt, i) = (outRange.at<uint16_t>(yt, i) + ptr_ran[i]) / 2;
						}
					}

					if (processSrc)
					{
						if (outImg2D.at<uint8_t>(yt, i) == 0)
							outImg2D.at<uint8_t>(yt, i) = ptr_src[i];
						else
						{
							if (mark.at<uint8_t>(yt, i) == 0)
								outImg2D.at<uint8_t>(yt, i) = (outImg2D.at<uint8_t>(yt, i) + ptr_src[i]) / 2;
						}
					}
				}

			}
		}
	}


	// version 2
	inline void run_perspective(const cv::Mat& inImg, cv::Mat& outImg, const cv::Mat& perspective_matrix_2rows_3cols_float32)
	{
		cv::warpPerspective(inImg, outImg, perspective_matrix_2rows_3cols_float32, inImg.size());
	}




}//namespace, color_image_run




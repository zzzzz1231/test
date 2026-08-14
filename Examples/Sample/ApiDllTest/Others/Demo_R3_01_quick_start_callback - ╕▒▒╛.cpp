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
#include <../CalibrationWrapper/CalibrationWrapper.h>





bool SICK_GTEST_LIBRARY calibrate_color(SickCam::ImgT& imgOut, const SickCam::ImgT& img, const std::string& json)
{
	if (img.isEmpty())
	{
		std::cout << "img isEmpty!" << std::endl;

		return false;
	}

	// -- check Range
	uint8_t* pRange = nullptr;
	if (img.has(SickCam::DataNames::RAN))
		pRange = (uint8_t*)img.getData(SickCam::DataNames::RAN);

	// -- check Reflectance
	uint8_t* pReflectance = nullptr;
	if (img.has(SickCam::DataNames::REF))
		pReflectance = (uint8_t*)img.getData(SickCam::DataNames::REF);

	// -- check Scatter
	auto scatterSize = img.has(SickCam::DataNames::SCA_8) ? 8 : 0;

	scatterSize = img.has(SickCam::DataNames::SCA_16) ? 16 : scatterSize;

	uint8_t* pScatter = nullptr;
	if (scatterSize == 16)
		pScatter = (uint8_t*)img.getData(SickCam::DataNames::SCA_16);
	else if (scatterSize == 8)
		pScatter = (uint8_t*)img.getData(SickCam::DataNames::SCA_8);

	// --- Rectification Spread
	double rectificationSpread = 1.2; // 可以获得较少的 missing data

	// --- Init Calibration Wrapper
	SiCaliWrapper::CalibrationWrapper Cali;
	uint32_t rectificationWidth = 2560;
	if (false == Cali.initCalibrationFilter(
		json, 
		img.get_cols(),		
		img.get_rows(),
		img.getR_aoiWidth(),
		img.getR_aoiHeight(),
		img.getRange_info().xml_scale_x,
		img.getRange_info().xml_origin_x,
		img.getRange_info().xml_scale_z,
		img.getRange_info().xml_origin_z,
		static_cast<uint32_t>(scatterSize),
		SiCaliWrapper::RectMethod::Top,
		rectificationWidth,
		MISSING_DATA,
		true,
		false,
		rectificationSpread
	))
	{
		std::cout << "initCalibrationFilter failed!" << std::endl;
		return false;
	}

	// --- Calibrate
	auto pD = std::make_shared<SiCaliWrapper::calibratedData>(rectificationWidth, img.get_rows());
#ifndef __arm__
	auto numberOfThread = Cali.get_NumberOfThreads_jsonOnly_X64() / 2; // 将标定的线程数，设定为 CPU 核数的一半，为了不让 CPU 使用率达到 100% 。
	Cali.set_NumberOfThreads_jsonOnly_X64(numberOfThread);
#endif // !__arm__
	if (false == Cali.calibrate(pD, pRange, pReflectance, pScatter, true))
	{
		std::cout << "calibrate failed!" << std::endl;
		return false;
	}

	// --- Get Calibrated Data
	double scaleY(0.044); // 自定义
	imgOut.setCaliInfo(
		pD->Width,
		pD->Height,
		pD->offsetX, 0.0, 0.0,
		pD->scaleX, scaleY, 1.0,
		pD->lower_bound_x, pD->upper_bound_x,
		pD->lower_bound_r, pD->upper_bound_r,
		img.m_info.m_CI.genistreamtraits.a_axis_range_scale, img.m_info.m_CI.genistreamtraits.a_axis_range_offset,
		img.m_info.m_CI.genistreamtraits.c_axis_range_scale, img.m_info.m_CI.genistreamtraits.c_axis_range_offset
	);


	imgOut.insertDataCopy(SickCam::DataNames::RAN_CAL, (uint8_t*)pD->getData(SiCaliWrapper::RAN_C), 0);
	imgOut.insertDataCopy(SickCam::DataNames::REF_CAL, (uint8_t*)pD->getData(SiCaliWrapper::REF_C), 0);

	if (scatterSize == 16)
		imgOut.insertDataCopy(SickCam::DataNames::SCA_16_CAL, (uint8_t*)pD->getData(SiCaliWrapper::SCA_16), 0);
	else if (scatterSize == 8)
		imgOut.insertDataCopy(SickCam::DataNames::SCA_8_CAL, (uint8_t*)pD->getData(SiCaliWrapper::SCA_8), 0);

	if (img.has(SickCam::DataNames::MAR)) // 若有编码器信息，不参与标定，直接拷贝到输出图像中。
		imgOut.insertDataCopy(SickCam::DataNames::MAR, (uint8_t*)img.getData(SickCam::DataNames::MAR), 0);




	return true;
}





void SICK_CALLBACK
on_color_image(SickCam::ImgT* pImg, void* any)
{
	SickCam::Ranger3* pC = (SickCam::Ranger3*)any;
	//std::cout << pC->get_DeviceIP() << ": " << pImg->get_ID() << std::endl;
	std::cout << pC->get_DeviceIP() << ": " << pImg->collectInfo() << std::endl;
	std::stringstream ss;
	ss << "D://123/" << pImg->get_ID();


	cv::Mat ref = cv::Mat(pImg->get_rows(), pImg->get_cols(), CV_8UC1, pImg->getData(SickCam::DataNames::REF));
	cv::Mat ran = cv::Mat(pImg->get_rows(), pImg->get_cols(), CV_8UC1, pImg->getData(SickCam::DataNames::RAN));
	cv::Mat white = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::WHITE));
	cv::Mat R = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::R));
	cv::Mat G = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::G));
	cv::Mat B = cv::Mat(pImg->get_rows(), pImg->get_cols() * 2, CV_8UC1, pImg->getData(SickCam::DataNames::B));

	cv::Mat color;
	cv::merge(std::vector<cv::Mat>{B,G,R}, color);

	cv::imwrite(ss.str() + "_ref.tiff", ref);
	cv::imwrite(ss.str() + "_ran.tiff", ref);
	cv::imwrite(ss.str() + "_white.tiff", white);
	cv::imwrite(ss.str() + "_color.tiff", color);
	cv::imwrite(ss.str() + "_cr.tiff", R);
	cv::imwrite(ss.str() + "_cg.tiff", G);
	cv::imwrite(ss.str() + "_cb.tiff", B);



	//calibrate_color();



}

bool SICK_GTEST_LIBRARY Demo_R3_color_image()
{
	std::cout << "Demo Function Name: " << __func__ << std::endl;
	std::cout << SickCam::VER::__version__() << std::endl;


	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<SickCam::CameraShared>(
		"",											/* 日志文件 */
		SickCam::CameraShared::getPathToRanger3Producer(),	/* cti 文件 */
		true,										/* 是否打印信息到屏幕 */
		false										/* 是否打印信息到文件 */
		);
	pR3S->scanDevice();


	// --- Check connected device
	std::vector<std::string> ip(0);
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		//auto devicesSN = pR3S->getConDevListSN();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
		{
			ip.push_back(sub.first);
			std::cout << "---- " << sub.first << ", sn=" << sub.second->getSN() << std::endl;
		}
	}
	else
	{
		print_process_ng("Error: scan camera error. \n");
		return false;
	}

	//  --- Init Ranger3
	std::vector<std::shared_ptr<SickCam::Ranger3>> pCams(0);
	for (int i = 0; i < ip.size(); ++i)
	{
		pCams.push_back(std::make_shared<SickCam::Ranger3>(pR3S, ip[0], true));
	}


	// --- Connect Ranger3
	for (int i = 0; i < ip.size(); ++i)
	{
		auto err = pCams[i]->connectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng(ip[0] + " Connect failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			print_process_ng("The camera may be connected by other progress!");
			return false;
		}

		pCams[i]->setTimeOut(1);
		std::cout << "\n\n\nBasic Information of " << ip[i] << ":\n" << pCams[i]->getDeviceInfoAsStructureString() << "\n\n\n" << std::endl;
	}


	// --- Start Ranger3
	for (int i = 0; i < ip.size(); ++i)
	{
		auto err = pCams[i]->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image -> Areascan; 3D Range-> Linescan3D
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng(ip[0] + "Set parameter failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			return false;
		}
		//pCams[i]->setParameterValue("TriggerMode_TriggerSelector_LineStart", "Off");
		//pCams[i]->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off");
		//pCams[i]->setParameterValue("AcquisitionLineRate", "3000");
		//pCams[i]->setParameterValue("Height_RegionSelector_Scan3dExtraction1", "1000");

		

		//pCams[i]->setCalibration_inDevice(2560);
		pCams[i]->setCalibration_No();

		err = pCams[i]->startCamera(on_color_image, pCams[i].get()); // pass variable to callback function
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng(ip[0] + "Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			print_process_ng("The camera may be connected by other progress!");
			return false;
		}
	}


	// --- Hold on main thread
	getchar();
	//for (int i = 0; i < 10; ++i)
	//{
	//	__sleep1MS(1000); // 1 sec
	//}



	// --- Stop Ranger3
	for (int i = 0; i < ip.size(); ++i)
	{
		auto err = pCams[i]->stopCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng(ip[0] + "Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			print_process_ng("The camera may be connected by other progress!");
			return false;
		}
	}

	// --- Disconnect Ranger3
	for (int i = 0; i < ip.size(); ++i)
	{
		auto err = pCams[i]->disconnectCamera();
		if (SickCam::CAM_STATUS::All_OK != err)
		{
			print_process_ng(ip[0] + "Disconnect camera! ErrorCode: " + SickCam::CAM_STATUS_str(err));
			return false;
		}
	}


	print_process_ok("All done! ");
	return true;
}


//bool SICK_GTEST_LIBRARY Demo_calirate_color_image()
//{
//	SickCam::ImgT imageTable;
//	//imageTable.
//
//}

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


// TODO: white 图像有单独处理 ---



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



bool offline_color_process()
{
	cv::Mat r = cv::imread("D:\\123\\1_cr.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat g = cv::imread("D:\\123\\1_cg.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat b = cv::imread("D:\\123\\1_cb.tiff", cv::IMREAD_UNCHANGED);



	// Python 中，算法直接找的rgb偏移不准


	///////////////////////////////////// 手动查找

	cv::Mat back = cv::Mat::zeros(b.size(), b.type());
	
	
	//for (int gs = -15; gs < 15; ++gs)
	//{
	//	cv::Mat color = color_shift(r, g, back, gs, 0);

	//	std::stringstream ss;
	//	ss << "D:\\123\\findshift_rg\\1_colorAdj_g" << gs << ".tiff";
	//	cv::imwrite(ss.str(), color);

	//	std::cout << ss.str() << std::endl;

	//}


	//for (int bs = -20; bs < -14; ++bs)
	//{
	//	cv::Mat color = color_shift(r, back, b, 0, bs);

	//	std::stringstream ss;
	//	ss << "D:\\123\\findshift_rb\\1_colorAdj_b" << bs << ".tiff";
	//	cv::imwrite(ss.str(), color);

	//	std::cout << ss.str() << std::endl;

	//}


	int green_s = -9, blue_s = -18;
	cv::Mat color = color_shift(r, g, b, green_s, blue_s);
	std::stringstream ss;
	ss << "D:\\123\\1_colorAdj_g"<<green_s<<"_b" << blue_s << ".tiff";
	cv::imwrite(ss.str(), color);

	return true;
}


// 参数加载
// 不同的架设，对应着这里的不同的参数。需要初始化过程确定。
struct clolor_params
{
	
	// 成像原理决定了R G B三通道存在Y向的偏差。这里设置的是：彩色 R G B 三通道的沿着 Y 方向的偏移量
	int red_shift = 0;     // red 是基准，所以无需偏移。
	int green_shift = -9;  // green 图像在Y方向上，相对 red 图像偏移。一般负数表示向Y的下方偏移。
	int blue_shift  = -18; // green 图像在Y方向上，相对 red 图像偏移。

	// 经过 Y 向偏移调整后的图像，在头尾会出现重影。这里设置的是：在彩色图像的Y方向头尾，需要剪除的行的数量。参见文档，附录1-1
	int remove_rows_Y_begin = 18;
	int remove_rows_Y_end   = 0;





};


bool find_rgb_shift()
{
	cv::Mat r = cv::imread("D:\\123\\1_cr.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat g = cv::imread("D:\\123\\1_cg.tiff", cv::IMREAD_UNCHANGED);
	cv::Mat b = cv::imread("D:\\123\\1_cb.tiff", cv::IMREAD_UNCHANGED);



	// Python 中，算法直接找的rgb偏移不准


	///////////////////////////////////// 手动查找

	cv::Mat back = cv::Mat::zeros(b.size(), b.type());


	//for (int gs = -15; gs < 15; ++gs)
	//{
	//	cv::Mat color = color_shift(r, g, back, gs, 0);

	//	std::stringstream ss;
	//	ss << "D:\\123\\findshift_rg\\1_colorAdj_g" << gs << ".tiff";
	//	cv::imwrite(ss.str(), color);

	//	std::cout << ss.str() << std::endl;

	//}


	//for (int bs = -20; bs < -14; ++bs)
	//{
	//	cv::Mat color = color_shift(r, back, b, 0, bs);

	//	std::stringstream ss;
	//	ss << "D:\\123\\findshift_rb\\1_colorAdj_b" << bs << ".tiff";
	//	cv::imwrite(ss.str(), color);

	//	std::cout << ss.str() << std::endl;

	//}


	int green_s = -9, blue_s = -18;
	cv::Mat color = color_shift(r, g, b, green_s, blue_s);
	std::stringstream ss;
	ss << "D:\\123\\1_colorAdj_g" << green_s << "_b" << blue_s << ".tiff";
	cv::imwrite(ss.str(), color);

	return true;
}


void init()
{
	// 保存 RGB 图像




}



////////////////////////////////////////////////////////////////////////////////
#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	//Demo_R3_color_image();
	offline_color_process();
	init();

	return 0;
}
#endif


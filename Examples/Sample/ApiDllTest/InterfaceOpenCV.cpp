/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	3.0.0.2
*	@date		2021.02.01
*/

#include "InterfaceOpenCV.h"

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)


#ifdef ENABLE_OPENCV

const std::map<DN, cStr> InterfaceOpenCV::xmlNames = {
	{DN::SEN	 , "SEN_INFO"},
	{DN::RAN	 , "RAN_INFO"},
	{DN::RAN_CAL , "CAL_INFO"}
};

std::mutex	mouse_para::m_locker;

//====================================================================
// InterfaceOpenCV : constructor/destructor   

InterfaceOpenCV::InterfaceOpenCV()
{
}

InterfaceOpenCV::~InterfaceOpenCV()
{
}


//====================================================================
// InterfaceOpenCV : constructor/destructor   


cv::Mat InterfaceOpenCV::floatPtrToMat(float* pD, size_t rows, size_t cols)
{
	if (pD == nullptr)	return cv::Mat();

	cv::Mat dst = cv::Mat(rows, cols, CV_32FC1, pD).clone(); // deep copy
	//cv::imwrite("D://img.tiff", dst);
	//cv::Mat dst = cv::Mat(rows, cols, CV_32FC1, pD);		 // shallow copy
	return dst;
}

cv::Mat InterfaceOpenCV::uint8_tPtrToMat(uint8_t * pD, size_t rows, size_t cols)
{
	if (pD == nullptr)	return cv::Mat();

	cv::Mat dst = cv::Mat(rows, cols, CV_8UC1, pD).clone(); // deep copy
	return dst;
}

cv::Mat InterfaceOpenCV::uint16_tPtrToMat(uint16_t * pD, size_t rows, size_t cols)
{
	if (pD == nullptr)	return cv::Mat();

	cv::Mat dst = cv::Mat(rows, cols, CV_16SC1, pD).clone(); // deep copy
	return dst;
}


double InterfaceOpenCV::Xmm(const uint16_t vpixel, double offsetX, double scaleX)
{
	return (offsetX + scaleX*vpixel);
}

double InterfaceOpenCV::Ymm(const uint16_t vpixel, double offsetY, double scaleY)
{
	return (offsetY + scaleY*vpixel);
}

void InterfaceOpenCV::removeNAN(cv::Mat & img)
{
	float fmin(0);
	cv::min(fmin, img);
	fmin = fmin * 0.1;

	if (img.type() != CV_32FC1) return;

	for (size_t j = 0; j < img.rows; ++j)
	{
		float* data = img.ptr<float>(j);
		for (size_t i = 0; i < img.cols; ++i)
		{
			data[i] = isnan(data[i]) ? fmin : data[i];
		}
	}
}

cv::Mat InterfaceOpenCV::remapToUint8_t	(const cv::Mat & img)
{
    cv::Mat dst;
    img.convertTo(dst, CV_32FC1);
    cv::normalize(dst, dst, 0.0, 255.0, cv::NormTypes::NORM_MINMAX);
    dst.convertTo(dst, CV_8UC1);
	return dst;
}

cv::Mat InterfaceOpenCV::getMat(const ImgT & _img, const DN & _dn)
{
	cv::Mat src;
	if (_img.has(_dn))
	{
		switch (_dn)
		{
		case DN::INV:		break;
		case DN::SEN:		src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::RAN:		src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_16SC1,	(uint16_t	*)_img.getData(_dn));	break;
		case DN::REF:		src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA_8:		src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA_16:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_16SC1,	(uint16_t	*)_img.getData(_dn));	break;
		case DN::MAR:		src = cv::Mat(_img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1,	(uint32_t	*)_img.getData(_dn));	break;
		case DN::RAN_CAL:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_32FC1,	(float		*)_img.getData(_dn));	break;
		case DN::REF_CAL:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::REF_CAL_F:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_32FC1,	(float		*)_img.getData(_dn));	break;
		case DN::SCA_8_CAL:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA_16_CAL:src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_16SC1,	(uint16_t	*)_img.getData(_dn));	break;
		case DN::RAN2:		src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_16SC1,	(uint16_t	*)_img.getData(_dn));	break;
		case DN::REF2:		src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA2_8:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA2_16:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_16SC1,	(uint16_t	*)_img.getData(_dn));	break;
		case DN::RAN2_CAL:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_32FC1,	(float		*)_img.getData(_dn));	break;
		case DN::REF2_CAL:	src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA2_8_CAL:src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_8UC1,	(uint8_t	*)_img.getData(_dn));	break;
		case DN::SCA2_16_CAL:src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_16SC1,	(uint16_t	*)_img.getData(_dn));	break;
		case DN::RAN_X:		src = cv::Mat(_img.get_rows(), _img.get_cols(), CV_32FC1,	(float		*)_img.getData(_dn));	break;
		}
	}
	return src;
}

cv::Mat InterfaceOpenCV::drawPoints(const cv::Mat& _img, const std::vector<cv::Point2f>& _pts, const cv::Scalar& _color)
{
	cv::Mat src;
	if (_img.channels() == 1)
	{
		_img.convertTo(src, CV_8U);
		cv::merge(std::vector<cv::Mat>{ _img, _img, _img }, src);
	}
	else if (_img.channels() == 3)
		src = _img;
	else
	{
		std::cerr << "Error: _img.channels() must be 1 or 3." << std::endl;
		return src;
	}

	for (int i=0; i< _pts.size(); ++i)
	{
		cv::circle(src, _pts[i], 0, _color, -1);
	}
	return src;
}

cv::Mat InterfaceOpenCV::drawPoints(const cv::Rect& _rect, const int& subPixels, const cv::Mat& _img, const std::vector<cv::Point2f>& _pts, const cv::Scalar& _color, const int& _ptSize)
{
	cv::Mat src;
	if (_img.channels() == 1)
	{
		_img.convertTo(src, CV_8U);
		cv::merge(std::vector<cv::Mat>{ _img, _img, _img }, src);
	}
	else if (_img.channels() == 3)
		src = _img;
	else
	{
		std::cerr << "Error: _img.channels() must be 1 or 3." << std::endl;
		return src;
	}

	cv::Mat srcROI = src.rowRange(_rect.y, _rect.y+_rect.height).colRange(_rect.x, _rect.x+_rect.width).clone();
	cv::resize(srcROI, srcROI, cv::Size(), subPixels, subPixels, cv::InterpolationFlags::INTER_NEAREST);

	for (int i = 0; i < _pts.size(); ++i)
	{
		cv::circle(srcROI, cv::Point((_pts[i].x - _rect.x) * subPixels, (_pts[i].y - _rect.y) * subPixels), _ptSize, _color, -1);
	}
	return srcROI;
}

cv::Mat InterfaceOpenCV::drawLines(
	cv::Rect _rect, const int& subPixels, const cv::Mat& _img, 
	const std::vector<cv::Point2f>& _ptsStart, const std::vector<cv::Point2f>& _ptsEnd, 
	const cv::Scalar& _color, const int& _ptSize)
{
	cv::Mat src;
	if (_img.channels() == 1)
	{
		_img.convertTo(src, CV_8U);
		cv::merge(std::vector<cv::Mat>{ _img, _img, _img }, src);
	}
	else if (_img.channels() == 3)
		src = _img;
	else
	{
		std::cerr << "Error: _img.channels() must be 1 or 3." << std::endl;
		return src;
	}

	cv::Mat srcROI;
	if(_rect == cv::Rect())
	{
		srcROI = src.clone();
		_rect.width = 2560;
		_rect.height = 832;
	}
	else
	{
		srcROI = src.rowRange(_rect.y, _rect.y + _rect.height).colRange(_rect.x, _rect.x + _rect.width).clone();
	}
	cv::resize(srcROI, srcROI, cv::Size(), subPixels, subPixels, cv::InterpolationFlags::INTER_NEAREST);

	float step = 0.5; // todo: check it later ----
	float minY = 0, maxY = 0; // to make roi to output

	for (int i = 0; i < _ptsStart.size(); ++i)
	{
		if (_ptsStart[i].x < _rect.x)
			continue;

		if (_ptsStart[i].x >= _rect.x + _rect.width)
			continue;

		if (_ptsStart[i].y < _rect.y)
			continue;

		if (_ptsStart[i].y >= _rect.y + _rect.height)
			continue;

		float yStart = (_ptsStart[i].y - _rect.y + step) * subPixels;
		float yEnd = (_ptsEnd[i].y - _rect.y + step) * subPixels;
		cv::arrowedLine(srcROI,
			cv::Point((_ptsStart[i].x - _rect.x + step) * subPixels, yStart),
			cv::Point((_ptsEnd[i].x - _rect.x + step) * subPixels, yEnd),
			_color, 1);

		
		minY = minY > yStart ? yStart : minY;
		minY = minY > yEnd ? yEnd : minY;

		maxY = maxY < yStart ? yStart : maxY;
		maxY = maxY < yEnd ? yEnd : maxY;
	}

	minY = 0.8 * minY;
	maxY += 0.2 * (srcROI.rows - maxY);

	return srcROI.rowRange(minY, maxY);
}


bool InterfaceOpenCV::SaveImgT2OpenCVXML(const ImgT & _img, cStr & path) const
{
	// save data
	auto _names = _img.getAvalibleDataNames();
	cv::FileStorage f;
	f.open(path, cv::FileStorage::WRITE);
	if (f.isOpened())
	{
		for (auto sub : _names)
		{
			if (_img.has(sub))
			{
				cv::Mat src = getMat(_img, sub);
				f << ImgT::DN2Str(sub) << src;
			}
		}
	}
	else
		return false;

	// save info
	cv::Mat
		SEN_info((cv::Mat_<uint16_t>(1, 2) << _img.getS_OffsetX(), _img.getS_OffsetY())),
		CAL_info((cv::Mat_<double>(1, 8) <<
			_img.getC_offsetX(), _img.getC_offsetY(), _img.getC_scaleX(), _img.getC_scaleY(),
			_img.getC_lower_bound_x(), _img.getC_upper_bound_x(), _img.getC_lower_bound_r(), _img.getC_upper_bound_r())),
		RAN_info((cv::Mat_<uint16_t>(1, 5) <<
			_img.getR_aoiOffsetX(), _img.getR_aoiOffsetY(), _img.getR_aoiHeight(), _img.getR_aoiWidth(),
			(_img.getR_RangeAxis() == RA_STANDARD ? 1 : 0)));

	f << xmlNames.at(DN::SEN) << SEN_info;
	f << xmlNames.at(DN::RAN_CAL) << CAL_info;
	f << xmlNames.at(DN::RAN) << RAN_info;

	return true;
}

ImgT InterfaceOpenCV::LoadImgTFromOpenCVXML(cStr & path, const ImgNames & _names) const
{
	ImgT imgT;

	cv::FileStorage f;
	f.open(path, cv::FileStorage::READ);

	if (f.isOpened())
	{
		for (auto sub : _names)
		{
			cv::Mat src;
			f[ImgT::DN2Str(sub)] >> src;
			if (src.empty())	continue;

			imgT.insertDataCopy({ sub }, { src.data }, 0);
			if (DN::SEN == sub)
			{
				cv::Mat info;
				f[xmlNames.at(DN::SEN)] >> info;
				imgT.setSensorInfo(src.cols, src.rows, info.at<int16_t>(0), info.at<int16_t>(1));
			}

			if (DN::RAN_CAL == sub)
			{
				cv::Mat info;
				f[xmlNames.at(DN::RAN_CAL)] >> info;
				imgT.setCaliInfo(
					src.cols, src.rows,
					info.at<double>(0), info.at<double>(1), 0.0, info.at<double>(2), info.at<double>(3), 1.0,
					info.at<double>(4), info.at<double>(5), info.at<double>(6), info.at<double>(7),
					1.0,
					1.0,
					1.0,
					1.0
				);
			}

			if (DN::RAN == sub)
			{
				cv::Mat info;
				f[xmlNames.at(DN::RAN)] >> info;
				imgT.setRangeInfo(
					src.cols, src.rows,
					info.at<int16_t>(0), info.at<int16_t>(1), info.at<int16_t>(2), info.at<int16_t>(3),
					(info.at<int16_t>(4) == 1 ? RA_STANDARD : RA_REVERSED)
				);
			}

		}

	}

	return imgT;

}


//====================================================================
// on_mouse_ran : constructor/destructor   


void on_mouse_ran(int event, int x, int y, int flags, void *_ustc)
{
	mouse_para *ustc = static_cast<mouse_para*>(_ustc);

#ifdef __linux__ // opencv4.3.1 in this testing environment
    if (event == cv::EVENT_MOUSEMOVE && !ustc->org_ran.empty())
#else
	if (event == CV_EVENT_MOUSEMOVE && !ustc->org_ran.empty())
#endif
	{
		std::cout << (ustc->org_ran.empty() ? "empty" : "") << std::endl;
        std::stringstream ss;
		//char sss[100] = { 0 };

		if (ustc->cal_ran.empty())
		{
            ss << "(px = "<< x <<", py = "<<y<<") = (x = "<<x<<", y = "<<y<<", z = "
               << ustc->org_ran.at<uint16_t>(y, x)<<")";

			//sprintf_s(sss, 100, "(px = %d, py = %d) = (x = %d, y = %d, z = %d)",
			//	x, y, x, y,
			//	ustc->org_ran.at<uint16_t>(y, x));
		}
		else
		{
            ss << "(px = "<< x <<", py = "<<y<<") = (x = "
               << InterfaceOpenCV::Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()) << "mm, y = "
               << InterfaceOpenCV::Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()) << "mm, z = "
               << ustc->cal_ran.at<float>(y, x) << "mm)";

			//sprintf_s(sss, 100, "(px = %d, py = %d) = (x = %fmm, y = %fmm, z = %fmm)",
			//	x, y,
			//	InterfaceOpenCV::Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()),
			//	InterfaceOpenCV::Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()),
			//	ustc->cal_ran.at<float>(y, x));
		}

		std::cout << ss.str() << std::endl;
	}
}

bool isWithinRangeComponent(int &x, int &y, cv::Rect & rec)
{
	return (rec.x < x && x < rec.x + rec.width) && (rec.y < y && y < rec.y + rec.height);
}

void positionInRawImage(int &x, int &y, cv::Rect & rec, cv::Size siz, int &ox, int &oy)
{
	ox = cvRound(static_cast<double>(x - rec.x)*siz.width / rec.width);
	oy = cvRound(static_cast<double>(y - rec.y)*siz.height / rec.height);

	ox = ox < 0 ? 0 : ox;
	oy = oy < 0 ? 0 : oy;

	ox = ox < siz.width ? ox : siz.width - 1;
	oy = oy < siz.height ? oy : siz.height - 1;
}

bool is_in_Image(int x, int y, cv::Mat & src)
{
	if (src.empty())
		return false;

	return (x < 0) || (src.cols <= x) || (y < 0) || (src.rows <= y);
}

void on_mouse(int event, int x, int y, int flags, void *_ustc)
{
	mouse_para *ustc = static_cast<mouse_para*>(_ustc);
	std::unique_lock<std::mutex> lock(mouse_para::m_locker);

#ifdef __linux__ // opencv4.3.1 in this testing environment
    if (event == cv::EVENT_MOUSEMOVE && !ustc->org_ran.empty())
#else
    if (event == CV_EVENT_MOUSEMOVE && !ustc->org_ran.empty())
#endif
	{
		if (isWithinRangeComponent(x, y, ustc->rect_ran))
		{
			cv::Mat imgShow = ustc->pano.clone();
			cv::circle(imgShow, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x + 640 + 3, y), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x, y + 480 + 50), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x + 640 + 3, y + 480 + 50), 2, cv::Scalar(0, 0, 255), -1);

            std::stringstream ss;
			//char sss[100] = { 0 };
			positionInRawImage(x, y, ustc->rect_ran, ustc->org_ran.size(), x, y);
			if (ustc->cal_ran.empty())
			{
				if (!is_in_Image(x, y, ustc->cal_ran))
				{
					//sprintf_s(sss, 100, "(px=%d, py=%d) = (x=%d, y=%d, z=%d)",
					//	x, y, x, y,
					//	ustc->org_ran.at<uint16_t>(y, x));

                    ss << "(px = "<< x <<", py = "<<y<<") = (x = "<<x<<", y = "<<y<<", z = "
                       << ustc->org_ran.at<uint16_t>(y, x)<<")";
				}
			}
			else
			{
				//sprintf_s(sss, 100, "(px=%d, py=%d) = (x=%fmm, y=%fmm, z=%fmm)",
				//	x, y,
				//	InterfaceOpenCV::Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()),
				//	InterfaceOpenCV::Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()),
				//	ustc->cal_ran.at<float>(y, x));

                ss << "(px = "<< x <<", py = "<<y<<") = (x = "
                   << InterfaceOpenCV::Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()) << "mm, y = "
                   << InterfaceOpenCV::Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()) << "mm, z = "
                   << ustc->cal_ran.at<float>(y, x) << "mm)";
			}
			ustc->text[9] = ss.str();
			cv::putText(imgShow, ustc->text[9], cv::Point(ustc->rect_text.x, ustc->rect_text.y + 20 * 9), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 1);
			cv::imshow(ustc->winName_ran, imgShow);
		}
	}
}

#ifndef __linux__
void on_mouse_trispector(int event, int x, int y, int flags, void *_ustc)
{
	mouse_para *ustc = static_cast<mouse_para*>(_ustc);
	std::unique_lock<std::mutex> lock(mouse_para::m_locker);

	if (event == CV_EVENT_MOUSEMOVE && !ustc->cal_ran.empty())
	{
		if (isWithinRangeComponent(x, y, ustc->rect_ran))
		{
			cv::Mat imgShow = ustc->pano.clone();
			cv::circle(imgShow, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x + 640 + 3, y), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x, y + 480 + 50), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x + 640 + 3, y + 480 + 50), 2, cv::Scalar(0, 0, 255), -1);

			char sss[100] = { 0 };
			positionInRawImage(x, y, ustc->rect_ran, ustc->cal_ran.size(), x, y);
			
			sprintf_s(sss, 100, "(px=%d, py=%d) = (x=%fmm, y=%fmm, z=%fmm)",
				x, y,
				InterfaceOpenCV::Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()),
				InterfaceOpenCV::Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()),
				ustc->cal_ran.at<float>(y, x));

			ustc->text[9] = std::string(sss);
			cv::putText(imgShow, ustc->text[9], cv::Point(ustc->rect_text.x, ustc->rect_text.y + 20 * 9), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 1);
			cv::imshow(ustc->winName_ran, imgShow);
		}
	}
}
#endif

void drawComponect(std::string comName, cv::Rect _rec, cv::Mat & dst, cv::Size imgSize = cv::Size(640, 480),
	int labelHeight = 50, int lineSize = -1, double frontScale = 0.7, int frontLineSize = 2)
{
	cv::Rect rec = _rec;
	cv::rectangle(dst, rec, cv::Scalar(0, 255, 0), lineSize);
	cv::putText(dst, comName, cv::Point(rec.x + imgSize.width / 2, rec.y - labelHeight / 2),
		cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, frontScale, cv::Scalar(0, 0, 255), frontLineSize);
}

void drawSubImg(cv::Mat & src, cv::Rect & _rec, cv::Mat & dst)
{
	if (src.empty())
		return;

	bool isHeigher = (double(src.rows) / src.cols) > 0.75; // 480/640 = 0.75
	cv::Size showSize(640, 480);
	if (isHeigher)
	{
		showSize.width = cvRound(src.cols*(480.0 / src.rows));
	}
	else
	{
		showSize.height = cvRound(src.rows*(640.0 / src.cols));
	}

	cv::Mat ori(showSize, src.type());
	cv::resize(src, ori, showSize);

	if (ori.type() != CV_8UC1)
	{
		InterfaceOpenCV::removeNAN(ori);
		cv::normalize(ori, ori, 0.0, 255.0, cv::NORM_MINMAX);
		ori.convertTo(ori, CV_8UC1);
	}


	cv::Mat src3;
	std::vector<cv::Mat> vimg;
	vimg.push_back(ori);
	vimg.push_back(ori);
	vimg.push_back(ori);
	cv::merge(vimg, src3);

	_rec.width = src3.cols;
	_rec.height = src3.rows;
	src3.copyTo(dst(_rec));
}

void Show(SPtr<mouse_para> mp, cv::Size imgSize, int labelHeight, int lineSize, double frontScale, int frontLineSize)
{
	Show(mp.get(), imgSize, labelHeight, lineSize, frontScale, frontLineSize);
}

void Show(mouse_para * mp, cv::Size imgSize, int labelHeight, int lineSize, double frontScale, int frontLineSize)
{
	cv::Mat dst = cv::Mat::ones(
		(imgSize.height + labelHeight) * 2 + lineSize * 1,
		(imgSize.width) * 3 + lineSize * 2,
		CV_8UC3) * 255;

	cv::Rect rec_sen(cv::Point(0, labelHeight), imgSize);
	drawComponect("SEN", rec_sen, dst);
	{
		// draw AOI
		cv::Mat mask = cv::Mat::ones(mp->org_sen.size(), mp->org_sen.type()) * 126;
		cv::rectangle(
			mask,
			cv::Rect(mp->pImgD->getR_aoiOffsetX(), mp->pImgD->getR_aoiOffsetY(), mp->pImgD->getR_aoiWidth(), mp->pImgD->getR_aoiHeight()),
			cv::Scalar(0), -1);

		mp->org_sen = mp->org_sen + mask;
	}
	drawSubImg(mp->org_sen, rec_sen, dst);

	cv::Rect rec_txt(cv::Point(0, rec_sen.y + imgSize.height + labelHeight), imgSize);
	for (size_t i = 0; i < mp->text.size(); ++i)
	{
		if (i == 9) continue;
		cv::putText(dst, mp->text[i], cv::Point(rec_txt.x, rec_txt.y + 20 * i), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 1);
	}

	cv::Rect rec_ran(cv::Point(rec_sen.x + imgSize.width + lineSize, rec_sen.y), imgSize);
	drawComponect("RAN", rec_ran, dst);
	drawSubImg(mp->org_ran, rec_ran, dst);

	cv::Rect rec_ref(cv::Point(rec_ran.x + imgSize.width + lineSize, rec_ran.y), imgSize);
	drawComponect("REF", rec_ref, dst);
	drawSubImg(mp->org_int, rec_ref, dst);

	cv::Rect re_ra_c(cv::Point(rec_sen.x + imgSize.width + lineSize, rec_sen.y + imgSize.height + labelHeight), imgSize);
	drawComponect("RAN_CAL", re_ra_c, dst);
	drawSubImg(mp->cal_ran, re_ra_c, dst);

	cv::Rect re_re_c(cv::Point(re_ra_c.x + imgSize.width + lineSize, re_ra_c.y), imgSize);
	drawComponect("REF_CAL", re_re_c, dst);
	drawSubImg(mp->cal_int, re_re_c, dst);


	cv::imshow(mp->winName_ran, dst);

	if (!mp->cal_sca8.empty())	cv::imshow(mp->winName_ran + "Scatter 8", mp->cal_sca8);
	if (!mp->cal_sca16.empty()) cv::imshow(mp->winName_ran + "Scatter 16", mp->cal_sca16);

	mp->pano = dst;
	mp->rect_ran = rec_ran;
	mp->rect_ref = rec_ref;
	mp->rect_ran_cal = re_ra_c;
	mp->rect_ref_cal = re_re_c;
	mp->rect_text = rec_txt;
}


//====================================================================



std::vector<uint64_t> getTimestamp_fromMat(const cv::Mat& mark)
{
	std::vector<uint64_t> res(mark.rows);

	for (int i = 0; i < res.size(); ++i)
	{
		// cv::Mat 不支持 uint32_t, 所以示例中，Mat 的创建过程，使用了宏 CV_32SC1，这是 int32_t
		// 为了保留符号位，所以先转化成 uint64_t，在转化成 uint32_t
		auto low_uInt64 = mark.at<int32_t>(i, 2) & 0x00000000ffffffff;
		auto low_uInt32 = static_cast<uint32_t>(low_uInt64); // 

		auto high_uInt64 = mark.at<int32_t>(i, 3) & 0x00000000ffffffff;
		auto high_uInt32 = static_cast<uint32_t>(high_uInt64); // 

		res[i] = combine_uint32_uint32(low_uInt32, high_uInt32);
	}

	return res;
}

uint64_t combine_uint32_uint32(const uint32_t& Low, const uint32_t& High)
{
	return uint64_t(Low) + (uint64_t(High) << 32);
}


//====================================================================


int			DEMO::__g_userSetId = 0;
std::string DEMO::__g_cameraName_userSet = "";
bool		DEMO::__g_save_image = false;

void DEMO::__start_all(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	return __start_all_by_callback(is_started, mapCams);
}

void DEMO::__stop_all(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	return __stop_all_by_callback(is_started, mapCams);
}

void DEMO::__parameter_setting(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	__stop_all(is_started, mapCams);

	std::cout << "\n\n" << "======== Mode : Parameter Settings ========" << "\n\nL: I found " << mapCams.size() << " device in this computer, please select one: \n" << std::endl;
	size_t i = 0; std::vector<std::string> camNameList;
	for (auto subCam : mapCams)
	{
		std::cout << "---- " << i++ << " : " << subCam.first << std::endl;
		camNameList.emplace_back(subCam.first);
	}

	std::cout << "\n\n\nL: Please input your choice: \n" << std::endl;
	int which(0);	std::cin >> which;

	if (0 <= which && which < camNameList.size())
	{
		auto cam = mapCams.at(camNameList[which]).inner_pCam;
		std::cout << "\nL: Parameters of " << camNameList[which] << " are :\n" << cam->getParametersAsStructureString() << "\n";
		SickCam::AllParams allParams;
		cam->getAllParameterInfo(allParams);

		std::string str_para;
		while (str_para != "quit")
		{
			str_para = "";	std::cout << "\nL: Please input the parameter name, or input \"quit\" to quit.";
			std::cin >> str_para;

			if (allParams.count(str_para) != 1)
				std::cout << "\nL: Wrong parameter name! Please check the spelling. Press \"quit\" to quit";
			else
			{
				auto param = allParams[str_para]; std::cout << "\nL: Current value is : " << param->getStrValue();
				std::cout << "\nL: Optional values are : ";	for (auto opt : param->getOptions()) 	std::cout << opt << "  ";

				std::string str_value;
				while (str_value != "quit")
				{
					str_value = "";
					std::cout << "\nL: Please input the value: ";
					std::cin >> str_value;
					if (SickCam::CAM_STATUS::All_OK != cam->setParameterValue(str_para, str_value))
						std::cout << "\nL: Wrong parameter value! Please check the spelling. Press \"quit\" to quit setting [ " << str_para << " ]\n";
					else
						std::cout << "\nL: Set " << str_para << " as " << str_value << " OK!" << std::endl;	break;
				}
			}
		}
	}
	else
		std::cout << "\nL: Wrong input. Quit Parameter Settings" << std::endl;

	__start_all(is_started, mapCams);
}

#ifdef _WIN32
void DEMO::__userSet_testing(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	if (mapCams.count(__g_cameraName_userSet) != 1)
		return;

	__stop_all(is_started, mapCams);
	std::cout << "\n\n======== Mode : UserSet Testing ======== \n\n" << std::endl;

	++__g_userSetId; if (__g_userSetId > 5) __g_userSetId = 1;

	auto userSet = mapCams.at(__g_cameraName_userSet).inner_pCam->getUserSet(UserSetId(__g_userSetId));
	userSet.load();

	std::cout << "L: UserSet switch to " << userSet.getName() << std::endl;
	__start_all(is_started, mapCams);
}

void DEMO::__userSet_init(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	__stop_all(is_started, mapCams);
	std::cout << "\n\n======== Mode : UserSet Setting ======== \n\n" << std::endl;
	size_t i = 0; std::vector<std::string> camNameList;
	for (auto subCam : mapCams) {
		std::cout << "---- " << i++ << " : " << subCam.first << std::endl; camNameList.emplace_back(subCam.first);
	}	std::cout << "\n\nL: Please input your choice: \n" << std::endl;
	int which(0);	std::cin >> which;
	if (0 <= which && which < camNameList.size())
	{
		__g_cameraName_userSet = camNameList[which];
		auto cam = mapCams.at(camNameList[which]).inner_pCam;
		std::cout << "\n\nL: There are 5 userSets in Ranger3. You can load one of them to be current parameter settings." << std::endl;

		for (int i = 1; i < 6; ++i)
		{
			auto userSet = cam->getUserSet(UserSetId(i));
			if (!userSet.exists())
			{
				std::cout << "\n\nL: UserSet [" << i << " : " << userSet.getName() << "] Not Exist!\nTry save current parameters to it." << std::endl;
				userSet.save();
				std::cout << "\n\nL: Save current parameters to UserSet [" << i << " : " << userSet.getName() << "] done! Return " << (userSet.exists() ? "OK" : "NG") << std::endl;
			}
		}

		std::cout << "\n\nL: For testing, I will set ExposureTime_RegionSelector_Region0 = 100 of all 5 userSets in the beginning." << std::endl;
		for (int i = 1; i < 6; ++i)
		{
			cam->setParameterValue("ExposureTime_RegionSelector_Region0", "100");
			auto userSet = cam->getUserSet(UserSetId(i)); 		userSet.save();
		}
		system("pause");
		std::cout << "\n\nL: Here are the userSet list: \n" << std::endl;
		for (int i = 0; i < 6; ++i)
		{
			auto userSet = cam->getUserSet(UserSetId(i));
			std::cout << " - " << i << " : " << userSet.getName() << " : " << (userSet.exists() ? "Exist" : "No Exist") << " : " << (userSet.getDescription().empty() ? "No description yet. You can set your own description." : userSet.getDescription()) << std::endl;
		}

		std::cout << "\n\nL: Next, I will show you how to save parameters to userSets.\n Before continuing, please keep in mind that the operation direction is \n\n * * * Current_Using_Parameters_in_Ranger3 ===>===>===> UserSet_Parameters. \n\n" << std::endl;
		system("pause");
		for (int i = 1; i < 6; ++i)
		{
			std::stringstream ss; ss << 100 + i; cam->setParameterValue("ExposureTime_RegionSelector_Region0", ss.str());
			auto userSet = cam->getUserSet(UserSetId(i));
			userSet.save();
			std::cout << "\n\nL: Change ExposureTime_RegionSelector_Region0 to " << cam->getParameterValue("ExposureTime_RegionSelector_Region0") << std::endl;
			std::cout << "L: Save current parameters to [" << i << " : " << userSet.getName() << "]" << std::endl;
		}

		std::cout << "\n\nL: Next, I will show you how to add description to your userSer. \n\n" << std::endl;
		system("pause");
		for (int i = 1; i < 6; ++i)
		{
			auto userSet = cam->getUserSet(UserSetId(i));
			std::cout << "\n\nL: Current description of [" << userSet.getName() << "] is: " << std::endl;
			std::cout << (userSet.getDescription().empty() ? "No description yet." : userSet.getDescription()) << std::endl;

			std::cout << "L: To set description? (y/n/quit/next)" << std::endl;
			std::string str_command = "";	std::getline(std::cin, str_command); if (str_command == "quit") break; if (str_command == "quit") continue;
			if (str_command == "y")
			{
				std::cout << "L: Please input new description : " << std::endl;
				str_command = "";	std::cin >> str_command;
				userSet.setDescription(str_command);
			}
		}

		std::cout << "\n\nL: Next, I will show you how to set a userSet to be used when start-up. \n\n" << std::endl;
		std::cout << "\n\nL: Please input the one you want to set as start-up (1-5) \n" << std::endl;
		__g_userSetId = 1;	std::cin >> __g_userSetId;
		cam->getUserSet(UserSetId(__g_userSetId)).useAtStartup();

		std::cout << "\n\nL: All done! Re-scan all userSets: \n" << std::endl;
		for (int i = 0; i < 6; ++i)
		{
			auto userSet = cam->getUserSet(UserSetId(i));
			std::cout << " - " << i << " : " << userSet.getName() << " : " << (userSet.exists() ? "Exist" : "No Exist") << " : " << (userSet.getDescription().empty() ? "No description yet. You can set your own description." : userSet.getDescription()) << std::endl;
		}

		std::cout << "\n\nL: All done! Going to test? (y/n) Press t or T to switch \n" << std::endl;
		std::string str_command("y");	std::cin >> str_command;
		if (str_command == "y") __userSet_testing(is_started, mapCams);
	}
}

void DEMO::__file_operation(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	__stop_all(is_started, mapCams);
	std::cout << "\n\n======== Mode : File Operation ======== \n\n" << std::endl;
	size_t i = 0; std::vector<std::string> camNameList;
	for (auto subCam : mapCams) {
		std::cout << "---- " << i++ << " : " << subCam.first << std::endl; camNameList.emplace_back(subCam.first);
	}	std::cout << "\n\n\nL: Please input your choice: \n" << std::endl;
	int which(0);	std::cin >> which;
	if (0 <= which && which < camNameList.size())
	{
		auto cam = mapCams.at(camNameList[which]).inner_pCam;
		std::cout << "L: I will help you to send a file to Ranger3. Before continuing, please note that:  \n - 1. The file must less than 100 K. \n - 2. Only one file is allowed to save in Ranger3. " << std::endl;
		std::cout << "L: Please input a available path: \n" << std::endl;
		std::string path("");
		std::getline(std::cin, path);

		std::ifstream ifile(path, std::ifstream::binary);
		auto ec = cam->sendFileToCamera(ifile);
		std::cout << "\nL: Send file[\"" << path << "\"] to Ranger3[" << camNameList[which] << "] " << (ec == SickCam::CAM_STATUS::All_OK ? "OK" : "NG") << "!\n\n" << std::endl;

		auto strFile = cam->retrieveFileFromCamera();
		std::cout << "\nL: Retrieve file from Ranger3[" << camNameList[which] << "] \nL: cat \"UserFile\" \n" << strFile << "\n\n" << std::endl;

		ec = cam->deleteFileFromCamera();
		std::cout << "\nL: Delete UserFile in Ranger3[" << camNameList[which] << "] " << (ec == SickCam::CAM_STATUS::All_OK ? "OK" : "NG") << "!" << std::endl;
		std::cout << "\nL: Try to retrieve file[\"UserFile\"] from Ranger3[" << camNameList[which] << "] to check the deleting." << std::endl;
		std::cout << "\nL: Please Wait ... " << std::endl;
		strFile = cam->retrieveFileFromCamera();	std::cout << "\nL: Operation done, deleting " << (strFile.empty() ? "OK!" : "Fail!") << std::endl;
	}

	system("pause");

	__start_all(is_started, mapCams);
}
#endif

void DEMO::__switch_mode(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	__stop_all(is_started, mapCams);

	for (auto subCam : mapCams)		// Switch grabbing mode : Sensor / Range
	{
		auto str = subCam.second.inner_pCam->getParameterValue("DeviceScanType");
		subCam.second.inner_pCam->setParameterValue("DeviceScanType", (str == "Linescan3D" ? "Areascan" : "Linescan3D"));
		str = subCam.second.inner_pCam->getParameterValue("DeviceScanType");
		std::cout << subCam.first << " changes [DeviceScanType] to " << str << std::endl;
	}
	__start_all(is_started, mapCams);
}


void DEMO::__start_all_by_callback(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	if (!is_started) {
		for (auto subCam : mapCams) {
			auto str_expo = subCam.second.inner_pCam->getParameterValue("ExposureTime_RegionSelector_Region0");
			subCam.second.inner_pMP->text[11] = std::string("ExposureTime_RegionSelector_Region0 = ") + str_expo;
#ifndef __linux__
			subCam.second.inner_pMP->text[12] = subCam.second.inner_pCam->getUserSet(UserSetId(__g_userSetId)).getName();
#endif
			// register callback 
			subCam.second.inner_pCam->startCamera(onCallback_2, subCam.second.inner_pMP.get());

			std::cout << "Start " << subCam.first << std::endl;
			is_started = true;
		}
	}
}

void DEMO::__stop_all_by_callback(bool& is_started, std::map<std::string, RANGER3>& mapCams)
{
	if (is_started) {
		for (auto subCam : mapCams) {
			subCam.second.inner_pCam->stopCamera();
			std::cout << "Stop " << subCam.first << std::endl;
			is_started = false;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
#ifndef __linux__
void DEMO_TRI::__start_all(bool& is_started, std::map<std::string, TRISPECTOR>& mapCams)
{
	if (!is_started) {
		for (auto subCam : mapCams) {
			// register callback 
			subCam.second.inner_pCam->startCamera(onCallback_2, subCam.second.inner_pMP.get());

			std::cout << "Start " << subCam.first << std::endl;
			is_started = true;
		}
	}
}

void DEMO_TRI::__stop_all(bool& is_started, std::map<std::string, TRISPECTOR>& mapCams)
{
	if (is_started) {
		for (auto subCam : mapCams) {
			subCam.second.inner_pCam->stopCamera();
			std::cout << "Stop " << subCam.first << std::endl;
			is_started = false;
		}
	}

}

void DEMO_TRI::__switch_mode(bool& is_started, std::map<std::string, TRISPECTOR>& mapCams)
{

}
#endif

////////////////////////////////////////////////////////////////////////////////


void
#ifndef __linux__
__stdcall
#endif
onCallback_2(SickCam::ImgT* pImgD, void* any)
{
	auto inner_pMP = static_cast<mouse_para*>(any);
	std::unique_lock<std::mutex> lock(mouse_para::m_locker);
	inner_pMP->pImgD = std::make_shared<SickCam::ImgT>(*pImgD);

	// if has sensor data, save it to mouse_para and prepare to show. Deep copy
	inner_pMP->setS(InterfaceOpenCV::uint8_tPtrToMat(
		pImgD->has(DN::SEN) ? (uint8_t*)pImgD->getData(DN::SEN) : nullptr,
		pImgD->get_rows(),
		pImgD->get_cols()
	));

	// if has reflectance data, save it to mouse_para and prepare to show.
	inner_pMP->setI(InterfaceOpenCV::uint8_tPtrToMat(
		pImgD->has(DN::REF) ? (uint8_t*)pImgD->getData(DN::REF) : nullptr,
		pImgD->get_rows(),
		pImgD->get_cols()
	));

	// if has range data, save it to mouse_para and prepare to show.
	inner_pMP->setR(InterfaceOpenCV::uint16_tPtrToMat(
		pImgD->has(DN::RAN) ? (uint16_t*)pImgD->getData(DN::RAN) : nullptr,
		pImgD->get_rows(),
		pImgD->get_cols()
	));

	// if has calibrated range data, save it to mouse_para and prepare to show.
	inner_pMP->setCR(InterfaceOpenCV::floatPtrToMat(
		pImgD->has(DN::RAN_CAL) ? (float*)pImgD->getData(DN::RAN_CAL) : nullptr,
		pImgD->get_rows(),
		pImgD->get_cols()
	));

	// if has calibrated reflectance data, save it to mouse_para and prepare to show.
	inner_pMP->setCI(InterfaceOpenCV::uint8_tPtrToMat(
		pImgD->has(DN::REF_CAL) ? (uint8_t*)pImgD->getData(DN::REF_CAL) : nullptr,
		pImgD->get_rows(),
		pImgD->get_cols()
	));

	// if has calibrated scatter8 data, save it to mouse_para and prepare to show.
	inner_pMP->setCS8(InterfaceOpenCV::uint8_tPtrToMat(
		pImgD->has(DN::SCA_8) ? (uint8_t*)pImgD->getData(DN::SCA_8) : nullptr,
		pImgD->get_rows(),
		pImgD->get_cols()
	));

	// if has calibrated scatter16 data, save it to mouse_para and prepare to show.
	inner_pMP->setCS16(InterfaceOpenCV::uint8_tPtrToMat(
		pImgD->has(DN::SCA_8) ? (uint8_t*)pImgD->getData(DN::SCA_8) : nullptr,
		pImgD->get_rows(),
		pImgD->get_cols()
	));


	// --- Prepare to show.
	std::stringstream ss; ss << "Image ID = " << inner_pMP->pImgD->get_ID();
	inner_pMP->text[8] = ss.str();
	Show(inner_pMP);

	// ---- Save
	if (DEMO::__g_save_image)
	{
		std::stringstream ss;
		ss << "img-" << inner_pMP->pImgD->get_ID();
		inner_pMP->pImgD->SaveToIconFile(ss.str() + "-raw", std::vector<DN>({ DN::RAN, DN::REF, DN::SCA_8, DN::SCA_16, DN::MAR }), false);
		inner_pMP->pImgD->SaveToIconFile(ss.str() + "-cal", std::vector<DN>({ DN::RAN_CAL, DN::REF_CAL, DN::SCA_8_CAL, DN::SCA_16_CAL, DN::MAR }), true);
		std::cout << "Save image to " << ss.str() << std::endl;
	}

}




#endif // ENABLE_OPENCV


#include "stdafx.h"
#include "../../Sample/ApiDllTest/InterfaceOpenCV.h"

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
			(_img.getR_RangeAxis() == RangeAxis::STANDARD ? 1 : 0)));

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
					info.at<double>(0), info.at<double>(1),0.0, info.at<double>(2), info.at<double>(3),1.0,
					info.at<double>(4), info.at<double>(5), info.at<double>(6), info.at<double>(7),
					1,1,1,1
				);
			}

			if (DN::RAN == sub)
			{
				cv::Mat info;
				f[xmlNames.at(DN::RAN)] >> info;
				imgT.setRangeInfo(
					src.cols, src.rows,
					info.at<int16_t>(0), info.at<int16_t>(1), info.at<int16_t>(2), info.at<int16_t>(3),
					(info.at<int16_t>(4) == 1 ? RangeAxis::STANDARD : RangeAxis::REVERSED)
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

	if (event == CV_EVENT_MOUSEMOVE && !ustc->org_ran.empty())
	{
		std::cout << (ustc->org_ran.empty() ? "empty" : "") << std::endl;

		char sss[100] = { 0 };

		if (ustc->cal_ran.empty())
		{
			sprintf_s(sss, 100, "(px = %d, py = %d) = (x = %d, y = %d, z = %d)",
				x, y, x, y,
				ustc->org_ran.at<uint16_t>(y, x));
		}
		else
		{
			sprintf_s(sss, 100, "(px = %d, py = %d) = (x = %fmm, y = %fmm, z = %fmm)",
				x, y,
				InterfaceOpenCV::Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()),
				InterfaceOpenCV::Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()),
				ustc->cal_ran.at<float>(y, x));
		}

		std::cout << sss << std::endl;
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

	if (event == CV_EVENT_MOUSEMOVE && !ustc->org_ran.empty())
	{
		if (isWithinRangeComponent(x, y, ustc->rect_ran))
		{
			cv::Mat imgShow = ustc->pano.clone();
			cv::circle(imgShow, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x + 640 + 3, y), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x, y + 480 + 50), 2, cv::Scalar(0, 0, 255), -1);
			cv::circle(imgShow, cv::Point(x + 640 + 3, y + 480 + 50), 2, cv::Scalar(0, 0, 255), -1);

			char sss[100] = { 0 };
			positionInRawImage(x, y, ustc->rect_ran, ustc->org_ran.size(), x, y);
			if (ustc->cal_ran.empty())
			{
				if (!is_in_Image(x, y, ustc->cal_ran))
				{
					sprintf_s(sss, 100, "(px=%d, py=%d) = (x=%d, y=%d, z=%d)",
						x, y, x, y,
						ustc->org_ran.at<uint16_t>(y, x));
				}
			}
			else
			{
				sprintf_s(sss, 100, "(px=%d, py=%d) = (x=%fmm, y=%fmm, z=%fmm)",
					x, y,
					InterfaceOpenCV::Xmm(x, ustc->pImgD->getC_offsetX(), ustc->pImgD->getC_scaleX()),
					InterfaceOpenCV::Ymm(y, ustc->pImgD->getC_offsetY(), ustc->pImgD->getC_scaleY()),
					ustc->cal_ran.at<float>(y, x));
			}
			ustc->text[9] = std::string(sss);
			cv::putText(imgShow, ustc->text[9], cv::Point(ustc->rect_text.x, ustc->rect_text.y + 20 * 9), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 1);
			cv::imshow(ustc->winName_ran, imgShow);
		}
	}
}

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



#endif // ENABLE_OPENCV


/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	3.0.0.2
*	@date		2021.02.01
*/

#pragma once

#include "Control.h"

#ifdef ENABLE_OPENCV

#include <Ranger3.h>
#include <ImageTable.h>
#include <opencv2/opencv.hpp>
#include <mutex>


typedef SickCam::ImgT			ImgT;
typedef SPtr<ImgT>				ImgTPtr;
typedef SickCam::DN				DN;
typedef Vec<DN>					ImgNames;


////////////////////////////////////////////////////////////////////////////////


/** @brief InterfaceOpenCV
*	This class shows how to manage threads of Ranger3.
*	It is just a sample, you should modify it in your project.
*/
class InterfaceOpenCV
{
public:
	InterfaceOpenCV		();
	~InterfaceOpenCV	();

	// ----------------------------------------------------------------------------
	// Image Grabber
	static cv::Mat		floatPtrToMat	(float * pD,	size_t rows, size_t cols);
	static cv::Mat		uint8_tPtrToMat	(uint8_t * pD,	size_t rows, size_t cols);
	static cv::Mat		uint16_tPtrToMat(uint16_t * pD, size_t rows, size_t cols);

	static double		Xmm				(const uint16_t vpixel, double offsetX, double scaleX);
	static double		Ymm				(const uint16_t vpixel, double offsetY, double scaleY);
	static void			removeNAN		(cv::Mat & img);
    static cv::Mat      remapToUint8_t	(const cv::Mat & img); // in order to make it easier to show in GUI.
    
	// ----------------------------------------------------------------------------
	// API of ImgT to cv::Mat
    static cv::Mat		getMat			(const ImgT & _img, const DN & _dn);
	static cv::Mat		drawPoints		(const cv::Mat& _img, const std::vector<cv::Point2f>& _pts, const cv::Scalar& _color);
	static cv::Mat		drawPoints		(const cv::Rect& _rect, const int& subPixels, const cv::Mat& _img, const std::vector<cv::Point2f>& _pts, const cv::Scalar& _color, const int& _ptSize);
	static cv::Mat		drawLines		(cv::Rect _rect, const int& subPixels, const cv::Mat& _img, const std::vector<cv::Point2f>& _ptsStart, const std::vector<cv::Point2f>& _ptsEnd, const cv::Scalar& _color, const int& _ptSize);

	bool		SaveImgT2OpenCVXML		(const ImgT & _img, cStr & path = "file.xml")					const;
	ImgT		LoadImgTFromOpenCVXML	(cStr & path = "file.xml", const ImgNames & _names = ImgNames())const;

public:
	static const std::map<DN, cStr> xmlNames;

private:

};


////////////////////////////////////////////////////////////////////////////////


/** @brief mouse_para
*	This class shows how to manage threads of Ranger3.
*	It is just a sample, you should modify it in your project.
*/
struct mouse_para
{
	mouse_para()
		: 
		org_int		(cv::Mat()),
		org_ran		(cv::Mat()), 
		cal_ran		(cv::Mat()),
		cal_int		(cv::Mat()),
		cal_sca8	(cv::Mat()), 
		cal_sca16	(cv::Mat()),
		pano		(cv::Mat()),
		pImgD		(nullptr), 
		winName_ran	(""), 
		text		(std::vector<std::string>())
	{
	}

	mouse_para(mouse_para & mp)
	{
		org_sen			= mp.org_sen;
		org_ran			= mp.org_ran;
		org_int			= mp.org_int;
		cal_ran			= mp.cal_ran;
		cal_int			= mp.cal_int;
		cal_sca8		= mp.cal_sca8;
		cal_sca16		= mp.cal_sca16;

		pano			= mp.pano;
		winName_ran		= mp.winName_ran;
		pImgD			= mp.pImgD;
		rect_text		= mp.rect_text;
		rect_ran		= mp.rect_ran;
		rect_ref		= mp.rect_ref;
		rect_ran_cal	= mp.rect_ran_cal;
		rect_ref_cal	= mp.rect_ref_cal;
		text			= mp.text;

	}

	void setS	(cv::Mat V) { org_sen = V; }
	void setR	(cv::Mat V) { org_ran = V; }
	void setI	(cv::Mat V) { org_int = V; }
	void setCR	(cv::Mat V) { cal_ran = V; }
	void setCI	(cv::Mat V) { cal_int = V; }
	void setCS8	(cv::Mat V) { cal_sca8 = V; }
	void setCS16(cv::Mat V) { cal_sca16 = V; }

	// data 
	cv::Mat			org_sen;		/**< Sensor image							   */
	cv::Mat			org_ran;		/**< Range image							   */
	cv::Mat			org_int;		/**< Reflactance image						   */
	cv::Mat			cal_ran;		/**< Range image calibrated					   */
	cv::Mat			cal_int;		/**< Reflactance image calibrated			   */
	cv::Mat			cal_sca8;		/**< Scatter 8 bit image calibrated			   */
	cv::Mat			cal_sca16;		/**< Scatter 16 bits image calibrated		   */

	// on_mouse parameters
	cv::Mat			pano;
	std::string		winName_ran;
	ImgTPtr			pImgD;
	cv::Rect		rect_text;
	cv::Rect		rect_ran;
	cv::Rect		rect_ref;
	cv::Rect		rect_ran_cal;
	cv::Rect		rect_ref_cal;
	std::vector<std::string> 
					text;

	// lock
	static std::mutex		
					m_locker;		/**< to lock mouse_para		   */

};


////////////////////////////////////////////////////////////////////////////////

/** @brief OpenCV mouse callback function, used in thread grabbing demo. */
void on_mouse_ran(int event, int x, int y, int flags, void *_ustc);


/** @brief OpenCV mouse callback function, used in callback demo. */
void on_mouse(int event, int x, int y, int flags, void * _ustc);

/** @brief OpenCV mouse callback function, used in trispector demo. */
void on_mouse_trispector(int event, int x, int y, int flags, void * _ustc);


void Show(	mouse_para* mp, 
			cv::Size imgSize = cv::Size(640, 480),
			int labelHeight = 50, 
			int lineSize = 3, 
			double frontScale = 0.7, 
			int frontLineSize = 2);

void Show(	SPtr<mouse_para> mp, 
			cv::Size imgSize = cv::Size(640, 480),
			int labelHeight = 50, 
			int lineSize = 3, 
			double frontScale = 0.7, 
			int frontLineSize = 2);

std::vector<uint64_t> getTimestamp_fromMat(const cv::Mat& mark);

// This is only used to get timestamp ;
uint64_t combine_uint32_uint32(const uint32_t& Low_2, const uint32_t& High_3);


//
struct RANGER3
{
	RANGER3() = delete;
	RANGER3(const bool			is_using_IP,
		const std::string& IP_MAC,
		const std::string& parameter_File_Path,
		const std::string& calibration_File_Path)
		:is_using_IP(is_using_IP)
		, IP_MAC(IP_MAC)
		, parameter_File_Path(parameter_File_Path)
		, calibration_File_Path(calibration_File_Path)
	{}

	bool		 is_using_IP;
	std::string	 IP_MAC;
	std::string	 parameter_File_Path;
	std::string	 calibration_File_Path;

	std::string	 inner_window_name;
	std::string  inner_ip;
	std::string  inner_mac;

	SPtr<mouse_para>		inner_pMP;
	SPtr<SickCam::Ranger3>	inner_pCam;  // This is the pointer to class Ranger3.
};



class DEMO
{
public:
	DEMO() = default;
	~DEMO() = default;

	static void __start_all(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __stop_all(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __parameter_setting(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __userSet_testing(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __userSet_init(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __file_operation(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __switch_mode(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __start_all_by_callback(bool& is_started, std::map<std::string, RANGER3>& mapCams);
	static void __stop_all_by_callback(bool& is_started, std::map<std::string, RANGER3>& mapCams);

	static int			__g_userSetId;
	static std::string	__g_cameraName_userSet;
	static bool			__g_save_image;

};

////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __linux__
#include <Trispector.h>

struct TRISPECTOR
{
	TRISPECTOR() = delete;
	TRISPECTOR(const std::string& SN)
		:SN(SN)
	{}

	std::string	 SN;

	std::string					inner_window_name;
	SPtr<mouse_para>			inner_pMP;
	SPtr<SickCam::Trispector>	inner_pCam;  // This is the pointer to class Ranger3.
};


class DEMO_TRI
{
public:
	DEMO_TRI() = default;
	~DEMO_TRI() = default;

	static void __start_all(bool& is_started, std::map<std::string, TRISPECTOR>& mapCams);
	static void __stop_all(bool& is_started, std::map<std::string, TRISPECTOR>& mapCams);
	static void __switch_mode(bool& is_started, std::map<std::string, TRISPECTOR>& mapCams);

};
#endif

void
#ifndef __linux__
__stdcall
#endif
onCallback_2(SickCam::ImgT* pImg, void* any);




#endif // ENABLE_OPENCV

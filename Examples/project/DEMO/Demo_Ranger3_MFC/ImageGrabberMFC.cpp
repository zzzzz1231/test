// 用于轮询采集图像。使用 callback 的示例程序无需使用此文件。
#include "stdafx.h"
#include "../../Sample/ApiDllTest/ImageGrabber.h"


//====================================================================
// ImageGrabber : constructor/destructor   


ImageGrabber::ImageGrabber()
	: is_connected(false), is_require_disconnect(false), is_grabbed(false), 
	is_require_restart(false), is_start_acquisition(false), is_require_update_parameter(false), 
	is_require_update_timeout(false), mMacAddress(""), is_require_start_acquisition(false), 
	is_require_stop_acquisition(false), is_require_change_output_mode(false), m_gCamStatus("<Disconnected>"), mRunRate(100)
{
	m_pCam = nullptr;
	m_pCam->setScaleY(1);
	
}

ImageGrabber::ImageGrabber(const std::shared_ptr<R3S>& _R3S, const std::string& _IP_MAC, const bool isIP)
	: is_connected(false), is_require_disconnect(false), is_grabbed(false), 
	is_require_restart(false), is_start_acquisition(false), is_require_update_parameter(false), 
	is_require_update_timeout(false), is_require_start_acquisition(false), 
	is_require_stop_acquisition(false), is_require_change_output_mode(false), m_gCamStatus("<Disconnected>"), mRunRate(100)
{
	m_pCam = std::make_shared<R3>(_R3S, _IP_MAC, isIP);
	if (SickCam::CAM_STATUS::All_OK == m_pCam->isReady())
	{
		mMacAddress = m_pCam->getMac();
		mIpAddress = m_pCam->getIp();
	}
	else
	{
		std::cout << "X\nX\nX\nX\nX\nX\n*  init ImageGrabber failed!!\n\n*  The SickCam::Ranger3Shared is not ready!!\n X\nX\nX\nX\nX\nX\n" << std::endl;
	}
}


//====================================================================
// ImageGrabber : public interface


bool 
ImageGrabber::setUserParameter(const double _scaleY, const std::string & para_csv_path,
	const std::string & calib_xml_path, 
#ifndef DISABLE_CAL_IN_PC
	const SiCaliWrapper::RectMethod method,
#endif // DISABLE_CAL_IN_PC
	const uint64_t timeOut, const float missing_data_of_calibrated_image)
{
	m_pCam->setScaleY(_scaleY);
	m_pCam->setParametPath(para_csv_path);
	m_pCam->setTimeOut(timeOut);
#ifndef DISABLE_CAL_IN_PC
	m_pCam->setCalibraPath(calib_xml_path);
	m_pCam->setRectMethos(method);
	m_pCam->setMissingData(missing_data_of_calibrated_image);
#endif // DISABLE_CAL_IN_PC
	return true;
}

bool 
ImageGrabber::connect()
{
	// check
	if (m_pCam->getDeviceName() == "")
	{
		printf("Error: Init camara failed, check mMacAddress please. input mac: %s, input ip: %s \n", (mMacAddress == "" ? "empty!" : mMacAddress.c_str()), (mIpAddress == "" ? "empty!" : mIpAddress.c_str()));
		return false;
	}

	Ecode ec;
	try
	{
		ec = m_pCam->connectCamera();
			
		if (ec != Ecode::All_OK)
		{
			cout << "Error: setCamera failed. para_csv_path=" << m_pCam->getParametPath()
#ifndef DISABLE_CAL_IN_PC
				<< ", calib_xml_path=" << m_pCam->getCalibraPath()
#endif // DISABLE_CAL_IN_PC
				<< endl;

			switch (ec)
			{
			case Ecode::ERROR_SET_CAM:
				printf("Error: ERROR_SET_CAM \n");
				return false;
				break;
#ifndef DISABLE_CAL_IN_PC
			case Ecode::ERROR_CALIBRATION_PATH:
				printf("Error: ERROR_CALIBRATION_PATH, Path: %s \n", m_pCam->getCalibraPath().c_str());
				return false;
				break;
#endif // DISABLE_CAL_IN_PC
			case Ecode::ERROR_CSV_PATH:
				printf("Error: ERROR_CSV_PATH, Path: %s \n", m_pCam->getParametPath().c_str());
				return false;
				break;
			default:
				printf("Error: UNKNOWN_ERROR \n");
				return false;
				break;
			}
		}

		is_connected = true;
		m_gCamStatus = "<Connected___>";
		is_sensor = getParameter("DeviceScanType") == "Areascan";
		m_pCam->setTimeOut(2000);
	}
	catch (const char * msg)
	{
		std::cerr << msg << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "Unknown Error happened in connectCamera() !" << std::endl;
		return false;
	}
	return true;
}

void 
ImageGrabber::run()
{
	while (is_connected)
	{
		//SickCam::ImgT imgTable;
		if (!is_require_disconnect && !is_require_restart && is_start_acquisition && 
			!is_require_start_acquisition && !is_require_stop_acquisition && 
			!is_require_update_parameter && !is_require_update_timeout &&
			!is_require_change_output_mode)
		{
			SickCam::ImgT imgTable;
#ifndef DEBUG_CODE
			try
#endif // !DEBUG_CODE
			{
				Ecode ec = m_pCam->getImageData(imgTable);
				if (ec == Ecode::TIME_OUT)
				{
					Sleep(0);
					continue;
				}

				if (ec != Ecode::All_OK)
				{
					std::cout << "getImageData failed! Ecode : " << SickCam::CAM_STATUS_str(ec) << std::endl;
					is_grabbed = false;
					continue;
				}
			}
#ifndef DEBUG_CODE
			catch (const char * msg)
			{
				std::cerr << msg << std::endl;
				continue;
			}
			catch (...)
			{
				std::cout << "Grab run error happended in getImageData!" << std::endl;
				continue;
			}
#endif // !DEBUG_CODE

			// save image
			std::unique_lock<std::mutex> lock(mMutexImageData);
			std::unique_lock<std::mutex> lock2(mMutexIsGrabbed);
			
			m_listImgTable.push_back(imgTable);

			is_grabbed = true;
		}
		else if (is_require_start_acquisition && !is_require_stop_acquisition)
		{
			if (is_start_acquisition)
			{
				is_require_start_acquisition = false;
				is_require_stop_acquisition = false;
			}
			else
			{
				mRunRate = _runRate();
				Ecode ec = m_pCam->startCamera();
				is_start_acquisition = (ec == Ecode::All_OK);
				if (is_start_acquisition)
					m_gCamStatus = "<Started_____>";
			}
		}
		else if (!is_require_start_acquisition && is_require_stop_acquisition)
		{
			if (!is_start_acquisition)
			{
				is_require_start_acquisition = false;
				is_require_stop_acquisition = false;
			}
			else
			{
				Ecode ec = m_pCam->stopCamera();
				is_start_acquisition = !(ec == Ecode::All_OK);
				if (!is_start_acquisition)
				{
					m_gCamStatus = "<Stopped_____>";
					std::unique_lock<std::mutex> lock(mMutexImageData);
					m_listImgTable.clear();
				}
			}
		}
		else if (is_require_disconnect)
		{
			if (isStartAcquition())
			{
				Ecode ec = m_pCam->stopCamera();
				is_start_acquisition = !(ec == Ecode::All_OK);
				is_require_start_acquisition = is_start_acquisition;
			}

			if (!isStartAcquition())
			{
				Ecode ec = m_pCam->disconnectCamera();
				is_connected = !(ec == Ecode::All_OK);
				is_require_disconnect = !(ec == Ecode::All_OK);
				if (!is_connected)
				{
					m_gCamStatus = "<Disconnected>"; 
					std::unique_lock<std::mutex> lock(mMutexImageData);
					m_listImgTable.clear();
					break;
				}
			}
		}
		else if (is_require_change_output_mode)
		{
			if (isStartAcquition())
				requireStopAcquition();

			if (!isStartAcquition() && _setSensorMode(!is_sensor))
			{
				printf("Stop, \n");
				is_require_change_output_mode = false;
				requireStartAcquition();
			}
		}

		Sleep(0);

	}
}

SickCam::ImgT 
ImageGrabber::getImgT()
{
	std::unique_lock<std::mutex> lock(mMutexImageData);
	if (m_listImgTable.empty())		return SickCam::ImgT();

	SickCam::ImgT head = m_listImgTable.front();
	m_listImgTable.pop_front();
	is_grabbed = !m_listImgTable.empty();

	return head;
}

std::shared_ptr<R3>& 
ImageGrabber::getR3()
{
	return m_pCam;
}

bool 
ImageGrabber::listAllParameters()
{
	m_pCam->getAllParameterInfo(m_gParameters);
	std::cout << "--------------- Parameters list ---------------" << std::endl;
	
	size_t i(0);
	for (auto sub : m_gParameters)
	{
		std::cout << std::endl << std::endl;
		std::cout << "* Param " << ++i << std::endl;
		std::cout << "- Name          - " <<  sub.first << std::endl;
		std::cout << "- Current Value - " << sub.second->getStrValue() << std::endl;
		std::cout << "- Is Read Only  - " << (sub.second->isReadOnly() ? "Yes" : "No") << std::endl;

		const std::vector<std::string> opts = sub.second->getOptions();
		if(opts.empty())
			std::cout << "- Opts      - Number " << std::endl;
		else
		{
			for(auto opt : opts)
				std::cout << "- Opts      - " << opt << std::endl;
		}
	}

	return false;
}

bool 
ImageGrabber::setParameter(const std::string & _P, const std::string & _V)
{
	if (is_start_acquisition) return false;

	if (_P.empty() || _V.empty()) return false;

	printf("%d, %s, %s\n", (Ecode::All_OK == m_pCam->setParameterValue(_P, _V)), _P.c_str(), _V.c_str());

	return getParameter(_P) == _V;
}

std::string 
ImageGrabber::getParameter(const std::string & _P)
{
	if (is_start_acquisition) return false;

	if (_P.empty()) return false;

	return m_pCam->getParameterValue(_P);
}

bool 
ImageGrabber::requireDisconnect()
{
	is_require_disconnect = true;
	return is_require_disconnect;
}

bool 
ImageGrabber::requireRestart()
{
	is_require_restart = true;
	return is_require_restart;
}

bool 
ImageGrabber::requireStartAcquition()
{
	is_require_start_acquisition = true;
	is_require_stop_acquisition = false;
	return is_require_start_acquisition;
}

bool 
ImageGrabber::requireStopAcquition()
{
	is_require_stop_acquisition = true;
	is_require_start_acquisition = false;
	return is_require_stop_acquisition;
}

bool 
ImageGrabber::requireUpdateParameter(const double _scaleY, const std::string & para_csv_path, const std::string & calib_xml_path, 
#ifndef DISABLE_CAL_IN_PC
	const SiCaliWrapper::RectMethod method,
#endif // DISABLE_CAL_IN_PC
	const float missingData)
{
	is_require_update_parameter = true;
	m_pCam->setScaleY(_scaleY);
	m_pCam->setParametPath(para_csv_path);
#ifndef DISABLE_CAL_IN_PC
	m_pCam->setCalibraPath(calib_xml_path);
	m_pCam->setRectMethos(method);
	m_pCam->setMissingData(missingData);
#endif // DISABLE_CAL_IN_PC

	return is_require_update_parameter;
}

bool 
ImageGrabber::requireUpdateTimeout(uint64_t _timeout)
{
	is_require_update_timeout = true;
	return is_require_update_timeout;
}

bool 
ImageGrabber::requireChangeOutputMode()
{
	is_require_change_output_mode = true;
	return is_require_change_output_mode;
}

bool 
ImageGrabber::isGrabbed()
{
	if (is_require_disconnect || !is_connected)
		is_grabbed = false;

	std::unique_lock<std::mutex> lock(mMutexIsGrabbed);
	return is_grabbed;
}

bool 
ImageGrabber::isConnected()
{
	return is_connected;
}

bool 
ImageGrabber::isDisconnected()
{
	return !is_connected;
}

bool 
ImageGrabber::isStartAcquition()
{
	return is_start_acquisition;
}

bool 
ImageGrabber::isUpdateParameterDone()
{
	return !is_require_update_timeout;
}

bool 
ImageGrabber::isSensorMode()
{
	return is_sensor;
}

bool 
ImageGrabber::clearData()
{
	m_listImgTable.clear();
	m_listImgTable.swap(std::list<SickCam::ImgT>());
	return m_listImgTable.empty();
}


//====================================================================
// ImageGrabber : protected interface


bool 
ImageGrabber::_setSensorMode(const bool enable)
{
	if (isStartAcquition()) return false;

	setParameter("DeviceScanType", (enable ? "Areascan" : "Linescan3D"));
	printf("IG -------  setParameter\n");

	std::string DeviceScanType = getParameter("DeviceScanType");
	printf("IG ------- %s \n", DeviceScanType.c_str());

	is_sensor = DeviceScanType == "Areascan";
	return DeviceScanType == (enable ? "Areascan" : "Linescan3D");
}

unsigned long 
ImageGrabber::_runRate()
{
	is_sensor = getParameter("DeviceScanType") == "Areascan";

	if (isSensorMode())
	{
		float linerate = atof(getParameter("AcquisitionFrameRate").c_str());
		return static_cast<unsigned long>(1000 / linerate);
	}
	else 
	{
		float linerate = atof(getParameter("AcquisitionLineRate").c_str());
		int height = atoi(getParameter("Height_RegionSelector_Region1").c_str());
		return static_cast<unsigned long>(height / linerate * 1000);
	}
}


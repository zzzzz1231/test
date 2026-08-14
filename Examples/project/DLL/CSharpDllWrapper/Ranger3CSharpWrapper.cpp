/**	@file	SampleApiDllCSharpWrapper.h
*
*	@brief	C# Wrapper Class of Ranger3.
*
*	@attention
*	This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
*
*
*	@copyright	Copyright 2016-2021 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*
*/

#include "stdafx.h"
#include "Ranger3CSharpWrapper.h"

namespace SICK3D
{

////////////////////////////////////////////////////////////////////////////////

GenICam3D::GenICam3D(	const String^ logPath,
						const String^ ctiPath,
						const bool enShow,
						const bool enWrite)
{
	pin_ptr<const wchar_t> wpStrlogPath = PtrToStringChars(logPath);
	auto strlogPath = wchart2string(wpStrlogPath);

	pin_ptr<const wchar_t> wpStrctiPath = PtrToStringChars(ctiPath);
	auto strctiPath = wchart2string(wpStrctiPath);
	gp_R3S = std::make_shared<R3S>(strlogPath, strctiPath, enShow, enWrite);
}

GenICam3D::~GenICam3D()
{
	cout << "Release CSR3S ----------------\n\n";
	gp_R3S->~CameraShared();
	gp_R3S.reset();
}

deviceList 
GenICam3D::getConDevList()
{
	auto devs = gp_R3S->getConDevList();
	auto res = gcnew array<String^>(devs.size());
	int i = 0;
	for (auto sub : devs)
	{
		std::wstring ws = string2wstring(sub.first);
		res[i++] = gcnew String(ws.c_str());
	}
	return res;
}

deviceList 
GenICam3D::getConDevListIP()
{
	auto devs = gp_R3S->getConDevListIP();
	auto res = gcnew array<String^>(devs.size());
	int i = 0;
	for (auto sub : devs)
	{
		std::wstring ws = string2wstring(sub.first);
		res[i++] = gcnew String(ws.c_str());
	}
	return res;
}

deviceList GenICam3D::getConDevListMAC()
{
	auto devs = gp_R3S->getConDevListMAC();
	auto res = gcnew array<String^>(devs.size());
	int i = 0;
	for (auto sub : devs)
	{
		std::wstring ws = string2wstring(sub.first);
		res[i++] = gcnew String(ws.c_str());
	}
	return res;
}

void
GenICam3D::_release()
{
	gp_R3S.reset(); 
}

String ^ 
GenICam3D::getPathToRanger3Producer()
{
	auto str = gp_R3S->getPathToRanger3Producer();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}


////////////////////////////////////////////////////////////////////////////////

#ifndef _DISABLE_GrabberThread

GrabberThread::GrabberThread(R3 * pr3)
	: mp_R3(pr3), m_IsRun(false), m_IsFinishGrab(true), m_req(RequireCode::REQ_Nothing), m_sleepTime(50)
{
	locker_data = gcnew Object;
	locker_req = gcnew Object;
	locker_sleepTime = gcnew Object;
}

GrabberThread::~GrabberThread()
{
	_clear_and_break();
}

void
GrabberThread::run()
{
	m_IsRun = true;
	while (true)
	{
		SickCam::ImgT img;

		if (_doRequest()) break;

		if (m_req == RequireCode::REQ_Nothing)
		{
			m_IsFinishGrab = false;

			if (Ecode::All_OK == mp_R3->getImageData(img) && !img.isEmpty())
			{
				// copy data
				CSImgT^ imgTable = gcnew CSImgT();
				imgTable->setData(img);

				lock l(locker_data);
				m_ImageData.push_back(imgTable);
				m_IsGrabbed = true;

			}

			m_IsFinishGrab = true;

		}

		if (_doRequest()) break;

		{	
		lock l(locker_sleepTime);
		Sleep(m_sleepTime);
		}
	}
	_clear_and_break();
	m_IsRun = false;
}

CSImgT^ 
GrabberThread::getData()
{
	lock l(locker_data);
	
	CSImgT^ head = gcnew CSImgT(); // return to caller. Deleted by caller.
	head->_deepCopy(m_ImageData.front());
	
	delete m_ImageData.front();
	m_ImageData.pop_front(); // delete data generate by "gcnew" in run().

	m_IsGrabbed = !m_ImageData.empty();

	return head;
}

bool
GrabberThread::sendRequest(const RequireCode req)
{
	lock l(locker_req);
	m_req = req;
	return true;
}

bool
GrabberThread::_doRequest()
{
	switch (m_req)
	{
	case RequireCode::REQ_QUIT_RUN:
		return _clear_and_break();
		break;
	case RequireCode::REQ_STOP_CAMERA:
		// do nothing, but wait grabbing routine over in Run().
		_clear_and_break();
		break;
	case RequireCode::REQ_Nothing:
		break;
	default:
		break;
	}
	return false;
}

bool 
GrabberThread::_clear_and_break()
{
	lock l(locker_data);

	m_IsGrabbed = false;
	for (auto it = m_ImageData.begin(); it != m_ImageData.end(); ++it)
	{
		delete *it;
	}
	m_ImageData.clear();


	GC::Collect();
	GC::WaitForFullGCComplete();

	return m_ImageData.empty();
}


////////////////////////////////////////////////////////////////////////////////
#ifdef _CALLBACK_ON

void
GrabberCallback::_callback_run()
{
	while (true)
	{
		Sleep(2);

		{
			lock l(m_callback_require_stop_locker);
			if (m_callback_require_stop)
				break;
		}

		SickCam::ImgT img;
		try
		{
			if (SickCam::CAM_STATUS::All_OK == mp_R3->getImageData(img) && !img.isEmpty())
			{

				{
					lock l(m_callback_is_finish_copy_locker);
					//std::unique_lock<std::mutex> lock(m_callback_is_finish_copy_locker);
					m_callback_is_finish_copy = false;
				}
				///auto _thread = std::make_shared<std::thread>(&Ranger3::_callback_run_on_grabbed, this, &img);
				///_thread->detach();

				///m_Thread = gcnew Thread(gcnew ThreadStart(this, &(Ranger3::_callback_run))); // todo ------------

				//auto th = gcnew Thread(gcnew ThreadStart(this, &Ranger3::_callback_run_on_grabbed)); // todo ------------
				//th->Start(img);

				while (true)
				{
					{
						//std::unique_lock<std::mutex> lock(m_callback_is_finish_copy_locker);
						lock l(m_callback_is_finish_copy_locker);
						if (m_callback_is_finish_copy)
							break;
					}
					Sleep(2);
				}

			}
			else
				continue;
		}
		catch (...)
		{
			continue;
		}
	}
	m_callback_is_on = false;
}

void
GrabberCallback::_callback_run_on_grabbed(Object^ _img)
{
	auto img = (_img);
	{
		lock l(m_callback_is_finish_copy_locker);
		m_callback_is_finish_copy = true;
	}

	//auto th = gcnew Thread(gcnew ThreadStart(this, &(Ranger3::m_callBack_function), img, m_callBack_inputs)); // todo ------------
	//th->Start();

	///auto _thread = std::make_shared<std::thread>(m_callBack_function, &img, m_callBack_inputs);
	///_thread->join(); // wait callback done, and free the img.

}


#endif

#endif

////////////////////////////////////////////////////////////////////////////////


Ranger3::Ranger3(const String ^ ip_mac, const bool isIP)
#ifndef _DISABLE_GrabberThread
	: m_enable_CPP_Grab_thread(false)
#endif
{
	pin_ptr<const wchar_t> wStr = PtrToStringChars(ip_mac);
	std::string sIpMac = wchart2string(wStr);

	mp_R3					= new SickCam::Ranger3(gp_R3S, sIpMac, isIP);
	mp_cate					= new SickCam::Categories;
	mp_map_Cata_ParamStr	= new SickCam::Params;
	mp_map_allPramPtr		= new SickCam::AllParams;
	
	locker_GetData = gcnew Object;

#ifndef _DISABLE_GrabberThread
	mp_gt = gcnew GrabberThread(mp_R3);
	locker_GetData = gcnew Object;
	locker_Start = gcnew Object;
#endif

}

Ranger3::~Ranger3()
{
	if (mp_R3 != nullptr)
	{
		delete mp_R3;
		mp_R3 = nullptr;
	}

	if (mp_cate != nullptr)
	{
		delete mp_cate;
		mp_cate = nullptr;
	}

	if (mp_map_Cata_ParamStr != nullptr)
	{
		delete mp_map_Cata_ParamStr;
		mp_map_Cata_ParamStr = nullptr;
	}

	if (mp_map_allPramPtr != nullptr)
	{
		delete mp_map_allPramPtr;
		mp_map_allPramPtr = nullptr;
	}
}

CAM_STATUS 
Ranger3::isReady()
{
	return static_cast<CAM_STATUS>(mp_R3->isReady());
}

CAM_STATUS
Ranger3::connectCamera(const bool do_not_use_it_an_more)
{
#ifndef _DISABLE_GrabberThread
	m_enable_CPP_Grab_thread = do_not_use_it_an_more;
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_Nothing);
		m_Thread = gcnew Thread(gcnew ThreadStart(mp_gt, &(GrabberThread::run)));
		m_Thread->Start();
	}
#endif
	return static_cast<CAM_STATUS>(mp_R3->connectCamera());
}

CAM_STATUS
Ranger3::connectCamera()
{
	return static_cast<CAM_STATUS>(mp_R3->connectCamera());
}

CAM_STATUS 
Ranger3::startCamera()
{
#ifndef _DISABLE_GrabberThread
	lock l(locker_Start);
	// start run 
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_Nothing);
		while (!mp_gt->isRun()) Sleep(0);
	}
#endif
	return static_cast<CAM_STATUS>(mp_R3->startCamera());
}

#ifdef _CALLBACK_ON
CAM_STATUS 
Ranger3::startCamera(CallbackEvent ^ pCallback, Object ^ any)
{
	auto err = startCamera();
	if (err == SICK3D::CAM_STATUS::All_OK)
	{
		m_callBack_function = pCallback;
		
		mp_cb->m_callBack_inputs = any;
		mp_cb->m_callback_is_on = true;
		mp_cb->m_callback_require_stop = false;

		Thread^ th = gcnew Thread(gcnew ThreadStart(mp_cb, &(GrabberCallback::_callback_run))); // todo ------------
		th->Start();
	}
	return err;
}

#endif

CAM_STATUS
Ranger3::stopCamera()
{
#ifndef _DISABLE_GrabberThread
	lock l(locker_GetData);

	// stop run 
	if (m_enable_CPP_Grab_thread && mp_R3->getStatus() == SickCam::CAM_STATUS::CAM_IS_STARTED)
	{
		mp_gt->sendRequest(RequireCode::REQ_STOP_CAMERA);
		while (!mp_gt->isFinishGrab()) Sleep(0);
	}
#endif

	auto status = mp_R3->stopCamera();
	return static_cast<CAM_STATUS>(status);
}

CAM_STATUS
Ranger3::disconnectCamera()
{
#ifndef _DISABLE_GrabberThread
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_QUIT_RUN);
		while (mp_gt->isRun()) Sleep(0);
	}
#endif
	auto status = mp_R3->disconnectCamera();
	return static_cast<CAM_STATUS>(status);
}

CAM_STATUS	
Ranger3::getImageData(CSImgT^ imgTable)
{
#ifndef _DISABLE_GrabberThread

	lock l(locker_GetData);
	if (m_enable_CPP_Grab_thread)
	{
		// getData from GrabberThread
		if (!isGrabbed())
			return CAM_STATUS::ERROR_IMAGE_NOT_READY;
		
		auto pData = mp_gt->getData(); // gcnew an extra memory
		imgTable->_deepCopy(pData); // copy memory
		delete pData; // delete here

		return CAM_STATUS::All_OK;
	} 
	else
#endif
	{
		lock l(locker_GetData);

		if (mp_R3->getStatus() != SickCam::CAM_STATUS::CAM_IS_STARTED)
			return CAM_STATUS::ERROR_CAM_NOT_START;

		//SickCam::ImgT img;
		auto status = mp_R3->getImageData(*(imgTable->mp_ImgT));
		//if (status == SickCam::CAM_STATUS::All_OK)
		//	imgTable->setData(img);

		//SickCam::ImgT img;
		//auto status = mp_R3->getImageData(img);
		//if (status == SickCam::CAM_STATUS::All_OK)
		//	imgTable->setData(img);


		return static_cast<CAM_STATUS>(status);
	}
}

CAM_STATUS 
Ranger3::setGrabSleepTime(const unsigned int sleepTime)
{
#ifndef _DISABLE_GrabberThread
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->setSleepTime(sleepTime);
		return CAM_STATUS::All_OK;
	}
	else
		return CAM_STATUS::UNKNOWN;
#else
	return CAM_STATUS::All_OK;
#endif

}

unsigned int 
Ranger3::getGrabSleepTime()
{
#ifndef _DISABLE_GrabberThread

	if (m_enable_CPP_Grab_thread)
	{
		return mp_gt->getSleepTime();
	}
	else
#endif
		return 0;
}

bool		
Ranger3::getHeartBeat()
{
	return mp_R3->getHeartBeat();
}

//bool		
//Ranger3::setStatusLostHeartBeat()
//{
//	return mp_R3->setStatusLostHeartBeat();
//}

CAM_STATUS	
Ranger3::reconnectCamera()
{
	return static_cast<CAM_STATUS>(mp_R3->reconnectCamera());
}



CAM_STATUS
Ranger3::setParametPath(const String^ value)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(value);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setParametPath(str));
}

CAM_STATUS 
Ranger3::setIp(const String ^ IP, const bool isPersistent)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(IP);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setIp(str, isPersistent));
}

CAM_STATUS 
Ranger3::setSubnet(const String ^ Su, const bool isPersistent)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(Su);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setSubnet(str, isPersistent));
}

CAM_STATUS 
Ranger3::setParameterValue(const String ^ ParamterName, const String ^ value)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(ParamterName);
	auto strPN = wchart2string(wpStrPN);

	pin_ptr<const wchar_t> wpStrVa = PtrToStringChars(value);
	auto strVa = wchart2string(wpStrVa);

	return static_cast<CAM_STATUS>(mp_R3->setParameterValue(strPN, strVa));
}

CAM_STATUS
Ranger3::setParameter(const String^ ParamterName, const String^ value)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(ParamterName);
	auto strPN = wchart2string(wpStrPN);

	pin_ptr<const wchar_t> wpStrVa = PtrToStringChars(value);
	auto strVa = wchart2string(wpStrVa);

	return static_cast<CAM_STATUS>(mp_R3->setParameter(strPN, strVa));
}

String ^
Ranger3::getParametPath()
{
	auto str = mp_R3->getParametPath();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

CAM_STATUS
Ranger3::enableCorrectRawImageEdgeEffect(const bool enable)
{
	return static_cast<CAM_STATUS>(mp_R3->enableCorrectRawImageEdgeEffect(enable));
}



String ^ 
Ranger3::getIp(const bool isPersistent)
{
	auto str = mp_R3->getIp(isPersistent);
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getSubNet(const bool isPersistent)
{
	auto str = mp_R3->getSubNet(isPersistent);
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getMac()
{
	auto str = mp_R3->getMac();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getDeviceName()
{
	auto str = mp_R3->getDeviceName();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

String ^ 
Ranger3::getParameterValue(const String ^ ParamterName)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(ParamterName);
	auto strPN = wchart2string(wpStrPN);

	auto strV = mp_R3->getParameterValue(strPN);
	auto wstrV = string2wstring(strV);
	return gcnew String(wstrV.c_str());
}

String^
Ranger3::getParameter(const String^ ParamterName)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(ParamterName);
	auto strPN = wchart2string(wpStrPN);

	//auto strV = mp_R3->getParameterValue(strPN);
	auto strV = mp_R3->getParameter(strPN);
	auto wstrV = string2wstring(strV);
	return gcnew String(wstrV.c_str());
}

#ifndef DISABLE_CAL_IN_PC
CAM_STATUS
Ranger3::setCalibration_inPC(const String ^ filePath, const bool toDoRectify, const int rectificationWidth, const int threads_number)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(filePath);
	auto strPN = wchart2string(wpStrPN);

	return static_cast<CAM_STATUS>(mp_R3->setCalibration_inPC(strPN, toDoRectify, static_cast<uint32_t>(rectificationWidth), threads_number));
}

CAM_STATUS 
Ranger3::setCalibration_inPC(const String^ filePath, const bool toDoRectify, const int rectificationWidth, const int threads_number, const int runMode)
{
	pin_ptr<const wchar_t> wpStrPN = PtrToStringChars(filePath);
	auto strPN = wchart2string(wpStrPN);

	return static_cast<CAM_STATUS>(mp_R3->setCalibration_inPC(strPN, toDoRectify, static_cast<uint32_t>(rectificationWidth), threads_number, runMode));
}

String^
Ranger3::getCalibraPath()
{
	auto str = mp_R3->getCalibraPath();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

CAM_STATUS
Ranger3::setCalibraPath(const String^ value)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(value);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->setCalibraPath(str));
}


#endif

CAM_STATUS 
Ranger3::exportConfiguretionFile(const String ^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->exportConfiguretionFile(str));
}

CAM_STATUS
Ranger3::saveParameterToCSV(const String ^ path)
{
	return exportConfiguretionFile(path);
}

CAM_STATUS 
Ranger3::importConfiguretionFile(const String ^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->importConfiguretionFile(str));
}

CAM_STATUS
Ranger3::loadParameterFrCSV(const String ^ path)
{
	return importConfiguretionFile(path);
}

bool 
Ranger3::updateParamtersInfo()
{
	auto ok1 = mp_R3->getAllParameterInfo(*mp_cate, *mp_map_Cata_ParamStr, true); // force to update all parameters
	auto ok2 = mp_R3->getAllParameterInfo(*mp_map_allPramPtr);
	return ok1 == ok2 && ok1 == SickCam::CAM_STATUS::All_OK;
}

array<String^>^
Ranger3::paramCategoryList()
{
	array<String^>^ arrCate = gcnew array<String^>((*mp_cate).size());
	for (int i = 0; i<(*mp_cate).size(); ++i)
	{
		arrCate[i] = gcnew String(string2wstring((*mp_cate)[i]).c_str());
	}

	return arrCate;
}

array<String^>^ 
Ranger3::paramFullNameOfCategory(const String ^ categoryName)
{
	if (mp_map_Cata_ParamStr == nullptr)
		return gcnew array<String^>(0);

	auto cataName = StringRef2string(categoryName);

	if (mp_map_Cata_ParamStr->count(cataName) != 1)
		return gcnew array<String^>(0);

	auto paramPtr = mp_map_Cata_ParamStr->at(cataName);

	array<String^>^ arrStr = gcnew array<String^>(paramPtr.size());
	for (int i = 0; i<paramPtr.size(); ++i)
	{
		arrStr[i] = gcnew String(string2wstring(paramPtr[i]->getCsvName()).c_str());
	}

	return arrStr;
}

array<String^>^ 
Ranger3::paramFullNameList()
{
	auto paramPtr = mp_map_allPramPtr;

	array<String^>^ arrStr = gcnew array<String^>(mp_map_allPramPtr->size());
	int i = 0; 
	for (auto sub : (*mp_map_allPramPtr))
	{
		arrStr[i] = gcnew String(string2wstring(sub.second->getCsvName()).c_str());
		++i;
	}

	return arrStr;
}

String ^ 
Ranger3::paramCurrentValue(const String ^ paramFullName)
{
	if (mp_map_allPramPtr == nullptr)
		return gcnew String(std::wstring().c_str());

	auto fullName = StringRef2string(paramFullName);

	if (mp_map_allPramPtr->count(fullName) != 1)
		return gcnew String(std::wstring().c_str());

	auto valueStr = mp_map_allPramPtr->at(fullName)->getStrValue();
	return gcnew String(string2wstring(valueStr).c_str());
}

array<String^>^ 
Ranger3::paramOptionalValues(const String ^ paramFullName, int% currentValueId)
{
	if (mp_map_allPramPtr == nullptr)
		return gcnew array<String^>(0);

	auto currentValueStr = paramCurrentValue(paramFullName);

	currentValueId = 0;

	auto fullName = StringRef2string(paramFullName);

	if (mp_map_allPramPtr->count(fullName) != 1)
		return gcnew array<String^>(0);

	auto optValus = mp_map_allPramPtr->at(fullName)->getOptions();
	array<String^>^ arrStr = gcnew array<String^>(optValus.size());
	for (int i = 0; i<optValus.size(); ++i)
	{
		arrStr[i] = gcnew String(string2wstring(optValus[i]).c_str());
		if (currentValueStr == arrStr[i])
			currentValueId = i;
	}

	return arrStr;
}

CAM_STATUS
Ranger3::sendFileToCamera(const String^ filePath)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(filePath);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->sendFileToCamera(str));
}

String ^
Ranger3::retrieveFileFromCamera()
{
	auto str = mp_R3->retrieveFileFromCamera();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

CAM_STATUS
Ranger3::deleteFileFromCamera()
{
	return static_cast<CAM_STATUS>(mp_R3->deleteFileFromCamera());
}

CAM_STATUS	
Ranger3::userSet_set_description(const int id, const String^ description)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(description);
	auto str = wchart2string(wpStr);
	return static_cast<CAM_STATUS>(mp_R3->userSet_set_description(id, str));
}

String ^
Ranger3::userSet_get_description(const int id)
{
	auto str = mp_R3->userSet_get_description(id);
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

bool Ranger3::updateFirmware(const String^ firmwarePackagePath)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(firmwarePackagePath);
	auto str = wchart2string(wpStr);
	return mp_R3->updateFirmware(str);
}



}

/**	@file	TrispectorCSharpWrapper.cpp
*
*	@brief	C# Wrapper Class of Trispector.
*
*	@attention
*	This is the C# API wrapper of C++ file trispector.cpp|h.
*
*
*	@copyright	Copyright 2016-2021 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*
*/

#include "stdafx.h"
#include "TrispectorCSharpWrapper.h"

namespace SICK3D
{
namespace TRI
{
////////////////////////////////////////////////////////////////////////////////

GenICam3D_Tri::GenICam3D_Tri(const String^ logPath,
	const String^ ctiPath,
	const bool enShow,
	const bool enWrite)
{
	pin_ptr<const wchar_t> wpStrlogPath = PtrToStringChars(logPath);
	auto strlogPath = wchart2string(wpStrlogPath);

	pin_ptr<const wchar_t> wpStrctiPath = PtrToStringChars(ctiPath);
	auto strctiPath = wchart2string(wpStrctiPath);
	gp_TriS = std::make_shared<SickCam::TrispectorShared>(strlogPath, strctiPath, enShow, enWrite);
}

GenICam3D_Tri::~GenICam3D_Tri()
{
	cout << "Release GenICam3D_Tri ----------------\n\n";
	gp_TriS->~CameraShared();
	gp_TriS.reset();
}

deviceList
	GenICam3D_Tri::getConDevList()
{
	auto devs = gp_TriS->getConDevList();
	auto res = gcnew array<String^>(devs.size());
	int i = 0;
	for (auto sub : devs)
	{
		std::wstring ws = string2wstring(sub.first);
		res[i++] = gcnew String(ws.c_str());
	}
	return res;
}

deviceList GenICam3D_Tri::getConDevListSN()
{
	auto devs = gp_TriS->getConDevListSN();
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
	GenICam3D_Tri::_release()
{
	gp_TriS.reset();
}

String ^
	GenICam3D_Tri::getPathToTrispectorProducer()
{
	auto str = gp_TriS->getPathToTrispectorProducer();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}



////////////////////////////////////////////////////////////////////////////////

#ifndef _DISABLE_GrabberThread

GrabberThread::GrabberThread(SickCam::Trispector * pTri)
	: mp_tri(pTri), m_IsRun(false), m_IsFinishGrab(true), m_req(RequireCode::REQ_Nothing), m_sleepTime(50)
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

			if (Ecode::All_OK == mp_tri->getImageData(img) && !img.isEmpty())
			{
				// copy data
				CSImgT^ imgTable = gcnew CSImgT();
				imgTable->setData(img);

				lock l(locker_data);
				m_ImageData.push_back(imgTable);
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
			if (SickCam::CAM_STATUS::All_OK == mp_tri->getImageData(img) && !img.isEmpty())
			{

				{
					lock l(m_callback_is_finish_copy_locker);
					//std::unique_lock<std::mutex> lock(m_callback_is_finish_copy_locker);
					m_callback_is_finish_copy = false;
				}
				///auto _thread = std::make_shared<std::thread>(&Trispector::_callback_run_on_grabbed, this, &img);
				///_thread->detach();

				///m_Thread = gcnew Thread(gcnew ThreadStart(this, &(Trispector::_callback_run))); // todo ------------

				//auto th = gcnew Thread(gcnew ThreadStart(this, &Trispector::_callback_run_on_grabbed)); // todo ------------
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

	//auto th = gcnew Thread(gcnew ThreadStart(this, &(Trispector::m_callBack_function), img, m_callBack_inputs)); // todo ------------
	//th->Start();

	///auto _thread = std::make_shared<std::thread>(m_callBack_function, &img, m_callBack_inputs);
	///_thread->join(); // wait callback done, and free the img.

}


#endif

#endif

////////////////////////////////////////////////////////////////////////////////


Trispector::Trispector(const String ^ sn)
#ifndef _DISABLE_GrabberThread
	: m_enable_CPP_Grab_thread(false)
#endif
{
	pin_ptr<const wchar_t> wStr = PtrToStringChars(sn);
	std::string sSN = wchart2string(wStr);

	mp_tri = new SickCam::Trispector(gp_TriS, sSN);
	//mp_cate = new SickCam::Categories;
	//mp_map_Cata_ParamStr = new SickCam::Params;
	//mp_map_allPramPtr = new SickCam::AllParams;

#ifndef _DISABLE_GrabberThread
	mp_gt = gcnew GrabberThread(mp_tri);
	locker_GetData = gcnew Object;
	locker_Start = gcnew Object;
#endif

}

Trispector::~Trispector()
{
	if (mp_tri != nullptr)
	{
		delete mp_tri;
		mp_tri = nullptr;
	}

	//if (mp_cate != nullptr)
	//{
	//	delete mp_cate;
	//	mp_cate = nullptr;
	//}
	//
	//if (mp_map_Cata_ParamStr != nullptr)
	//{
	//	delete mp_map_Cata_ParamStr;
	//	mp_map_Cata_ParamStr = nullptr;
	//}
	//
	//if (mp_map_allPramPtr != nullptr)
	//{
	//	delete mp_map_allPramPtr;
	//	mp_map_allPramPtr = nullptr;
	//}
}

CAM_STATUS
Trispector::isReady()
{
	return static_cast<CAM_STATUS>(mp_tri->isReady());
}

CAM_STATUS
Trispector::connectCamera(const bool enable_CPP_Grab_thread)
{
#ifndef _DISABLE_GrabberThread
	m_enable_CPP_Grab_thread = enable_CPP_Grab_thread;
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_Nothing);
		m_Thread = gcnew Thread(gcnew ThreadStart(mp_gt, &(GrabberThread::run)));
		m_Thread->Start();
	}
#endif
	return static_cast<CAM_STATUS>(mp_tri->connectCamera());
}


CAM_STATUS
Trispector::startCamera()
{
#ifndef _DISABLE_GrabberThread
	lock l(locker_Start);
	//std::cout << "locker_Start" << std::endl;
	// start run 
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_Nothing);
		while (!mp_gt->isRun()) Sleep(0);
	}
#endif
	return static_cast<CAM_STATUS>(mp_tri->startCamera());
}

#ifdef _CALLBACK_ON
CAM_STATUS
	Trispector::startCamera(CallbackEvent ^ pCallback, Object ^ any)
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
Trispector::stopCamera()
{
#ifndef _DISABLE_GrabberThread
	lock l(locker_GetData);

	// stop run 
	if (m_enable_CPP_Grab_thread && mp_tri->getStatus() == SickCam::CAM_STATUS::CAM_IS_STARTED)
	{
		mp_gt->sendRequest(RequireCode::REQ_STOP_CAMERA);
		while (!mp_gt->isFinishGrab()) Sleep(0);
	}
#endif

	return static_cast<CAM_STATUS>(mp_tri->stopCamera());
}

CAM_STATUS
Trispector::disconnectCamera()
{
#ifndef _DISABLE_GrabberThread
	if (m_enable_CPP_Grab_thread)
	{
		mp_gt->sendRequest(RequireCode::REQ_QUIT_RUN);
		while (mp_gt->isRun()) Sleep(0);
	}
#endif
	auto status = mp_tri->disconnectCamera();
	return static_cast<CAM_STATUS>(status);
}

CAM_STATUS
Trispector::getImageData(CSImgT^ imgTable)
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
		if (mp_tri->getStatus() != SickCam::CAM_STATUS::CAM_IS_STARTED)
			return CAM_STATUS::ERROR_CAM_NOT_START;

		SickCam::ImgT img;
		auto status = mp_tri->getImageData(img);
		if(status == SickCam::CAM_STATUS::All_OK)
			imgTable->setData(img);
		return static_cast<CAM_STATUS>(status);
	}
}

CAM_STATUS Trispector::setGrabSleepTime(const unsigned int sleepTime)
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

unsigned int Trispector::getGrabSleepTime()
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

String ^
	Trispector::getDeviceName()
{
	auto str = mp_tri->getDeviceName();
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}
}
}

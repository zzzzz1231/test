/**	@file	ImgTCSharpWrapper.h
*
*	@brief	C# Wrapper Class of ImgT.
*
*	@attention
*	This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
*
*
*	@copyright	Copyright 2016-2023 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*
*/

#include "stdafx.h"
#include "ImgTCSharpWrapper.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace SICK3D
{

CSImgT::CSImgT()
{
	mp_ImgT = new SickCam::ImgT();
}

CSImgT::~CSImgT()
{
	delete mp_ImgT;
}

CSImgT::CSImgT(const CSImgT^ img)
{
	_deepCopy(img);
}

CSImgT::CSImgT(const CSImgT% img)
{
	_deepCopy(%img);
}

//CSImgT::CSImgT(const CSImgT img)
//{
//	_deepCopy(img);
//}

CSImgT^
CSImgT::operator=(const CSImgT^ img)
{
	if (*this == img)
		return this;

	_deepCopy(img);
	return this;
}

bool 
CSImgT::operator==(const CSImgT^ img)
{
	return *this->mp_ImgT == *img->mp_ImgT;
}

bool 
CSImgT::operator!=(const CSImgT^ img)
{
	return !(*this->mp_ImgT == *img->mp_ImgT);
}



String^ 
CSImgT::DN2Str(const DN _dn)
{
	auto str = mp_ImgT->DN2Str(static_cast<SickCam::DN>(_dn));
	auto wstr = string2wstring(str);
	return gcnew String(wstr.c_str());
}

DN 
CSImgT::Str2DN(String^ _str)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(_str);
	auto str = wchart2string(wpStr);
	auto dn = mp_ImgT->Str2DN(str);
	return static_cast<DN>(dn);
}

array<uint8_t>^ 
CSImgT::getDataByte(const DN _dn)
{
	auto dn = static_cast<SickCam::DN>(_dn);
	uint8_t* data = (uint8_t*)mp_ImgT->getData(dn);
	auto cols = mp_ImgT->get_cols(dn);
	//auto cols = mp_ImgT->get_rows(dn);
	if (_dn == DN::WHITE || _dn == DN::R || _dn == DN::G || _dn == DN::B)
	{
		cols = 5120;
	}

	auto rows = mp_ImgT->get_rows(dn);

	array<uint8_t>^ res = gcnew array<uint8_t>(cols * rows);

	System::Runtime::InteropServices::Marshal::Copy((IntPtr)data, res, 0, (int)(cols * rows));
	//for (size_t i = 0; i < cols * rows; ++i)
	//{
	//	res[i] = data[i];
	//}
	return res;
}

array<uint16_t>^
CSImgT::getDataWord(const DN _dn)
{
	auto dn = static_cast<SickCam::DN>(_dn);
	uint16_t* data = (uint16_t*)mp_ImgT->getData(dn);
	auto cols = mp_ImgT->get_cols(dn);
	auto rows = mp_ImgT->get_rows(dn);

	array<uint16_t>^ res = gcnew array<uint16_t>(cols * rows);

	//Marshal::Copy((IntPtr)data, res, 0, (int)(cols * rows));
	for (size_t i = 0; i < cols * rows; ++i)
	{
		res[i] = data[i];
	}
	return res;
}

array<float>^
CSImgT::getDataFloat(const DN _dn)
{
	auto dn = static_cast<SickCam::DN>(_dn);
	float* data = (float*)mp_ImgT->getData(dn);
	auto cols = mp_ImgT->get_cols(dn);
	auto rows = mp_ImgT->get_rows(dn);

	array<float>^ res = gcnew array<float>(cols * rows);

	// https://learn.microsoft.com/zh-cn/cpp/dotnet/how-to-load-unmanaged-resources-into-a-byte-array?view=msvc-170
	System::Runtime::InteropServices::Marshal::Copy((IntPtr)data, res, 0, (int)(cols * rows));

	//for (size_t i = 0; i < cols * rows; ++i)
	//{
	//	res[i] = data[i];
	//}
	return res;
}

array<uint32_t>^ CSImgT::getDataMark()
{
	uint32_t* data = (uint32_t*)mp_ImgT->getData(SickCam::DN::MAR);
	auto cols = CHUNK_DATA_WIDTH;
	auto rows = mp_ImgT->get_rows(SickCam::DN::MAR);

	array<uint32_t>^ res = gcnew array<uint32_t>(cols * rows);

	for (size_t i = 0; i < cols * rows; ++i)
	{
		res[i] = data[i];
	}
	return res;
}

array<DN>^
CSImgT::getAvalibleDataNames()
{
	auto adn = mp_ImgT->getAvalibleDataNames();
	array<DN>^ res = gcnew array<DN>(adn.size());
	for (size_t i=0; i<adn.size(); ++i)
	{
		res[i] = static_cast<DN>(adn[i]);
	}
	return res;
}

bool 
CSImgT::insertDataByteClone(const DN _name, array<uint8_t>^ _pointer, const uint64_t _id)
{
	pin_ptr<uint8_t> p = &_pointer[0];
	return mp_ImgT->insertDataCopy(static_cast<SickCam::DN>(_name), p, _id);
}

bool 
CSImgT::insertDataWordClone(const DN _name, array<uint16_t>^ _pointer, const uint64_t _id)
{
	pin_ptr<uint16_t> p = &_pointer[0];
	return mp_ImgT->insertDataCopy(static_cast<SickCam::DN>(_name), (uint8_t*)p, _id);
}

bool 
CSImgT::insertDataFloatClone(const DN _name, array<float>^ _pointer, const uint64_t _id)
{
	pin_ptr<float> p = &_pointer[0];
	return mp_ImgT->insertDataCopy(static_cast<SickCam::DN>(_name), (uint8_t*)p, _id);
}



#ifndef DISABLE_CAL_IN_PC

//
//bool 
//CSImgT::InitCalibration(const int handle, const String^ calibrationFile, const double rectificationSpread, 
//	const int rawRangeWidth, const int rectificationWidth, const int rectMethod)
//{
//	pin_ptr<const wchar_t> wpStr = PtrToStringChars(calibrationFile);
//	auto spath = wchart2string(wpStr);
//
//	return mp_ImgT->InitCalibration(handle, spath, rectificationSpread, rawRangeWidth, rectificationWidth, SiCaliWrapper::RectMethod(rectMethod));
//}

bool CSImgT::DeleteCalibration(const int handle)
{
	return SickCam::ImgT::DeleteCalibration(handle);
}

CSImgT^ 
CSImgT::ApplyCalibration(
	const int calibrationHandle,
	const String^ calibrationFile,
	const int removeLastNRows,
	bool doRectify,
	const double rectificationSpread,
	const int rawRangeWidth,
	const int rectificationWidth,
	const int rectMethod,
	const int numberOfThreads
)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(calibrationFile);
	auto spath = wchart2string(wpStr);

	auto imgt = mp_ImgT->ApplyCalibration(
		calibrationHandle,
		spath,
		removeLastNRows,
		doRectify,
		rectificationSpread,
		rawRangeWidth,
		rectificationWidth,
		SiCaliWrapper::RectMethod(rectMethod)
	);
	CSImgT^ csImg = gcnew CSImgT();
	csImg->setData(imgt);
	return csImg;
}

//
//bool CSImgT::InitCalibrationColor(const int handle, const String^ calibrationFile, 
//	const String^ colorFile, const double rectificationSpread, 
//	const int rawColoreWidth, const int rectificationWidth, const int rectMethod)
//{
//	pin_ptr<const wchar_t> wpStr = PtrToStringChars(calibrationFile);
//	auto spath = wchart2string(wpStr);
//
//	pin_ptr<const wchar_t> wpStrColor = PtrToStringChars(colorFile);
//	auto spathColor = wchart2string(wpStrColor);
//
//
//	return mp_ImgT->InitCalibrationColor(handle, spath, spathColor, rectificationSpread, rawColoreWidth, 
//		rectificationWidth, SiCaliWrapper::RectMethod(rectMethod));
//
//}

bool CSImgT::DeleteCalibrationColor(const int handle)
{
	return SickCam::ImgT::DeleteCalibrationColor(handle);
}

CSImgT^ CSImgT::ApplyCalibrationColor(
	const int colorCalibrateAlignHandle,
	const String^ calibrationFile,
	const String^ colorFile,
	const int removeLastNRows,
	bool  doFCC,
	const int colorInterpolationStride,
	const double rectificationSpread,
	const int rawColoreResizedWidth,
	const int rectificationWidth,
	const int rectMethod,
	const int removeHeadNRows,
	bool doCalibration,
	const int numberOfThreads
)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(calibrationFile);
	auto spath = wchart2string(wpStr);
	
	pin_ptr<const wchar_t> wpStrColor = PtrToStringChars(colorFile);
	auto spathColor = wchart2string(wpStrColor);


	auto imgt = mp_ImgT->ApplyCalibrationColor(
		colorCalibrateAlignHandle, 
		spath,
		spathColor,
		removeLastNRows,
		doFCC,
		colorInterpolationStride,
		rectificationSpread, rawColoreResizedWidth, rectificationWidth, SiCaliWrapper::RectMethod(rectMethod),
		removeHeadNRows, doCalibration, numberOfThreads
	);
	CSImgT^ csImg = gcnew CSImgT();
	csImg->setData(imgt);
	return csImg;
}

void CSImgT::Append(CSImgT^ src)
{
	//auto imgt = mp_ImgT->Append(*(src->mp_ImgT));
	mp_ImgT->Append(*(src->mp_ImgT));
	//CSImgT^ csImg = gcnew CSImgT();
	//csImg->setData(imgt);
	//return csImg;
}

void CSImgT::Divide(const int parts, array<CSImgT^>^ imageArray)
{
	auto imgt = mp_ImgT->Divide(parts);
	array<CSImgT^>^ res = gcnew array<CSImgT^>(parts);

	for (int i=0; i<parts; ++i)
	{
		CSImgT^ csImg = gcnew CSImgT();
		csImg->setData(imgt[i]);
		res[i] = csImg;
	}

	throw res;
}

#endif



bool
CSImgT::loadFromIconFile(const String^ path, const double resolutonY, const double resolutonZ, const bool convertRangeToFloat)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->loadFromIconFile(spath, resolutonY, resolutonZ, convertRangeToFloat);
}

bool 
CSImgT::SaveToIconFile(const String^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	mp_ImgT->SaveToIconFile(spath, std::vector<SickCam::DN>{ SickCam::DN::RAN_CAL, SickCam::DN::REF_CAL, SickCam::DN::SCA_8_CAL, SickCam::DN::MAR });
	mp_ImgT->SaveToIconFile(spath+"_cal", std::vector<SickCam::DN>{ SickCam::DN::SEN, SickCam::DN::RAN, SickCam::DN::REF, SickCam::DN::SCA_8, SickCam::DN::MAR });
	return true;
}

bool 
CSImgT::SaveToIconFile(const String^ path, array<DN>^ components, bool is_calibrated)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);

	std::vector<SickCam::DN> com;
	for (int i =0; i<components->Length; ++i )
	{
		com.emplace_back(static_cast<SickCam::DN>(components[i]));
	}

	return mp_ImgT->SaveToIconFile(spath, com, is_calibrated);
}

bool 
CSImgT::SaveSensorImageToIconFile(const String ^ path, bool byFStream)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->SaveSensorImageToIconFile(spath, byFStream);
}

bool 
CSImgT::SaveRawImagesToIconFile(const String ^ path, bool withChunkData, bool byFStream)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->SaveRawImagesToIconFile(spath, withChunkData, byFStream);
}

bool 
CSImgT::SaveCalibratedImagesToIconFile(const String ^ path, bool withChunkData, bool byFStream)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->SaveCalibratedImagesToIconFile(spath, withChunkData, byFStream);
}

bool 
CSImgT::SaveCalibratedDataToPointCloud(const String^ path)
{
	pin_ptr<const wchar_t> wpStr = PtrToStringChars(path);
	auto spath = wchart2string(wpStr);
	return mp_ImgT->SaveCalibratedDataToPointCloud(spath);
}

bool
CSImgT::setData(const SickCam::ImgT & p)
{
	*mp_ImgT = p;
	return true;
}

void 
CSImgT::_deepCopy(const CSImgT^ img)
{
	delete this->mp_ImgT;

	this->mp_ImgT = new SickCam::ImgT();
	*this->mp_ImgT = *img->mp_ImgT;
}


}

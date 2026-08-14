// Version info : 2026 / 5 / 13


#include <cmath>
#include "ImageTable.h"

namespace SickCam
{

	const std::map<DN, const std::string>
		ImgT::DN_StrName = {
			{ DN::INV			, "INVALID" },
			{ DN::SEN			, "SENSOR" },

			{ DN::RAN			, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"			},
			{ DN::REF			, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"		},
			{ DN::SCA_8			, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1" },
			{ DN::SCA_16		, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1" },
			{ DN::MAR			, "Encoder_information"			},
			{ DN::RAN_CAL		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"		},
			{ DN::RAN_CAL_16	, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL_16"		},
			{ DN::REF_CAL		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"	},
			{ DN::REF_CAL_F		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F"},
			{ DN::SCA_8_CAL		, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL" },
			{ DN::SCA_16_CAL	, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"},

			{ DN::RAN2			, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"			},
			{ DN::REF2			, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"		},
			{ DN::SCA2_8		, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2" },
			{ DN::SCA2_16		, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"	},
			{ DN::RAN2_CAL		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"		},
			{ DN::REF2_CAL		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"	},
			{ DN::SCA2_8_CAL	, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL" },
			{ DN::SCA2_16_CAL	, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"},

			{ DN::RAN_X			, "RangeX_Calibration_Only_Without_Rectification"		},
			{ DN::RAN_X_16		, "16_Byte_RangeX_Calibration_Only_Without_Rectification"		},


			{ DN::WHITE	, "White"		},
			{ DN::R		, "R"		},
			{ DN::G		, "G"		},
			{ DN::B		, "B"		},
	};

	const std::map<const std::string, DataNames>
		ImgT::StrName_DN = {
			{ "INVALID"																	, DN::INV		},
			{ "SENSOR"																	, DN::SEN		},

			{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"				, DN::RAN		},
			{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"			, DN::REF		},
			{ "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"		, DN::SCA_8		},
			{ "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"		, DN::SCA_16	},
			{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"			, DN::RAN_CAL	},
			{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL_16"			, DN::RAN_CAL_16},
			{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"		, DN::REF_CAL	},
			{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F"	, DN::REF_CAL_F },
			{ "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"	, DN::SCA_8_CAL	},
			{ "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"	, DN::SCA_16_CAL},

			{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"				, DN::RAN2		},
			{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"			, DN::REF2		},
			{ "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"		, DN::SCA2_8	},
			{ "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"		, DN::SCA2_16	},
			{ "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"			, DN::RAN2_CAL	},
			{ "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"		, DN::REF2_CAL	},
			{ "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"	, DN::SCA2_8_CAL },
			{ "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"	, DN::SCA2_16_CAL},

			{ "RangeX_Calibration_Only_Without_Rectification"							, DN::RAN_X		},
			{ "16_Byte_RangeX_Calibration_Only_Without_Rectification"					, DN::RAN_X_16	},



			{ "White",  DN::WHITE	},
			{ "R"	 ,  DN::R		},
			{ "G"	 ,  DN::G		},
			{ "B"	 ,  DN::B		},
	};

	const std::string
		ImgT::DN2Str(const DN& _dn)
	{
		if (DN_StrName.count(_dn) == 1)
			return DN_StrName.at(_dn);
		return std::string();
	}

	DN
		ImgT::Str2DN(const std::string& _str)
	{
		if (StrName_DN.count(_str) == 1)
			return StrName_DN.at(_str);
		return DN::INV;
	}

	//#endif

	//////////////////////////////// ImgT function definitions ///////////////////////////////////////

	ImgT::ImgT()
		: m_Data(ImgTData()), m_type(TYPE::INVALID)
	{}

	ImgT::ImgT(const std::string& path, const double& scaleY)
	{
		loadFromIconFile(path, scaleY);
	}

	ImgT::ImgT(const ImgT& img)
		: m_Data(ImgTData())
	{
		__doapl(this, img);
	}

	ImgT&
		ImgT::operator=(const ImgT& img)
	{
		if (*this == img)
			return *this;

		return __doapl(this, img);
	}

	bool
		ImgT::operator==(const ImgT& v)
	{
		bool isSame(this->m_info == v.m_info);
		//isSame &= (this->m_info.);
		if (!isSame)
			return false;

		for (auto sub : this->m_Data)
		{
			if (v.m_Data.count(sub.first) == 0)
				return false;

			isSame &= _compareBuffer(v, sub.first);
		}
		return  isSame;
	}

	ImgT::~ImgT()
	{
		bool isOK = _clearBuffer(*this);

		//#ifndef DISABLE_CAL_IN_PC
		//	//for (int i=0; i< 255; ++i)
		//	//{
		//	//	if (m_CaliPtr[i] != nullptr)
		//	//	{
		//	//		delete m_CaliPtr[i];
		//	//		m_CaliPtr[i] = nullptr;
		//	//	}
		//
		//	//	if (m_CaliColorPtr[i] != nullptr)
		//	//	{
		//	//		delete m_CaliColorPtr[i];
		//	//		m_CaliColorPtr[i] = nullptr;
		//	//	}
		//
		//	//	if (m_ColorParam[i] != nullptr)
		//	//	{
		//	//		delete m_ColorParam[i];
		//	//		m_ColorParam[i] = nullptr;
		//	//	}
		//	//}
		//#endif

	}

	void*
		ImgT::getData(const DN& _dn) const
	{
		if (m_Data.count(_dn) == 1)
			return m_Data.at(_dn);
		return nullptr;
	}

	std::vector<DN>
		ImgT::getAvalibleDataNames() const
	{
		std::vector<DN> vec;
		for (auto sub : getData())		vec.push_back(sub.first);
		return vec;
	}

	//std::vector<DN>
	//	ImgT::getAvalibleDataNamesRaw() const
	//{
	//	std::vector<DN> vec;
	//	for (auto sub : getData())
	//	{
	//		if (sub.first <= DN::MAR)
	//			vec.push_back(sub.first);
	//	}
	//	return vec;
	//}

	//std::vector<DN>
	//	ImgT::getAvalibleDataNamesCal() const
	//{
	//	std::vector<DN> vec;
	//	for (auto sub : getData())
	//	{
	//		if (DN::MAR <= sub.first && sub.first <= DN::SCA_16_CAL)
	//			vec.push_back(sub.first);
	//	}
	//	return vec;
	//}

	//const uint32_t& 
	//	ImgT::get_cols(DataNames _dn) const
	//{
	//	// TODO: insert return statement here
	//	if (m_type == TYPE::SENSOR)
	//	{
	//		return m_info.m_SI.cols;
	//	}
	//	else if (m_type == TYPE::RANGE3D)
	//	{
	//		switch (_dn)
	//		{
	//		case SickCam::INV:
	//			return 0;
	//			break;
	//		case SickCam::SEN:
	//			return 0;
	//			break;
	//		case SickCam::RAN:
	//		case SickCam::REF:
	//		case SickCam::SCA_8:
	//		case SickCam::SCA_16:
	//			return m_info.m_RI.rows;
	//		case SickCam::MAR:
	//			return 5;
	//		case SickCam::RAN_CAL:
	//		case SickCam::RAN_CAL_16:
	//		case SickCam::REF_CAL:
	//		case SickCam::REF_CAL_F:
	//		case SickCam::SCA_8_CAL:
	//		case SickCam::SCA_16_CAL:
	//			return m_info.m_RI.rows; // 前面的程序会将这个参数更新到 RectificationWidth
	//		case SickCam::RAN2:
	//		case SickCam::REF2:
	//		case SickCam::SCA2_8:
	//		case SickCam::SCA2_16:
	//			return m_info.m_RI.rows;
	//		case SickCam::RAN2_CAL:
	//		case SickCam::REF2_CAL:
	//		case SickCam::SCA2_8_CAL:
	//		case SickCam::SCA2_16_CAL:
	//			return m_info.m_RI.rows; // 前面的程序会将这个参数更新到 RectificationWidth
	//		case SickCam::RAN_X:
	//		case SickCam::RAN_X_16:
	//			return m_info.m_RI.rows;
	//		case SickCam::WHITE:
	//		case SickCam::R:
	//		case SickCam::G:
	//		case SickCam::B:
	//			return m_info.m_RI.rows;
	//		default:
	//			return 0;
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		return 0;
	//	}
	//}

	const uint32_t&
		ImgT::get_cols() const
	{
		switch (m_type)
		{
		case TYPE::SENSOR:			return m_info.m_SI.cols;
		case TYPE::RANGE3D:			return m_info.m_RI.cols;
		}
		return m_info.m_RI.cols;
	}


	const uint32_t&
		ImgT::get_rows() const
	{
		switch (m_type)
		{
		case TYPE::SENSOR:				return m_info.m_SI.rows;
		case TYPE::RANGE3D:				return m_info.m_RI.rows;
		}
		return m_info.m_RI.rows;
	}


	bool
		ImgT::insertDataCopy(const DN& _name,
			const uint8_t* _pointer,
			const uint64_t& _id, const int& _offset)
	{
		if (_pointer == nullptr)
			return false;

		size_t byteSize = get_rows() * (_name == SickCam::MAR ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(_name));
		size_t offsetSize = _offset * (_name == SickCam::MAR ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(_name));

		// TODO: 高精度灰度和彩色图像是 2560*2=5120 宽度的。
		if (_name == SickCam::WHITE || _name == SickCam::R || _name == SickCam::G || _name == SickCam::B)
		{
			byteSize = get_rows() * 5120 * _DN2PixelSize(_name);
			offsetSize = _offset * 5120 * _DN2PixelSize(_name);
		}


		if (m_Data.count(_name) != 0)
		{
			delete[](uint8_t*)m_Data.at(_name);
			m_Data.erase(_name);
		}

		uint8_t* pd = new uint8_t[byteSize];
		std::memcpy(pd, _pointer + offsetSize, byteSize);
		m_Data.insert({ _name, pd });
		m_info.m_id = _id;

		return true;
	}

	bool
		ImgT::setSensorInfo(const uint32_t& _c, const uint32_t& _r, const uint32_t& _ox, const uint32_t& _oy)
	{
		m_type = TYPE::SENSOR;
		m_info.m_SI.cols = _c;
		m_info.m_SI.rows = _r;
		m_info.m_SI.senOffsetX = _ox;
		m_info.m_SI.senOffsetY = _oy;
		return true;
	}

	bool
		ImgT::setRangeInfo(const uint32_t& _c, const uint32_t& _r, const uint32_t& _aox, const uint32_t& _aoy,
			const uint32_t& _ah, const uint32_t& _aw, const RangeAxis& _rAxis)
	{
		m_type = TYPE::RANGE3D;
		m_info.m_RI.cols = _c;
		m_info.m_RI.rows = _r;
		m_info.m_RI.aoiOffsetX = _aox;
		m_info.m_RI.aoiOffsetY = _aoy;
		m_info.m_RI.aoiHeight = _ah;
		m_info.m_RI.aoiWidth = _aw;
		m_info.m_RI.m_RangeAxis = _rAxis;
		m_info.m_RI.fillXML_byDeviceParameters();
		return true;
	}


	bool
		ImgT::setCaliInfo(const CalibInfo& _c)
	{
		m_type = TYPE::RANGE3D;
		m_info.m_RI.cols = _c.genistreamtraits.width;
		m_info.m_RI.rows = _c.genistreamtraits.height;

		m_info.m_CI = _c;
		return true;
	}


	bool
		ImgT::setCaliInfo(const uint32_t& _c, const uint32_t& _r, const double& _offsetX, const double& _offsetY, const double& _offsetZ,
			const double& _scaleX, const double& _scaleY, const double& _scaleZ,
			const double& _lower_bound_x, const double& _upper_bound_x, const double& _lower_bound_r, const double& _upper_bound_r,
			const double& _a_axis_range_scale, const double& _a_axis_range_offset, const double& _c_axis_range_scale, const double& _c_axis_range_offset)
	{
		m_type = TYPE::RANGE3D;
		m_info.m_RI.cols = _c; // 这里的宽度要统一
		m_info.m_RI.rows = _r;

		m_info.m_CI.offsetX = _offsetX;
		m_info.m_CI.offsetY = _offsetY;
		m_info.m_CI.offsetZ = _offsetZ;
		m_info.m_CI.scaleX = _scaleX;
		m_info.m_CI.scaleY = _scaleY;
		m_info.m_CI.scaleZ = _scaleZ;
		m_info.m_CI.lower_bound_x = _lower_bound_x;
		m_info.m_CI.upper_bound_x = _upper_bound_x;
		m_info.m_CI.lower_bound_r = _lower_bound_r;
		m_info.m_CI.upper_bound_r = _upper_bound_r;

		m_info.m_CI.genistreamtraits.a_axis_range_scale = _a_axis_range_scale;
		m_info.m_CI.genistreamtraits.a_axis_range_offset = _a_axis_range_offset;
		m_info.m_CI.genistreamtraits.c_axis_range_scale = _c_axis_range_scale;
		m_info.m_CI.genistreamtraits.c_axis_range_offset = _c_axis_range_offset;

		return true;
	}

	bool
		ImgT::setChunkInfo(const ChunkData& _Chunk, const uint64_t& _id)
	{
		if (_Chunk.empty()) return false;
		m_info.m_ChunkData = _Chunk;
		if (has_ChunkInfo())
		{
			uint32_t* pChunkData = new uint32_t[CHUNK_DATA_WIDTH * m_info.m_ChunkData.size()];

			//#pragma omp parallel for
			for (int j = 0; j < m_info.m_ChunkData.size(); ++j)
			{
				auto cline = j * CHUNK_DATA_WIDTH;

				uint32_t statusBits = 0;
				statusBits |= (static_cast<uint32_t>(m_info.m_ChunkData[j].overtriggerCount) << STATUS_BIT_OVERTRIG);
				statusBits |= (static_cast<uint32_t>(m_info.m_ChunkData[j].encoderResetActive) << STATUS_BIT_ENCODER_RESET);
				statusBits |= (static_cast<uint32_t>(m_info.m_ChunkData[j].lineTriggerActive) << STATUS_BIT_LINE_TRIGGER);
				statusBits |= (static_cast<uint32_t>(m_info.m_ChunkData[j].encoderA) << STATUS_BIT_ENCODER_A);
				statusBits |= (static_cast<uint32_t>(m_info.m_ChunkData[j].encoderB) << STATUS_BIT_ENCODER_B);
				statusBits |= (static_cast<uint32_t>(m_info.m_ChunkData[j].frameTriggerActive) << STATUS_BIT_ENABLE);

				pChunkData[cline + 0] = static_cast<uint32_t>(m_info.m_ChunkData[j].encoderValue);
				pChunkData[cline + 1] = statusBits;
				pChunkData[cline + 2] = static_cast<uint32_t>(m_info.m_ChunkData[j].timestamp & 0x00000000ffffffff); // Low
				pChunkData[cline + 3] = static_cast<uint32_t>((m_info.m_ChunkData[j].timestamp & 0xffffffff00000000) >> 32); // High
				pChunkData[cline + 4] = static_cast<uint32_t>(0);
			}

			bool is_ok = insertDataCopy(DN::MAR, (uint8_t*)pChunkData, _id);
			delete[] pChunkData;
			return is_ok;
		}
		return false;
	}

	void
		ImgT::printInfo() const
	{
		std::cout << collectInfo().c_str();
	}

	std::string
		ImgT::collectInfo() const
	{
		std::stringstream ss;
		ss << "\r\n\r\n";
		ss << "Image_id_________: " << std::to_string(get_ID()) << "\r\n";
		ss << "Image_isEmpty____: " << (isEmpty() ? "1" : "0") << "\r\n";
		ss << "Image_Number_____: " << m_Data.size() << "\r\n";
		for (auto sub : m_Data)
		{
			ss << "*_____Image_Name_: " << DN2Str(sub.first) << "\r\n";
		}
		ss << "Sensor_Image_____: " << (has(DN::SEN) ? "1" : "0") << "\r\n";
		ss << "Chunk_Data_______: " << (has_ChunkInfo() ? "1" : "0") << "\r\n";
		ss << "Size_Sen(c,_r)___: " << m_info.m_SI.cols << ", " << m_info.m_SI.rows << "\r\n";
		ss << "Size_Ran(c,_r)___: " << m_info.m_RI.cols << ", " << m_info.m_RI.rows << "\r\n";
		ss << "Size_Cal(c,_r)___: " << m_info.m_RI.cols << ", " << m_info.m_RI.rows << "\r\n";
		ss << "offsetX__________: " << m_info.m_CI.offsetX << "\r\n";
		ss << "offsetY__________: " << m_info.m_CI.offsetY << "\r\n";
		ss << "scaleX___________: " << m_info.m_CI.scaleX << "\r\n";
		ss << "scaleY___________: " << m_info.m_CI.scaleY << "\r\n";
		ss << "lower_bound_x____: " << m_info.m_CI.lower_bound_x << "\r\n";
		ss << "upper_bound_x____: " << m_info.m_CI.upper_bound_x << "\r\n";
		ss << "lower_bound_r____: " << m_info.m_CI.lower_bound_r << "\r\n";
		ss << "upper_bound_r____: " << m_info.m_CI.upper_bound_r << "\r\n";

		return ss.str();
	}








#ifndef DISABLE_CAL_IN_PC
#ifndef EZR_EXPORTS


	SiCaliWrapper::CalibrationWrapper* ImgT::m_CaliPtr[HANDLE_NUM] = { nullptr };
	int ImgT::m_RawImageWidth[HANDLE_NUM] = { 0 };
	int ImgT::m_CaliRectWidth[HANDLE_NUM] = { 0 };


#ifndef DISABLE_R380 // RANGER380
	SiCaliWrapper::CalibrationWrapper* ImgT::m_CaliColorPtr[HANDLE_NUM] = { nullptr };
	int ImgT::m_RawImageColorWidth[HANDLE_NUM] = { 0 };
	int ImgT::m_CaliColorRectWidth[HANDLE_NUM] = { 0 };
	color_image_run::colorParam* ImgT::m_ColorParam[HANDLE_NUM] = { nullptr };

#endif

	bool
		ImgT::_InitCalibration(
			const int& handle,
			const std::string& calibrationFile,
			const double& rectificationSpread,
			const int& rawRangeWidth,
			const int& rectificationWidth,
			const SiCaliWrapper::RectMethod& rectMethod,
			const int& numberOfThreads)
	{
		m_RawImageWidth[handle] = rawRangeWidth;
		m_CaliRectWidth[handle] = rectificationWidth;

		if (m_CaliPtr[handle] != nullptr)
			delete m_CaliPtr[handle];

		m_CaliPtr[handle] = new SiCaliWrapper::CalibrationWrapper();

		if (false == m_CaliPtr[handle]->initCalibrationFilter(
			calibrationFile, // json file recommended
			//get_cols(), // raw image width
			m_RawImageWidth[handle], // raw image width
			get_rows(), // image height (rows)
			m_info.m_RI.aoiWidth, // aoi width
			m_info.m_RI.aoiHeight,  // aoi height
			1.0,
			m_info.m_RI.aoiOffsetX, // region1, offsetx
			m_info.m_RI.m_RangeAxis == RangeAxis::STANDARD ? 0.0625f : -0.0625f,
			(float)(m_info.m_RI.m_RangeAxis == RangeAxis::STANDARD ?
				m_info.m_RI.aoiOffsetY : m_info.m_RI.aoiOffsetY + m_info.m_RI.aoiHeight),
			has(DataNames::SCA_8) ? 8 : (has(DataNames::SCA_16) ? 16 : 0),
			rectMethod,
			m_CaliRectWidth[handle],
			MISSING_DATA,
			true,
			false,
			rectificationSpread
		)) {
			std::cout << "initCalibrationFilter failed!" << std::endl;
			return false;
		}

#ifndef __aarch64__
		if (numberOfThreads <= 0)
		{
			auto numberOfThread = m_CaliPtr[handle]->get_NumberOfThreads_jsonOnly_X64() / 2; // 将标定的线程数，设定为 CPU 核数的一半，为了不让 CPU 使用率达到 100% 。
			m_CaliPtr[handle]->set_NumberOfThreads_jsonOnly_X64(numberOfThread);
		}
		else
		{
			m_CaliPtr[handle]->set_NumberOfThreads_jsonOnly_X64(numberOfThreads);
		}
#endif // !__aarch64__

		return true;
	}


	bool
		ImgT::DeleteCalibration(const int& handle)
	{
		if (m_CaliPtr[handle] != nullptr)
		{
			delete m_CaliPtr[handle];
			m_CaliPtr[handle] = nullptr;
		}

		return true;
	}

	ImgT
		ImgT::ApplyCalibration(const int& calibrationHandle, const std::string& calibrationFile,
			const int& removeLastNRows,
			bool doRectify, 
			const double& rectificationSpread,
			const int& rawRangeWidth, const int& rectificationWidth,
			const SiCaliWrapper::RectMethod& rectMethod,
			const int& numberOfThreads)
	{

		if (isEmpty() || (calibrationHandle < 0 || 255 < calibrationHandle))
		{
			return ImgT();
		}

		if (m_CaliPtr[calibrationHandle] == nullptr)
		{
			_InitCalibration(calibrationHandle, calibrationFile, rectificationSpread, rawRangeWidth, rectificationWidth, rectMethod, numberOfThreads);
		}

		//return _ApplyCalibration(calibrationHandle, doRectify);
		ImgT imgT = ImgT();
		if (isEmpty() || (calibrationHandle < 0 || 255 < calibrationHandle) ||
			m_CaliPtr[calibrationHandle] == nullptr || m_CaliRectWidth[calibrationHandle] == 0
			)
		{
			return imgT;
		}


		// -- check Range
		uint8_t* pRange = nullptr;
		if (has(DataNames::RAN))
			pRange = (uint8_t*)getData(DataNames::RAN);

		// -- check Reflectance
		uint8_t* pReflectance = nullptr;
		if (has(DataNames::REF))
			pReflectance = (uint8_t*)getData(DataNames::REF);

		// -- check Scatter
		auto scatterSize = has(DataNames::SCA_8) ? 8 : 0;
		scatterSize = has(DataNames::SCA_16) ? 16 : scatterSize;

		uint8_t* pScatter = nullptr;
		if (scatterSize == 16)
			pScatter = (uint8_t*)getData(DataNames::SCA_16);
		else if (scatterSize == 8)
			pScatter = (uint8_t*)getData(DataNames::SCA_8);

		// --- Calibrate
		auto pD = std::make_shared<SiCaliWrapper::calibratedData>(m_CaliRectWidth[calibrationHandle], get_rows());

		if (false == m_CaliPtr[calibrationHandle]->calibrate(pD, pRange, pReflectance, pScatter, doRectify))
		{
			std::cout << "calibrate failed!" << std::endl;
			return imgT;
		}


		// --- Get Calibrated Data
		double offsetY(0.0), offsetZ(pD->lower_bound_r), scaleY(0.044);
		double X_cal_Offset = pD->lower_bound_x;
		double Z_cal_Offset = pD->lower_bound_r;
		double X_cal_Scale = (pD->upper_bound_x - pD->lower_bound_x) / pD->Width; // 通过
		double Z_cal_Scale = 0.001; // 需要从 Ranger3Studio 的参数中得到 
		imgT.setCaliInfo(
			pD->Width,
			pD->Height - removeLastNRows, // 去除最后的 n 行
			pD->offsetX, offsetY, 0.0,
			pD->scaleX, scaleY, 0.0,
			pD->lower_bound_x, pD->upper_bound_x,
			pD->lower_bound_r, pD->upper_bound_r,
			X_cal_Offset, Z_cal_Offset, X_cal_Scale, Z_cal_Scale
		);

		imgT.insertDataCopy(DataNames::RAN_CAL, (uint8_t*)pD->getData(SiCaliWrapper::RAN_C), m_info.m_id);

		if (has(DataNames::REF))
			imgT.insertDataCopy(DataNames::REF_CAL, (uint8_t*)pD->getData(SiCaliWrapper::REF_C), 0);

		if (scatterSize == 16)
			imgT.insertDataCopy(DataNames::SCA_16_CAL, (uint8_t*)pD->getData(SiCaliWrapper::SCA_16), 0);
		else if (scatterSize == 8)
			imgT.insertDataCopy(DataNames::SCA_8_CAL, (uint8_t*)pD->getData(SiCaliWrapper::SCA_8), 0);

		if (!pD->doRectify)
			imgT.insertDataCopy(DataNames::RAN_X, (uint8_t*)pD->getData(SiCaliWrapper::RAN_X), 0);

		return imgT;
	}


#ifndef DISABLE_R380 // RANGER380
	bool
		ImgT::_InitCalibrationColor(
			const int& handle,
			const std::string& calibrationFile,
			const std::string& colorFile,
			const double& rectificationSpread,
			const int& rawColoreResizedWidth,
			const int& rectificationWidth,
			const SiCaliWrapper::RectMethod& rectMethod,
			const int& numberOfThreads)
	{
		if (isEmpty() || (handle < 0 || 255 < handle))
		{
			return false;
		}

		//m_ColorParam[handle] = new color_image_run::colorParam(color_image_run::loadColorParam(colorFile));

		if (m_ColorParam[handle] != nullptr)
			delete m_ColorParam[handle];
		else
			m_ColorParam[handle] = new color_image_run::colorParam(color_image_run::loadColorParam(colorFile));


		m_RawImageColorWidth[handle] = rawColoreResizedWidth;
		m_CaliColorRectWidth[handle] = rectificationWidth;

		if (m_CaliColorPtr[handle] != nullptr)
			delete m_CaliColorPtr[handle];
		else
			m_CaliColorPtr[handle] = new SiCaliWrapper::CalibrationWrapper();



		if (false == m_CaliColorPtr[handle]->initCalibrationFilter(
			calibrationFile, // json file recommended
			m_RawImageColorWidth[handle], // raw image width resized
			get_rows(), // image height (rows)
			m_info.m_RI.aoiWidth, // aoi width
			m_info.m_RI.aoiHeight,  // aoi height
			1.0,
			m_info.m_RI.aoiOffsetX, // region1, offsetx
			m_info.m_RI.m_RangeAxis == RangeAxis::STANDARD ? 0.0625f : -0.0625f,
			(float)(m_info.m_RI.m_RangeAxis == RangeAxis::STANDARD ?
				m_info.m_RI.aoiOffsetY : m_info.m_RI.aoiOffsetY + m_info.m_RI.aoiHeight),
			has(DataNames::SCA_8) ? 8 : (has(DataNames::SCA_16) ? 16 : 0),
			rectMethod,
			m_CaliColorRectWidth[handle],
			MISSING_DATA,
			true,
			false,
			rectificationSpread
		)) {
			std::cerr << "initCalibrationFilter failed!" << std::endl;
			return false;
		}


#ifndef __aarch64__
		if (numberOfThreads <= 0)
		{
			auto numberOfThread = m_CaliPtr[handle]->get_NumberOfThreads_jsonOnly_X64() / 2; // 将标定的线程数，设定为 CPU 核数的一半，为了不让 CPU 使用率达到 100% 。
			m_CaliPtr[handle]->set_NumberOfThreads_jsonOnly_X64(numberOfThread);
		}
		else
		{
			m_CaliPtr[handle]->set_NumberOfThreads_jsonOnly_X64(numberOfThreads);
		}
#endif // !__aarch64__

		return true;
	}


	bool ImgT::DeleteCalibrationColor(const int& handle)
	{
		if (m_CaliColorPtr[handle] != nullptr)
		{
			delete m_CaliColorPtr[handle];
			m_CaliColorPtr[handle] = nullptr;
		}


		if (m_CaliColorPtr[handle] != nullptr)
		{
			delete m_CaliColorPtr[handle];
			m_CaliColorPtr[handle] = nullptr;
		}



		return true;
	}

	ImgT ImgT::ApplyCalibrationColor(
		const int colorCalibrateAlignHandle,
		const std::string& calibrationFile,
		const std::string& colorFile,
		const int& removeLastNRows,
		bool  doFCC,
		const int& colorInterpolationStride,
		const double& rectificationSpread,
		const int& rawColoreResizedWidth,
		const int& rectificationWidth,
		const SiCaliWrapper::RectMethod& rectMethod,
		const int& removeHeadNRows,
		const bool doCalibration,
		const int& numberOfThreads
	)
	{
		int handle = colorCalibrateAlignHandle;
		ImgT imgTOut = ImgT();
		if (isEmpty() || (handle < 0 || 255 < handle) ||
			m_CaliColorPtr[handle] == nullptr || m_CaliColorRectWidth[handle] == 0
			)
		{
			_InitCalibrationColor(handle, calibrationFile, colorFile, rectificationSpread,
				rawColoreResizedWidth, rectificationWidth, rectMethod, numberOfThreads);
		}


		cv::Mat i_range = cv::Mat(get_rows(), get_cols(), CV_16SC1, getData(DN::RAN)).clone(); // deep copy
		cv::Mat estimatedRange;
		cv::Mat red_fcc = cv::Mat();
		cv::Mat green_fcc = cv::Mat();
		cv::Mat blue_fcc = cv::Mat();
		cv::Mat white_fcc = cv::Mat();




		// 全部都有 or 只有彩色
		if (has(DN::R) && has(DN::G) && has(DN::B))
		{
			///////////////////////////////////////////////////////////////////////


			cv::Mat i_red = cv::Mat(get_rows(), HIRES_IMG_WIDTH, CV_8UC1, getData(DN::R)).clone();
			cv::Mat i_green = cv::Mat(get_rows(), HIRES_IMG_WIDTH, CV_8UC1, getData(DN::G)).clone();
			cv::Mat i_blue = cv::Mat(get_rows(), HIRES_IMG_WIDTH, CV_8UC1, getData(DN::B)).clone();
			cv::Mat i_white = has(DN::WHITE) ? cv::Mat(get_rows(), HIRES_IMG_WIDTH, CV_8UC1, getData(DN::WHITE)).clone() : cv::Mat();

			if (rawColoreResizedWidth != HIRES_IMG_WIDTH)
			{
				auto ss = cv::Size(rawColoreResizedWidth, i_red.rows);
				cv::resize(i_red, i_red, ss, 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
				cv::resize(i_green, i_green, ss, 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
				cv::resize(i_blue, i_blue, ss, 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
				if (has(DN::WHITE))
					cv::resize(i_white, i_white, ss, 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
			}


			// 放大 Range 和 Reflectance 图像方便后续对齐。
			// 也可以缩小 RGBW 

			//cv::resize(i_reflectance, i_reflectance, i_red.size());
			cv::resize(i_range, i_range, i_red.size(), 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
			i_range.convertTo(i_range, CV_16SC1);


			// 获得 Red 图像的 Range 估计图像
			estimatedRange = color_image_run::process_estimate_range_component(i_range,
				m_ColorParam[handle]->FactorXRed, m_ColorParam[handle]->FactorYRed, true, colorInterpolationStride);

			// 将 G B W 和 Red 对齐
			auto green = color_image_run::process_translate_move_image(i_green, m_ColorParam[handle]->ShiftYG);
			auto blue  = color_image_run::process_translate_move_image(i_blue, m_ColorParam[handle]->ShiftYB);
			cv::Mat white;
			if (has(DN::WHITE))
				white = color_image_run::process_translate_move_image(i_white, m_ColorParam[handle]->ShiftYW);

			red_fcc = i_red;
			green_fcc = green;
			blue_fcc = blue;
			white_fcc = white;

			// fcc
			if (doFCC)
			{
				cv::Mat RedCorrectionFactors, GreenCorrectionFactors, BlueCorrectionFactors, GrayCorrectionFactors;
				//factorMatrix.convertTo(factorMatrix, CV_8U);

				m_ColorParam[handle]->RedCorrectionFactors.convertTo(RedCorrectionFactors, CV_32F);
				m_ColorParam[handle]->GreenCorrectionFactors.convertTo(GreenCorrectionFactors, CV_32F);
				m_ColorParam[handle]->BlueCorrectionFactors.convertTo(BlueCorrectionFactors, CV_32F);
				m_ColorParam[handle]->GrayCorrectionFactors.convertTo(GrayCorrectionFactors, CV_32F);


				//重复列矩阵
				cv::Mat expRedCorrectionFactors, expGreenCorrectionFactors, expBlueCorrectionFactors, expGrayCorrectionFactors;
				cv::repeat(RedCorrectionFactors, i_red.rows, 1, expRedCorrectionFactors);
				cv::repeat(GreenCorrectionFactors, i_red.rows, 1, expGreenCorrectionFactors);
				cv::repeat(BlueCorrectionFactors, i_red.rows, 1, expBlueCorrectionFactors);
				cv::repeat(GrayCorrectionFactors, i_red.rows, 1, expGrayCorrectionFactors);
				i_red.convertTo(i_red, CV_32F);
				green.convertTo(green, CV_32F);
				blue.convertTo(blue, CV_32F);

				if (has(DN::WHITE))
					white.convertTo(white, CV_32F);


				red_fcc = color_image_run::FlatFieldingCorrection(i_red, expRedCorrectionFactors);
				green_fcc = color_image_run::FlatFieldingCorrection(green, expGreenCorrectionFactors);
				blue_fcc = color_image_run::FlatFieldingCorrection(blue, expBlueCorrectionFactors);
				if (has(DN::WHITE))
					white_fcc = color_image_run::FlatFieldingCorrection(white, expGrayCorrectionFactors);
			}


			///////////////////////////////////////////////////////////////////////
		}
		// 只有高精度灰
		else if (has(DN::WHITE) && !(has(DN::R) && has(DN::G) && has(DN::B)))
		{
			cv::Mat i_white = cv::Mat(get_rows(), HIRES_IMG_WIDTH, CV_8UC1, getData(DN::WHITE)).clone();

			if (rawColoreResizedWidth != HIRES_IMG_WIDTH)
			{
				auto ss = cv::Size(rawColoreResizedWidth, i_white.rows);
				cv::resize(i_white, i_white, ss, 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
			}

			// 放大 Range 和 Reflectance 图像方便后续对齐。
			// 也可以缩小 RGBW 

			//cv::resize(i_reflectance, i_reflectance, i_red.size(), 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
			cv::resize(i_range, i_range, i_white.size(), 0.0, 0.0, cv::InterpolationFlags::INTER_NEAREST);
			i_range.convertTo(i_range, CV_16SC1);


			// 获得 图像的 Range 估计图像
			estimatedRange = color_image_run::process_estimate_range_component(i_range,
				m_ColorParam[handle]->FactorXRed, m_ColorParam[handle]->FactorYRed, true, colorInterpolationStride);

			// 和  对齐
			cv::Mat white = color_image_run::process_translate_move_image(i_white, m_ColorParam[handle]->ShiftYW);

			white_fcc = white;

			// fcc
			if (doFCC)
			{
				cv::Mat GrayCorrectionFactors;
				m_ColorParam[handle]->GrayCorrectionFactors.convertTo(GrayCorrectionFactors, CV_32F);


				//重复列矩阵
				cv::Mat expGrayCorrectionFactors;
				cv::repeat(GrayCorrectionFactors, i_white.rows, 1, expGrayCorrectionFactors);

				white.convertTo(white, CV_32F);
				white_fcc = color_image_run::FlatFieldingCorrection(white, expGrayCorrectionFactors);
			}

		}
		// 其他情况未定义
		else
		{
			std::cerr << "No supported color image components setting! Valid only within (RGBW / RGB / W) " << std::endl;
			return imgTOut;
		}

		if (doCalibration)
		{

			// --- Calibrate
			auto pD = std::make_shared<SiCaliWrapper::calibratedData>(m_CaliColorRectWidth[handle], get_rows());

			if (m_CaliColorPtr[handle]->calibrate_color(pD, estimatedRange.data, nullptr, nullptr,
				has(DN::R) ? red_fcc.data : nullptr,
				has(DN::G) ? green_fcc.data : nullptr,
				has(DN::B) ? blue_fcc.data : nullptr,
				has(DN::WHITE) ? white_fcc.data : nullptr,
				true)
				)
			{
				// --- Get Calibrated Data
				double offsetY(0.0), offsetZ(pD->lower_bound_r), scaleY(0.044);
				double X_cal_Offset = pD->lower_bound_x;
				double Z_cal_Offset = pD->lower_bound_r;
				double X_cal_Scale = (pD->upper_bound_x - pD->lower_bound_x) / pD->Width; // 通过
				double Z_cal_Scale = 0.001; // 需要从 Ranger3Studio 的参数中得到 
				imgTOut.setCaliInfo(
					pD->Width,
					pD->Height - removeLastNRows - removeHeadNRows, // 去除最后的 n 行，头部的 n 行
					pD->offsetX, offsetY, 0.0,
					pD->scaleX, scaleY, 0.0,
					pD->lower_bound_x, pD->upper_bound_x,
					pD->lower_bound_r, pD->upper_bound_r,
					X_cal_Offset, Z_cal_Offset, X_cal_Scale, Z_cal_Scale
				);

				//if (has(DataNames::RAN))
				imgTOut.insertDataCopy(DataNames::RAN_CAL, (uint8_t*)pD->getData(SiCaliWrapper::RAN_C), 0, removeHeadNRows);

				if (has(DataNames::R))
					imgTOut.insertDataCopy(DataNames::R, (uint8_t*)pD->getData(SiCaliWrapper::RED_C), 0, removeHeadNRows);
				if (has(DataNames::G))
					imgTOut.insertDataCopy(DataNames::G, (uint8_t*)pD->getData(SiCaliWrapper::GRE_C), 0, removeHeadNRows);
				if (has(DataNames::B))
					imgTOut.insertDataCopy(DataNames::B, (uint8_t*)pD->getData(SiCaliWrapper::BLU_C), 0, removeHeadNRows);
				if (has(DataNames::WHITE))
					imgTOut.insertDataCopy(DataNames::WHITE, (uint8_t*)pD->getData(SiCaliWrapper::WHI_C), 0, removeHeadNRows);
			}

			return imgTOut;
		}
		else
		{
			imgTOut.m_info = m_info;
			imgTOut.m_info.m_RI.cols = rawColoreResizedWidth;
			imgTOut.m_info.m_RI.rows = m_info.m_RI.rows - removeLastNRows - removeHeadNRows; // 去除最后的 n 行，头部的 n 行
			
			imgTOut.insertDataCopy(DataNames::RAN, estimatedRange.data, 0, removeHeadNRows);

			// 不应该有 range 图像
			//if (has(DataNames::REF))
			//	imgTOut.insertDataCopy(DataNames::REF, red_fcc.data, 0, removeHeadNRows);

			if (has(DataNames::R))
				imgTOut.insertDataCopy(DataNames::R, red_fcc.data, 0, removeHeadNRows);
			if (has(DataNames::G))
				imgTOut.insertDataCopy(DataNames::G, green_fcc.data, 0, removeHeadNRows);
			if (has(DataNames::B))
				imgTOut.insertDataCopy(DataNames::B, blue_fcc.data, 0, removeHeadNRows);
			if (has(DataNames::WHITE))
				imgTOut.insertDataCopy(DataNames::WHITE, white_fcc.data, 0, removeHeadNRows);
			
			
			return imgTOut;
		}
	}

#endif // RANGER380


#endif // EZR_EXPORTS


#endif // DISABLE_CAL_IN_PC




	void ImgT::Append(const ImgT& src)
	{
		if (get_cols() !=0 && (get_cols() != src.get_cols())) // range 的宽度要相同
		{
			return; // ImgT();
		}


		for (auto it = src.m_Data.begin(); it != src.m_Data.end(); ++it)
		{
			auto _name = it->first;

			// 拼接数据
			size_t byteSize1 = get_rows() * (_name == SickCam::MAR ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(_name));
			size_t byteSize2 = src.get_rows() * (_name == SickCam::MAR ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(_name));

			// 高精度灰度和彩色图像是 2560*2=5120 宽度的。
			if (_name == SickCam::WHITE || _name == SickCam::R || _name == SickCam::G || _name == SickCam::B)
			{
				byteSize1 = get_rows() * 5120 * _DN2PixelSize(_name);
				byteSize2 = src.get_rows() * 5120 * _DN2PixelSize(_name);
			}

			uint8_t* pd = new uint8_t[byteSize1 + byteSize2];
			std::memcpy(pd, this->getData(_name), byteSize1);
			std::memcpy(pd + byteSize1, src.getData(_name), byteSize2);


			if (m_Data.count(_name) == 1)
			{
				delete[](uint8_t*)m_Data.at(_name);
				m_Data[_name] = pd;
				//
				//m_Data.erase(_name);
			}
			else
			{
				m_Data.insert({ _name, pd });
			}

			//m_Data.insert({ _name, pd });
		}

		// 设置参数
		m_info.m_RI.rows += src.get_rows();
	}

	//ImgT ImgT::Append(const ImgT& src) const
	//{
	//	if (get_cols() != 0 && (get_cols() != src.get_cols())) // range 的宽度要相同
	//	{
	//		return ImgT();
	//	}


	//	//ImgT dst = ImgT();
	//	for (auto it = src.m_Data.begin(); it != src.m_Data.end(); ++it)
	//	{
	//		//if (has(it->first)) // 数据类型的交集
	//		{
	//			auto _name = it->first;

	//			// 拼接数据
	//			size_t byteSize1 = get_rows() * (_name == SickCam::MAR ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(_name));
	//			size_t byteSize2 = src.get_rows() * (_name == SickCam::MAR ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(_name));

	//			// 高精度灰度和彩色图像是 2560*2=5120 宽度的。
	//			if (_name == SickCam::WHITE || _name == SickCam::R || _name == SickCam::G || _name == SickCam::B)
	//			{
	//				byteSize1 = get_rows() * 5120 * _DN2PixelSize(_name);
	//				byteSize2 = src.get_rows() * 5120 * _DN2PixelSize(_name);
	//			}

	//			if (dst.m_Data.count(_name) != 0)
	//			{
	//				delete[](uint8_t*)dst.m_Data.at(_name);
	//				dst.m_Data.erase(_name);
	//			}

	//			uint8_t* pd = new uint8_t[byteSize1 + byteSize2];
	//			std::memcpy(pd, this->getData(_name), byteSize1);
	//			std::memcpy(pd + byteSize1, src.getData(_name), byteSize2);

	//			dst.m_Data.insert({ _name, pd });
	//		}
	//	}

	//	// 设置参数
	//	if (!dst.isEmpty())
	//	{
	//		dst.m_info = this->m_info;
	//		if (!dst.has(DataNames::MAR))
	//		{
	//			dst.m_info.m_ChunkData.clear();
	//		}
	//		dst.m_info.m_RI.rows += src.get_rows();
	//		dst.m_info.m_id = get_ID();
	//		return *this;
	//	}
	//	else
	//		return ImgT();

	//}



	std::vector<ImgT> ImgT::Divide(const int& parts) const
	{
		std::vector<ImgT> imageArray;

		int height = get_rows() / parts;

		if (height * parts != get_rows())
		{
			std::cerr << "Assert(height * parts == get_rows()) failed. Invalid parts" << std::endl;
			return imageArray;
		}



		for (int i = 0; i < parts; ++i)
		{
			ImgT image = ImgT();
			image.m_info = m_info;
			image.m_type = m_type;
			image.m_info.m_RI.rows = height;

			for (auto it = m_Data.begin(); it != m_Data.end(); ++it)
			{
				image.insertDataCopy(it->first, (uint8_t*)(it->second), i, i*height);
			}

			imageArray.push_back(image);
		}


		return imageArray;
	}





	void ImgT::_readXML(const std::string& fixpath_nosuffix, std::string& rangetraits_name,
		double& oz, double& sz,
		double& lx, double& ux, double& lr, double& ur,
		int64_t& fov___x0,
		int64_t& fov___x1,
		int64_t& fov___x2,
		int64_t& fov___x3,
		int64_t& fov___z0,
		int64_t& fov___z1,
		int64_t& fov___z2,
		int64_t& fov___z3,
		int64_t& origin_x,
		int64_t& scale__x,
		double& origin_z,
		double& scale__z,
		int64_t& size___x,
		int64_t& size___y,
		int64_t& offset_x,
		int64_t& offset_y,
		int64_t& rAxis,
		std::vector<std::string>& vec_subcomponentName,
		std::vector<size_t>& vec_subcomponentSize,
		std::vector<size_t>& vec_subcomponentWidth,
		size_t& total_LineSizeBytes,
		int& col,
		std::string& range_valuetype,
		std::string& scatter_valuetype,
		int& Scan3dExtraction_number /*双曝光的参数*/,
		int& hiresWhiteSize,
		int& hiresColorSize
	)
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err = doc.LoadFile((fixpath_nosuffix + ".xml").c_str());
		if (err != tinyxml2::XML_SUCCESS) return;

		tinyxml2::XMLElement* icon_data_format = doc.FirstChildElement("icon_data_format");
		if (icon_data_format == nullptr) return;

		/* xml file example (SICK standar ICON format)
		*
		<icon_data_format valuetype="" name="">
			<parameter name="size">		7700			</parameter>
			<parameter name="version">	1				</parameter>
			<parameter name="layout">	SUBCOMPONENT    </parameter>
			<component name="Scan3dExtraction1 valuetype="Hi3D""> // or name="Ranger3Range"
				...
			</component>
			<component name="Scan3dExtraction2" valuetype="Hi3D" >
				...
			</component>
			...
		</icon_data_format>
		*/

		//<icon_data_format valuetype="" name="">
		//  <parameter name="size">		7700			</parameter>
		tinyxml2::XMLElement* L1_para_size = icon_data_format->FirstChildElement();
		int valsize = 0;
		L1_para_size->QueryIntText(&valsize);
		total_LineSizeBytes = valsize;

		//  <parameter name="version">	1				</parameter>
		tinyxml2::XMLElement* L1_para_version = L1_para_size->NextSiblingElement();

		//  <parameter name="layout">	SUBCOMPONENT    </parameter>
		tinyxml2::XMLElement* L1_para_layout = L1_para_version->NextSiblingElement();

		//  <component name="Scan3dExtraction1" valuetype="Hi3D"> // or name="Ranger3Range" Ranger3Hires
		tinyxml2::XMLElement* L1_component = L1_para_layout->NextSiblingElement();

		/* xml file example (SICK standar ICON format)
		*
		...
			<component valuetype="Hi3D" name="Scan3dExtraction1">	// or name="Ranger3Range"
				<parameter name="size">7700</parameter>
				<parameter name="height">1</parameter>
				<sensorrangetraits>
					...												// We suppose that fov parameters are the same in different "Scan3dExtractionX".
					</sensorrangetraits>
				<subcomponent valuetype="WORD" name="Range">		// We suppose this is the same in different "Scan3dExtractionX".
					<parameter name="size">5120</parameter>
					<parameter name="width">2560</parameter>
				</subcomponent>
				<subcomponent valuetype="BYTE" name="Intensity">	// We suppose this is the same in different "Scan3dExtractionX".
					<parameter name="size">2560</parameter>
					<parameter name="width">2560</parameter>
				</subcomponent>
				<subcomponent valuetype="INT" name="Mark">			// We suppose that Mark is only in "Scan3dExtraction1".
					<parameter name="size">20</parameter>
					<parameter name="width">5</parameter>
				</subcomponent>
			</component>
		...
		*/

		tinyxml2::XMLElement* parameterC1 = L1_component->FirstChildElement();    // <parameter name="size">7700</parameter>
		tinyxml2::XMLElement* parameterC2 = parameterC1->NextSiblingElement(); // <parameter name="height">1</parameter>

		//// xxxxx_traits
		tinyxml2::XMLElement* XXXXrangetraits = parameterC2->NextSiblingElement(); // XXXXrangetraits
		std::string name(XXXXrangetraits->Value());
		if (name == "worldrangetraits") // calibrated
		{
			m_type = TYPE::RANGE3D;
			rangetraits_name = name;
			tinyxml2::XMLElement* paramXX = XXXXrangetraits->FirstChildElement();

			//paramXX->QueryDoubleText(&lx);		paramXX = paramXX->NextSiblingElement();
			//paramXX->QueryDoubleText(&lr);		paramXX = paramXX->NextSiblingElement();
			//paramXX->QueryDoubleText(&ux);		paramXX = paramXX->NextSiblingElement();
			//paramXX->QueryDoubleText(&ur);

			for (int i_w = 0; i_w < 4; ++i_w)
			{
				auto name = std::string(paramXX->Attribute("name"));
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				if (name == "lower bound x")
				{
					paramXX->QueryDoubleText(&lx);
				}
				else if (name == "lower bound r")
				{
					paramXX->QueryDoubleText(&lr);
				}
				else if (name == "upper bound x")
				{
					paramXX->QueryDoubleText(&ux);
				}
				else if (name == "upper bound r")
				{
					paramXX->QueryDoubleText(&ur);
				}

				if (i_w < 3)
					paramXX = paramXX->NextSiblingElement();
			}

			// FW 3.0
			auto genistreamtraits = XXXXrangetraits->NextSiblingElement();
			if (genistreamtraits != NULL && std::string(genistreamtraits->Value()) == "genistreamtraits") // genistreamtraits is a new component in FW 3.0
			{
				tinyxml2::XMLElement* genistreamtraits_paramXX = genistreamtraits->FirstChildElement();
				do
				{
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "region id")
					{
						m_info.m_CI.genistreamtraits.region_id = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "extraction method")
					{
						m_info.m_CI.genistreamtraits.extraction_method = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "output mode")
					{
						m_info.m_CI.genistreamtraits.output_mode = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "width")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.width = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "height")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.height = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "offset X")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.offset_X = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "offset Y")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.offset_Y = val;
					}


					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "a axis range scale")
					{
						double val(0); genistreamtraits_paramXX->QueryDoubleText(&val); 	m_info.m_CI.genistreamtraits.a_axis_range_scale = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "a axis range offset")
					{
						double val(0); genistreamtraits_paramXX->QueryDoubleText(&val); 	m_info.m_CI.genistreamtraits.a_axis_range_offset = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "a axis range min")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.a_axis_range_min = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "a axis range max")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.a_axis_range_max = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "a axis range missing")
					{
						m_info.m_CI.genistreamtraits.a_axis_range_missing = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "a axis range missing value")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val);     m_info.m_CI.genistreamtraits.a_axis_range_missing_value = val;
					}


					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "b axis range scale")
					{
						double val(0); genistreamtraits_paramXX->QueryDoubleText(&val); 	m_info.m_CI.genistreamtraits.b_axis_range_scale = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "b axis range offset")
					{
						double val(0); genistreamtraits_paramXX->QueryDoubleText(&val); 	m_info.m_CI.genistreamtraits.b_axis_range_offset = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "b axis range min")
					{
						m_info.m_CI.genistreamtraits.b_axis_range_min = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "b axis range max")
					{
						m_info.m_CI.genistreamtraits.b_axis_range_max = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "b axis range missing")
					{
						m_info.m_CI.genistreamtraits.b_axis_range_missing = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "b axis range missing value")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val);     m_info.m_CI.genistreamtraits.b_axis_range_missing_value = val;
					}


					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "c axis range scale") // special
					{
						genistreamtraits_paramXX->QueryDoubleText(&sz);                     m_info.m_CI.genistreamtraits.c_axis_range_scale = sz;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "c axis range offset") // special
					{
						genistreamtraits_paramXX->QueryDoubleText(&oz);  					m_info.m_CI.genistreamtraits.c_axis_range_offset = oz;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "c axis range min")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.c_axis_range_min = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "c axis range max")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val); 	m_info.m_CI.genistreamtraits.c_axis_range_max = val;
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "c axis range missing")
					{
						m_info.m_CI.genistreamtraits.c_axis_range_missing = genistreamtraits_paramXX->GetText();
					}
					if (std::string(genistreamtraits_paramXX->Attribute("name")) == "c axis range missing value")
					{
						int val(0); genistreamtraits_paramXX->QueryIntText(&val);     m_info.m_CI.genistreamtraits.c_axis_range_missing_value = val;
					}

					genistreamtraits_paramXX = genistreamtraits_paramXX->NextSiblingElement();
				} while (genistreamtraits_paramXX != NULL);
				XXXXrangetraits = genistreamtraits;
			}

		}
		else if (name == "sensorrangetraits")
		{
			m_type = TYPE::RANGE3D;
			rangetraits_name = name;
			tinyxml2::XMLElement* paramXX = XXXXrangetraits->FirstChildElement();

			paramXX->QueryInt64Text(&fov___x0); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___x1); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___x2); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___x3); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z0); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z1); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z2); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&fov___z3); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&origin_x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&scale__x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryDoubleText(&origin_z); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryDoubleText(&scale__z); paramXX = paramXX->NextSiblingElement();
			offset_x = fov___x0;
			offset_y = (fov___z2 + 1) / 16;
			rAxis = scale__z > 0 ? 1 : 0;
		}
		else if (name == "imagetraits")
		{
			m_type = TYPE::SENSOR;
			tinyxml2::XMLElement* paramXX = XXXXrangetraits->FirstChildElement();

			paramXX->QueryInt64Text(&size___x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&size___y); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&offset_x); paramXX = paramXX->NextSiblingElement();
			paramXX->QueryInt64Text(&offset_y); paramXX = paramXX->NextSiblingElement();

			total_LineSizeBytes = size___x;
		}
		else
			return;

		//// subcomponent
		tinyxml2::XMLElement* subcomponentX = XXXXrangetraits->NextSiblingElement();
		if (std::string(subcomponentX->Value()) == "genistreamtraits")
		{
			subcomponentX = subcomponentX->NextSiblingElement();
		}
		while (subcomponentX)
		{
			vec_subcomponentName.push_back(std::string(subcomponentX->Attribute("name")));
			vec_subcomponentSize.push_back(_subcomponentX_size(subcomponentX, "size"));
			vec_subcomponentWidth.push_back(_subcomponentX_size(subcomponentX, "width"));

			//total_LineSizeBytes += _subcomponentX_size(subcomponentX, "size");

			if (*(vec_subcomponentName.end() - 1) == "Range" || *(vec_subcomponentName.end() - 1) == "Image")
			{
				tinyxml2::XMLElement* size = subcomponentX->FirstChildElement();
				tinyxml2::XMLElement* width = size->NextSiblingElement();
				width->QueryIntText(&col);

				if (*(vec_subcomponentName.end() - 1) == "Range")
					range_valuetype = std::string(subcomponentX->Attribute("valuetype"));
			}

			if (*(vec_subcomponentName.end() - 1) == "Scatter")
			{
				// scatter type can be WORD or BYTE 
				scatter_valuetype = std::string(subcomponentX->Attribute("valuetype"));
			}

			subcomponentX = subcomponentX->NextSiblingElement();
		}


		// FW Dual exposure
		Scan3dExtraction_number = 1;
		if (std::string(L1_component->Attribute("name")) == "Scan3dExtraction1")
		{
			Scan3dExtraction_number = 0;
			//total_LineSizeBytes = 0;

			auto L1_component_Scan3dExtraction1 = L1_component;

			auto L1_component_Scan3dExtraction2 = L1_component_Scan3dExtraction1->NextSiblingElement();

			auto L1_component_Scan3dExtraction3 = L1_component_Scan3dExtraction2->NextSiblingElement();

			if (L1_component_Scan3dExtraction1)
			{
				int64_t val = 0;
				L1_component_Scan3dExtraction1->FirstChildElement()->QueryInt64Text(&val);
				//total_LineSizeBytes += val; // 这里的总大小就是上面第一个节点的大小吧？
				++Scan3dExtraction_number;
			}

			if (L1_component_Scan3dExtraction2)
			{
				int64_t val = 0;
				L1_component_Scan3dExtraction2->FirstChildElement()->QueryInt64Text(&val);
				//total_LineSizeBytes += val; // 这里的总大小就是上面第一个节点的大小吧？
				++Scan3dExtraction_number;
			}

			if (L1_component_Scan3dExtraction3)
			{
				int64_t val = 0;
				L1_component_Scan3dExtraction3->FirstChildElement()->QueryInt64Text(&val);
				//total_LineSizeBytes += val; // 这里的总大小就是上面第一个节点的大小吧？
				++Scan3dExtraction_number;
			}
		}


		//Ranger3Hires white
		tinyxml2::XMLElement* L1_component_Ranger3Hires_1 = L1_component->NextSiblingElement();
		hiresWhiteSize = 0;
		if (std::string(L1_component_Ranger3Hires_1->Value()) == "component")
		{
			auto r3hires_subcomponent1 = L1_component_Ranger3Hires_1->FirstChild();
			auto r3hires_subcomponent2 = r3hires_subcomponent1->NextSiblingElement();
			auto r3hires_subcomponent3 = r3hires_subcomponent2->NextSiblingElement();
			auto r3hires_subcomponentX = r3hires_subcomponent3->NextSiblingElement();

			while (r3hires_subcomponentX)
			{

				if (std::string(r3hires_subcomponentX->Attribute("name")) == "HiresWhite")
				{
					// L1_component_Ranger3Hires is white
					hiresWhiteSize = 5120;
					vec_subcomponentName.push_back("HiresWhite");
					vec_subcomponentSize.push_back(5120);
					vec_subcomponentWidth.push_back(5120);
				}
				r3hires_subcomponentX = r3hires_subcomponentX->NextSiblingElement();
			}
		}


		//Ranger3Hires color
		tinyxml2::XMLElement* L1_component_Ranger3Hires_2 = L1_component_Ranger3Hires_1->NextSiblingElement();
		hiresColorSize = 0;
		if (L1_component_Ranger3Hires_2 != nullptr && std::string(L1_component_Ranger3Hires_2->Value()) == "component")
		{
			auto r3hires_subcomponent1 = L1_component_Ranger3Hires_2->FirstChild();
			auto r3hires_subcomponent2 = r3hires_subcomponent1->NextSiblingElement();
			auto r3hires_subcomponent3 = r3hires_subcomponent2->NextSiblingElement();
			auto r3hires_subcomponentX = r3hires_subcomponent3->NextSiblingElement();

			//char* tt = "";
			//if (r3hires_subcomponentX->QueryStringAttribute("name", &tt) == tinyxml2::XMLError::XML_SUCCESS)
			//{
			while (r3hires_subcomponentX)
			{
				if (std::string(r3hires_subcomponentX->Attribute("name")) == "HiresRed")
				{
					// L1_component_Ranger3Hires is white
					hiresColorSize += 5120;
					vec_subcomponentName.push_back("HiresRed");
					vec_subcomponentSize.push_back(5120);
					vec_subcomponentWidth.push_back(5120);
				}
				if (std::string(r3hires_subcomponentX->Attribute("name")) == "HiresGreen")
				{
					// L1_component_Ranger3Hires is white
					hiresColorSize += 5120;
					vec_subcomponentName.push_back("HiresGreen");
					vec_subcomponentSize.push_back(5120);
					vec_subcomponentWidth.push_back(5120);
				}
				if (std::string(r3hires_subcomponentX->Attribute("name")) == "HiresBlue")
				{
					// L1_component_Ranger3Hires is white
					hiresColorSize += 5120;
					vec_subcomponentName.push_back("HiresBlue");
					vec_subcomponentSize.push_back(5120);
					vec_subcomponentWidth.push_back(5120);
				}

				r3hires_subcomponentX = r3hires_subcomponentX->NextSiblingElement();
			}
			//}
		}






		// 
	   //{
	   //	//Ranger3Hires
	   //	int64_t val = 0;
	   //	L1_component_Ranger3Hires->FirstChildElement()->QueryInt64Text(&val);

	   //	if (val == 5120)
	   //	{
	   //		//// L1_component_Ranger3Hires is white
	   //		vec_subcomponentName.push_back("HiresWhite");
	   //		vec_subcomponentSize.push_back(5120);
	   //		vec_subcomponentWidth.push_back(5120);

	   //		hiresWhiteSize = 5120;
	   //		tinyxml2::XMLElement* L1_component_Ranger3Hires_2 = L1_component_Ranger3Hires->NextSiblingElement();
	   //		if (L1_component_Ranger3Hires_2)
	   //		{
	   //			hiresColorSize = 15360;
	   //			vec_subcomponentName.push_back("HiresRed");
	   //			vec_subcomponentSize.push_back(5120);
	   //			vec_subcomponentWidth.push_back(5120);
	   //			vec_subcomponentName.push_back("HiresGreen");
	   //			vec_subcomponentSize.push_back(5120);
	   //			vec_subcomponentWidth.push_back(5120);
	   //			vec_subcomponentName.push_back("HiresBlue");
	   //			vec_subcomponentSize.push_back(5120);
	   //			vec_subcomponentWidth.push_back(5120);
	   //		}
	   //		else
	   //		{
	   //			hiresColorSize = 0;
	   //		}
	   //	}
	   //	
	   //	if (val == 15360)
	   //	{
	   //		hiresWhiteSize = 0;
	   //		hiresColorSize = 15360;
	   //		vec_subcomponentName.push_back("HiresRed");
	   //		vec_subcomponentSize.push_back(5120);
	   //		vec_subcomponentWidth.push_back(5120);
	   //		vec_subcomponentName.push_back("HiresGreen");
	   //		vec_subcomponentSize.push_back(5120);
	   //		vec_subcomponentWidth.push_back(5120);
	   //		vec_subcomponentName.push_back("HiresBlue");
	   //		vec_subcomponentSize.push_back(5120);
	   //		vec_subcomponentWidth.push_back(5120);
	   //	}
	   //}
	}


	bool
		ImgT::loadFromIconFile(const std::string& path, const double& resolutonY, const double& resolutonZ, const bool convertRangeToFloat)
	{
		if (!_clearBuffer(*this))
		{
			return false;
		}

		// Image size
		int col(0), row(0);

		/// xml general params
		size_t total_LineSizeBytes(0);
		std::vector<std::string> vec_subcomponentName(0);
		std::vector<size_t>		vec_subcomponentSize(0);
		std::vector<size_t>		vec_subcomponentWidth(0);
		std::string scatter_valuetype("");
		std::string rangetraits_name("");
		std::string range_valuetype("");	// > FW3.0
		//double		range_resolution	(resolutonZ); // > FW3.0
		int		Scan3dExtraction_number(1);	// > FW Dual exposure

		/// range params
		int64_t	aox(0), aoy(0), ah(0), aw(0), rAxis(1);
		int64_t
			fov___x0(0),
			fov___x1(0),
			fov___x2(0),
			fov___x3(0),
			fov___z0(0),
			fov___z1(0),
			fov___z2(0),
			fov___z3(0),
			origin_x(0),
			scale__x(0);
		double
			origin_z(0.0),
			scale__z(0.0);

		/// calibration params
		double
			ox(0.0), oy(0.0), oz(0.0), sx(1.0), sy(1.0), sz(1.0),
			lx(0.0), ux(0.0), lr(0.0), ur(0.0);

		/// sensor params
		int64_t
			size___x(0),
			size___y(0),
			offset_x(0),
			offset_y(0);

		/// check path format 
		std::string path_name_only("");
		if (path.size() < 4)
			path_name_only = path;
		else
			path_name_only = path.substr(path.size() - 3, 3); // suffix

		if (path_name_only == "xml" || path_name_only == "XML" || path_name_only == "dat" || path_name_only == "DAT")
			path_name_only = path.substr(0, path.size() - 4);
		else
			path_name_only = path;


		int hiresWhiteSize = 0;
		int hiresColorSize = 0;

		// Read xml
		_readXML(path, rangetraits_name,
			oz,
			sz,
			lx,
			ux,
			lr,
			ur,
			fov___x0,
			fov___x1,
			fov___x2,
			fov___x3,
			fov___z0,
			fov___z1,
			fov___z2,
			fov___z3,
			origin_x,
			scale__x,
			origin_z,
			scale__z,
			size___x,
			size___y,
			offset_x,
			offset_y,
			rAxis,
			vec_subcomponentName,
			vec_subcomponentSize,
			vec_subcomponentWidth,
			total_LineSizeBytes,
			col,
			range_valuetype,
			scatter_valuetype,
			Scan3dExtraction_number, hiresWhiteSize, hiresColorSize);



		if (col == 0) return false;

		// Read dat
		std::ifstream inFile;
		inFile.open(path_name_only + ".dat", std::ios_base::in | std::ios_base::binary);
		if (inFile.is_open())
		{
			// binary file size info
			inFile.seekg(0, inFile.end);
			int nLength = inFile.tellg();
			row = nLength / total_LineSizeBytes;

			// set ImgT data
			uint8_t* pBuffer = new uint8_t[nLength];
			inFile.seekg(0, inFile.beg);
			inFile.read((char*)&pBuffer[0], nLength);

			if (m_type == TYPE::SENSOR)
			{
				setSensorInfo(col, row, offset_x, offset_y);
				insertDataCopy(DN::SEN, pBuffer, 0);
			}
			else
			{
				// set ImgT info
				if (rangetraits_name == "worldrangetraits")
				{
					ox = lx;
					oy = 0.0;
					sx = (ux - lx) / col;
					sy = resolutonY;
					setCaliInfo(col, row, ox, oy, oz, sx, sy, sz, lx, ux, lr, ur,
						m_info.m_CI.genistreamtraits.a_axis_range_scale, m_info.m_CI.genistreamtraits.a_axis_range_offset, m_info.m_CI.genistreamtraits.c_axis_range_scale, m_info.m_CI.genistreamtraits.c_axis_range_offset
					);
				}
				else if (rangetraits_name == "sensorrangetraits")
				{
					aox = fov___x0;
					aw = fov___x1 - aox + 1;

					ah = (fov___z2 + 1) / 16;	/// 16 bits data with sub-pixel accuracy -- see SaveBuffer.cpp
					aoy = (rAxis == 1 ? origin_z : origin_z - ah); ///  -- see SaveBuffer.cpp

					m_info.m_RI.cols = static_cast<size_t>(col);
					m_info.m_RI.rows = static_cast<size_t>(row);
					m_info.m_RI.aoiOffsetX = aox;
					m_info.m_RI.aoiOffsetY = aoy;
					m_info.m_RI.aoiHeight = ah;
					m_info.m_RI.aoiWidth = aw;
					m_info.m_RI.m_RangeAxis = (rAxis == 1 ? RangeAxis::STANDARD : RangeAxis::REVERSED);
					m_info.m_RI.xml_fov_x0 = static_cast<float>(fov___x0);
					m_info.m_RI.xml_fov_x1 = static_cast<float>(fov___x1);
					m_info.m_RI.xml_fov_x2 = static_cast<float>(fov___x2);
					m_info.m_RI.xml_fov_x3 = static_cast<float>(fov___x3);
					m_info.m_RI.xml_fov_z0 = static_cast<float>(fov___z0);
					m_info.m_RI.xml_fov_z1 = static_cast<float>(fov___z1);
					m_info.m_RI.xml_fov_z2 = static_cast<float>(fov___z2);
					m_info.m_RI.xml_fov_z3 = static_cast<float>(fov___z3);
					m_info.m_RI.xml_origin_x = static_cast<float>(origin_x);
					m_info.m_RI.xml_scale_x = static_cast<float>(scale__x);
					m_info.m_RI.xml_origin_z = static_cast<float>(origin_z);
					m_info.m_RI.xml_scale_z = static_cast<float>(scale__z);
				}

				// insert Data according to subcomponent orders
				uint8_t* pBufferNew = pBuffer;
				for (size_t j = 1; j <= Scan3dExtraction_number; ++j)
				{
					for (size_t iSubcomponentName = 0; iSubcomponentName < vec_subcomponentName.size(); ++iSubcomponentName)
					{
						// copy data
						if (vec_subcomponentName[iSubcomponentName] == "Intensity" || vec_subcomponentName[iSubcomponentName] == "Reflectance")
						{
							if (vec_subcomponentSize[iSubcomponentName] / vec_subcomponentWidth[iSubcomponentName] == sizeof(float))
							{
								/* ------------------------------------------------------------------------------
								* The old format REF_CAL_F will not be supported any more,
								* but we have to make it can be loaded.
								* So the code here will load data from the binary file(*.dat) using sizeod(float);
								* ------------------------------------------------------------------------------ */
								uint8_t* pRef = new uint8_t[row * col];
								uint8_t* pByte = new uint8_t[row * col * sizeof(float)];
								memcpy(pByte, (uint8_t*)(pBufferNew), row * col * sizeof(float));

								/* ------------------------------------------------------------------------------
								* We do not want it to be a part of SickCam::Img, so convert its type into "uint8_t"
								* to match DN::REF_CAL.
								* ------------------------------------------------------------------------------ */
								float* pFloat = (float*)pByte;
								for (size_t i = 0; i < row * col; ++i)
									pRef[i] = static_cast<uint8_t>(pFloat[i]);

								/* ------------------------------------------------------------------------------
								* The data will be used as DN::REF_CAL in this code. Please pay attention to it.
								* ------------------------------------------------------------------------------ */
								if (j == 1)
									insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::REF_CAL : DN::REF, pRef, 0);
								if (j == 2) // FW Dual exposure
									insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::REF2_CAL : DN::REF2, pRef, 0);
								pBufferNew = (pBufferNew + row * col * sizeof(float));

								delete[] pByte;
								delete[] pRef;
							}
							else
							{
								if (j == 1)
									insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::REF_CAL : DN::REF, (uint8_t*)(pBufferNew), 0);
								if (j == 2) // FW Dual exposure
									insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::REF2_CAL : DN::REF2, (uint8_t*)(pBufferNew), 0);
								pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
							}

						}
						else if (vec_subcomponentName[iSubcomponentName] == "Range")
						{
							// FW 3.0
							if (rangetraits_name == "worldrangetraits" && range_valuetype == "WORD") // cal data
							{
								uint16_t* p16 = (uint16_t*)pBufferNew;

								if (convertRangeToFloat)
								{
									std::vector<float> pFloat(col * row);
									for (int i = 0; i < col * row; ++i)
									{
										pFloat[i] = p16[i] == 0 ? -1.0e6f : static_cast<float>(p16[i] * sz + lr);
									}

									if (j == 1)
										insertDataCopy(DN::RAN_CAL, (uint8_t*)(pFloat.data()), 0);
									if (j == 2) // FW Dual exposure
										insertDataCopy(DN::RAN2_CAL, (uint8_t*)(pFloat.data()), 0);
								}
								else
								{
									if (j == 1)
										insertDataCopy(DN::RAN_CAL_16, (uint8_t*)(p16), 0);
								}
							}
							else
							{
								// old
								if (j == 1)
									insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::RAN_CAL : DN::RAN, pBufferNew, 0);
								if (j == 2) // FW Dual exposure
									insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::RAN2_CAL : DN::RAN2, pBufferNew, 0);
							}
							pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);

						}
						else if (vec_subcomponentName[iSubcomponentName] == "Scatter")
						{
							if (j != 1) continue; // FW Dual exposure
							if (scatter_valuetype == "WORD")
							{
								insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::SCA_16_CAL : DN::SCA_16, pBufferNew, 0);
								pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
							}
							else if (scatter_valuetype == "BYTE")
							{
								insertDataCopy(rangetraits_name == "worldrangetraits" ? DN::SCA_8_CAL : DN::SCA_8, pBufferNew, 0);
								pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
							}
						}
						else if (vec_subcomponentName[iSubcomponentName] == "Mark")
						{
							if (j != 1) continue; // FW Dual exposure
							insertDataCopy(DN::MAR, pBufferNew, 0);
							_bufferToChunkInfo(pBufferNew, row); // set mark data to chunkinfo
							pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]); // move pointer
						}
						else if (vec_subcomponentName[iSubcomponentName] == "Image") // sensor
						{
							if (j != 1) continue; // FW Dual exposure
							insertDataCopy(DN::SEN, pBufferNew, 0);
							pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
						}
						else if (vec_subcomponentName[iSubcomponentName] == "RangeA")
						{
							// FW 3.0
							if (rangetraits_name == "worldrangetraits" && range_valuetype == "WORD") // cal data
							{
								if (j != 1) continue; // FW Dual exposure
								insertDataCopy(DN::RAN_X_16, pBufferNew, 0);
								pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
							}
							else if (rangetraits_name == "worldrangetraits" && range_valuetype == "FLOAT") // cal data
							{
								if (j != 1) continue; // FW Dual exposure
								insertDataCopy(DN::RAN_X, pBufferNew, 0);
								pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
							}
						}
						else if (vec_subcomponentName[iSubcomponentName] == "HiresWhite")
						{
							if (j != 1) continue; // FW Dual exposure
							insertDataCopy(DN::WHITE, pBufferNew, 0);
							pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
						}
						else if (vec_subcomponentName[iSubcomponentName] == "HiresRed")
						{
							if (j != 1) continue; // FW Dual exposure
							insertDataCopy(DN::R, pBufferNew, 0);
							pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
						}
						else if (vec_subcomponentName[iSubcomponentName] == "HiresGreen")
						{
							if (j != 1) continue; // FW Dual exposure
							insertDataCopy(DN::G, pBufferNew, 0);
							pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
						}
						else if (vec_subcomponentName[iSubcomponentName] == "HiresBlue")
						{
							if (j != 1) continue; // FW Dual exposure
							insertDataCopy(DN::B, pBufferNew, 0);
							pBufferNew = (pBufferNew + row * vec_subcomponentSize[iSubcomponentName]);
						}
					}

				}
			}

			delete[] pBuffer;
			return true;

		}
		return false;
	}


	bool
		ImgT::SaveToIconFile(const std::string& path, const std::vector<DN>& components, bool is_calibrated) const
	{
		return _saveBuffer(components, path, is_calibrated);
	}

	bool
		ImgT::SaveToIconFile(const std::string& path, const std::vector<DN>& components) const
	{
		bool is_calibrated = true;
		for (auto sub : components)
			is_calibrated &= (DN::MAR <= sub) && (sub <= DN::SCA_16_CAL);

		return _saveBuffer(components, path, is_calibrated);
	}

	bool
		ImgT::SaveSensorImageToIconFile(const std::string& path, bool byFStream) const
	{
		if (has(DN::SEN))
			return byFStream ? _saveBuffer({ SickCam::DN::SEN }, path) : _saveBufferByMapping({ SickCam::DN::SEN }, path);
		else
			return false;
	}

	bool
		ImgT::SaveRawImagesToIconFile(const std::string& path, bool withChunkData, bool byFStream) const
	{
		if (has(DN::RAN))
			return byFStream
			? _saveBuffer(
				(withChunkData ? std::vector<DN>({ DN::RAN, DN::REF, DN::SCA_16, DN::MAR, DN::WHITE, DN::R, DN::G, DN::B })
					: std::vector<DN>({ DN::RAN, DN::REF, DN::SCA_16, DN::WHITE, DN::R, DN::G, DN::B })), path, false
			)
			: _saveBufferByMapping(
				(withChunkData ? std::vector<DN>({ DN::RAN, DN::REF, DN::SCA_16, DN::MAR, DN::WHITE, DN::R, DN::G, DN::B })
					: std::vector<DN>({ DN::RAN, DN::REF, DN::SCA_16, DN::WHITE, DN::R, DN::G, DN::B })), path, false
			);
		else
			return false;
	}

	bool
		ImgT::SaveCalibratedImagesToIconFile(const std::string& path, bool withChunkData, bool byFStream) const
	{
		if (has(DN::RAN_CAL) || has(DN::RAN_CAL_16))
			return byFStream
			? _saveBuffer((std::vector<DN>
				({ DN::RAN_X,DN::RAN_X_16,DN::RAN_CAL, DN::RAN_CAL_16, DN::REF_CAL, DN::SCA_16_CAL,
					(withChunkData ? DN::MAR : DN::INV) , DN::WHITE, DN::R, DN::G, DN::B })), path, true)
			: _saveBufferByMapping((std::vector<DN>
				({ DN::RAN_X,DN::RAN_X_16,DN::RAN_CAL, DN::RAN_CAL_16, DN::REF_CAL, DN::SCA_16_CAL,
					(withChunkData ? DN::MAR : DN::INV), DN::WHITE, DN::R, DN::G, DN::B })), path, true);
		else
			return false;
	}

	bool
		ImgT::SaveCalibratedDataToPointCloud(const std::string& _path, bool please_do_not_use_it_any_more) const
	{
		if (_path.empty() || !has(DN::RAN_CAL)) return false;

		std::string path(_path);
		if (path.substr(path.size() - 4, 3) == "txt")
			path += ".txt";

		std::ofstream ofile(path);
		if (ofile.is_open())
		{
			double
				xResolution(getC_scaleX()), yResolution(getC_scaleY()), zResolution(getCalibration_info().scaleZ),
				xOffset(getC_offsetX()), yOffset(0), zOffset(getCalibration_info().offsetZ);

			//bool hasRef = has(DN::REF_CAL);
			float* pd = (float*)getData(DN::RAN_CAL);

			for (int j = 0; j < static_cast<int>(get_rows()); ++j)
			{
				for (int i = 0; i < static_cast<int>(get_cols()); ++i)
				{
					if (pd[i + j * get_cols()] == -1.0e6f)	continue;
					if (pd[i + j * get_cols()] == 0.f)		continue;
					if (std::isnan(pd[i + j * get_cols()]))	continue;

					// make its coordinate value right-hand, so reverse y! 
					ofile
						<< (i * xResolution + xOffset) << ","
						<< (j * yResolution + yOffset) << ","
						<< (pd[i + j * get_cols()]) << std::endl;
				}
			}
			ofile.close();
		}

		return true;
	}


	//////////////////////////////////// protected ////////////////////////////////////////////

	ImgT&
		ImgT::__doapl(ImgT* ths, const ImgT& img)
	{
		if (!img.isEmpty())
		{
			// data
			if (_clearBuffer(*ths))
			{
				ths->m_info = img.m_info;
				ths->m_type = img.m_type;
				ths->m_previousImageID = img.m_previousImageID;
				for (auto sub : img.m_Data)
					ths->insertDataCopy(sub.first, (uint8_t*)sub.second, img.m_info.m_id);
			}
		}
		return *ths;
	}

	bool
		ImgT::_clearBuffer(ImgT& img)
	{
		if (img.isEmpty())
			return true;
		for (auto sub : img.m_Data)
		{
			if (sub.second != nullptr)
			{
				delete[](uint8_t*)sub.second;
				sub.second = nullptr;
			}
		}
		img.m_Data.clear();
		return img.m_Data.empty();
	}

	bool
		ImgT::_compareBuffer(const ImgT& img, const DN dn)
	{
		bool isSame(false);
		if (has(dn) && img.has(dn))
		{
			size_t tSizeThis = get_cols(dn) * get_rows(dn);
			size_t tSize = img.get_cols(dn) * img.get_rows(dn);
			isSame = tSizeThis == tSize;
			if (!isSame)
				return false;

			uint8_t* pdThis = (uint8_t*)getData(dn);
			uint8_t* pd = (uint8_t*)img.getData(dn);

			// MAR 只有一列
			if (dn == DN::MAR)
			{
				for (size_t i = 0; i < get_rows(); ++i)
				{
					isSame &= (pdThis[i] == pd[i]);

					if (isSame == false)
					{
						break;
					}
				}
			}
			else
			{
				// 仅验证 15% 的数据，提高速度
				for (size_t i = 0; i < tSize*0.15; ++i)
				{
					isSame &= (pdThis[i] == pd[i]);

					if (isSame == false)
					{
						break;
					}
				}
			}
		}

		return isSame;
	}

	size_t
		ImgT::_subcomponentX_size(tinyxml2::XMLElement* subcomponentX, const std::string& target)
	{
		tinyxml2::XMLElement* paramXX = subcomponentX->FirstChildElement();
		int64_t value(0);

		for (int i = 0; i < 2; ++i)
		{
			if (std::string(paramXX->Attribute("name")) == target)
			{
				paramXX->QueryInt64Text(&value);
				break;
			}
			paramXX = paramXX->NextSiblingElement();
		}

		return static_cast<size_t>(value);
	}

	void
		ImgT::_writeXML(const std::string& filePath, const std::vector<DataNames>& dataNames,
			const int& rowSize, const int& Ranger3RangeSize, bool is_calibrated) const
	{
		std::ofstream xmlFile(filePath + ".xml");

		if (dataNames.size() == 1 && dataNames[0] == DataNames::SEN)
		{
			xmlFile << "<icon_data_format name=\"IMAGE\" valuetype=\"IMAGE\">\n";
			xmlFile << "  <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
			xmlFile << "  <parameter name=\"version\">1</parameter>\n";
			xmlFile << "  <parameter name=\"layout\">SCAN</parameter>\n";
			xmlFile << "  <component valuetype=\"image\" name=\"Ranger3Image\">\n";
			xmlFile << "    <parameter name=\"size\">" << std::to_string(m_info.m_SI.cols) << "</parameter>\n";
			xmlFile << "    <parameter name=\"height\">" << std::to_string(m_info.m_SI.rows) << "</parameter>\n";
			xmlFile << "    <imagetraits>\n";
			xmlFile << "      <parameter name=\"size x\">" << std::to_string(m_info.m_SI.cols) << "</parameter>\n";
			xmlFile << "      <parameter name=\"size y\">" << std::to_string(m_info.m_SI.rows) << "</parameter>\n";
			xmlFile << "      <parameter name=\"offset x\">0</parameter>\n";
			xmlFile << "      <parameter name=\"offset y\">0</parameter>\n";
			xmlFile << "    </imagetraits>\n";
		}
		else
		{
			xmlFile << "<icon_data_format name=\"\" valuetype=\"\">\n";
			xmlFile << "  <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
			xmlFile << "  <parameter name=\"version\">1</parameter>\n";
			xmlFile << "  <parameter name=\"layout\">SUBCOMPONENT</parameter>\n";
			xmlFile << "  <component name=\"Ranger3Range\" valuetype=\"Hi3D\">\n";
			xmlFile << "    <parameter name=\"size\">" << std::to_string(Ranger3RangeSize) << "</parameter>\n";
			xmlFile << "    <parameter name=\"height\">1</parameter>\n";

			if (is_calibrated)
			{
				xmlFile << "    <worldrangetraits>\n";
				xmlFile << "      <parameter name=\"lower bound x\">" << std::to_string(m_info.m_CI.lower_bound_x) << "</parameter>\n";
				xmlFile << "      <parameter name=\"lower bound r\">" << std::to_string(m_info.m_CI.lower_bound_r) << "</parameter>\n";
				xmlFile << "      <parameter name=\"upper bound x\">" << std::to_string(m_info.m_CI.upper_bound_x) << "</parameter>\n";
				xmlFile << "      <parameter name=\"upper bound r\">" << std::to_string(m_info.m_CI.upper_bound_r) << "</parameter>\n";
				xmlFile << "      <parameter name=\"coordinate unit\"></parameter>\n";
				xmlFile << "    </worldrangetraits>\n";

				xmlFile << "    <genistreamtraits>\n";
				xmlFile << "      <parameter name=\"region id\">scan 3d extraction 1</parameter>\n";
				xmlFile << "      <parameter name=\"extraction method\">hi 3d</parameter>\n";
				xmlFile
					<< "      <parameter name=\"output mode\">"
					<< (m_info.m_CI.genistreamtraits.output_mode == ""
						? (has(DN::RAN_X) ? "Calibrated AC" : (has(DN::RAN) || has(DN::RAN2) ? "Uncalibrated" : "rectified c"))
						: m_info.m_CI.genistreamtraits.output_mode)
					<< "</parameter>\n";


				xmlFile << "      <parameter name=\"width\">" << std::to_string(m_info.m_CI.genistreamtraits.width) << "</parameter>\n";
				xmlFile << "      <parameter name=\"height\">" << std::to_string(m_info.m_CI.genistreamtraits.height) << "</parameter>\n";
				xmlFile << "      <parameter name=\"offset X\">" << std::to_string(m_info.m_CI.genistreamtraits.offset_X) << "</parameter>\n";
				xmlFile << "      <parameter name=\"offset Y\">" << std::to_string(m_info.m_CI.genistreamtraits.offset_Y) << "</parameter>\n";

				xmlFile << "      <parameter name=\"a axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_scale) << "</parameter>\n";
				xmlFile << "      <parameter name=\"a axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_offset) << "</parameter>\n";
				xmlFile << "      <parameter name=\"a axis range min\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_min) << "</parameter>\n";
				xmlFile << "      <parameter name=\"a axis range max\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_max) << "</parameter>\n";
				xmlFile << "      <parameter name=\"a axis range missing\">" << m_info.m_CI.genistreamtraits.a_axis_range_missing.c_str() << "</parameter>\n";
				xmlFile << "      <parameter name=\"a axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_missing_value) << "</parameter>\n";

				xmlFile << "      <parameter name=\"b axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_scale) << "</parameter>\n";
				xmlFile << "      <parameter name=\"b axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_offset) << "</parameter>\n";
				xmlFile << "      <parameter name=\"b axis range min\">" << m_info.m_CI.genistreamtraits.b_axis_range_min << "</parameter>\n";
				xmlFile << "      <parameter name=\"b axis range max\">" << m_info.m_CI.genistreamtraits.b_axis_range_max << "</parameter>\n";
				xmlFile << "      <parameter name=\"b axis range missing\">" << m_info.m_CI.genistreamtraits.b_axis_range_missing << "</parameter>\n";
				xmlFile << "      <parameter name=\"b axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_missing_value) << "</parameter>\n";

				xmlFile << "      <parameter name=\"c axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_scale) << "</parameter>\n";
				xmlFile << "      <parameter name=\"c axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_offset) << "</parameter>\n";
				xmlFile << "      <parameter name=\"c axis range min\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_min) << "</parameter>\n";
				xmlFile << "      <parameter name=\"c axis range max\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_max) << "</parameter>\n";
				xmlFile << "      <parameter name=\"c axis range missing\">" << m_info.m_CI.genistreamtraits.c_axis_range_missing << "</parameter>\n";
				xmlFile << "      <parameter name=\"c axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_missing_value) << "</parameter>\n";

				xmlFile << "      <parameter name=\"unit\">" << "millimeter" << "</parameter>\n";
				xmlFile << "    </genistreamtraits>\n";

			}
			else
			{
				xmlFile << "    <sensorrangetraits>\n";
				xmlFile << "      <parameter name=\"fov x0\">" << std::to_string(0) << "</parameter>\n";
				xmlFile << "      <parameter name=\"fov x1\">" << std::to_string(m_info.m_RI.aoiWidth - 1) << "</parameter>\n";
				xmlFile << "      <parameter name=\"fov x2\">" << std::to_string(0) << "</parameter>\n";
				xmlFile << "      <parameter name=\"fov x3\">" << std::to_string(m_info.m_RI.aoiWidth - 1) << "</parameter>\n";
				xmlFile << "      <parameter name=\"fov z0\">" << std::to_string(1) << "</parameter>\n";
				xmlFile << "      <parameter name=\"fov z1\">" << std::to_string(1) << "</parameter>\n";
				xmlFile << "      <parameter name=\"fov z2\">" << std::to_string(m_info.m_RI.aoiHeight * 16 - 1) << "</parameter>\n";
				xmlFile << "      <parameter name=\"fov z3\">" << std::to_string(m_info.m_RI.aoiHeight * 16 - 1) << "</parameter>\n";
				xmlFile << "      <parameter name=\"origin x\">" << std::to_string(0) << "</parameter>\n";
				xmlFile << "      <parameter name=\"scale x\">" << std::to_string(1) << "</parameter>\n";
				xmlFile << "      <parameter name=\"origin z\">" << std::to_string(m_info.m_RI.m_RangeAxis == RangeAxis::STANDARD ? m_info.m_RI.aoiOffsetY : m_info.m_RI.aoiOffsetY + m_info.m_RI.aoiHeight) << "</parameter>\n";
				xmlFile << "      <parameter name=\"scale z\">" << std::to_string(m_info.m_RI.xml_scale_z) << "</parameter>\n";
				xmlFile << "    </sensorrangetraits>\n";
			}
		}

		for (auto sub : dataNames)
		{
			if (sub == DN::R || sub == DN::G || sub == DN::B || sub == DN::WHITE)
				continue;

			if (has(sub))
			{
				xmlFile << "    <subcomponent name=\"" << _DN2Name(sub) << "\" valuetype=\"" << _DN2ValueType(sub) << "\">\n";
				xmlFile << "      <parameter name=\"size\">" << std::to_string(_DN2RowSize(sub)) << "</parameter>\n";
				xmlFile << "      <parameter name=\"width\">" << std::to_string(_DN2RowSize(sub) / _DN2PixelSize(sub)) << "</parameter>\n";
				xmlFile << "    </subcomponent>\n";
			}
		}

		xmlFile << "  </component>\n";

		// 高精度灰
		if (has(DN::WHITE))
		{
			xmlFile << "  <component name=\"Ranger3Hires\" valuetype=\"Hires\">\n";
			xmlFile << "    <parameter name=\"size\">5120</parameter>\n";
			xmlFile << "    <parameter name=\"height\">1</parameter>\n";
			xmlFile << "    <genistreamtraits>\n";
			xmlFile << "      <parameter name=\"region id\">hires extraction white</parameter>\n";
			xmlFile << "      <parameter name=\"width\">5120</parameter>\n";
			xmlFile << "      <parameter name=\"height\">1</parameter>\n";
			xmlFile << "      <parameter name=\"offset X\">0</parameter>\n";
			xmlFile << "      <parameter name=\"offset Y\">0</parameter>\n";
			xmlFile << "    </genistreamtraits>\n";
			xmlFile << "    <subcomponent name=\"HiresWhite\" valuetype=\"BYTE\">\n";
			xmlFile << "      <parameter name=\"size\">5120</parameter>\n";
			xmlFile << "      <parameter name=\"width\">5120</parameter>\n";
			xmlFile << "    </subcomponent>\n";
			xmlFile << "  </component>\n";
		}

		// 彩色图像组件
		if (has(DN::R) && has(DN::G) && has(DN::B))
		{
			xmlFile << "  <component name=\"Ranger3Hires\" valuetype=\"Hires\">\n";
			xmlFile << "    <parameter name=\"size\">15360</parameter>\n";
			xmlFile << "    <parameter name=\"height\">1</parameter>\n";
			xmlFile << "    <genistreamtraits>\n";
			xmlFile << "      <parameter name=\"region id\">hires extraction color</parameter>\n";
			xmlFile << "      <parameter name=\"width\">5120</parameter>\n";
			xmlFile << "      <parameter name=\"height\">1</parameter>\n";
			xmlFile << "      <parameter name=\"offset X\">0</parameter>\n";
			xmlFile << "      <parameter name=\"offset Y\">0</parameter>\n";
			xmlFile << "    </genistreamtraits>\n";
			xmlFile << "    <subcomponent name=\"HiresRed\" valuetype=\"BYTE\">\n";
			xmlFile << "      <parameter name=\"size\">5120</parameter>\n";
			xmlFile << "      <parameter name=\"width\">5120</parameter>\n";
			xmlFile << "    </subcomponent>\n";
			xmlFile << "    <subcomponent name=\"HiresGreen\" valuetype=\"BYTE\">\n";
			xmlFile << "      <parameter name=\"size\">5120</parameter>\n";
			xmlFile << "      <parameter name=\"width\">5120</parameter>\n";
			xmlFile << "    </subcomponent>\n";
			xmlFile << "    <subcomponent name=\"HiresBlue\" valuetype=\"BYTE\">\n";
			xmlFile << "      <parameter name=\"size\">5120</parameter>\n";
			xmlFile << "      <parameter name=\"width\">5120</parameter>\n";
			xmlFile << "    </subcomponent>\n";
			xmlFile << "  </component>\n";
		}



		xmlFile << "</icon_data_format>";
		xmlFile.close();
	}

	bool
		ImgT::_saveBuffer(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated) const
	{
		// data collection, raw data only
		if (_dataNames.empty()) return false;

		// check path format 
		std::string filePath;
		if (_filePath.size() < 4)
			filePath = _filePath;
		else
			filePath = _filePath.substr(_filePath.size() - 3, 3); // suffix

		if (filePath == "xml" || filePath == "XML" || filePath == "dat" || filePath == "DAT")
			filePath = _filePath.substr(0, _filePath.size() - 4);
		else
			filePath = _filePath;

		// write data
		size_t rowSize(0);
		int Ranger3RangeSize = 0;

		std::vector<DataNames> dataNames;
		std::ofstream iconFile(filePath + ".dat", std::ios::binary);
		for (auto sub : _dataNames)
		{
			if (has(sub))
			{
				size_t subSize =
					static_cast<const size_t>(
						(sub == DataNames::MAR) ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(sub)
						); // Mark data has the fixed 5 columns, data type is uint32_t.

				Ranger3RangeSize += subSize;

				if (sub == DN::R || sub == DN::G || sub == DN::B || sub == DN::WHITE)
				{
					Ranger3RangeSize -= subSize;
					subSize = 5120;
				}

				iconFile.write(reinterpret_cast<const char*>(getData(sub)), get_rows() * subSize);
				rowSize += subSize;
				dataNames.push_back(sub);
			}
		}

		// add genirangertraits ----------
		// write xml
		_writeXML(filePath, dataNames, rowSize, Ranger3RangeSize, is_calibrated);

		return true;
	}




	bool
		ImgT::_saveBufferByMapping(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated) const
	{
#ifdef __linux__
		return _saveBuffer(_dataNames, _filePath, is_calibrated);
#else //_WIN32
		// data collection, raw data only
		if (_dataNames.empty()) return false;

		// check path format 
		std::string filePath;
		if (_filePath.size() < 4)
			filePath = _filePath;
		else
			filePath = _filePath.substr(_filePath.size() - 3, 3); // suffix

		if (filePath == "xml" || filePath == "XML" || filePath == "dat" || filePath == "DAT")
			filePath = _filePath.substr(0, _filePath.size() - 4);
		else
			filePath = _filePath;

		// write data
		size_t rowSize(0);
		size_t Ranger3RangeSize = 0;

		std::vector<DataNames> dataNames;
		std::vector<size_t> dataRowSizes;
		for (auto sub : _dataNames)
		{
			if (has(sub))
			{
				size_t subSize = static_cast<const size_t>((sub == DataNames::MAR) ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(sub));
				Ranger3RangeSize += subSize;

				if (sub == DN::R || sub == DN::G || sub == DN::B || sub == DN::WHITE)
				{
					Ranger3RangeSize -= subSize;
					subSize = 5120;
				}


				dataRowSizes.emplace_back(subSize);
				rowSize += subSize;
				dataNames.emplace_back(sub);
			}
		}

		//auto beg = cv::getTickCount();

		// done : check _saveByMapping
		{
			HANDLE fh, MapFileH, MapHandle;
			fh = CreateFileA((filePath + ".dat").c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (fh == INVALID_HANDLE_VALUE)
			{
				std::cerr << "[_saveBufferByMapping()] Error in CreateFile! Path = \"" << filePath << "\"" << std::endl;
				return false;
			}

			MapFileH = CreateFileMappingA(fh, NULL, PAGE_READWRITE, 0, static_cast<DWORD>(get_rows() * rowSize), NULL);
			if (MapFileH == NULL)
			{
				std::cerr << "[_saveBufferByMapping()] Error in CreateFileMapping! Create size(Byte) = " << get_rows() * rowSize << std::endl;
				return false;
			}

			MapHandle = MapViewOfFile(MapFileH, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (MapHandle == NULL)
			{
				return false;
			}
			else
			{
				char* pDst = (char*)MapHandle;
				for (int i = 0; i < dataRowSizes.size(); ++i)
				{
					auto cmpSize = dataRowSizes[i] * get_rows();
					std::memcpy(pDst, reinterpret_cast<char*>(getData(dataNames[i])), cmpSize);
					pDst = pDst + cmpSize;
				}
			}

			// release mapping
			UnmapViewOfFile(MapHandle);
			CloseHandle(MapFileH);
			CloseHandle(fh);
		}

		//auto end = cv::getTickCount();
		//std::cout << get_ID() << ", saving time = " << (end - beg) / cv::getTickFrequency() << " second. " << std::endl;

		// write xml
		_writeXML(filePath, dataNames, rowSize, Ranger3RangeSize, is_calibrated);
		return true;
#endif
	}



	//	bool
	//		ImgT::_saveBuffer_old(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated) const
	//	{
	//		// data collection, raw data only
	//		if (_dataNames.empty()) return false;
	//
	//		// check path format 
	//		std::string filePath;
	//		if (_filePath.size() < 4)
	//			filePath = _filePath;
	//		else
	//			filePath = _filePath.substr(_filePath.size() - 3, 3); // suffix
	//
	//		if (filePath == "xml" || filePath == "XML" || filePath == "dat" || filePath == "DAT")
	//			filePath = _filePath.substr(0, _filePath.size() - 4);
	//		else
	//			filePath = _filePath;
	//
	//		// write data
	//		std::ofstream iconFile(filePath + ".dat", std::ios::binary);
	//		size_t rowSize(0);
	//		std::vector<DataNames> dataNames;
	//		for (auto sub : _dataNames)
	//		{
	//			if (has(sub))
	//			{
	//				size_t subSize =
	//					static_cast<const size_t>(
	//						(sub == DataNames::MAR) ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(sub)
	//						); // Mark data has the fixed 5 columns, data type is uint32_t.
	//
	//				iconFile.write(reinterpret_cast<const char*>(getData(sub)), get_rows() * subSize);
	//				rowSize += subSize;
	//				dataNames.push_back(sub);
	//			}
	//		}
	//
	//		// add genirangertraits ----------
	//		// write xml
	//		std::ofstream xmlFile(filePath + ".xml");
	//
	//		if (dataNames.size() == 1 && dataNames[0] == DataNames::SEN)
	//		{
	//			xmlFile << "<icon_data_format name=\"IMAGE\" valuetype=\"IMAGE\">\n";
	//			xmlFile << "  <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
	//			xmlFile << "  <parameter name=\"version\">1</parameter>\n";
	//			xmlFile << "  <parameter name=\"layout\">SCAN</parameter>\n";
	//			xmlFile << "  <component valuetype=\"image\" name=\"Ranger3Image\">\n";
	//			xmlFile << "    <parameter name=\"size\">" << std::to_string(m_info.m_SI.cols) << "</parameter>\n";
	//			xmlFile << "    <parameter name=\"height\">" << std::to_string(m_info.m_SI.rows) << "</parameter>\n";
	//			xmlFile << "    <imagetraits>\n";
	//			xmlFile << "      <parameter name=\"size x\">" << std::to_string(m_info.m_SI.cols) << "</parameter>\n";
	//			xmlFile << "      <parameter name=\"size y\">" << std::to_string(m_info.m_SI.rows) << "</parameter>\n";
	//			xmlFile << "      <parameter name=\"offset x\">0</parameter>\n";
	//			xmlFile << "      <parameter name=\"offset y\">0</parameter>\n";
	//			xmlFile << "    </imagetraits>\n";
	//		}
	//		else
	//		{
	//			xmlFile << "<icon_data_format name=\"\" valuetype=\"\">\n";
	//			xmlFile << "  <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
	//			xmlFile << "  <parameter name=\"version\">1</parameter>\n";
	//			xmlFile << "  <parameter name=\"layout\">SUBCOMPONENT</parameter>\n";
	//			xmlFile << "  <component name=\"Ranger3Range\" valuetype=\"Hi3D\">\n";
	//			xmlFile << "    <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
	//			xmlFile << "    <parameter name=\"height\">1</parameter>\n";
	//
	//			if (is_calibrated)
	//			{
	//				xmlFile << "    <worldrangetraits>\n";
	//				xmlFile << "      <parameter name=\"lower bound x\">" << std::to_string(m_info.m_CI.lower_bound_x) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"lower bound r\">" << std::to_string(m_info.m_CI.lower_bound_r) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"upper bound x\">" << std::to_string(m_info.m_CI.upper_bound_x) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"upper bound r\">" << std::to_string(m_info.m_CI.upper_bound_r) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"coordinate unit\"></parameter>\n";
	//				xmlFile << "    </worldrangetraits>\n";
	//
	//				xmlFile << "    <genistreamtraits>\n";
	//				xmlFile << "      <parameter name=\"region id\">scan 3d extraction 1</parameter>\n";
	//				xmlFile << "      <parameter name=\"extraction method\">hi 3d</parameter>\n";
	//				xmlFile << "      <parameter name=\"output mode\">" << m_info.m_CI.genistreamtraits.output_mode << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"width\">" << std::to_string(m_info.m_CI.genistreamtraits.width) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"height\">" << std::to_string(m_info.m_CI.genistreamtraits.height) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"offset X\">" << std::to_string(m_info.m_CI.genistreamtraits.offset_X) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"offset Y\">" << std::to_string(m_info.m_CI.genistreamtraits.offset_Y) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"a axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_scale) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_offset) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range min\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_min) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range max\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_max) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range missing\">" << m_info.m_CI.genistreamtraits.a_axis_range_missing.c_str() << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_missing_value) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"b axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_scale) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_offset) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range min\">" << m_info.m_CI.genistreamtraits.b_axis_range_min << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range max\">" << m_info.m_CI.genistreamtraits.b_axis_range_max << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range missing\">" << m_info.m_CI.genistreamtraits.b_axis_range_missing << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_missing_value) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"c axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_scale) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_offset) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range min\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_min) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range max\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_max) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range missing\">" << m_info.m_CI.genistreamtraits.c_axis_range_missing << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_missing_value) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"unit\">" << "millimeter" << "</parameter>\n";
	//				xmlFile << "    </genistreamtraits>\n";
	//
	//			}
	//			else
	//			{
	//				xmlFile << "    <sensorrangetraits>\n";
	//				xmlFile << "      <parameter name=\"fov x0\">" << std::to_string(0) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov x1\">" << std::to_string(m_info.m_RI.aoiWidth - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov x2\">" << std::to_string(0) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov x3\">" << std::to_string(m_info.m_RI.aoiWidth - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z0\">" << std::to_string(1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z1\">" << std::to_string(1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z2\">" << std::to_string(m_info.m_RI.aoiHeight * 16 - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z3\">" << std::to_string(m_info.m_RI.aoiHeight * 16 - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"origin x\">" << std::to_string(0) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"scale x\">" << std::to_string(1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"origin z\">" << std::to_string(m_info.m_RI.m_RangeAxis == RangeAxis::STANDARD ? m_info.m_RI.aoiOffsetY : m_info.m_RI.aoiOffsetY + m_info.m_RI.aoiHeight) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"scale z\">" << std::to_string(m_info.m_RI.xml_scale_z) << "</parameter>\n";
	//				xmlFile << "    </sensorrangetraits>\n";
	//			}
	//		}
	//
	//		for (auto sub : dataNames)
	//		{
	//			if (has(sub))
	//			{
	//				xmlFile << "    <subcomponent name=\"" << _DN2Name(sub) << "\" valuetype=\"" << _DN2ValueType(sub) << "\">\n";
	//				xmlFile << "      <parameter name=\"size\">" << std::to_string(_DN2RowSize(sub)) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"width\">" << std::to_string(_DN2RowSize(sub) / _DN2PixelSize(sub)) << "</parameter>\n";
	//				xmlFile << "    </subcomponent>\n";
	//			}
	//		}
	//
	//		xmlFile << "  </component>\n";
	//		xmlFile << "</icon_data_format>";
	//		xmlFile.close();
	//
	//		return true;
	//	}
	//
	//	bool
	//		ImgT::_saveBufferByMapping_old(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated) const
	//	{
	//#ifdef __linux__
	//		return _saveBuffer(_dataNames, _filePath, is_calibrated);
	//#else //_WIN32
	//		// data collection, raw data only
	//		if (_dataNames.empty()) return false;
	//
	//		// check path format 
	//		std::string filePath;
	//		if (_filePath.size() < 4)
	//			filePath = _filePath;
	//		else
	//			filePath = _filePath.substr(_filePath.size() - 3, 3); // suffix
	//
	//		if (filePath == "xml" || filePath == "XML" || filePath == "dat" || filePath == "DAT")
	//			filePath = _filePath.substr(0, _filePath.size() - 4);
	//		else
	//			filePath = _filePath;
	//
	//		// write data
	//		size_t rowSize(0);
	//		std::vector<DataNames> dataNames;
	//		std::vector<size_t> dataRowSizes;
	//		for (auto sub : _dataNames)
	//		{
	//			if (has(sub))
	//			{
	//				size_t subSize = static_cast<const size_t>((sub == DataNames::MAR) ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(sub));
	//				dataRowSizes.emplace_back(subSize);
	//				rowSize += subSize;
	//				dataNames.emplace_back(sub);
	//			}
	//		}
	//
	//		//auto beg = cv::getTickCount();
	//
	//		// done : check _saveByMapping
	//		{
	//			HANDLE fh, MapFileH, MapHandle;
	//			fh = CreateFileA((filePath + ".dat").c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//			if (fh == INVALID_HANDLE_VALUE)
	//			{
	//				std::cerr << "[_saveBufferByMapping()] Error in CreateFile! Path = \"" << filePath << "\"" << std::endl;
	//				return false;
	//			}
	//
	//			MapFileH = CreateFileMappingA(fh, NULL, PAGE_READWRITE, 0, static_cast<DWORD>(get_rows() * rowSize), NULL);
	//			if (MapFileH == NULL)
	//			{
	//				std::cerr << "[_saveBufferByMapping()] Error in CreateFileMapping! Create size(Byte) = " << get_rows() * rowSize << std::endl;
	//				return false;
	//			}
	//
	//			MapHandle = MapViewOfFile(MapFileH, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	//			if (MapHandle == NULL)
	//			{
	//				return false;
	//			}
	//			else
	//			{
	//				char* pDst = (char*)MapHandle;
	//				for (int i = 0; i < dataRowSizes.size(); ++i)
	//				{
	//					auto cmpSize = dataRowSizes[i] * get_rows();
	//					std::memcpy(pDst, reinterpret_cast<char*>(getData(dataNames[i])), cmpSize);
	//					pDst = pDst + cmpSize;
	//				}
	//			}
	//
	//			// release mapping
	//			UnmapViewOfFile(MapHandle);
	//			CloseHandle(MapFileH);
	//			CloseHandle(fh);
	//		}
	//
	//		//auto end = cv::getTickCount();
	//		//std::cout << get_ID() << ", saving time = " << (end - beg) / cv::getTickFrequency() << " second. " << std::endl;
	//
	//		// write xml
	//		std::ofstream xmlFile(filePath + ".xml");
	//
	//		if (dataNames.size() == 1 && dataNames[0] == DataNames::SEN)
	//		{
	//			xmlFile << "<icon_data_format name=\"IMAGE\" valuetype=\"IMAGE\">\n";
	//			xmlFile << "  <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
	//			xmlFile << "  <parameter name=\"version\">1</parameter>\n";
	//			xmlFile << "  <parameter name=\"layout\">SCAN</parameter>\n";
	//			xmlFile << "  <component valuetype=\"image\" name=\"Ranger3Image\">\n";
	//			xmlFile << "    <parameter name=\"size\">" << std::to_string(m_info.m_SI.cols) << "</parameter>\n";
	//			xmlFile << "    <parameter name=\"height\">" << std::to_string(m_info.m_SI.rows) << "</parameter>\n";
	//			xmlFile << "    <imagetraits>\n";
	//			xmlFile << "      <parameter name=\"size x\">" << std::to_string(m_info.m_SI.cols) << "</parameter>\n";
	//			xmlFile << "      <parameter name=\"size y\">" << std::to_string(m_info.m_SI.rows) << "</parameter>\n";
	//			xmlFile << "      <parameter name=\"offset x\">0</parameter>\n";
	//			xmlFile << "      <parameter name=\"offset y\">0</parameter>\n";
	//			xmlFile << "    </imagetraits>\n";
	//		}
	//		else
	//		{
	//			xmlFile << "<icon_data_format name=\"\" valuetype=\"\">\n";
	//			xmlFile << "  <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
	//			xmlFile << "  <parameter name=\"version\">1</parameter>\n";
	//			xmlFile << "  <parameter name=\"layout\">SUBCOMPONENT</parameter>\n";
	//			xmlFile << "  <component name=\"Ranger3Range\" valuetype=\"Hi3D\">\n";
	//			xmlFile << "    <parameter name=\"size\">" << std::to_string(rowSize) << "</parameter>\n";
	//			xmlFile << "    <parameter name=\"height\">1</parameter>\n";
	//
	//			if (is_calibrated)
	//			{
	//				xmlFile << "    <worldrangetraits>\n";
	//				xmlFile << "      <parameter name=\"lower bound x\">" << std::to_string(m_info.m_CI.lower_bound_x) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"lower bound r\">" << std::to_string(m_info.m_CI.lower_bound_r) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"upper bound x\">" << std::to_string(m_info.m_CI.upper_bound_x) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"upper bound r\">" << std::to_string(m_info.m_CI.upper_bound_r) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"coordinate unit\"></parameter>\n";
	//				xmlFile << "    </worldrangetraits>\n";
	//
	//				xmlFile << "    <genistreamtraits>\n";
	//				xmlFile << "      <parameter name=\"region id\">scan 3d extraction 1</parameter>\n";
	//				xmlFile << "      <parameter name=\"extraction method\">hi 3d</parameter>\n";
	//				xmlFile << "      <parameter name=\"output mode\">" << m_info.m_CI.genistreamtraits.output_mode << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"width\">" << std::to_string(m_info.m_CI.genistreamtraits.width) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"height\">" << std::to_string(m_info.m_CI.genistreamtraits.height) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"offset X\">" << std::to_string(m_info.m_CI.genistreamtraits.offset_X) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"offset Y\">" << std::to_string(m_info.m_CI.genistreamtraits.offset_Y) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"a axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_scale) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_offset) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range min\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_min) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range max\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_max) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range missing\">" << m_info.m_CI.genistreamtraits.a_axis_range_missing.c_str() << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"a axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.a_axis_range_missing_value) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"b axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_scale) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_offset) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range min\">" << m_info.m_CI.genistreamtraits.b_axis_range_min << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range max\">" << m_info.m_CI.genistreamtraits.b_axis_range_max << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range missing\">" << m_info.m_CI.genistreamtraits.b_axis_range_missing << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"b axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.b_axis_range_missing_value) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"c axis range scale\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_scale) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range offset\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_offset) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range min\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_min) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range max\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_max) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range missing\">" << m_info.m_CI.genistreamtraits.c_axis_range_missing << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"c axis range missing value\">" << std::to_string(m_info.m_CI.genistreamtraits.c_axis_range_missing_value) << "</parameter>\n";
	//
	//				xmlFile << "      <parameter name=\"unit\">" << "millimeter" << "</parameter>\n";
	//				xmlFile << "    </genistreamtraits>\n";
	//			}
	//			else
	//			{
	//				xmlFile << "    <sensorrangetraits>\n";
	//				xmlFile << "      <parameter name=\"fov x0\">" << std::to_string(0) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov x1\">" << std::to_string(m_info.m_RI.aoiWidth - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov x2\">" << std::to_string(0) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov x3\">" << std::to_string(m_info.m_RI.aoiWidth - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z0\">" << std::to_string(1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z1\">" << std::to_string(1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z2\">" << std::to_string(m_info.m_RI.aoiHeight * 16 - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"fov z3\">" << std::to_string(m_info.m_RI.aoiHeight * 16 - 1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"origin x\">" << std::to_string(0) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"scale x\">" << std::to_string(1) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"origin z\">" << std::to_string(m_info.m_RI.m_RangeAxis == RangeAxis::STANDARD ? m_info.m_RI.aoiOffsetY : m_info.m_RI.aoiOffsetY + m_info.m_RI.aoiHeight) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"scale z\">" << std::to_string(m_info.m_RI.xml_scale_z) << "</parameter>\n";
	//				xmlFile << "    </sensorrangetraits>\n";
	//			}
	//		}
	//
	//		for (auto sub : dataNames)
	//		{
	//			if (has(sub))
	//			{
	//				xmlFile << "    <subcomponent name=\"" << _DN2Name(sub) << "\" valuetype=\"" << _DN2ValueType(sub) << "\">\n";
	//				xmlFile << "      <parameter name=\"size\">" << std::to_string(_DN2RowSize(sub)) << "</parameter>\n";
	//				xmlFile << "      <parameter name=\"width\">" << std::to_string(_DN2RowSize(sub) / _DN2PixelSize(sub)) << "</parameter>\n";
	//				xmlFile << "    </subcomponent>\n";
	//			}
	//		}
	//
	//		xmlFile << "  </component>\n";
	//		xmlFile << "</icon_data_format>";
	//		xmlFile.close();
	//		return true;
	//#endif
	//	}

	const std::string
		ImgT::_DN2ValueType(const DN& _dn)const
	{
		switch (_dn)
		{
		case SickCam::INV:			return "";
		case SickCam::SEN:			return "BYTE";
		case SickCam::RAN:			return "WORD";
		case SickCam::REF:			return "BYTE";
		case SickCam::SCA_8:		return "BYTE";
		case SickCam::SCA_16:		return "WORD";
		case SickCam::MAR:			return "INT";
		case SickCam::RAN_CAL:		return "FLOAT";
		case SickCam::RAN_CAL_16:	return "WORD";
		case SickCam::REF_CAL:		return "BYTE";
		case SickCam::REF_CAL_F:	return "FLOAT";
		case SickCam::SCA_8_CAL:	return "BYTE";
		case SickCam::SCA_16_CAL:	return "WORD";
		case SickCam::RAN2:			return "WORD";
		case SickCam::REF2:			return "BYTE";
		case SickCam::SCA2_8:		return "BYTE";
		case SickCam::SCA2_16:		return "WORD";
		case SickCam::RAN2_CAL:		return "FLOAT";
		case SickCam::REF2_CAL:		return "BYTE";
		case SickCam::SCA2_8_CAL:	return "BYTE";
		case SickCam::SCA2_16_CAL:	return "WORD";
		case SickCam::RAN_X:		return "FLOAT";
		case SickCam::RAN_X_16:		return "WORD";

		case SickCam::WHITE:	return "BYTE";
		case SickCam::R:		return "BYTE";
		case SickCam::G:		return "BYTE";
		case SickCam::B:		return "BYTE";
		}
		return "";
	}

	const std::string
		ImgT::_DN2Name(const DN& _dn)const
	{
		switch (_dn)
		{
		case SickCam::INV:			return "";
		case SickCam::SEN:			return "Image";
		case SickCam::RAN:			return "Range";
		case SickCam::REF:			return "Intensity";
		case SickCam::SCA_8:		return "Scatter";
		case SickCam::SCA_16:		return "Scatter";
		case SickCam::MAR:			return "Mark";
		case SickCam::RAN_CAL:		return "Range";
		case SickCam::RAN_CAL_16:	return "Range";
		case SickCam::REF_CAL:		return "Intensity";
		case SickCam::REF_CAL_F:	return "Intensity";
		case SickCam::SCA_8_CAL:	return "Scatter";
		case SickCam::SCA_16_CAL:	return "Scatter";
		case SickCam::RAN2:			return "Range";
		case SickCam::REF2:			return "Intensity";
		case SickCam::SCA2_8:		return "Scatter";
		case SickCam::SCA2_16:		return "Scatter";
		case SickCam::RAN2_CAL:		return "Range";
		case SickCam::REF2_CAL:		return "Intensity";
		case SickCam::SCA2_8_CAL:	return "Scatter";
		case SickCam::SCA2_16_CAL:	return "Scatter";
		case SickCam::RAN_X:		return "RangeA";
		case SickCam::RAN_X_16:		return "RangeA";

		case SickCam::WHITE:	return "HiresWhite";
		case SickCam::R:		return "HiresRed";
		case SickCam::G:		return "HiresGreen";
		case SickCam::B:		return "HiresBlue";
		}
		return "";
	}

	size_t
		ImgT::_DN2PixelSize(const DN& _dn)const
	{
		switch (_dn)
		{
		case SickCam::INV:			return 0;
		case SickCam::SEN:			return sizeof(uint8_t);
		case SickCam::RAN:			return sizeof(uint16_t);
		case SickCam::REF:			return sizeof(uint8_t);
		case SickCam::SCA_8:		return sizeof(uint8_t);
		case SickCam::SCA_16:		return sizeof(uint16_t);
		case SickCam::MAR:			return sizeof(uint32_t);
		case SickCam::RAN_CAL:		return sizeof(float);
		case SickCam::RAN_CAL_16:	return sizeof(uint16_t);
		case SickCam::REF_CAL:		return sizeof(uint8_t);
		case SickCam::REF_CAL_F:	return sizeof(float);
		case SickCam::SCA_8_CAL:	return sizeof(uint8_t);
		case SickCam::SCA_16_CAL:	return sizeof(uint16_t);
		case SickCam::RAN2:			return sizeof(uint16_t);
		case SickCam::REF2:			return sizeof(uint8_t);
		case SickCam::SCA2_8:		return sizeof(uint8_t);
		case SickCam::SCA2_16:		return sizeof(uint16_t);
		case SickCam::RAN2_CAL:		return sizeof(float);
		case SickCam::REF2_CAL:		return sizeof(uint8_t);
		case SickCam::SCA2_8_CAL:	return sizeof(uint8_t);
		case SickCam::SCA2_16_CAL:	return sizeof(uint16_t);
		case SickCam::RAN_X:		return sizeof(float);
		case SickCam::RAN_X_16:		return sizeof(uint16_t);

			//case SickCam::WHITE:		return sizeof(uint16_t);
			//case SickCam::R:			return sizeof(uint16_t);
			//case SickCam::G:			return sizeof(uint16_t);
			//case SickCam::B:			return sizeof(uint16_t);

		case SickCam::WHITE:		return sizeof(uint8_t);
		case SickCam::R:			return sizeof(uint8_t);
		case SickCam::G:			return sizeof(uint8_t);
		case SickCam::B:			return sizeof(uint8_t);
		}
		return 0;
	}

	size_t
		ImgT::_DN2RowSize(const DN& _dn)const
	{
		return _dn == SickCam::MAR ? CHUNK_DATA_SIZE : get_cols() * _DN2PixelSize(_dn);
	}

	void
		ImgT::_bufferToChunkInfo(uint8_t* pd, const int& nRows)
	{
		m_info.m_ChunkData = ChunkData(nRows);

		uint32_t* pd32 = (uint32_t*)pd;

#pragma omp parallel for
		for (int i = 0; i < nRows; ++i)
		{
			Ranger3LineMetadata lineData;
			uint32_t* pd32_t = pd32;
			pd32_t += (i * CHUNK_DATA_WIDTH);
			lineData.encoderValue = pd32_t[0];
			lineData.overtriggerCount = static_cast<uint8_t>  ((pd32_t[1] >> STATUS_BIT_OVERTRIG) & 0xff);
			lineData.encoderResetActive = static_cast<bool>     ((pd32_t[1] >> STATUS_BIT_ENCODER_RESET) & 1);
			lineData.lineTriggerActive = static_cast<bool>     ((pd32_t[1] >> STATUS_BIT_LINE_TRIGGER) & 1);
			lineData.frameTriggerActive = static_cast<bool>     ((pd32_t[1] >> STATUS_BIT_ENABLE) & 1);
			lineData.encoderA = static_cast<bool>     ((pd32_t[1] >> STATUS_BIT_ENCODER_A) & 1);
			lineData.encoderB = static_cast<bool>     ((pd32_t[1] >> STATUS_BIT_ENCODER_B) & 1);

			lineData.timestamp = (static_cast<uint64_t>(pd32_t[2])         /* Low  */ +
				(static_cast<uint64_t>(pd32_t[3]) << 32)  /* High */);

			//pd32 += CHUNK_DATA_WIDTH;
			m_info.m_ChunkData[i] = lineData;
		}
	}

}

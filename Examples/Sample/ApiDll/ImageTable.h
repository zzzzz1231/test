/**	@file	ImageTable.h
*
*	@brief	Class ImgT Head File.
*
*	@attention
*	All supported image data formats are listed in DataNames.
*
*
*	@copyright	Copyright 2016-2026 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@time		2026/5/13
*
*/

#pragma once
#ifndef EZR_EXPORTS
#include "Typedef.h"
#include "tinyxml2/tinyxml2.h"

//#ifdef WIN32

#ifndef DISABLE_CAL_IN_PC
#include "../CalibrationWrapper/CalibrationWrapper.h"

#ifndef DISABLE_R380 // RANGER380
#include "ColorImageRun.hpp"
#endif // DISABLE_R380

#endif // DISABLE_CAL_IN_PC


//#endif // WIN32
#endif


#ifdef EZR_EXPORTS
#define EXPORT_TO_DLL

#include "./IO/Ranger3LineMetadata.h"
#include "tinyxml2/tinyxml2.h"
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>


#define CHUNK_DATA_SIZE 20
#define CHUNK_DATA_WIDTH 5

using ChunkData = std::vector<Ranger3LineMetadata>;


#endif

#define HIRES_IMG_WIDTH 5120
#define HANDLE_NUM 64

namespace SickCam
{
	//////////////////////////////// from SaveBuffer.cpp ///////////////////////////////////////

	const size_t STATUS_BIT_ENABLE = 30;
	const size_t STATUS_BIT_ENCODER_A = 28;
	const size_t STATUS_BIT_ENCODER_B = 27;
	const size_t STATUS_BIT_OVERTRIG = 16;
	const size_t STATUS_BIT_LINE_TRIGGER = 25;
	const size_t STATUS_BIT_ENCODER_RESET = 24;

	////////////////////////////////////////////////////////////////////////////////




	/** @brief 输出图像的类型。 Names of output Datas. */
#ifndef SWIG
	typedef
#endif
		enum DataNames
	{
		INV = 0,		/**< INVALID	*	| INVALID		| 无效值					| INVALID														*/
		SEN,			/**< uint8_t	*	| [---]			| 激光线图像				| SENSOR														*/

		RAN,			/**< uint16_t	*	| [Raw]			| 高度图像				| ComponentSelector_Range_______RegionSelector_Scan3dExtraction1*/
		REF,			/**< uint8_t	*	| [Raw]			| 激光强度图像			| ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1*/
		SCA_8,			/**< uint8_t	*	| [Raw]			| 激光散射图像			| 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1*/
		SCA_16,			/**< uint16_t	*	| [Raw]			| 激光散射图像			| 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1*/

		MAR,			/**< uint32_t	*	| [---]			| 编码器信息				| Encoder_information */
		RAN_CAL,		/**< float		*	| [Calibrated]	| 标定后的高度图像			| ComponentSelector_Range_______RegionSelector_Scan3dExtraction1*/
		RAN_CAL_16,		/**< uint16_t	*	| [Calibrated]	| 标定后的高度图像			| ComponentSelector_Range_______RegionSelector_Scan3dExtraction1*/
		REF_CAL,		/**< uint8_t	*	| [Calibrated]	| 标定后的激光强度图像		| ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1*/
		REF_CAL_F,		/**< 弃用		*	| [弃用]			| 弃用					| 弃用															*/
		SCA_8_CAL,		/**< uint8_t	*	| [Calibrated]	| 标定后的激光散射图像		| 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1*/
		SCA_16_CAL,		/**< uint16_t	*	| [Calibrated]	| 标定后的激光散射图像		| 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1*/

		RAN2,			/**< uint16_t	*	| [Raw]			| 高度图像 2				| ComponentSelector_Range_______RegionSelector_Scan3dExtraction2*/
		REF2,			/**< uint8_t	*	| [Raw]			| 激光强度图像 2			| ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2*/
		SCA2_8,			/**< uint8_t	*	| [Raw]			| 激光散射图像 2			| 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2*/
		SCA2_16,		/**< uint16_t	*	| [Raw]			| 激光散射图像 2 			| 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2*/

		RAN2_CAL,		/**< float		*	| [Calibrated]	| 标定后的高度图像 2		| ComponentSelector_Range_______RegionSelector_Scan3dExtraction2*/
		REF2_CAL,		/**< uint8_t	*	| [Calibrated]	| 标定后的激光强度图像 2	| ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2*/
		SCA2_8_CAL,		/**< uint8_t    *	| [Calibrated]	| 标定后的激光散射图像 2	| 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2*/
		SCA2_16_CAL,	/**< uint16_t   *	| [Calibrated]	| 标定后的激光散射图像 2	| 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2*/

		RAN_X,			/**< float		*	| [Calibrated]  | 仅标定，无校正，X 数据。 | Float32_RangeA */
		RAN_X_16,       /**< uint16_t	*	| [Calibrated]  | 仅标定，无校正，X 数据。 | 16_Byte_RangeA */

		WHITE,			/**< uint16_t	*	| [Raw]			| 高精度灰度图像			| White */
		R,				/**< uint16_t	*	| [Raw]			| 彩色图像 Red 通道		| R */
		G,				/**< uint16_t	*	| [Raw]			| 彩色图像 Red 通道		| G */
		B,				/**< uint16_t	*	| [Raw]			| 彩色图像 Red 通道		| B */


		/*
		如有修改，请同步修改 C# 中定义
		*/

	}
#ifndef SWIG
	DN
#endif
		;

	typedef std::map<DataNames, void*>		ImgTData;

	/** @brief 激光线图像数据信息  Sensor image data information. */
	struct EXPORT_TO_DLL SensoInfo
	{
		SensoInfo()
			: cols(0), rows(0), senOffsetX(0), senOffsetY(0)
		{}


#ifndef SWIG
		bool operator == (const SensoInfo& sen)
		{
			return this->cols == sen.cols
				&& this->rows == sen.rows
				&& this->senOffsetX == sen.senOffsetX
				&& this->senOffsetY == sen.senOffsetY;
		}
#endif

		uint32_t	cols = 2560;				/**< sensor 图像宽度。Width of sensor image */
		uint32_t	rows = 832;				/**< sensor 图像高度。Height of sensor image. */
		uint32_t	senOffsetX = 0;			/**< sensor 图像起始列，始终为0，表示使用全部区域。The default is zero. */
		uint32_t	senOffsetY = 0;			/**< sensor 图像起始行，始终为0，表示使用全部区域。The default is zero.	 */
	};
#ifdef EZR_EXPORTS
	/** Range axis direction. */
	enum class RangeAxis
	{
		STANDARD = 0,
		REVERSE,
	};
#endif
	/** @brief 未标定的高度数据信息  Raw range data information. */
	struct EXPORT_TO_DLL RangeInfo
	{
		RangeInfo()
			: cols(0), rows(0), aoiOffsetX(0), aoiOffsetY(0), aoiHeight(0), aoiWidth(0), m_RangeAxis(RangeAxis::STANDARD)
		{}
#ifndef SWIG
		bool operator == (const RangeInfo& v)
		{
			return this->cols == v.cols
				&& this->rows == v.rows
				&& this->aoiOffsetX == v.aoiOffsetX
				&& this->aoiOffsetY == v.aoiOffsetY
				&& this->aoiHeight == v.aoiHeight
				&& this->aoiWidth == v.aoiWidth
				&& this->m_RangeAxis == v.m_RangeAxis
				&& this->xml_fov_x0 == v.xml_fov_x0
				&& this->xml_fov_x1 == v.xml_fov_x1
				&& this->xml_fov_x2 == v.xml_fov_x2
				&& this->xml_fov_x3 == v.xml_fov_x3
				&& this->xml_fov_z0 == v.xml_fov_z0
				&& this->xml_fov_z1 == v.xml_fov_z1
				&& this->xml_fov_z2 == v.xml_fov_z2
				&& this->xml_fov_z3 == v.xml_fov_z3
				&& this->xml_origin_x == v.xml_origin_x
				&& this->xml_scale_x == v.xml_scale_x
				&& this->xml_origin_z == v.xml_origin_z
				&& this->xml_scale_z == v.xml_scale_z
				;
		}
#endif
		void fillXML_byDeviceParameters()
		{
			xml_fov_x0 = 0.0f;
			//xml_fov_x1 = aoiHeight - 1.0f;
			xml_fov_x1 = aoiWidth - 1.0f;
			xml_fov_x2 = 0.0f;
			xml_fov_x3 = aoiWidth - 1.0f;
			xml_fov_z0 = 1.0f;
			//xml_fov_z1 = aoiHeight * 16 - 1.0f;
			//xml_fov_z2 = 1.0f;
			xml_fov_z1 = 1.0f;
			xml_fov_z2 = aoiHeight * 16 - 1.0f;
			xml_fov_z3 = aoiHeight * 16 - 1.0f;
			xml_origin_x = static_cast<float>(aoiOffsetX);
			xml_scale_x = 1.0f;
			xml_origin_z = static_cast<float>(m_RangeAxis == RangeAxis::STANDARD ? aoiOffsetY : aoiOffsetY + aoiHeight);
			xml_scale_z = m_RangeAxis == RangeAxis::STANDARD ? 0.0625f : -0.0625f;
		}

		uint32_t	cols = 2560;				/**< 3D 图像宽度。 Width of range image. */
		uint32_t	rows = 1024;				/**< 3D 图像高度。 Height of range image.*/
		uint32_t	aoiOffsetX = 0;			/**< 在 sensor 上设置的 AOI 的左上 X 坐标，像素。[ICON | XML 参数]->["origin x"]。[相机参数]->[OffsetX_RegionSelector_Region1]。*/
		uint32_t	aoiOffsetY = 0;			/**< 在 sensor 上设置的 AOI 的左上 Y 坐标，像素。[ICON | XML 参数]->["origin z"]。[相机参数]->[OffsetY_RegionSelector_Region1]。*/
		uint32_t	aoiHeight = 832;			/**< 在 sensor 上设置的 AOI 的高度，像素。[ICON | XML 参数]->[("fov z2" + 1) / 16.0]。[相机参数]->[Height_RegionSelector_Region1]。*/
		uint32_t	aoiWidth = 2560;			/**< 在 sensor 上设置的 AOI 的宽度，像素。[ICON | XML 参数]->["fov x1" - "fov x0" + 1]。[相机参数]->[Width_RegionSelector_Region1]。*/
		RangeAxis	m_RangeAxis = RangeAxis::STANDARD;		/**< 3D 图像的 R 轴方向。激光竖直架设，R 就是 Z 轴。3D 图像的 R 轴对应于 sensor 上的 Y 轴。支持将 Y 轴取反。"STANDARD" 表示 sensor 上到下。*/

		// XML paras - sensorrangetraits
		float		xml_fov_x0 = { 0.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左上，X 坐标），像素，始终为0。*/
		float		xml_fov_x1 = { -1.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右上，X 坐标），像素，aoiHeight - 1。*/
		float		xml_fov_x2 = { 0.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左下，X 坐标），像素，始终为0。*/
		float		xml_fov_x3 = { -1.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右下，X 坐标），像素，aoiWidth - 1。*/
		float		xml_fov_z0 = { 0.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左上，Y 坐标），像素，始终为1。*/
		float		xml_fov_z1 = { 0.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右上，Y 坐标），像素，aoiHeight * 16 - 1 。*/
		float		xml_fov_z2 = { 0.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左下，Y 坐标），像素，始终为1。*/
		float		xml_fov_z3 = { 0.0f };			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右下，Y 坐标），像素，aoiHeight * 16 - 1 。*/
		float		xml_origin_x = { 0.0f };		/**< 在 sensor 上设置的 AOI 的左上 X 坐标，像素。[ICON | XML 参数]->["origin x"]。[相机参数]->[OffsetX_RegionSelector_Region1]。*/
		float		xml_scale_x = { 1.0f };		    /**< 提取激光线算法，在 sensor 图像的 X 坐标上的亚像素分辨率。始终为 1.0。*/
		float		xml_origin_z = { 0.0f };		/**< 在 sensor 上设置的 AOI 的左上 Y 坐标，像素。[ICON | XML 参数]->["origin z"]。[相机参数]->[OffsetY_RegionSelector_Region1]。*/
		float		xml_scale_z = { 0.0625f };		/**< 提取激光线算法，在 sensor 图像的 Y 坐标上的亚像素分辨率。始终为 0.0625(=1/16.0)。[RangeAxis = "STANDARD"] 取正值，[RangeAxis = "REVERSE"] 取负值，*/
	};

	/** @brief 标定的高度数据信息  Calibration data information. */
	struct EXPORT_TO_DLL genistreamtraits_
	{
		std::string region_id = "scan 3d extraction 1";
		std::string extraction_method = "hi 3d";
		std::string output_mode = "";
		int width = 2560;
		int height = 832;
		int offset_X = 0;
		int offset_Y = 0;

		double      a_axis_range_scale = 1.0;
		double      a_axis_range_offset = 0.0; // todo: linux USE_PC_CALIBRATION ---
		int			a_axis_range_min = 0;
		int			a_axis_range_max = 2560;
		std::string a_axis_range_missing = "false";
		int			a_axis_range_missing_value = 0;

		double      b_axis_range_scale = 1.0;
		double      b_axis_range_offset = 0.0;
		std::string b_axis_range_min = "-inf";
		std::string b_axis_range_max = "inf";
		std::string b_axis_range_missing = "false";
		int			b_axis_range_missing_value = 0;

		double      c_axis_range_scale = 1.0;
		double      c_axis_range_offset = 0.0;
		int			c_axis_range_min = 0;
		int			c_axis_range_max = 832;
		std::string c_axis_range_missing = "false";
		int			c_axis_range_missing_value = 0;

		std::string unit = "";

	};
	
	/** @brief 标定的高度数据信息  Calibration data information. */
	struct EXPORT_TO_DLL CalibInfo
	{
		CalibInfo()
			:
			//cols(0), rows(0), 
			offsetX(0.0), offsetY(0.0), offsetZ(0.0), scaleX(1.0), scaleY(1.0), scaleZ(1.0),
			lower_bound_x(0.0), upper_bound_x(0.0), lower_bound_r(0.0), upper_bound_r(0.0)
		{}
#ifndef SWIG
		bool operator == (const CalibInfo& v)
		{
			return
				this->offsetX == v.offsetX &&
				this->offsetY == v.offsetY &&
				this->offsetZ == v.offsetZ &&
				this->scaleX == v.scaleX &&
				this->scaleY == v.scaleY &&
				this->scaleZ == v.scaleZ &&
				this->lower_bound_x == v.lower_bound_x &&
				this->upper_bound_x == v.upper_bound_x &&
				this->lower_bound_r == v.lower_bound_r
				&& this->upper_bound_r == v.upper_bound_r
				//&& this->genistreamtraits == v.genistreamtraits
				;
		}
#endif

		double		offsetX = 0;		/**< 视野在 X 方向的最小值，毫米。					Unit millimeter, minimum x in FOV*/
		double		offsetY = 0;		/**< 视野在 Y 方向的最小值，毫米。					Unit millimeter, minimum y in FOV*/
		double		offsetZ = 0;		/**< 【此参数仅在启用相机内部（in-device）标定时有效，（in-PC）标定时此参数没有意义，可以给 0.0】视野在 Z 方向的最小值，毫米。
																							Unit millimeter, minimum z in FOV*/
		double		scaleX = 1.0;		/**< 视野在 X 方向的分辨率，毫米/像素。				Unit millimeter/pixel, x resolution*/
		double		scaleY = 1.0;		/**< 视野在 Y 方向的分辨率，毫米/像素。				Unit millimeter/pixel, y resolution*/
		double		scaleZ = 1.0;		/**< 【此参数仅在启用相机内部（in-device）标定时有效，（in-PC）标定时此参数没有意义，可以给 1.0】视野在 Z 方向的分辨率，毫米/像素。
																							Unit millimeter/pixel, z resolution*/
		double		lower_bound_x = 0.0;		/**< [ICON | XML 参数] 视野在 X 方向的最小值，毫米。	Unit millimeter, minimum x in FOV*/
		double		upper_bound_x = 2559.0;		/**< [ICON | XML 参数] 视野在 X 方向的最大值，毫米。	Unit millimeter, maximum x in FOV*/
		double		lower_bound_r = 0.0;		/**< [ICON | XML 参数] 视野在 R 方向的最小值，毫米。激光竖直架设，R 就是 Z 轴。	Unit millimeter, minimum r in FOV, when set lazer vertically, same as z*/
		double		upper_bound_r = 832.0;		/**< [ICON | XML 参数] 视野在 R 方向的最大值，毫米。激光竖直架设，R 就是 Z 轴。	Unit millimeter, maximum r in FOV, when set lazer vertically, same as z*/

		genistreamtraits_ genistreamtraits;
	};

	/** @brief 图像的基本信息。 struct of output images information. */
	struct EXPORT_TO_DLL ImgInfo
	{
		ImgInfo()
			: m_id(0)
		{}
#ifndef SWIG
		bool operator == (const ImgInfo& v)
		{
			return this->m_RI == v.m_RI
				&& this->m_SI == v.m_SI
				&& this->m_CI == v.m_CI
				&& this->m_ChunkData == v.m_ChunkData
				&& this->m_id == v.m_id
				;
		}
#endif
		RangeInfo	m_RI;				/**< 未标定的高度数据信息。		Range Information */
		SensoInfo	m_SI;				/**< 感光元件的基本信息。		Sensor Information */
		CalibInfo	m_CI;				/**< 标定的高度数据信息。		Calibration Information */
		ChunkData	m_ChunkData;		/**< 结构化的编码器信息。		ChunkInfo */
		uint64_t	m_id;				/**< Image ID */
	};

	/** @brief 双曝光中 Missing data 的处理选项。 Missing data processing mode. */
	enum class HDR_missingDataMode
	{
		Pick_Long_Exposure = 0, /**< 使能双曝光。 Enable dual-exposure */
		Pick_Missing_Data		/**< 不处理图像。 No changing original image */
	};

	/** @brief 输出图像 Class of output images. */
	class ImgT final
	{
	public:
		EXPORT_TO_DLL ImgT();
		EXPORT_TO_DLL ImgT(const std::string& path, const double& scaleY);
		EXPORT_TO_DLL ImgT(const ImgT& img);
		EXPORT_TO_DLL ImgT& operator =	(const ImgT& img);
		EXPORT_TO_DLL bool	operator == (const ImgT& img);
		EXPORT_TO_DLL ~ImgT();

		////////////////////////////////////////////////////////////////////////////////

		/**
		* @brief 将数据的枚举名转化为对应的字符串。\n\n
		* Convert DataNames to its corresponding string name.
		*
		* @param [in] _dn   数据的枚举名   DataNames
		*
		* @par 数据枚举名和对应字符串的对应表    The list of enum DataNames and corresponding string name.
		* @verbatim
		{ DN::INV			, "INVALID" },
		{ DN::SEN			, "SENSOR" },

		{ DN::RAN			, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"			},
		{ DN::REF			, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"		},
		{ DN::SCA_8			, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1" },
		{ DN::SCA_16		, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1" },
		{ DN::MAR			, "Encoder_information"			},
		{ DN::RAN_CAL		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"		},
		{ DN::RAN_CAL_16	, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL_16"  	},
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

		{ DN::RAN_X	        , "RangeX_Calibration_Only_Without_Rectification"						},
		{ DN::RAN_X_16	    , "16_Byte_RangeX_Calibration_Only_Without_Rectification"				},
		@endverbatim
		*/
		EXPORT_TO_DLL static const std::string	DN2Str(const DN& _dn);

		/**
		* @brief 将相应的字符串转化为数据的枚举名。\n\n
		* Convert string name to its corresponding DataNames.
		*
		* @param [in] _str   数据的枚举名的对应字符串   DataNames string name.
		*
		* @par 对应字符串和数据枚举名的对应表    The list of string name and corresponding enum DataNames.
		* @verbatim
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
		{ "16_Byte_RangeX_Calibration_Only_Without_Rectification"					, DN::RAN_X_16	}
		@endverbatim
		*/
		EXPORT_TO_DLL static DN		Str2DN(const std::string& _str);

		////////////////////////////////////////////////////////////////////////////////

		/**
		* @brief 【仅 C++，CS 不可用】根据给定的数据枚举名，获得数据的指针 \n\n
		* Get data pointer according to DN.
		*
		* @param [in] _dn   数据的枚举名   DataNames
		* 
		* @return
		* - uint8_t *   激光线图像、激光强度图像、激光散射图像、标定后的激光强度图像、标定后的激光散射图像 \n
		*				Sensor, Reflectance, Scatter, Calibrated Reflectance, Calibrated Scatter.\n
		*				DN::SEN, DN::REF, DN::REF_CAL.
		* - uint16_t *  高度图象。\n  Range.\n  DN::RAN, DN::SCA, DN::SCA_CAL.
		* - float *		标定后的高度图像、旧版本的标定后的激光强度图像。  \n
		*				Calibrated Range, Old version Calibrated Reflectance \n
		*				DN::RAN_CAL, DN::REF_CAL_F,
		*/
		EXPORT_TO_DLL void* getData(const DN& _dn)	const;

		/**
		* @brief 获得所有的数据指针。 \n\n
		* Get all data pointers.
		* \n
		* @return
		* - std::map<DN, void *>
		*/
		EXPORT_TO_DLL const ImgTData& getData()	const { return m_Data; }

		/**
		* @brief 获得当前所有有效数据的枚举名列表。\n\n
		*/
		EXPORT_TO_DLL std::vector<DN>		getAvalibleDataNames()	const;

		/**
		* @brief 获得当前所有有效（未标定）数据的枚举名列表。\n\n
		*/
		//EXPORT_TO_DLL std::vector<DN>		getAvalibleDataNamesRaw()	const;

		/**
		* @brief 获得当前所有有效（未定）数据的枚举名列表。\n\n
		*/
		//EXPORT_TO_DLL std::vector<DN>		getAvalibleDataNamesCal()	const;

		/**
		* @brief 图像数据宽度，注意，当图像中有多个图像组时候，得到的结果仅为Range图像的原始大小。\n\n
		* 数据类型的名字定义，详见：enum DataNames
		* 
		* 宽度为
		* 宽度为【ROI 宽度】的组件：SEN,	RAN,REF,SCA_8,SCA_16,RAN2,REF2,SCA2_8,SCA2_16,RAN_X,RAN_X_16,
		* 宽度为【5】 的组件：MAR,
		* 宽度为【RectificationWidth（用户定义的梯形校正插值宽度）】的组件：RAN_CAL,RAN_CAL_16,REF_CAL,REF_CAL_F,SCA_8_CAL,SCA_16_CAL,RAN2_CAL,REF2_CAL,SCA2_8_CAL,SCA2_16_CAL,
		* 宽度为【5120】的组件：	WHITE,R,G,B,
		*
		* @param [in] _dn   数据的枚举名   DataNames
		*/
		//EXPORT_TO_DLL const uint32_t& get_cols(DataNames _dn) const;



		/**
		* @brief 
		* 图像数据宽度，注意，当图像中有多个图像组时候，得到的结果仅为Range图像的原始大小。\n\n
		* 当使用 in-device 标定时，返回标定后的图像大小；
		* 当使用 in-pc 标定时，相机返回的是未标定图像，那么这里返回的是 DataName::RAN 的宽度；
		* 当使用 in-pc 标定时，经过标定后的图像，它的宽度是由用户决定的，见 InitCalibration() 的 RectificationWidth 参数；
		* 当使用 in-pc 标定时，经过标定后的彩色图像，它的宽度是由用户决定的，见 InitCalibrationColor() 的 RectificationWidth 参数；
		*
		* @param [in] _dn   数据的枚举名   DataNames
		*/
		EXPORT_TO_DLL const uint32_t& get_cols() const;

		// todo : get_cols 需要重载，因为现在存在不同的图像组建的宽度不同。




		/**
		* @brief 获得指定的图像数据高度。\n\n
		* @param [in] _dn   数据的枚举名   DataNames
		*/
		EXPORT_TO_DLL const uint32_t& get_rows() const;


		////////////////////////////////////////////////////////////////////////////////


		EXPORT_TO_DLL const SensoInfo& getSensor_info()	const { return m_info.m_SI; }
		EXPORT_TO_DLL const RangeInfo& getRange_info()	const { return m_info.m_RI; }
		EXPORT_TO_DLL const CalibInfo& getCalibration_info()const { return m_info.m_CI; }

		////////////////////////////////////////////////////////////////////////////////

		/**
		* @brief 获得每个剖面的编码器信息。 \n\n
		* Get encoder information of each profile.
		* \n
		* \n
		* @note
		* - timestamp           uint64_t   单位是纳秒，表示取剖面时相机内部时钟的时间戳。\n
		* - encoderValue        int32_t    编码器旋转，该值会记录相应的脉冲数。当编码器反转，计数值会减少相应的脉冲数。该值可能是负数。\n
		* - overtriggerCount    uint8_t    在获取一个剖面还未完成时，如果接收到获取下一个剖面的触发信号，该值为1，否则为0。 1 if a line trigger has been requested before the sensor was actually ready for a new trigger.\n
		* - frameTriggerActive  bool       在获取图像还未完成时，如果 frame trigger pin (IO pin 1) 是高电平，该值为1，否则为0。 1 if frame trigger pin (IO pin 1) is high, otherwise 0. \n
		* - lineTriggerActive   bool       在获取一个剖面还未完成时，如果 line trigger pin (IO pin 2) 是高电平，该值为1，否则为0。 1 if line trigger pin (IO pin 2) is high, otherwise 0.\n
		* - encoderResetActive  bool       1 if Encoder Reset (IO pin 3) is high, otherwise 0.\n
		* - encoderA            bool       Value 0 (for low) or 1 (for high) at encoder channel A.\n
		* - encoderB            bool       Value 0 (for low) or 1 (for high) at encoder channel B.\n
		*/
		EXPORT_TO_DLL const ChunkData& get_ChunkInfo()	const { return m_info.m_ChunkData; }

		/**
		* @brief 0初始。相机停止后重置。 \n\n
		* Start from 0. Reset when camera stop.
		*/
		EXPORT_TO_DLL const uint64_t& get_ID()	const { return m_info.m_id; }

		////////////////////////////////////////////////////////////////////////////////

		/**
		* @brief 无数据指针返回 false. \n\n
		* Return false if no data pointer.
		*/
		EXPORT_TO_DLL bool	isEmpty()	const { return m_Data.empty(); }

		/**
		* @brief 如果没有指定的数据则返回 false. \n\n
		* Return false if no such data pointer.
		*/
		EXPORT_TO_DLL bool	has(const DN& _name)	const { return m_Data.count(_name) == 1; }

		/**
		* @brief 无编码器信息返回 false. \n\n
		* Return false if no encoder information.
		*/
		EXPORT_TO_DLL bool	has_ChunkInfo()	const { return !m_info.m_ChunkData.empty(); }


		////////////////////////////////////////////////////////////////////////////////


#ifndef SWIG

		/**
		* @brief 【仅 C++，CS 不可用】插入数据。数据指针均按照 uint8_t* 的形式保存。类型不同请事先执行强制转化。nullptr被忽略。 \n\n
		* Any data pointers type need to be uint8_t*, nullptr will be ignored.
		* \n
		* @param [in] _name 图像数据的枚举值。  DataName of inserting data.
		* @param [in] _pointer 数据指针。 Data pointer.
		* @param [in] _id	图像序号  Image id.
		* @param [in] _offset	数据的开头部分要被裁剪的行数
		* \n
		* @note 此函数用于相机获得图像并返回。\n
		* 图像基本信息设置必须在调用之前完成。\n
		* 深拷贝。复制所有数据。 \n\n
		* 此函数中申请的内存在析构函数中释放。 \n\n
		* It is used in passing data from device to user.\n
		* Set image information before call it.\n
		* It will allocate new memory. It is deep copy!
		* The memory will be deleted when disconstructing ImgT!
		*/
		EXPORT_TO_DLL bool	insertDataCopy(const DN& _name,
			const uint8_t* _pointer,
			const uint64_t& _id, const int& _offset = 0);

		////////////////////////////////////////////////////////////////////////////////

		EXPORT_TO_DLL bool	setSensorInfo(const uint32_t& _c,
			const uint32_t& _r,
			const uint32_t& _ox,
			const uint32_t& _oy);

		EXPORT_TO_DLL bool	setS_cols(const uint32_t& _v) { m_info.m_SI.cols = _v; return true; }
		EXPORT_TO_DLL bool	setS_rows(const uint32_t& _v) { m_info.m_SI.rows = _v; return true; }
		EXPORT_TO_DLL bool	setS_OffsetX(const uint32_t& _v) { m_info.m_SI.senOffsetX = _v; return true; }
		EXPORT_TO_DLL bool	setS_OffsetY(const uint32_t& _v) { m_info.m_SI.senOffsetY = _v; return true; }

		// ---------- Set Range Info ---------------------------------------------------------------

		EXPORT_TO_DLL bool	setRangeInfo(const uint32_t& _c,
			const uint32_t& _r,
			const uint32_t& _aox,
			const uint32_t& _aoy,
			const uint32_t& _ah,
			const uint32_t& _aw,
			const RangeAxis& _rAxis);
		EXPORT_TO_DLL bool	setRangeInfo(const RangeInfo& info) { m_info.m_RI = info; return true; };


		EXPORT_TO_DLL bool	setR_cols(const uint32_t& _v) { m_info.m_RI.cols = _v; return true; }
		EXPORT_TO_DLL bool	setR_rows(const uint32_t& _v) { m_info.m_RI.rows = _v; return true; }
		EXPORT_TO_DLL bool	setR_aoiOffsetX(const uint32_t& _v) { m_info.m_RI.aoiOffsetX = _v; return true; }
		EXPORT_TO_DLL bool	setR_aoiOffsetY(const uint32_t& _v) { m_info.m_RI.aoiOffsetY = _v; return true; }
		EXPORT_TO_DLL bool	setR_aoiHeight(const uint32_t& _v) { m_info.m_RI.aoiHeight = _v; return true; }
		EXPORT_TO_DLL bool	setR_aoiWidth(const uint32_t& _v) { m_info.m_RI.aoiWidth = _v; return true; }
		EXPORT_TO_DLL bool	setR_RangeAxis(const RangeAxis& _v) { m_info.m_RI.m_RangeAxis = _v; return true; }

		// ---------- Set Calibration Info -------------------------------------------------------------
		
		/** @brief 标定后图像参数的设定
		*
		* @param [in] _offsetZ 【此参数仅在启用相机内部（in-device）标定时有效，（in-PC）标定时此参数没有意义，可以给 0.0】视野在 Z 方向的最小值，毫米。
		* @param [in] _scaleZ  【此参数仅在启用相机内部（in-device）标定时有效，（in-PC）标定时此参数没有意义，可以给 1.0】视野在 Z 方向的分辨率，毫米/像素。
		*/
		EXPORT_TO_DLL bool	setCaliInfo(const uint32_t& _c, const uint32_t& _r, const double& _offsetX, const double& _offsetY, const double& _offsetZ,
			const double& _scaleX, const double& _scaleY, const double& _scaleZ,
			const double& _lower_bound_x, const double& _upper_bound_x, const double& _lower_bound_r, const double& _upper_bound_r,
			const double& _a_axis_range_scale = 1.0, const double& _a_axis_range_offset = 0,
			const double& _c_axis_range_scale = 1.0, const double& _c_axis_range_offset = 0.0);

		EXPORT_TO_DLL bool	setCaliInfo(const CalibInfo& _c);


		EXPORT_TO_DLL bool	setC_cols(const uint32_t& _v) { m_info.m_RI.cols = _v; return true; }
		EXPORT_TO_DLL bool	setC_rows(const uint32_t& _v) { m_info.m_RI.rows = _v; return true; }
		EXPORT_TO_DLL bool	setC_offsetX(const double& _v) { m_info.m_CI.offsetX = _v; return true; }
		EXPORT_TO_DLL bool	setC_offsetY(const double& _v) { m_info.m_CI.offsetY = _v; return true; }
		//EXPORT_TO_DLL bool	setC_offsetZ	  (	const double & _v)	{	m_info.m_CI.offsetZ			= _v; return true;}
		EXPORT_TO_DLL bool	setC_scaleX(const double& _v) { m_info.m_CI.scaleX = _v; return true; }
		EXPORT_TO_DLL bool	setC_scaleY(const double& _v) { m_info.m_CI.scaleY = _v; return true; }
		//EXPORT_TO_DLL bool	setC_scaleZ		  (	const double & _v)	{	m_info.m_CI.scaleZ			= _v; return true;}
		EXPORT_TO_DLL bool	setC_lower_bound_x(const double& _v) { m_info.m_CI.lower_bound_x = _v; return true; }
		EXPORT_TO_DLL bool	setC_upper_bound_x(const double& _v) { m_info.m_CI.upper_bound_x = _v; return true; }
		EXPORT_TO_DLL bool	setC_lower_bound_r(const double& _v) { m_info.m_CI.lower_bound_r = _v; return true; }
		EXPORT_TO_DLL bool	setC_upper_bound_r(const double& _v) { m_info.m_CI.upper_bound_r = _v; return true; }


		EXPORT_TO_DLL bool	setChunkInfo(const	ChunkData& _Chunk, const uint64_t& _id);

#endif

		EXPORT_TO_DLL void	printInfo()	const;

		/** @brief 仅测试用。读取图形的基本信息。*/
		EXPORT_TO_DLL std::string	collectInfo()	const;



#ifndef DISABLE_CAL_IN_PC

#ifndef EZR_EXPORTS

	protected:
		static SiCaliWrapper::CalibrationWrapper* m_CaliPtr[HANDLE_NUM];
		static int m_RawImageWidth[HANDLE_NUM];
		static int m_CaliRectWidth[HANDLE_NUM];
		
		bool	_InitCalibration(const int& handle,
			const std::string& calibrationFile,
			const double& rectificationSpread = 1.2,
			const int& rawRangeWidth = 2560,
			const int& rectificationWidth = 4096,
			const SiCaliWrapper::RectMethod& rectMethod = SiCaliWrapper::RectMethod::Top,
			const int& numberOfThreads = -1);

#ifndef DISABLE_R380 // RANGER380

		static SiCaliWrapper::CalibrationWrapper* m_CaliColorPtr[HANDLE_NUM];
		static int m_RawImageColorWidth[HANDLE_NUM];
		static int m_CaliColorRectWidth[HANDLE_NUM];
		static color_image_run::colorParam* m_ColorParam[HANDLE_NUM];

		bool	_InitCalibrationColor(
			const int& handle,
			const std::string& calibrationFile,
			const std::string& colorFile,
			const double& rectificationSpread = 1.2,
			const int& rawColoreResizedWidth = 5120,
			const int& rectificationWidth = 5120,
			const SiCaliWrapper::RectMethod& rectMethod = SiCaliWrapper::RectMethod::Top,
			const int& numberOfThreads = -1);
#endif

	public:

		/** @brief 释放标定资源（静态资源）。*/
		EXPORT_TO_DLL static bool	DeleteCalibration(const int& handle);

		EXPORT_TO_DLL ImgT	ApplyCalibration(
			const int& calibrationHandle,
			const std::string& calibrationFile,
			const int& removeLastNRows = 0,
			bool doRectify = true,
			const double& rectificationSpread = 1.2,
			const int& rawRangeWidth = 2560,
			const int& rectificationWidth = 2560,
			const SiCaliWrapper::RectMethod& rectMethod = SiCaliWrapper::RectMethod::Top,
			const int& numberOfThreads = -1
		);



#ifndef DISABLE_R380 // RANGER380
		/** @brief 释放标定资源（静态）。*/
		EXPORT_TO_DLL static bool DeleteCalibrationColor(const int& handle);


		/** @brief 初始化的彩色标定（申请静态资源）。当无需继续使用后，请调用 DeleteCalibrationColor 释放资源。
		*
		* @param [in] colorCalibrateAlignHandle    用户定义的标定句柄id，范围是[0,64)。一般，当程序退出前，
		*                       请调用 ImgT::DeleteCalibrationColor() 释放则个句柄。
		*                       程序中，当句柄对应的资源已经被初始化时，将会使用已初始化的资源标定图像。
		* @param [in] calibrationFile 用于彩色对齐的 json 标定文件
		* @param [in] colorFile 用于彩色对齐的 xml 参数文件
		* @param [in] removeLastNRows 标定好之后，要去除的最后行数量。一般是在彩色对齐中，在多张图像才能扫描完成的情况下，需要多扫一点数据，在对齐后删除多余行。
		* @param [in] doFCC 是否需要执行平场矫正
		* @param [in] colorInterpolationStride 彩色数据对齐的插值步长，默认是3。设置大些可以连接一些数据缺失点，但是会增加处理时间。
		* @param [in] rectificationSpread 进行第 2 步梯形矫正的插值系数，默认是 1.2。
		* @param [in] rawColoreResizedWidth 将原始彩色数据的宽度，缩放到指定的大小。这个参数需要和标定期间使用的图像宽度一致，和json文件绑定。
		*                           如果您使用的是宽度为 2560 的激光线图像作为标定图像，那么这个值应该是2560。
		*                           如果您使用的是宽度为 5120 的图像作为标定图像，那么这个值应该是5120。
		* @param [in] rectificationWidth 输出图像的大小，默认 5120。
		* @param [in] rectMethod 默认是 1，表示梯形矫正的插值方法。可选值详见：enum RectMethod
		*
			enum RectMethod
			{
				Mean = 0,
				Top,
				Bottom,
				DARKEST,
				BRIGHTEST,
				NOT_DO,
			};
		*/
		EXPORT_TO_DLL ImgT	ApplyCalibrationColor(
			const int colorCalibrateAlignHandle,
			const std::string& calibrationFile,
			const std::string& colorFile,
			const int& removeLastNRows = 0,
			bool  doFCC = true,
			const int& colorInterpolationStride = 3,
			const double& rectificationSpread = 1.2,
			const int& rawColoreResizedWidth = 5120,
			const int& rectificationWidth = 5120,
			const SiCaliWrapper::RectMethod& rectMethod = SiCaliWrapper::RectMethod::Top,
			const int& removeHeadNRows = 0,
			const bool doCalibration = true,
			const int& numberOfThreads = -1
		);
#endif



#endif // 

#endif // DISABLE_CAL_IN_PC

		/** @brief 将目标图像 (src) 拼接到当前图像 (this) 的尾后，并且返回合并后数据，修改原始图像。对 src 和 this 的要求如下：
		* - 假如 src[RAN, REF, R, G, B], this[RAN, REF, G, WHITE]，那么返回的图像只含有[RAN, REF, G]。\n
		* - 假如 src[RAN, REF, R, G, B]的 Height 是 1000, this[RAN, REF] 的 height 是 500，那么返回的图像 height 是 1500。\n
		* - 假如 src[RAN, REF, R, G, B]中 RAN 和 REF 的 width 是 2560，RGB 的 width 是 5120， this[RAN, REF]的width是2560，那么返回的图像[RAN, REF]的width是2560。\n
		* - 假如 src[RAN, REF, R, G, B]中 RAN 和 REF 的 width 是 1280，RGB 的 width 是 5120， this[RAN, REF]的width是2560，那么返回空图像。\n
		*
		* @param [in] src    被添加到尾后的图像。
		*/
		EXPORT_TO_DLL void	Append(const ImgT& src);

		EXPORT_TO_DLL std::vector<ImgT>	Divide(const int& parts) const;




		////////////////////////////////////////////////////////////////////////////////



		/**
		* @brief 读取 ICON 图像文件。 \n\n
		* load image data from Icon file.
		* \n
		* @param [in] path		无需拓展名。  Please do not provide the suffix;
		* @param [in] resolutonY	由机台的编码器决定。若为原始图像，该值不起作用，可设为 1。 Set 1.0 if it is raw data;
		* @param [in] resolutonZ	【此参数已无效】相机的 Z 分辨率。若为原始图像，该值不起作用，可设为 1。Float 类型的标定后数据，该值会自动计算，无需设定；Word(uint16_t) 类型的标定后数据，该值需要指定。
		* @param [in] convertRangeToFloat	将Word(uint16_t) 类型的标定后Range数据转化成 float。
		* \n
		* @note
		* 如果该图像是使用 WORD 表示标定后的 Region 图像，读取函数会将其转化成 FLOAT 类型，并体现为 DataNames::RAN_CAL \n
		* \n
		*/
		EXPORT_TO_DLL bool	loadFromIconFile(const std::string& path, const double& resolutonY, const double& resolutonZ = 1.0, const bool convertRangeToFloat = true);
		//EXPORT_TO_DLL bool	loadFromIconFile_old(const std::string& path, const double& resolutonY, const double& resolutonZ = 1.0, const bool convertRangeToFloat = true);

		/** @brief 保存激光线图像。相机若不处于激光线模式，则返回 false 。成功保存，则返回 true。
		* \n
		* @param [in] path				路径，不带后缀名。  Path; without suffix
		* @param [in] byFStream			默认为 false。设置为 true 表示使用 3.3.0.0 之前版本的写入方式，设置为 false 表示使用最新的写入方式，速度提升。
		*/
		EXPORT_TO_DLL bool	SaveSensorImageToIconFile(const std::string& path, bool byFStream = false)	const;

		/** @brief 保存未标定的原始图像。相机若不处于高度图模式，则返回 false 。成功保存，则返回 true。
		* \n
		* @param [in] path				路径，不带后缀名。  Path; without suffix
		* @param [in] byFStream			默认为 false。设置为 true 表示使用 3.3.0.0 之前版本的写入方式，设置为 false 表示使用最新的写入方式，速度提升。
		*/
		EXPORT_TO_DLL bool	SaveRawImagesToIconFile(const std::string& path, bool withChunkData, bool byFStream = false)	const;

		/** @brief 保存标定后的图像。相机若不处于高度图模式且启用标定功能，则返回 false 。成功保存，则返回 true。
		* \n
		* @param [in] path				路径，不带后缀名。  Path; without suffix
		* @param [in] byFStream			默认为 false。设置为 true 表示使用 3.3.0.0 之前版本的写入方式，设置为 false 表示使用最新的写入方式，速度提升。
		*/
		EXPORT_TO_DLL bool	SaveCalibratedImagesToIconFile(const std::string& path, bool withChunkData, bool byFStream = false)	const;


		//////////////////////////////////// discard ////////////////////////////////////////////

#pragma region PLEASE_DO_NOT_USE_THEM_ANY_MORE

	/**
	* @brief 【不建议再继续使用】（此函数已经弃用，请见 Demo_ImgT_convert_to_point_cloud.cpp）保存为 txt 点云。 \n\n
	* Save data as point cloud format, txt. \n
	* \n
	* @verbatim
	// data example:
	xValue1, yValue1, zValue1,
	xValue2, yValue2, zValue2,
	...
	@endverbatim
	* \n
	* @note 仅标定后的数据可以调用该函数。 \n\n
	* Calibrated Data only!
	*/
		EXPORT_TO_DLL bool	SaveCalibratedDataToPointCloud(const std::string& pathconst, bool please_do_not_use_it_any_more = true)	const;

		/** @brief 【不建议再继续使用】，请使用 get_cols(void) */
		EXPORT_TO_DLL const uint32_t& get_cols(const DN& please_do_not_use_it_any_more)	const { return get_cols(); };

		/** @brief 【不建议再继续使用】，请使用 get_rows(void) */
		EXPORT_TO_DLL const uint32_t& get_rows(const DN& please_do_not_use_it_any_more)	const { return get_rows(); };

		/** @brief 【不建议再继续使用】，请使用 get_cols(void) */
		EXPORT_TO_DLL const uint32_t& getS_cols(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_SI.cols; }

		/** @brief 【不建议再继续使用】，请使用 get_rows(void) */
		EXPORT_TO_DLL const uint32_t& getS_rows(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_SI.rows; }

		/** @brief 【不建议再继续使用】，请使用 getSensor_info() */
		EXPORT_TO_DLL const uint32_t& getS_OffsetX(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_SI.senOffsetX; }

		/** @brief 【不建议再继续使用】，请使用 getSensor_info() */
		EXPORT_TO_DLL const uint32_t& getS_OffsetY(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_SI.senOffsetY; }

		/**@brief 【不建议再继续使用】，请使用 get_cols() */
		EXPORT_TO_DLL const uint32_t& getR_cols(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.cols; }

		/** @brief 【不建议再继续使用】，请使用 get_rows() */
		EXPORT_TO_DLL const uint32_t& getR_rows(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.rows; }

		/** @brief 【不建议再继续使用】，请使用 getRange_info().aoiOffsetX */
		EXPORT_TO_DLL const uint32_t& getR_aoiOffsetX(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.aoiOffsetX; }

		/** @brief 【不建议再继续使用】，请使用 getRange_info().aoiOffsetY */
		EXPORT_TO_DLL const uint32_t& getR_aoiOffsetY(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.aoiOffsetY; }

		/** @brief 【不建议再继续使用】，请使用 getRange_info() */
		EXPORT_TO_DLL const uint32_t& getR_aoiHeight(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.aoiHeight; }

		/** @brief 【不建议再继续使用】，请使用 getRange_info() */
		EXPORT_TO_DLL const uint32_t& getR_aoiWidth(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.aoiWidth; }

		/** @brief 【不建议再继续使用】，请使用 getRange_info() */
		EXPORT_TO_DLL const RangeAxis& getR_RangeAxis(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.m_RangeAxis; }

		/** @brief 【不建议再继续使用】，请使用 getRange_info() */
		EXPORT_TO_DLL const double	  getR_ScaleZ(const int& please_do_not_use_it_any_more = 0)	const { return (getR_RangeAxis() == RangeAxis::STANDARD ? 0.0625 : -0.0625); }

		/** @brief 【不建议再继续使用】，请使用 get_cols() */
		EXPORT_TO_DLL const uint32_t& getC_cols(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.cols; }

		/** @brief 【不建议再继续使用】，请使用 get_rows() */
		EXPORT_TO_DLL const uint32_t& getC_rows(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_RI.rows; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_offsetX(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.offsetX; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_offsetY(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.offsetY; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_scaleX(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.scaleX; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_scaleY(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.scaleY; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_lower_bound_x(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.lower_bound_x; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_upper_bound_x(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.upper_bound_x; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_lower_bound_r(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.lower_bound_r; }

		/** @brief 【不建议再继续使用】，请使用 getCalibration_info() */
		EXPORT_TO_DLL const double& getC_upper_bound_r(const int& please_do_not_use_it_any_more = 0)	const { return m_info.m_CI.upper_bound_r; }

		/**
		* @brief 【不建议再继续使用】，请使用以下的函数替代。\n
		* bool SaveSensorImageToIconFile      (string path)	\n
		* bool SaveRawImagesToIconFile	      (string path, bool withChunkData)	\n
		* bool SaveCalibratedImagesToIconFile (string path, bool withChunkData)	\n
		* \n
		* \n
		* 保存为 ICON 格式的图像数据。 \n\n
		* Save data as SICK icon format.\n
		* \n
		* @param [in] path				路径，不带后缀名。  Path; without suffix
		* @param [in] components		指定要保存的图像类型。必须是同一类型的图像才可以存在同一份 dat 文件中。   Saving type. See note for details.
		* @param [in] is_calibrated		指定的图像，是否都是属于标定后图像。
		* @param [in] byFStream			设置为 true 表示使用 3.3.0.0 之前版本的写入方式，设置为 false 表示使用最新的写入方式，速度提升。
		* \n
		* @note
		* components 之中不可以混合原始数据和标定后数据。\n\n
		* \n
		* @code
		SaveToIconFile("D:/img_sen", { DataNames::SEN }, false); // only sensor is allowed.
		SaveToIconFile("D:/img_raw", { DataNames::RAN, DataNames::REF, DataNames::SCA_16, DataNames::MAR }, false); // SCA_8 and SCA_16 are mutually exclusive.
		SaveToIconFile("D:/img_cal", { DataNames::RAN_CAL, DataNames::REF_CAL, DataNames::SCA_16_CAL, DataNames::MAR  }, true); // MAR is the encoder data, can be saved together with both.

		// Wrong example:
		SaveToIconFile("D:/img_cal", { DataNames::RAN, DataNames::REF, DataNames::RAN_CAL, DataNames::REF_CAL }, true); // Do not mix them!
		* @endcode
		* \n
		*/
		EXPORT_TO_DLL bool	SaveToIconFile(const std::string& please_do_not_use_it_any_more, const std::vector<DN>& components, bool is_calibrated)	const;

		/**
		* @brief 【不建议再继续使用】，请使用以下的函数替代。\n
		* bool SaveSensorImageToIconFile      (string path)	\n
		* bool SaveRawImagesToIconFile	      (string path, bool withChunkData)	\n
		* bool SaveCalibratedImagesToIconFile (string path, bool withChunkData)	\n
		* \n
		* \n
		* 保存为 ICON 格式的图像数据。 \n\n
		* Save data as SICK icon format.\n
		* \n
		* @param [in] path				路径，不带后缀名。  Path; without suffix
		* @param [in] components		指定要保存的图像类型。必须是同一类型的图像才可以存在同一份 dat 文件中。   Saving type. See note for details.
		*/
		EXPORT_TO_DLL bool	SaveToIconFile(const std::string& please_do_not_use_it_any_more, const std::vector<DN>& components)	const;

#pragma endregion



	protected:
		ImgT& __doapl(ImgT* ths, const ImgT& img);
		bool	_clearBuffer(ImgT& img);
		bool	_compareBuffer(const ImgT& img, const DN dn);

		size_t	_subcomponentX_size(
			tinyxml2::XMLElement* subcomponentX, const std::string& target);

		void    _writeXML(const std::string& filePath, const std::vector<DataNames>& dataNames,
			const int& rowSize, const int& Ranger3RangeSize, bool is_calibrated
		) const;

		bool	_saveBuffer(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated = false) const;

		bool	_saveBufferByMapping
		(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated = false) const;

		void _readXML(const std::string& fixpath_nosuffix, std::string& rangetraits_name,
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
			int& Scan3dExtraction_number,
			int& hiresWhiteSize,
			int& hiresColorSize
		);

		//bool	_saveBuffer_old(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated = false) const;

		//bool	_saveBufferByMapping_old
		//(const std::vector<DataNames>& _dataNames, const std::string& _filePath, bool is_calibrated = false) const;

		/** convert DataNames to its corresponding data type in ICON. \n\n
		* 返回在 ICON 的 XML 文件中，Dataneme 对应的 subcomponent -> valuetype 字符串。
		*
		* @param [in] _dn 数据名，见 SickCam::DataNames
		* @return Type string name
		* - "BYTE"	= 1 Bype, uint8_t
		* - "FLOAT"	= 4 Bype, float
		* - "WORD"	= 2 Bype, uint16_t
		* - "INT"	= 4 Bype, uint32_t
		*/
		const std::string _DN2ValueType(const DN& _dn) const;

		/** convert DataNames to its corresponding data type in ICON. \n\n
		* 返回在 ICON 的 XML 文件中，Dataneme 对应的 subcomponent -> name 字符串。
		*
		* @param [in] _dn 数据名，见 SickCam::DataNames
		* @return Type string name
		*/
		const std::string _DN2Name(const DN& _dn) const;

		/** 返回图像的一个数据，所占的字节数。\n\n
		* Return how many bytes a single value has.
		*
		* @param [in] _dn 数据名，见 SickCam::DataNames
		*/
		size_t _DN2PixelSize(const DN& _dn) const;

		/** 返回图像的一行数据，所占的字节数。 \n\n
		* Return how many bytes a row has.
		*
		* @param [in] _dn 数据名，见 SickCam::DataNames
		*/
		size_t _DN2RowSize(const DN& _dn) const;

		void   _bufferToChunkInfo(uint8_t* pd, const int& nRows);

	private:
		ImgTData	m_Data;		/**< std::map of all data pointers, keys are "DataNames".*/
		//ImgInfo		m_info;		/**< the image data information. The chunkdata is inside this struct. */

		enum class TYPE
		{
			INVALID = 0,
			SENSOR,
			RANGE3D
		};
		TYPE		m_type;

		int* test_extractProfile_ptr = nullptr;

	public:
#ifndef SWIG
		EXPORT_TO_DLL const static std::map<DN, const std::string> DN_StrName; /**< data name and parameter name.*/
		EXPORT_TO_DLL const static std::map<const std::string, DN> StrName_DN; /**< data name and parameter name.*/
#endif
		uint64_t	m_previousImageID; /**< Please do not modify it! Used in online grabbing. if (m_previousImageID+1 != get_ID()), images lost.*/
		ImgInfo		m_info;		/**< the image data information. The chunkdata is inside this struct. */

	};


}


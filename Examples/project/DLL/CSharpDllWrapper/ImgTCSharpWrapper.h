/// <summary>
/// C# Wrapper Class of Ranger3.
/// </summary>
/// <remarks>
/// This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
/// Copyright 2016-2026 SICK AG. All rights reserved.
/// </remarks>


#pragma once

#include "CommonFunctions.h"
#include <Ranger3.h>
#include <cliext\list>
#include <cliext\map>
#include <vcclr.h>

#include <string>
#include <sstream>

#include <windows.h>

using namespace System;

namespace SICK3D
{

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 输出图像的类型。 Names of output Datas.
/// </summary>
public enum class DataNames
{
	INV = 0,		/// <summary>INVALID | 无效值 | INVALID</summary>
	SEN,			/// <summary>uint8_t | [---] | 激光线图像 | SENSOR</summary>

	RAN,			/// <summary>uint16_t | [Raw] | 高度图像 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction1</summary>
	REF,			/// <summary>uint8_t | [Raw] | 激光强度图像 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1</summary>
	SCA_8,			/// <summary>uint8_t | [Raw] | 激光散射图像 | 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1</summary>
	SCA_16,			/// <summary>uint16_t | [Raw] | 激光散射图像 | 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1</summary>

	MAR,			/// <summary>uint32_t | [---] | 编码器信息 | Encoder_information</summary>
	RAN_CAL,		/// <summary>float | [Calibrated] | 标定后的高度图像 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction1</summary>
	RAN_CAL_16,		/// <summary>uint16_t | [Calibrated] | 标定后的高度图像 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction1</summary>
	REF_CAL,		/// <summary>uint8_t | [Calibrated] | 标定后的激光强度图像 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1</summary>
	REF_CAL_F,		/// <summary>弃用 | [弃用] | 弃用 | 弃用</summary>
	SCA_8_CAL,		/// <summary>uint8_t | [Calibrated] | 标定后的激光散射图像 | 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1</summary>
	SCA_16_CAL,		/// <summary>uint16_t | [Calibrated] | 标定后的激光散射图像 | 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction1</summary>

	RAN2,			/// <summary>uint16_t | [Raw] | 高度图像 2 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction2</summary>
	REF2,			/// <summary>uint8_t | [Raw] | 激光强度图像 2 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2</summary>
	SCA2_8,			/// <summary>uint8_t | [Raw] | 激光散射图像 2 | 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2</summary>
	SCA2_16,		/// <summary>uint16_t | [Raw] | 激光散射图像 2 | 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2</summary>

	RAN2_CAL,		/// <summary>float | [Calibrated] | 标定后的高度图像 2 | ComponentSelector_Range_______RegionSelector_Scan3dExtraction2</summary>
	REF2_CAL,		/// <summary>uint8_t | [Calibrated] | 标定后的激光强度图像 2 | ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2</summary>
	SCA2_8_CAL,		/// <summary>uint8_t | [Calibrated] | 标定后的激光散射图像 2 | 8_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2</summary>
	SCA2_16_CAL,	/// <summary>uint16_t | [Calibrated] | 标定后的激光散射图像 2 | 16_Byte_ComponentSelector_Scatter_____RegionSelector_Scan3dExtraction2</summary>

	RAN_X,			/// <summary>float | [Calibrated] | 仅标定，无校正，X 数据 | Float32_RangeA</summary>
	RAN_X_16,        /// <summary>uint16_t | [Calibrated] | 仅标定，无校正，X 数据 | 16_Byte_RangeA</summary>

	WHITE,			/// <summary>uint16_t | [Raw] | 高精度灰度图像 | White</summary>
	R,				/// <summary>uint16_t | [Raw] | 彩色图像 Red 通道 | R</summary>
	G,				/// <summary>uint16_t | [Raw] | 彩色图像 Red 通道 | G</summary>
	B,				/// <summary>uint16_t | [Raw] | 彩色图像 Red 通道 | B</summary>
};

typedef DataNames DN;

public enum class HDR_missingDataMode
{
	Pick_Long_Exposure = 0,
	Pick_Missing_Data
};

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 输出图像。 
/// </summary>
public ref class CSImgT
{
public:
	CSImgT();
	~CSImgT();
	CSImgT(const CSImgT^ img);
	CSImgT(const CSImgT% img);

	CSImgT^ operator = (const CSImgT^ img);
	bool operator == (const CSImgT^ img);
	bool operator != (const CSImgT^ img);



	////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 将数据的枚举名转化为对应的字符串。 Convert DataNames to its corresponding string name.
	/// </summary>
	/// <param name="_dn">数据的枚举名 DataNames</param>
	/// <returns>对应的字符串名称</returns>
	/// <remarks>
	/// 数据枚举名和对应字符串的对应表 The list of enum DataNames and corresponding string name:
	/// <code>
	/// { DN::INV			, "INVALID" },
	/// { DN::SEN			, "SENSOR" },
	/// { DN::RAN			, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"			},
	/// { DN::REF			, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"		},
	/// { DN::SCA_8			, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1" },
	/// { DN::SCA_16		, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1" },
	/// { DN::MAR			, "Encoder_information"			},
	/// { DN::RAN_CAL		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"		},
	/// { DN::REF_CAL		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"		},
	/// { DN::REF_CAL_F		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F"},
	/// { DN::SCA_8_CAL		, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL" },
	/// { DN::SCA_16_CAL	, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"},
	/// { DN::RAN2			, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"			},
	/// { DN::REF2			, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"		},
	/// { DN::SCA2_8		, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2" },
	/// { DN::SCA2_16		, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"	},
	/// { DN::RAN2_CAL		, "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"		},
	/// { DN::REF2_CAL		, "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"		},
	/// { DN::SCA2_8_CAL	, "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL" },
	/// { DN::SCA2_16_CAL	, "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"},
	/// { DN::RAN_X	        , "RangeX_Calibration_Only_Without_Rectification"						},
	/// { DN::RAN_X_16	    , "16_Byte_RangeX_Calibration_Only_Without_Rectification"				},
	/// </code>
	/// </remarks>
	String^ DN2Str(const DN _dn);

	/// <summary>
	/// 将相应的字符串转化为数据的枚举名。 Convert string name to its corresponding DataNames.
	/// </summary>
	/// <param name="_str">数据的枚举名的对应字符串 DataNames string name</param>
	/// <returns>对应的枚举值</returns>
	/// <remarks>
	/// 对应字符串和数据枚举名的对应表 The list of string name and corresponding enum DataNames:
	/// <code>
	/// { "INVALID"																	, DN::INV		},
	/// { "SENSOR"																	, DN::SEN		},
	/// { "ComponentSelector_Range_RegionSelector_Scan3dExtraction1"				, DN::RAN		},
	/// { "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"			, DN::REF		},
	/// { "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"		, DN::SCA_8		},
	/// { "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1"		, DN::SCA_16	},
	/// { "ComponentSelector_Range_RegionSelector_Scan3dExtraction1_CAL"			, DN::RAN_CAL	},
	/// { "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL"		, DN::REF_CAL	},
	/// { "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1_CAL_F"	, DN::REF_CAL_F },
	/// { "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"	, DN::SCA_8_CAL	},
	/// { "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1_CAL"	, DN::SCA_16_CAL},
	/// { "ComponentSelector_Range_RegionSelector_Scan3dExtraction2"				, DN::RAN2		},
	/// { "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2"			, DN::REF2		},
	/// { "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"		, DN::SCA2_8	},
	/// { "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2"		, DN::SCA2_16	},
	/// { "ComponentSelector_Range_RegionSelector_Scan3dExtraction2_CAL"			, DN::RAN2_CAL	},
	/// { "ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2_CAL"		, DN::REF2_CAL	},
	/// { "8_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"	, DN::SCA2_8_CAL },
	/// { "16_Byte_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction2_CAL"	, DN::SCA2_16_CAL},
	/// { "RangeX_Calibration_Only_Without_Rectification"							, DN::RAN_X		},
	/// { "16_Byte_RangeX_Calibration_Only_Without_Rectification"					, DN::RAN_X_16	}
	/// </code>
	/// </remarks>
	DN Str2DN(String^ _str);


	////////////////////////////////////////////////////////////////////////////////


	/// <summary>
	/// 根据给定的数据枚举名，获得数据的指针。 Get data pointer according to DN.
	/// </summary>
	/// <param name="_dn">数据枚举名</param>
	/// <returns>
	/// uint8_t 数组，包含激光线图像、激光强度图像、激光散射图像、标定后的激光强度图像、标定后的激光散射图像
	/// Sensor, Reflectance, Scatter, Calibrated Reflectance, Calibrated Scatter.
	/// DN::SEN, DN::REF, DN::REF_CAL.
	/// </returns>
	array<uint8_t>^		getDataByte(const DN _dn);

	/// <summary>
	/// 根据给定的数据枚举名，获得数据的指针。 Get data pointer according to DN.
	/// </summary>
	/// <param name="_dn">数据枚举名</param>
	/// <returns>
	/// uint16_t 数组，包含高度图象。 Range.
	/// DN::RAN, DN::SCA, DN::SCA_CAL.
	/// </returns>
	array<uint16_t>^	getDataWord(const DN _dn);

	/// <summary>
	/// 根据给定的数据枚举名，获得数据的指针。 Get data pointer according to DN.
	/// </summary>
	/// <param name="_dn">数据枚举名</param>
	/// <returns>
	/// float 数组，包含标定后的高度图像、旧版本的标定后的激光强度图像。
	/// Calibrated Range, Old version Calibrated Reflectance.
	/// DN::RAN_CAL, DN::REF_CAL_F, DN::RAN_X.
	/// </returns>
	array<float>^		getDataFloat(const DN _dn);

	/// <summary>
	/// 获得每个剖面的信息，比如时间戳、编码器脉冲计数等。 Get chunkdata.
	/// </summary>
	/// <returns>uint32_t 数组，包含剖面信息</returns>
	array<uint32_t>^	getDataMark();

	/// <summary>
	/// 获得当前所有有效数据的枚举名列表。 Get all avaliable DataNames.
	/// </summary>
	/// <returns>所有有效数据的枚举名数组</returns>
	array<DN>^			getAvalibleDataNames();

	/// <summary>
	/// 获得图像数据宽度。 Get width.
	/// </summary>
	/// <returns>图像宽度</returns>
	const uint32_t		get_cols () { return mp_ImgT->get_cols(); }

	/// <summary>
	/// 获得图像数据高度。 Get height.
	/// </summary>
	/// <returns>图像高度</returns>
	const uint32_t		get_rows () { return mp_ImgT->get_rows(); }


	////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 【不建议再继续使用，可以用 get_cols() 替代】获得激光线图像的数据宽度。 Get width of sensor image.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <returns>激光线图像的数据宽度</returns>
	/// <deprecated>建议使用 get_cols() 替代</deprecated>
	const uint32_t	getS_cols		(int do_not_use_any_more)	{ return get_cols(); }

	/// <summary>
	/// 【不建议再继续使用，可以用 get_rows() 替代】获得激光线图像的数据高度。 Get height of sensor image.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <returns>激光线图像的数据高度</returns>
	/// <deprecated>建议使用 get_rows() 替代</deprecated>
	const uint32_t	getS_rows		(int do_not_use_any_more)	{ return get_rows(); }

	/// <summary>
	/// 不设定，默认为0，单位像素。 To be zero always, uint is pixel.
	/// </summary>
	/// <returns>Sensor 的 X 偏移量，单位像素</returns>
	const uint32_t	getS_OffsetX	()	{ return mp_ImgT->getSensor_info().senOffsetX; }

	/// <summary>
	/// 不设定，默认为0，单位像素。 To be zero always, uint is pixel.
	/// </summary>
	/// <returns>Sensor 的 Y 偏移量，单位像素</returns>
	const uint32_t	getS_OffsetY	()	{ return mp_ImgT->getSensor_info().senOffsetY; }
	
	
	////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 【不建议再继续使用，可以用 get_cols() 替代】获得高度图像的数据宽度。 Get width of range image.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <returns>高度图像的数据宽度</returns>
	/// <deprecated>建议使用 get_cols() 替代</deprecated>
	const uint32_t	getR_cols		(int do_not_use_any_more)		{ return get_cols(); }

	/// <summary>
	/// 【不建议再继续使用，可以用 get_rows() 替代】获得高度图像的数据高度。 Get height of range image.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <returns>高度图像的数据高度</returns>
	/// <deprecated>建议使用 get_rows() 替代</deprecated>
	const uint32_t	getR_rows		(int do_not_use_any_more)		{ return get_rows(); }

	/// <summary>
	/// 获得 ROI 的左上 X 坐标。 Get X of ROI.
	/// </summary>
	/// <returns>ROI 的左上 X 坐标</returns>
	const uint32_t	getR_AoiOffsetX	()	{ return mp_ImgT->getRange_info().aoiOffsetX;}

	/// <summary>
	/// 获得 ROI 的左上 Y 坐标。 Get Y of ROI.
	/// </summary>
	/// <returns>ROI 的左上 Y 坐标</returns>
	const uint32_t	getR_AoiOffsetY	()	{ return mp_ImgT->getRange_info().aoiOffsetY;}

	/// <summary>
	/// 获得 ROI 的高度。 Get Height of ROI.
	/// </summary>
	/// <returns>ROI 的高度</returns>
	const uint32_t	getR_AoiHeight	()	{ return mp_ImgT->getRange_info().aoiHeight;}

	/// <summary>
	/// 获得 ROI 的宽度。 Get Width of ROI.
	/// </summary>
	/// <returns>ROI 的宽度</returns>
	const uint32_t	getR_AoiWidth	()	{ return mp_ImgT->getRange_info().aoiWidth;}

	/// <summary>
	/// 获得 R 轴方向。 Get Axis of Range.
	/// </summary>
	/// <returns>如果为 true 表示标准方向，false 表示反向</returns>
	const bool		getR_RangeAxis	()	{ return RangeAxis::STANDARD == mp_ImgT->getR_RangeAxis();}

	
	////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// 【不建议再继续使用，可以用 get_cols() 替代】获得标定后图像的数据宽度。 Get width of calibrated image.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <returns>标定后图像的数据宽度</returns>
	/// <deprecated>建议使用 get_cols() 替代</deprecated>
	const uint32_t	 getC_cols			(int do_not_use_any_more) { return get_cols(); }

	/// <summary>
	/// 【不建议再继续使用，可以用 get_rows() 替代】获得标定后图像的数据高度。 Get height of calibrated image.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <returns>标定后图像的数据高度</returns>
	/// <deprecated>建议使用 get_rows() 替代</deprecated>
	const uint32_t	 getC_rows			(int do_not_use_any_more) { return get_rows(); }

	/// <summary>
	/// 获得标定后图像的 X 起始值，单位毫米。 Get the offset in X axis of calibrated image, uint is millimeter.
	/// </summary>
	/// <returns>X 起始值，单位毫米。点云 X_mm = scaleX * X_pixel + offsetX。 X of cloud point is X_mm = scaleX * X_pixel + offsetX。</returns>
	const double getC_offsetX		()	{ return mp_ImgT->getCalibration_info().offsetX;}

	/// <summary>
	/// 获得标定后图像的 Y 起始值，单位毫米。 Get the offset in Y axis of calibrated image, uint is millimeter.
	/// </summary>
	/// <returns>Y 起始值，单位毫米。点云 Y_mm = scaleY * Y_pixel + offsetY。 Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY。</returns>
	const double getC_offsetY		()	{ return mp_ImgT->getCalibration_info().offsetY;}

	/// <summary>
	/// 获得标定后图像的 Z 起始值，单位毫米。 Get the offset in Z axis of calibrated image, uint is millimeter.
	/// </summary>
	/// <returns>Z 起始值，单位毫米。点云 Z_mm = scaleZ * Z_pixel + offsetZ。 Z of cloud point is Z_mm = scaleZ * Z_pixel + offsetZ。</returns>
	const double getC_offsetZ		()	{ return mp_ImgT->getCalibration_info().offsetZ;}

	/// <summary>
	/// 获得标定后图像的 X 分辨率，单位 毫米/像素。 Get the scale in X axis of calibrated image, uint is millimeter/pixel.
	/// </summary>
	/// <returns>X 分辨率，单位毫米/像素。点云 X_mm = scaleX * X_pixel + offsetX。 X of cloud point is X_mm = scaleX * X_pixel + offsetX。</returns>
	const double getC_scaleX		()	{ return mp_ImgT->getCalibration_info().scaleX;}

	/// <summary>
	/// 获得标定后图像的 Y 分辨率，单位 毫米/像素，由用户输入。 Get the scale in Y axis of calibrated image, uint is millimeter/pixel. Input by user.
	/// </summary>
	/// <returns>Y 分辨率，单位毫米/像素。点云 Y_mm = scaleY * Y_pixel + offsetY。 Y of cloud point is Y_mm = scaleY * Y_pixel + offsetY。</returns>
	const double getC_scaleY		()	{ return mp_ImgT->getCalibration_info().scaleY;}

	/// <summary>
	/// 获得标定后图像的 Z 分辨率，单位 毫米/像素，由用户输入。 Get the scale in Z axis of calibrated image, uint is millimeter/pixel. Input by user.
	/// </summary>
	/// <returns>Z 分辨率，单位毫米/像素。点云 Z_mm = scaleZ * Z_pixel + offsetZ。 Z of cloud point is Z_mm = scaleZ * Z_pixel + offsetZ。</returns>
	const double getC_scaleZ		()	{ return mp_ImgT->getCalibration_info().scaleZ;}

	/// <summary>
	/// 获得标定后图像换算成点云后，X 的最小值，单位毫米，和 getC_offsetX() 相等。 Get the minimum X in cloud points, uint is millimeter. Equal to getC_offsetX().
	/// </summary>
	/// <returns>X 的最小值，单位毫米。点云 X_mm = scaleX * X_pixel + offsetX。 X of cloud point is X_mm = scaleX * X_pixel + offsetX。</returns>
	const double getC_lower_bound_x	()	{ return mp_ImgT->getCalibration_info().lower_bound_x;}

	/// <summary>
	/// 获得标定后图像换算成点云后，X 的最大值，单位毫米。 Get the maximum X in cloud points, uint is millimeter.
	/// </summary>
	/// <returns>X 的最大值，单位毫米</returns>
	const double getC_upper_bound_x	()	{ return mp_ImgT->getCalibration_info().upper_bound_x;}

	/// <summary>
	/// 获得标定后图像换算成点云后，Z 的最小值，单位毫米。 Get the minimum Z in cloud points, uint is millimeter.
	/// </summary>
	/// <returns>Z 的最小值，单位毫米</returns>
	const double getC_lower_bound_r	()	{ return mp_ImgT->getCalibration_info().lower_bound_r;}

	/// <summary>
	/// 获得标定后图像换算成点云后，Z 的最大值，单位毫米。 Get the maximum Z in cloud points, uint is millimeter.
	/// </summary>
	/// <returns>Z 的最大值，单位毫米</returns>
	const double getC_upper_bound_r	()	{ return mp_ImgT->getCalibration_info().upper_bound_r;}


	////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 0初始，相机停止后重置。 Start from 0. Reset when camera stop.
	/// </summary>
	/// <returns>图像ID</returns>
	const uint64_t	get_ID	()				{ return mp_ImgT->get_ID(); }


	////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 无数据指针返回 false。 Return false if no data pointer.
	/// </summary>
	/// <returns>如果无数据指针返回 false，否则返回 true</returns>
	bool		isEmpty	()				{ return mp_ImgT->isEmpty(); }

	/// <summary>
	/// 无指定的数据返回 false。 Return false if no such data pointer.
	/// </summary>
	/// <param name="_name">数据名称</param>
	/// <returns>如果无指定的数据返回 false，否则返回 true</returns>
	bool		has	(const DN _name)	{ return mp_ImgT->has(static_cast<SickCam::DN>(_name)); }

	/// <summary>
	/// 无编码器信息返回 false。 Return false if no encoder information.
	/// </summary>
	/// <returns>如果无编码器信息返回 false，否则返回 true</returns>
	bool		has_ChunkInfo()			{ return mp_ImgT->has_ChunkInfo(); }

	
	////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 插入 uint8_t* 类型（Byte[]）的数据。 Insert uint8_t* (Byte[]).
	/// </summary>
	/// <param name="_name">图像数据的枚举值。 DataName of inserting data</param>
	/// <param name="_pointer">数据指针。 Data pointer</param>
	/// <param name="_id">图像序号。 Image id</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// 此函数用于相机获得图像并返回。图像基本信息设置必须在调用之前完成。深拷贝，复制所有数据。
	/// It is used in passing data from device to user. Set image information before call it. It will allocate new memory. It is deep copy!
	/// </remarks>
	bool		insertDataByteClone(const DN _name, array<uint8_t>^ _pointer, const uint64_t _id);

	/// <summary>
	/// 插入 uint16_t* 类型（int[]）的数据。 Insert uint16_t* (int[]).
	/// </summary>
	/// <param name="_name">图像数据的枚举值。 DataName of inserting data</param>
	/// <param name="_pointer">数据指针。 Data pointer</param>
	/// <param name="_id">图像序号。 Image id</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// 此函数用于相机获得图像并返回。图像基本信息设置必须在调用之前完成。深拷贝，复制所有数据。
	/// It is used in passing data from device to user. Set image information before call it. It will allocate new memory. It is deep copy!
	/// </remarks>
	bool		insertDataWordClone(const DN _name, array<uint16_t>^ _pointer, const uint64_t _id);

	/// <summary>
	/// 插入 float* 类型（float[]）的数据。 Insert float* (float[]).
	/// </summary>
	/// <param name="_name">图像数据的枚举值。 DataName of inserting data</param>
	/// <param name="_pointer">数据指针。 Data pointer</param>
	/// <param name="_id">图像序号。 Image id</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// 此函数用于相机获得图像并返回。图像基本信息设置必须在调用之前完成。深拷贝，复制所有数据。
	/// It is used in passing data from device to user. Set image information before call it. It will allocate new memory. It is deep copy!
	/// </remarks>
	bool		insertDataFloatClone(const DN _name, array<float>^ _pointer, const uint64_t _id);

	////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// 设置 Sensor 信息
	/// </summary>
	/// <param name="_c">列数</param>
	/// <param name="_r">行数</param>
	/// <param name="_ox">X 偏移量</param>
	/// <param name="_oy">Y 偏移量</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	bool		setSensorInfo	(const uint32_t _c, const uint32_t _r, const uint32_t _ox, const uint32_t _oy)
													{ return mp_ImgT->setSensorInfo(_c, _r, _ox, _oy);}

	/// <summary>
	/// 设置 Sensor 列数
	/// </summary>
	/// <param name="_v">列数</param>
	void		setSen_cols		(const uint32_t _v) { mp_ImgT->setS_cols	(_v);}

	/// <summary>
	/// 设置 Sensor 行数
	/// </summary>
	/// <param name="_v">行数</param>
	void		setSen_rows		(const uint32_t _v) { mp_ImgT->setS_rows	(_v);}

	/// <summary>
	/// 设置 Sensor X 偏移量
	/// </summary>
	/// <param name="_v">X 偏移量</param>
	void		setSen_OffsetX	(const uint32_t _v) { mp_ImgT->setS_OffsetX (_v);}

	/// <summary>
	/// 设置 Sensor Y 偏移量
	/// </summary>
	/// <param name="_v">Y 偏移量</param>
	void		setSen_OffsetY	(const uint32_t _v) { mp_ImgT->setS_OffsetY (_v);}


	////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// 设置 Range 信息
	/// </summary>
	/// <param name="_c">列数</param>
	/// <param name="_r">行数</param>
	/// <param name="_aox">AOI X 偏移量</param>
	/// <param name="_aoy">AOI Y 偏移量</param>
	/// <param name="_ah">AOI 高度</param>
	/// <param name="_aw">AOI 宽度</param>
	/// <param name="_rAxis">Range 轴方向</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	bool		setRangeInfo	(	const uint32_t _c,
									const uint32_t _r,
									const uint32_t _aox,
									const uint32_t _aoy,
									const uint32_t _ah,
									const uint32_t _aw,
									const bool  _rAxis)
									{ return mp_ImgT->setRangeInfo(_c, _r, _aox, _aoy, _ah, _aw, _rAxis == true ? RangeAxis::STANDARD : RangeAxis::REVERSED);}

	/// <summary>
	/// 设置 Range 列数
	/// </summary>
	/// <param name="_v">列数</param>
	void		setRan_cols			(const uint32_t _v) { mp_ImgT->setR_cols		(_v);}

	/// <summary>
	/// 设置 Range 行数
	/// </summary>
	/// <param name="_v">行数</param>
	void		setRan_rows			(const uint32_t _v) { mp_ImgT->setR_rows		(_v);}

	/// <summary>
	/// 设置 Range AOI X 偏移量
	/// </summary>
	/// <param name="_v">AOI X 偏移量</param>
	void		setRan_aoiOffsetX	(const uint32_t _v) { mp_ImgT->setR_aoiOffsetX	(_v);}

	/// <summary>
	/// 设置 Range AOI Y 偏移量
	/// </summary>
	/// <param name="_v">AOI Y 偏移量</param>
	void		setRan_aoiOffsetY	(const uint32_t _v) { mp_ImgT->setR_aoiOffsetY	(_v);}

	/// <summary>
	/// 设置 Range AOI 高度
	/// </summary>
	/// <param name="_v">AOI 高度</param>
	void		setRan_aoiHeight	(const uint32_t _v) { mp_ImgT->setR_aoiHeight	(_v);}

	/// <summary>
	/// 设置 Range AOI 宽度
	/// </summary>
	/// <param name="_v">AOI 宽度</param>
	void		setRan_aoiWidth		(const uint32_t _v) { mp_ImgT->setR_aoiWidth	(_v);}

	/// <summary>
	/// 设置 Range 轴方向
	/// </summary>
	/// <param name="_v">轴方向，true 为标准方向，false 为反向</param>
	void		setRan_RangeAxis	(const bool _v) { mp_ImgT->setR_RangeAxis	(_v==true ? RangeAxis::STANDARD : RangeAxis::REVERSED);}

	
	////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// 标定后图像参数的设定
	/// </summary>
	/// <param name="_c">列数</param>
	/// <param name="_r">行数</param>
	/// <param name="_offsetX">X 起始值，单位毫米</param>
	/// <param name="_offsetY">Y 起始值，单位毫米</param>
	/// <param name="_scaleX">X 分辨率，单位毫米/像素</param>
	/// <param name="_scaleY">Y 分辨率，单位毫米/像素</param>
	/// <param name="_lower_bound_x">X 最小值，单位毫米</param>
	/// <param name="_upper_bound_x">X 最大值，单位毫米</param>
	/// <param name="_lower_bound_r">Z 最小值，单位毫米</param>
	/// <param name="_upper_bound_r">Z 最大值，单位毫米</param>
	/// <param name="_a_axis_range_scale">A 轴范围缩放</param>
	/// <param name="_a_axis_range_offset">A 轴范围偏移</param>
	/// <param name="_c_axis_range_scale">C 轴范围缩放</param>
	/// <param name="_c_axis_range_offset">C 轴范围偏移</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// 注意：offsetZ 参数仅在启用相机内部（in-device）标定时有效，（in-PC）标定时此参数没有意义，可以给 0.0。
	/// 视野在 Z 方向的最小值，毫米。
	/// scaleZ 参数仅在启用相机内部（in-device）标定时有效，（in-PC）标定时此参数没有意义，可以给 1.0。
	/// 视野在 Z 方向的分辨率，毫米/像素。
	/// </remarks>
	bool		setCaliInfo(const int _c,
							const int _r,
							const double _offsetX,
							const double _offsetY,
							const double _scaleX,
							const double _scaleY,
							const double _lower_bound_x,
							const double _upper_bound_x,
							const double _lower_bound_r,
							const double _upper_bound_r,
							const double _a_axis_range_scale, 
							const double _a_axis_range_offset, 
							const double _c_axis_range_scale, 
							const double _c_axis_range_offset )
							{ 
								return mp_ImgT->setCaliInfo(static_cast<size_t>	(_c),
															static_cast<size_t>	(_r),
															_offsetX,
															_offsetY,0.0,
															_scaleX,
															_scaleY,1.0,
															_lower_bound_x,
															_upper_bound_x,
															_lower_bound_r,
															_upper_bound_r,
															_a_axis_range_scale,
															_a_axis_range_offset,
															_c_axis_range_scale,
															_c_axis_range_offset);
							}

	/// <summary>
	/// 设置标定列数
	/// </summary>
	/// <param name="_v">列数</param>
	void		setCali_cols			(const int _v)	  { mp_ImgT->setC_cols		  (_v);}

	/// <summary>
	/// 设置标定行数
	/// </summary>
	/// <param name="_v">行数</param>
	void		setCali_rows			(const int _v)	  { mp_ImgT->setC_rows		  (_v);}

	/// <summary>
	/// 设置标定 X 起始值
	/// </summary>
	/// <param name="_v">X 起始值</param>
	void		setCali_offsetX			(const double _v) { mp_ImgT->setC_offsetX	  (_v);}

	/// <summary>
	/// 设置标定 Y 起始值
	/// </summary>
	/// <param name="_v">Y 起始值</param>
	void		setCali_offsetY			(const double _v) { mp_ImgT->setC_offsetY	  (_v);}

	/// <summary>
	/// 设置标定 X 分辨率
	/// </summary>
	/// <param name="_v">X 分辨率</param>
	void		setCali_scaleX			(const double _v) { mp_ImgT->setC_scaleX	  (_v);}

	/// <summary>
	/// 设置标定 Y 分辨率
	/// </summary>
	/// <param name="_v">Y 分辨率</param>
	void		setCali_scaleY			(const double _v) { mp_ImgT->setC_scaleY	  (_v);}

	/// <summary>
	/// 设置标定 X 最小值
	/// </summary>
	/// <param name="_v">X 最小值</param>
	void		setCali_lower_bound_x	(const double _v) { mp_ImgT->setC_lower_bound_x(_v);}

	/// <summary>
	/// 设置标定 X 最大值
	/// </summary>
	/// <param name="_v">X 最大值</param>
	void		setCali_upper_bound_x	(const double _v) { mp_ImgT->setC_upper_bound_x(_v);}

	/// <summary>
	/// 设置标定 Z 最小值
	/// </summary>
	/// <param name="_v">Z 最小值</param>
	void		setCali_lower_bound_r	(const double _v) { mp_ImgT->setC_lower_bound_r(_v);}

	/// <summary>
	/// 设置标定 Z 最大值
	/// </summary>
	/// <param name="_v">Z 最大值</param>
	void		setCali_upper_bound_r	(const double _v) { mp_ImgT->setC_upper_bound_r(_v);}
	
	
	////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// 仅测试用。读取图形的基本信息。
	/// </summary>
	/// <returns>图像基本信息字符串</returns>
	String^		collectInfo	() { return gcnew String(string2wstring(mp_ImgT->collectInfo()).c_str()); }
	




#ifndef DISABLE_CAL_IN_PC


	/// <summary>
	/// 释放标定资源（静态资源）。
	/// </summary>
	/// <param name="handle">标定句柄</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	static bool	DeleteCalibration(const int handle);


	/// <summary>
	/// 标定和梯形矫正。
	/// </summary>
	/// <param name="calibrationHandle">用户定义的标定句柄id，范围是[0,64)。一般，当程序退出前，请调用 ImgT::DeleteCalibration() 释放这个句柄。程序中，当句柄对应的资源已经被初始化时，将会使用已初始化的资源标定图像。</param>
	/// <param name="calibrationFile">json文件的绝对路径</param>
	/// <param name="removeLastNRows">标定好之后，要去除的最后行数量。一般是在彩色对齐中，在多张图像才能扫描完成的情况下，需要多扫一点数据，在对齐后删除多余行</param>
	/// <param name="doRectify">是否进行第 2 步梯形矫正</param>
	/// <param name="rectificationSpread">进行第 2 步梯形矫正的插值系数，默认是 1.2</param>
	/// <param name="rawRangeWidth">原始数据的宽度，有用户指定的 ROI 区域宽度决定</param>
	/// <param name="rectificationWidth">经过第 1 步标定的图像，它的宽度为 rawRangeWidth。第 2 步梯形矫正的输出图像的宽度，由这个参数设定</param>
	/// <param name="rectMethod">默认是 1，表示梯形矫正的插值方法。可选值详见：enum RectMethod</param>
	/// <param name="numberOfThreads">线程数</param>
	/// <returns>标定后的图像</returns>
	/// <remarks>
	/// RectMethod 可选值：
	/// <code>
	/// enum RectMethod
	/// {
	///     Mean = 0,
	///     Top,
	///     Bottom,
	///     DARKEST,
	///     BRIGHTEST,
	///     NOT_DO,
	/// };
	/// </code>
	/// </remarks>
	CSImgT^	ApplyCalibration(
		const int calibrationHandle,
		const String^ calibrationFile,
		const int removeLastNRows,
		bool doRectify,
		const double rectificationSpread,
		const int rawRangeWidth,
		const int rectificationWidth,
		const int rectMethod,
		const int numberOfThreads
	);

	

	/// <summary>
	/// 释放标定资源（静态）。
	/// </summary>
	/// <param name="handle">标定句柄</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	static bool DeleteCalibrationColor(const int handle);


	/// <summary>
	/// 初始化的彩色标定（申请静态资源）。当无需继续使用后，请调用 DeleteCalibrationColor 释放资源。
	/// </summary>
	/// <param name="colorCalibrateAlignHandle">用户定义的标定句柄id，范围是[0,64)。一般，当程序退出前，请调用 ImgT::DeleteCalibrationColor() 释放这个句柄。程序中，当句柄对应的资源已经被初始化时，将会使用已初始化的资源标定图像。</param>
	/// <param name="calibrationFile">用于彩色对齐的 json 标定文件</param>
	/// <param name="colorFile">用于彩色对齐的 xml 参数文件</param>
	/// <param name="removeLastNRows">标定好之后，要去除的最后行数量。一般是在彩色对齐中，在多张图像才能扫描完成的情况下，需要多扫一点数据，在对齐后删除多余行</param>
	/// <param name="doFCC">是否需要执行平场矫正</param>
	/// <param name="colorInterpolationStride">彩色数据对齐的插值步长，默认是3。设置大些可以连接一些数据缺失点，但是会增加处理时间</param>
	/// <param name="rectificationSpread">进行第 2 步梯形矫正的插值系数，默认是 1.2</param>
	/// <param name="rawColoreResizedWidth">将原始彩色数据的宽度，缩放到指定的大小。这个参数需要和标定期间使用的图像宽度一致，和json文件绑定。如果您使用的是宽度为 2560 的激光线图像作为标定图像，那么这个值应该是2560。如果您使用的是宽度为 5120 的图像作为标定图像，那么这个值应该是5120。</param>
	/// <param name="rectificationWidth">输出图像的大小，默认 5120</param>
	/// <param name="rectMethod">默认是 1，表示梯形矫正的插值方法。可选值详见：enum RectMethod</param>
	/// <param name="removeHeadNRows">要去除的前面行数量</param>
	/// <param name="doCalibration">是否进行标定</param>
	/// <param name="numberOfThreads">线程数</param>
	/// <returns>标定后的图像</returns>
	/// <remarks>
	/// RectMethod 可选值：
	/// <code>
	/// enum RectMethod
	/// {
	///     Mean = 0,
	///     Top,
	///     Bottom,
	///     DARKEST,
	///     BRIGHTEST,
	///     NOT_DO,
	/// };
	/// </code>
	/// </remarks>
	CSImgT^	ApplyCalibrationColor(
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
	);


#endif // DISABLE_CAL_IN_PC


	/// <summary>
	/// 将目标图像 (src) 拼接到当前图像 (this) 的尾后，并且返回合并后数据，修改原始图像。
	/// </summary>
	/// <param name="src">被添加到尾后的图像</param>
	/// <remarks>
	/// 对 src 和 this 的要求如下：
	/// - 假如 src[RAN, REF, R, G, B], this[RAN, REF, G, WHITE]，那么返回的图像只含有[RAN, REF, G]。
	/// - 假如 src[RAN, REF, R, G, B]的 Height 是 1000, this[RAN, REF] 的 height 是 500，那么返回的图像 height 是 1500。
	/// - 假如 src[RAN, REF, R, G, B]中 RAN 和 REF 的 width 是 2560，RGB 的 width 是 5120， this[RAN, REF]的width是2560，那么返回的图像[RAN, REF]的width是2560。
	/// - 假如 src[RAN, REF, R, G, B]中 RAN 和 REF 的 width 是 1280，RGB 的 width 是 5120， this[RAN, REF]的width是2560，那么返回空图像。
	/// </remarks>
	void Append(CSImgT^ src);

	/// <summary>
	/// 将图像分割成多个部分
	/// </summary>
	/// <param name="parts">分割的份数</param>
	/// <param name="imageArray">输出的图像数组</param>
	void	Divide(const int parts, array<CSImgT^>^ imageArray);



	////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// 读取 ICON 图像文件。 load image data from Icon file.
	/// </summary>
	/// <param name="path">无需拓展名。 Please do not provide the suffix</param>
	/// <param name="resolutonY">由机台的编码器决定。若为原始图像，该值不起作用，可设为 1。 Set 1.0 if it is raw data</param>
	/// <param name="resolutonZ">【此参数已无效】相机的 Z 分辨率。若为原始图像，该值不起作用，可设为 1。Float 类型的标定后数据，该值会自动计算，无需设定；Word(uint16_t) 类型的标定后数据，该值需要指定。</param>
	/// <param name="convertRangeToFloat">默认 true。将Word(uint16_t) 类型的标定后Range数据转化成 float。</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// 如果该图像是使用 WORD 表示标定后的 Region 图像，读取函数会将其转化成 FLOAT 类型，并体现为 DataNames::RAN_CAL
	/// </remarks>
	bool		loadFromIconFile(const String^ path, const double resolutonY, const double resolutonZ, const bool convertRangeToFloat);

	/// <summary>
	/// 【不建议再继续使用】，请使用以下的函数替代。
	/// bool SaveSensorImageToIconFile      (string path)
	/// bool SaveRawImagesToIconFile	      (string path, bool withChunkData)
	/// bool SaveCalibratedImagesToIconFile (string path, bool withChunkData)
	/// 保存图像。保存为 ICON 格式的图像数据。 Save data as SICK icon format.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// 如果同时包含原始数据和标定后数据，该函数会保存成两组文件。标定后文件使用 _CAL 进行区分
	/// </remarks>
	/// <deprecated>建议使用 SaveSensorImageToIconFile、SaveRawImagesToIconFile 或 SaveCalibratedImagesToIconFile 替代</deprecated>
	bool		SaveToIconFile(const String^ do_not_use_any_more);

	/// <summary>
	/// 【不建议再继续使用】，请使用以下的函数替代。
	/// bool SaveSensorImageToIconFile      (string path)
	/// bool SaveRawImagesToIconFile	      (string path, bool withChunkData)
	/// bool SaveCalibratedImagesToIconFile (string path, bool withChunkData)
	/// 保存为 ICON 格式的图像数据。 Save data as SICK icon format.
	/// </summary>
	/// <param name="do_not_use_any_more">不使用此参数</param>
	/// <param name="components">指定要保存的图像类型。必须是同一类型的图像才可以存在同一份 dat 文件中。 Saving type. See note for details.</param>
	/// <param name="is_calibrated">指定的图像，是否都是属于标定后图像</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// components 之中不可以混合原始数据和标定后数据。
	/// <code>
	/// SaveToIconFile("D:/img_sen", { DataNames::SEN }, false); // only sensor is allowed.
	/// SaveToIconFile("D:/img_raw", { DataNames::RAN, DataNames::REF, DataNames::SCA_16, DataNames::MAR }, false); // SCA_8 and SCA_16 are mutually exclusive.
	/// SaveToIconFile("D:/img_cal", { DataNames::RAN_CAL, DataNames::REF_CAL, DataNames::SCA_16_CAL, DataNames::MAR  }, true); // MAR is the encoder data, can be saved together with both.
	/// 
	/// // Wrong example:
	/// SaveToIconFile("D:/img_cal", { DataNames::RAN, DataNames::REF, DataNames::RAN_CAL, DataNames::REF_CAL }, true); // Do not mix them!
	/// </code>
	/// </remarks>
	/// <deprecated>建议使用 SaveSensorImageToIconFile、SaveRawImagesToIconFile 或 SaveCalibratedImagesToIconFile 替代</deprecated>
	bool		SaveToIconFile(const String^ do_not_use_any_more, array<DN>^ components, bool is_calibrated);

	/// <summary>
	/// 保存激光线图像。相机若不处于激光线模式，则返回 false。成功保存，则返回 true。
	/// </summary>
	/// <param name="path">路径，不带后缀名。 Path; without suffix</param>
	/// <param name="byFStream">默认为 false。设置为 true 表示使用 3.3.0.0 之前版本的写入方式，设置为 false 表示使用最新的写入方式，速度提升。</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	bool		SaveSensorImageToIconFile		(const String^ path, bool byFStream);

	/// <summary>
	/// 保存未标定的原始图像。相机若不处于高度图模式，则返回 false。成功保存，则返回 true。
	/// </summary>
	/// <param name="path">路径，不带后缀名。 Path; without suffix</param>
	/// <param name="withChunkData">是否包含块数据</param>
	/// <param name="byFStream">默认为 false。设置为 true 表示使用 3.3.0.0 之前版本的写入方式，设置为 false 表示使用最新的写入方式，速度提升。</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	bool		SaveRawImagesToIconFile			(const String^ path, bool withChunkData, bool byFStream);

	/// <summary>
	/// 保存标定后的图像。相机若不处于高度图模式且启用标定功能，则返回 false。成功保存，则返回 true。
	/// </summary>
	/// <param name="path">路径，不带后缀名。 Path; without suffix</param>
	/// <param name="withChunkData">是否包含块数据</param>
	/// <param name="byFStream">默认为 false。设置为 true 表示使用 3.3.0.0 之前版本的写入方式，设置为 false 表示使用最新的写入方式，速度提升。</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	bool		SaveCalibratedImagesToIconFile	(const String^ path, bool withChunkData, bool byFStream);

	/// <summary>
	/// 保存为 txt 的点云格式。 Save data as pointCloud format, txt.
	/// </summary>
	/// <param name="path">保存路径</param>
	/// <returns>成功返回 true，失败返回 false</returns>
	/// <remarks>
	/// 数据示例：
	/// <code>
	/// xValue1, yValue1, zValue1,
	/// xValue2, yValue2, zValue2,
	/// ...
	/// </code>
	/// 该操作仅支持标定后的数据。 Calibrated Data only!
	/// </remarks>
	bool		SaveCalibratedDataToPointCloud(const String^ path);


public:

	bool setData(const SickCam::ImgT & p);
	void _deepCopy(const CSImgT^ img);

public:
	SickCam::ImgT * mp_ImgT;
};


}

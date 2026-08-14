/**	@file	SampleApiDllCSharpWrapper.h
*
*	@brief	C# Wrapper Class of Ranger3.
*
*	@attention
*	This is the C# API wrapper of C++ Class Ranger3 and Ranger3Shared in ranger3.cpp|h.
*
*
*	@copyright	Copyright 2016-2026 SICK AG. All rights reserved.
*
*/

#pragma once

#include "ImgTCSharpWrapper.h"
#include "CommonFunctions.h"
#include <Ranger3.h>
#include <cliext\list>
#include <cliext\map>
#include <vcclr.h>
#include <msclr\lock.h>

#include <string>
#include <sstream>

#include <windows.h>

using namespace System;
using namespace System::Threading;
using namespace msclr;

//#define _CALLBACK_ON
#define _DISABLE_GrabberThread

namespace SICK3D
{

private enum class RequireCode
{
REQ_QUIT_RUN,
REQ_STOP_CAMERA,
REQ_Nothing
};

////////////////////////////////////////////////////////////////////////////////

static SPtr<R3S> gp_R3S;

typedef array<String^>^ deviceList;


/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// </summary>
public ref class GenICam3D
{
public:
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// CTI文件应与exe文件位于同一文件夹中。CTI是由IVP生成的二进制文件，是API库的一部分。
/// </summary>
/// <param name="logPath">日志文件路径。如果设置为""，则使用默认路径。详情见下文。</param>
/// <param name="ctiPath">CTI文件路径。如果设置为""，则使用默认路径。详情见下文。</param>
/// <param name="enShow">若为true，则将日志打印到命令行。True means to print the log to the command line.</param>
/// <param name="enWrite">若为true，则将日志内容写入文件。True means to write the log content to a file.</param>
/// <remarks>
/// 构造函数的基本动作包括：
/// - 1. 将线程优先级设置为最高，以确保在CPU执行期间不受干扰。
/// - 2. 加载日志。默认文件路径为：exe文件路径 + "_时间_log.txt"。
/// - 3. 加载CTI文件。默认搜索范围为exe文件所在文件夹。
/// scanDevice()将在构造函数中调用，用于扫描连接到计算机的所有设备。
/// </remarks>
GenICam3D(	const String^ logPath, 
			const String^ ctiPath, 
			const bool enShow,
			const bool enWrite);
	
~GenICam3D();

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取连接到电脑的设备名列表，以及设备对象指针。
/// </summary>
/// <returns>Dictionary(device_name_string, device_ptr) where key is device name string and value is device pointer.</returns>
deviceList	getConDevList	();

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取连接到电脑的设备IP列表，以及设备对象指针。
/// </summary>
/// <remarks>
/// Invalid of trispector.
/// </remarks>
/// <returns>Dictionary(device_IP_string, device_ptr) where key is device IP string and value is device pointer.</returns>
deviceList	getConDevListIP	();


/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取连接到电脑的设备IP列表，以及设备对象指针。
/// </summary>
/// <remarks>
/// Invalid of trispector.
/// </remarks>
/// <returns>Dictionary(device_MAC_string, device_ptr) where key is device MAC string and value is device pointer.</returns>
deviceList	getConDevListMAC();

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取连接到电脑的设备列表。
/// </summary>
/// <returns>
/// 一个元组，包含：
/// - string: 错误代码或状态。
/// - string: 错误描述，包括中文和英文说明。
/// </returns>
CAM_STATUS	scanDevice		()	{ return static_cast<CAM_STATUS>(gp_R3S->scanDevice());}
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 加载cti文件。
/// </summary>
/// <param name="ctiPath">cti文件路径。The cti file path.</param>
/// <remarks>
/// cti文件由SICK生成。此函数将在构造器中调用。如果isCtiFound()为false且isAvailable()为false，请手动调用它并传递有效的cti文件路径。
/// </remarks>
/// <returns>
/// 一个元组，包含：
/// - ERROR_OPEN_CONSUMER		打开 consumer 失败。Can not open consumer model of GenICam.
/// - All_OK
/// </returns>
CAM_STATUS	loadCtiFile		(const String^ ctiPath)
								{ return static_cast<CAM_STATUS>(gp_R3S->loadCtiFile(StringRef2string(ctiPath)));}
	

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取初始化结果，检测cti文件和连接设备是否都存在。
/// </summary>
/// <remarks>
/// 结果为：isCtiFound() && isDevFound();
/// </remarks>
/// <returns>
/// true - cti文件存在，并且找到连接设备。Both CTI file and available device found!
/// false - 未连接任何设备、设备未就绪或加载cti文件失败！No device is connected, device is not ready or loading cti file failed!
/// </returns>
bool		isAvaliable		()	{ return gp_R3S->isAvaliable(); };
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取cti文件导入结果。
/// </summary>
/// <returns>
/// true - OK.
/// false - cti文件路径无效，或者未指定。
/// </returns>
bool		isCtiFound		()	{ return gp_R3S->isCtiFound(); };
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取设备扫描结果。
/// </summary>
/// <returns>
/// true - OK.
/// false - 未连接任何设备、设备未就绪！No device is connected, device is not ready!
/// </returns>
bool		isDevFound		()	{ return gp_R3S->isDevFound(); };

bool		isLogOutput		()	{ return gp_R3S->isLogOutput(); };
bool		isLogToFile		()	{ return gp_R3S->isLogToFile(); };
	
void 		_release		();
static String^ getPathToRanger3Producer		();

private:

};

////////////////////////////////////////////////////////////////////////////////

#ifndef _DISABLE_GrabberThread
ref class CLR_Mutex {};
private ref class GrabberThread
{
public:
GrabberThread(R3 * pr3);
~GrabberThread	();
	
void	run			();
CSImgT^ getData		();
void	setSleepTime(const unsigned int sleepTime)	{ lock l(locker_sleepTime);	m_sleepTime = sleepTime;}
int		getSleepTime()						{ lock l(locker_sleepTime); return m_sleepTime; };

bool	sendRequest	(const RequireCode req);
	
bool	isFinishGrab() { return m_IsFinishGrab;}
bool	isRun		() { return m_IsRun;}
bool	isGrabbed	() { return m_IsGrabbed; }

protected:
// return true to break run().
bool	_doRequest	();
bool	_clear_and_break	();

private:
R3 *  mp_R3;

// modified by different threads
cliext::list<CSImgT^> m_ImageData;
RequireCode m_req;
unsigned int m_sleepTime;

// status
bool m_IsGrabbed;
bool m_IsRun;
bool m_IsFinishGrab;

// locker
Object^ locker_data;
Object^ locker_req;
Object^ locker_sleepTime;

};

////////////////////////////////////////////////////////////////////////////////
#ifdef _CALLBACK_ON
private ref class GrabberCallback
{
public:
GrabberCallback(R3 * pr3) {};

void	_callback_run();
void	_callback_run_on_grabbed(Object^ _img);

Object^				m_callBack_inputs;
bool				m_callback_is_on;
bool				m_callback_is_finish_copy;
Object^				m_callback_is_finish_copy_locker;
bool				m_callback_require_stop;
Object^				m_callback_require_stop_locker;


R3 *  mp_R3;

};
#endif

#endif

////////////////////////////////////////////////////////////////////////////////


/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 每个 Ranger3 对应应该初始化为一个 类Ranger3 的对象。
/// </summary>
/// 
/// <remarks>
/// csv 文件参数中的参数均为可读可写。 Write&Read Parameters Example
/// </remarks>
/// <code>
/// #Version, 1
/// #Root
/// #DeviceControl
/// DeviceScanType, Linescan3D
/// DeviceLinkThroughputLimit, 118750000
/// DeviceLinkHeartbeatTimeout, 3000
/// #ImageFormatControl
/// ExposureTime_RegionSelector_Region0, 100
/// ExposureTime_RegionSelector_Region1, 100
/// Width_RegionSelector_Region1, 2560
/// Height_RegionSelector_Region1, 672
/// Height_RegionSelector_Scan3dExtraction1, 1000
/// OffsetX_RegionSelector_Region1, 0
/// OffsetY_RegionSelector_Region1, 60
/// ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1, 0
/// ComponentEnable_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1, 0
/// PixelFormat_ComponentSelector_Range_RegionSelector_Scan3dExtraction1, Coord3D_C16
/// PixelFormat_ComponentSelector_Scatter_RegionSelector_Scan3dExtraction1, Mono16
/// TestPattern_ComponentSelector_Intensity_RegionSelector_Region0, Off
/// TestPattern_ComponentSelector_Range_RegionSelector_Scan3dExtraction1, Off
/// #Scan3dControl
/// Scan3dExtractionMethod_Scan3dExtractionSelector_Scan3dExtraction1, Hi3D
/// WamSize_Scan3dExtractionSelector_Scan3dExtraction1, Normal
/// SearchMode3D_Scan3dExtractionSelector_Scan3dExtraction1, GlobalMax
/// RangeAxis_Scan3dExtractionSelector_Scan3dExtraction1, Reverse
/// DetectionThreshold_Scan3dExtractionSelector_Scan3dExtraction1, 20
/// SearchDirection_Scan3dExtractionSelector_Scan3dExtraction1, Standard
/// ReflectanceFilter_Scan3dExtractionSelector_Scan3dExtraction1, 1
/// ScatterMode_Scan3dExtractionSelector_Scan3dExtraction1, SymmetricSideBand
/// ScatterOffset_Scan3dExtractionSelector_Scan3dExtraction1, 6
/// ScatterWidth_Scan3dExtractionSelector_Scan3dExtraction1, 4
/// ScatterGain_Scan3dExtractionSelector_Scan3dExtraction1, 4
/// ScatterReductionCurve_Scan3dExtractionSelector_Scan3dExtraction1, Linear
/// ScatterReductionLowerLimit_Scan3dExtractionSelector_Scan3dExtraction1, 0
/// ScatterReductionUpperLimit_Scan3dExtractionSelector_Scan3dExtraction1, 1500
/// ScatterReductionKnee1Input_Scan3dExtractionSelector_Scan3dExtraction1, 22
/// ScatterReductionKnee1Output_Scan3dExtractionSelector_Scan3dExtraction1, 12
/// ScatterReductionKnee2Input_Scan3dExtractionSelector_Scan3dExtraction1, 54
/// ScatterReductionKnee2Output_Scan3dExtractionSelector_Scan3dExtraction1, 39
/// Scan3dOutputMode_Scan3dExtractionSelector_Scan3dExtraction1, UncalibratedC
/// #AcquisitionControl
/// AcquisitionMode, Continuous
/// AcquisitionFrameRate, 20
/// AcquisitionLineRate, 1000
/// TriggerMode_TriggerSelector_LineStart, Off
/// TriggerMode_TriggerSelector_FrameStart, Off
/// TriggerMode_TriggerSelector_AreascanFrameStart, Off
/// TriggerSource_TriggerSelector_LineStart, Encoder
/// TriggerSource_TriggerSelector_FrameStart, FrameTriggerInput
/// TriggerSource_TriggerSelector_ExposureStart, LineStart
/// TriggerSource_TriggerSelector_AreascanFrameStart, Encoder
/// TriggerActivation_TriggerSelector_FrameStart, RisingEdge
/// TriggerDelay_TriggerSelector_ExposureStart, 0
/// MultiSlopeMode, PresetMedium
/// MultiSlopeKneePointCount, 1
/// #DigitalIOControl
/// LineInverter_LineSelector_LaserStrobe1Output, 1
/// LineInverter_LineSelector_LaserStrobe2Output, 1
/// LineFormat_LineSelector_EncoderAInput, RS422
/// LineFormat_LineSelector_EncoderBInput, RS422
/// #TimerControl
/// TimerDuration_TimerSelector_LaserStrobe1Timer, 0
/// TimerDuration_TimerSelector_LaserStrobe2Timer, 0
/// TimerDelay_TimerSelector_LaserStrobe1Timer, 0
/// TimerDelay_TimerSelector_LaserStrobe2Timer, 0
/// TimerTriggerSource_TimerSelector_LaserStrobe1Timer, Off
/// TimerTriggerSource_TimerSelector_LaserStrobe2Timer, Off
/// #EncoderControl
/// EncoderResolution, 0.00400000019
/// EncoderMode, FourPhase
/// EncoderDivider, 1
/// EncoderOutputMode, DirectionUp
/// EncoderResetSource, Off
/// #EventControl
/// EventNotification_EventSelector_LogMessage, On
/// #LogMessageData
/// #EventTestData
/// #UserSetControl
/// #FileAccessControl
/// #ChunkDataControl
/// ChunkModeActive, 0
/// #TestControl
/// #TransportLayerControl
/// #GigEVision
/// GevSCPD, 0
/// #Capabilities
/// #ProductConfiguration
/// #FirmwareUpdate
/// #FrameBufferStatus
/// #PartSyncStatus
/// #MultipartFrontendStatus
/// #DescramblerStatus
/// #PostProcessingStatus
/// #ImagerInterfaceStatus
/// #M30Debug
/// #M30RegisterPeekPoke
/// #DataDumper
/// #LogEventTest
/// #DataTypesTest
/// #ValidationTest
/// #AvailableTest
/// </code>
public ref class Ranger3
{
public:

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 使用 IP 或者 MAC 地址指定相机。
/// </summary>
/// <param name="ip_mac">IP 或者 MAC 地址。IP地址可以通过Ranger3Studio 或者 MFC demo 中找到。请看 R3S::scanDevice() 中展示的扫描过程。
/// IP 示例："192.168.0.12"。Mac 地址可以在设备的标签上找到。Mac 示例："0006770c69e1"。
/// </param>
/// <param name="isIP">置为 true 表示使用 IP 地址。</param>
Ranger3(const String ^ ip_mac, const bool isIP);
~Ranger3();

public:
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取初始化结果。
/// </summary>
/// <returns>
/// - CAM_STATUS::ERROR_NULL_PTR_DEV	初始化失败。
/// - CAM_STATUS::All_OK
/// </returns>
CAM_STATUS	isReady();

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 连接相机
/// </summary>
/// <remarks>
/// 连接相机的动作包括：打开设备句柄，打开数据流句柄，加载参数文件，扫描相机参数。
/// 当相机连接时，采集图像过程并没有开始。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_STARTED	相机处于 start 状态。Camera is started.
/// - CAM_STATUS::ERROR_SCAN_PARAMETERS	扫描相机参数时发生错误。Error happened when scaning parameters in device.
/// </returns>
CAM_STATUS	connectCamera();

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取初始化结果。
/// </summary>
CAM_STATUS	connectCamera(const bool do_not_use_it_an_more);

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 开始采集
/// </summary>
/// <remarks>
/// 开始采集的动作包括：加载标定文件，申请相机缓存，锁定相机参数，开始采集图像。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。Camera is disconnected.
/// - CAM_STATUS::ERROR_START_ACQUISITION		相机开始采集失败。Failed to start acquisition.
/// - CAM_STATUS::ERROR_CALIBRATION_PATH		使用标定文件初始化标定变量错误。Error happened when initialzing calibration variables.
/// </returns>
CAM_STATUS	startCamera();

#ifdef _CALLBACK_ON
//typedef void(__stdcall CallbackEvent^)			(CSImgT^ img, Object^ any);
delegate void CallbackEvent(CSImgT^ img, Object^ any); // todo -------

/**
* @brief 开始采集，并启动 callback 。 \n\n
* Start acquisition and passing callback function
*
* @note 开始采集的动作包括：加载标定文件，申请相机缓存，锁定相机参数，开始采集图像。\n
* Callback 函数中的第一个参数是返回的图像。其内存由 API 管理，在 callback 函数调用结束后，API 将释放 ImgT 中的内存。\n
* 所以，使用者在拿到数据之后，应该第一时间做“深拷贝”。\n
* ImgT 中的操作符 = ，已经重载为深拷贝。\n
*
* It will load calibration file, initialize device buffer, lock device parameter, start acquisition.
*
* @param [in] pCallback		Callback funtion pointer;
* @param [in] any			在 callback 中要使用的参数。 The parameters will be passed into callback funtion;
*
* @return
* - All_OK
* - ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。			Camera is disconnected.
* - ERROR_START_ACQUISITION		相机开始采集失败。					Failed to start acquisition.
* - ERROR_CALIBRATION_PATH		使用标定文件初始化标定变量错误。	Error happened when initialzing calibration variables.
*/
CAM_STATUS	startCamera(CallbackEvent^ pCallback, Object^ any);
#endif

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 停止采集
/// </summary>
/// <remarks>
/// 停止采集的动作包括：停止采集图像，解锁相机参数，清除相机缓存。本质上 connected 和 stopped 是一样的状态。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。Camera is disconnected.
/// - CAM_STATUS::ERROR_STOP_ACQUISITION		相机停止采集失败。Failed to stop acquisition.
/// </returns>
CAM_STATUS	stopCamera();

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 断开相机连接
/// </summary>
/// <remarks>
/// 断开相机连接的动作包括：关闭数据流句柄，关闭设备句柄。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_STARTED	相机处于 start 状态。Camera is started.
/// - CAM_STATUS::UNKNOW					未知错误。Unknown problem.
/// </returns>
CAM_STATUS	disconnectCamera();
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 获取图像。
/// </summary>
/// <param name="imgTable">Images data.</param>
/// <remarks>
/// 相机必须处于start状态时才可以采集图像。请注意，非线程安全。
/// Camera must be started! And it is not thread-safe.
/// </remarks>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_IMAGE_NOT_READY - 线程未采集到图像，CLR部分独有。
/// </returns>
CAM_STATUS	getImageData(CSImgT^ imgTable);


/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 设置内置轮询线程的循环等待时间，单位毫秒。建议一般在start之前设置，理论上可以在任何时候设置。
/// </summary>
/// <param name="sleepTime">轮询线程的循环等待时间，默认为50毫秒。The default is 50 ms.</param>
CAM_STATUS	setGrabSleepTime(const unsigned int sleepTime);
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取内置轮询线程的循环等待时间，单位毫秒。
/// </summary>
unsigned int getGrabSleepTime();

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取相机的心跳状态。
/// </summary>
/// <returns>
/// - true	相机心跳读取正常
/// - false 相机心跳读取异常
/// </returns>
bool		getHeartBeat();



/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 尝试重新连接相机
/// </summary>
/// <remarks>
/// 当失去相机心跳时，相机的指针将暂时不可用。当重新发现相机后，可以调用此程序，恢复相机的指针。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK					重新连接成功。
/// - CAM_STATUS::CAM_IS_XXXXXXXXXXXX	    重新连接成功后，再调用此函数，将返回相机的状态。见 getStatus()。
/// - CAM_STATUS::CAM_IS_DISCONNECTED 	重新连接成功后，再调用此函数，将返回相机的状态。一般，相机将处于 Disconnected 状态。
/// - CAM_STATUS::ERROR_CAM_IS_LOST		丢失心跳后，相机将处于此状态。
/// </returns>
CAM_STATUS	reconnectCamera();


////////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_CAL_IN_PC
/// <summary>
/// 设置标定的矫正方法
/// </summary>
/// <param name="value">Top(defalut) / Mean / Buttom.</param>
/// <remarks>
/// 详见标定过程的介绍。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。  Camera is started.
/// - CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。  Camera is disconnected.
/// </returns>
CAM_STATUS	setRectMethos	(const RMethod value)
				{ return static_cast<CAM_STATUS>(mp_R3->setRectMethos(static_cast<SickCam::RectMethod>(value))); };


/// <summary>
/// 设置标定的矫正分割大小
/// </summary>
/// <param name="value">Top(defalut) / Mean / Buttom.</param>
/// <remarks>
/// 详见标定过程的介绍。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。  Camera is started.
/// - CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。  Camera is disconnected.
/// </returns>
CAM_STATUS	setRectSpread(const double value)
{
	return static_cast<CAM_STATUS>(mp_R3->setRectSpread(value));
};
/// <summary>
/// 设置空数据的像素值
/// </summary>
/// <param name="value">用于填补空数据的像素值，默认 -1.0e6，见宏 MISSING_DATA。</param>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。 Camera is started.
/// - CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。 Camera is disconnected.
/// </returns>
CAM_STATUS	setMissingData	(const float value)		{ return static_cast<CAM_STATUS>(mp_R3->setMissingData(value)); };
	
/// <summary>
/// 设置标定文件的路径
/// </summary>
/// <param name="not_use_anymore_call_setCalibration_inPC_instead">若 value 为空，则输出未标定的原始数据据。  If value=="", output uncalibrated image.</param>
/// <returns>
/// - CAM_STATUS::All_OK
/// - CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。  Camera is started.
/// - CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。  Camera is disconnected.
/// </returns>
CAM_STATUS	setCalibraPath	(const String^ not_use_anymore_call_setCalibration_inPC_instead);



/// <summary>
/// 设置是否进行矫正，如需矫正，则请指定校正后的图像宽度。
/// </summary>
/// <param name="toDoRectify">控制是否进行矫正。<para>To rectify or not.</para></param>
/// <param name="rectificationWidth">矫正宽度。在PC上执行的情况下，支持设置范围（0-10000）。一般而言，该值越大，X方向的分辨率越高。但是算法得到的虚拟点也会影响真值，需要根据实际应用情况权衡。<para>Rectification Width.</para></param>
/// <returns>
/// <para>CAM_STATUS.All_OK：执行成功</para>
/// <para>CAM_STATUS.ERROR_CAM_IS_STARTED：相机处于 start 状态。<para>Camera is started.</para></para>
/// <para>CAM_STATUS.ERROR_CAM_IS_DISCONNECTED：相机处于 disconnect 状态。<para>Camera is disconnected.</para></para>
/// </returns>
CAM_STATUS	setDoRectify(const bool toDoRectify, const int rectificationWidth)
{
	return static_cast<CAM_STATUS>(mp_R3->setDoRectify(toDoRectify, static_cast<uint32_t>(rectificationWidth)));
};



#endif // DISABLE_CAL_IN_PC



/// <summary>
/// 设置Y方向分辨率。
/// </summary>
/// <param name="value">分辨率的值，计算方式为：(y_range_in_mm) / (profiles_number).</param>
/// <remarks>
/// 点云Y坐标 = Y_offset + Y_scale * Y_pixelValueInRange。
/// </remarks>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// </returns>
CAM_STATUS	setScaleY		(const double value)	{ return static_cast<CAM_STATUS>(mp_R3->setScaleY(value)); };

/// <summary>
/// 设置参数文件路径。
/// </summary>
/// <param name="value">参数文件的路径。如果value为空，则使用相机的默认参数。If value is "", using device inner parameters.</param>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// </returns>
CAM_STATUS	setParametPath	(const String^ value);
	
/// <summary>
/// 设置相机取图的最大等待时间。
/// </summary>
/// <param name="value">相机取图的最大等待时间，单位毫秒。Maximum waiting time when grabbing an image, unit is milliseconds.</param>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// </returns>
CAM_STATUS	setTimeOut		(const int value)		{ return static_cast<CAM_STATUS>(mp_R3->setTimeOut(value)); };
	
/// <summary>
/// 设置图像缓存的张数。
/// </summary>
/// <param name="value">图像缓存的张数。The number of image buffers.</param>
/// <remarks>
/// 默认是20，最小可以设置为1。设置越大，内存占用越多。设置太小，有丢帧风险。
/// </remarks>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// </returns>
CAM_STATUS	setBufferCount	(const int value)		{ return static_cast<CAM_STATUS>(mp_R3->setBufferCount(value)); };



/// <summary>
/// 设置IP地址。
/// 当相机处于started状态时，不可执行以上任何一种设置。When camera is started, neither settings is available.
/// 设置相机的IP/Subnet的过程较为复杂，请严格按照这里给出的说明操作。相机的IP/Subnet设置分为“永久性IP/Subnet设置”与“临时性IP/Subnet设置”。
/// <strong>永久性IP/Subnet设置:</strong> 相机必须处于connected或者stopped状态。请将"isPersistent"设置为true以执行持久设置。关闭电源时，IP将被保留。
/// <strong>临时性IP/Subnet设置:</strong> 相机必须处于disconnected状态。请将"isPersistent"设置为false。
/// 它将执行"ForcoIP"，这将设置临时IP/Subnet。关闭电源后，新设置的IP/Subnet将丢失，设备将恢复成设置前的IP/Subnet地址。
/// 注意：执行“永久性IP/Subnet设置”时，设置成功后，setIP()将返回CAM_STATUS::ALL_OK。您必须调用R3S::scanDevice()以重新扫描所有设备！然后再次连接相机。
/// </summary>
/// <param name="IP">要设置的新IP地址。The new IP to be set.</param>
/// <param name="isPersistent">如果为true，表示设置永久性的IP。true means to set a persistent IP.</param>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// CAM_STATUS.ERROR_SET_IP - 相机设置失败。Setting failed.
/// </returns>
CAM_STATUS	setIp			(const String^ IP, const bool isPersistent);



/// <summary>
/// 设置Subnet地址。
/// </summary>
/// <param name="Su">要设置的新Subnet地址。The new Subnet to be set.</param>
/// <param name="isPersistent">如果为true，表示设置永久性的Subnet。True means to set a persistent Subnet.</param>
/// <remarks>
/// 设置相机的IP/Subnet的过程较为复杂，请严格按照这里给出的说明操作。相机的IP/Subnet设置分为“永久性IP/Subnet设置”与“临时性IP/Subnet设置”。
/// When setting Subnet, please follow the description here strictly. The settings include "Persistent IP/Subnet setting" and "Temporary IP/Subnet setting".
/// <para>
/// <strong>永久性IP/Subnet设置:</strong> 相机必须处于connected或者stopped状态。请将"isPersistent"设置为true以执行持久设置。关闭电源时，Subnet将被保留。
/// </para>
/// <para>
/// <strong>临时性IP/Subnet设置:</strong> 相机必须处于disconnected状态。请将"isPersistent"设置为false。它将执行"ForcoIP"，这将设置临时IP/Subnet。关闭电源后，新设置的IP/Subnet将丢失，设备将恢复成设置前的IP/Subnet地址。
/// </para>
/// <para>
/// 注意：执行“永久性IP/Subnet设置”时，设置成功后，setSubnet()将返回CAM_STATUS::ALL_OK。但是您必须调用R3S::scanDevice()以重新扫描所有设备！然后再次连接相机。
/// </para>
/// </remarks>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// CAM_STATUS.ERROR_SET_SUBNET - 相机设置失败。Setting failed.
/// </returns>
/// <note>
/// 当相机处于started状态时，不可执行以上任何一种设置。When camera is started, neither settings is available.
/// </note>
CAM_STATUS	setSubnet		(const String^ Su, const bool isPersistent);
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 设置设备参数，即csv文件中的参数。【不支持started时设置参数】
/// </summary>
/// <param name="ParameterName">在csv文件中的参数名。Name shown in csv file.</param>
/// <param name="value">要设置的新值。The new value to be set.</param>
/// <remarks>
/// 设置参数时，相机必须处于connected或者stopped状态。
/// 如果需要设置其他参数，请见3.3.0.4之后的新增接口：setParameter()
/// </remarks>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// CAM_STATUS.ERROR_PARAMETER_VALUE_INVALID - 设置值非法。Invalid setting value.
/// CAM_STATUS.ERROR_PARAMETER_INVALID - 设置失败。Setting failed.
/// </returns>
CAM_STATUS	setParameterValue(const String^ ParamterName, const String^ value);
	
/// <summary>
/// 设置设备参数（不仅包含csv文件中的参数，还可以是其他有多级selector的参数）。但是，需要注意“可写”的参数一般都在csv文件中，所以这里主要考虑的是一些“指令”参数，比如xxxReset之类。【完全兼容setParameterValue，不支持started时设置参数】
/// </summary>
/// <param name="ParameterName">在csv文件的参数名。Name shown in csv file.</param>
/// <param name="value">要设置的新值。The new value to be set.</param>
/// <remarks>
/// 设置参数时，相机必须处于connected或者stopped状态【不支持started时设置参数】。注意，Ranger3Studio中的参数，根据功能，可以分为3种：这个函数支持1和2。
/// <list type="bullet">
/// <item>
/// <description>数值型：整数、小数、字符串；Ranger3Studio上体现为【输入框 / Editbox】。</description>
/// </item>
/// <item>
/// <description>使能型：布尔（使用0，1表示）；Ranger3Studio上体现为【勾选框 / CheckBox】。</description>
/// </item>
/// <item>
/// <description>命令型：Ranger3Studio上体现为【按钮 / Button】。如需使用此类型参数，请调用excuteCommand()</description>
/// </item>
/// </list>
/// </remarks>
/// <returns>
/// CAM_STATUS.All_OK,
/// CAM_STATUS.ERROR_CAM_IS_STARTED - 相机处于start状态。Camera is started.
/// CAM_STATUS.ERROR_CAM_IS_DISCONNECTED - 相机处于disconnect状态。Camera is disconnected.
/// CAM_STATUS.ERROR_PARAMETER_VALUE_INVALID - 设置值非法。Invalid setting value.
/// CAM_STATUS.ERROR_PARAMETER_INVALID - 设置失败。Setting failed.
/// </returns>
/// <seealso cref="excuteCommand()"/>
CAM_STATUS	setParameter(const String^ ParamterName, const String^ value);


////////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_CAL_IN_PC
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取标定的矫正方法
/// </summary>
/// <remarks>
/// 详见标定过程的介绍。
/// <para>建议在相机处于 connected 或者 Stopped 状态时，执行该操作。</para>
/// </remarks>
RMethod	getRectMethos	()	{ return static_cast<RMethod>(mp_R3->getRectMethos());}

/// <summary>
/// 读取标定的矫正分割大小
/// </summary>
/// <remarks>
/// <para>详见标定过程的介绍。</para>
/// <para>建议在相机处于 connected 或者 Stopped 状态时，执行该操作。</para>
/// </remarks>
double	getRectSpread	() { return mp_R3->getRectSpread(); }

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取标定文件的路径
/// </summary>
/// <returns>
/// <para>空字符串：未导入标定文件，输出未标定的原始数据；</para>
/// <para>Empty string. No calibration file, output uncalibrated image.</para>
/// </returns>
/// <remarks>
/// <para>建议在相机处于 connected 或者 Stopped 状态时，执行该操作。</para>
/// <para>Camera should be connected or stopped.</para>
/// </remarks>
String^	getCalibraPath	();

/// <summary>
/// 读取是否进行矫正
/// </summary>
/// <remarks>
/// <para>Get to rectify or not.</para>
/// <para>建议在相机处于 connected 或者 Stopped 状态时，执行该操作。</para>
/// <para>Camera should be connected or stopped.</para>
/// </remarks>
bool	getDoRectify	()	{ return mp_R3->getDoRectify	();}



#endif


/// <summary>
/// 读取 Y 方向分辨率
/// </summary>
/// <remarks>
/// 点云 y 坐标 = Y_offset + Y_scale * Y_pixelValueInRange。
/// </remarks>
/// <remarks>
/// 建议在相机处于connected或者Stopped状态时，执行该操作。
/// </remarks>
double	getScaleY		()	{ return mp_R3->getScaleY		();}
	
/// <summary>
/// 【不建议继续使用，功能冗余。读取空数据的像素值，可以通过解析图像数据来获得。】
/// </summary>
/// <remarks>
/// MissingData 表示用于填补空数据的像素值，默认 -1.0e6，见宏 MISSING_DATA。
/// </remarks>
/// <remarks>
/// 建议在相机处于connected或者Stopped状态时，执行该操作。
/// </remarks>
float	getMissingData	()	{ return mp_R3->getMissingData	();}
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取参数文件路径
/// Get parameters file path.
/// </summary>
/// <returns>
/// "" 空字符串表示未导入参数文件。Empty string indicates no parameters file.
/// </returns>
/// <remarks>
/// 建议在相机处于connected或者Stopped状态时，执行该操作。
/// Camera should be connected or stopped.
/// </remarks>
String^	getParametPath	();
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取相机取图的最大等待时间，单位毫秒
/// </summary>
/// <remarks>
/// 建议在相机处于connected或者Stopped状态时，执行该操作。
/// </remarks>
int		getTimeOut		()	{ return static_cast<int>(mp_R3->getTimeOut());}
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取图像缓存的张数
/// Get the number of image buffers.
/// </summary>
/// <remarks>
/// 默认是 20 ，最小可以设置为 1。设置越大，内存占用越多。设置太小，有丢帧风险。
/// </remarks>
/// <remarks>
/// 建议在相机处于connected或者Stopped状态时，执行该操作。
/// Camera should be connected or stopped.
/// </remarks>
int		getBufferCount	()	{ return static_cast<int>(mp_R3->getBufferCount());}

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 启用边缘增强功能。
/// </summary>
/// <param name="enable">是否启用</param>
/// <returns>
/// CAM_STATUS::All_OK
/// </returns>
CAM_STATUS	enableCorrectRawImageEdgeEffect(const bool enable);

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取 IP 地址
/// Get IP address.
/// </summary>
/// <param name="isPersistent">true 表示读取永久性的 IP。 true means to get persistent IP.</param>
/// <returns>
/// "" 错误返回空字符串。 return empty string when error happened.
/// </returns>
/// <remarks>
/// When getting IP, please follow the description here strictly;
/// The IP/Subnet include "Persistent IP/Subnet" and "Temporary IP/Subnet".
/// </remarks>
/// <returns>
/// - "" 错误返回空字符串。  return empty string when error happened.
/// </returns>
/// <remarks>
/// When getting IP, please follow the description here strictly;
/// The IP/Subnet include "Persistent IP/Subnet" and "Temporary IP/Subnet".
/// </remarks>
/// <par 永久性 IP/Subnet   Persisitent IP/Subnet.>
/// Camera must be connected or stopped. Please set "isPersistent" as true to get persistent IP.
/// </par>
/// <par 临时性 IP/Subnet  Force IP, temporary IP/Subnet.>
/// Camera must be disconnected. Please set "isPersistent" as false;
/// It will get temporary IP/Subnet
/// </par>
/// <note>
/// 相机处于 started 状态时，不可执行以上任何一种设置。
/// When camera is started, neither settings is avaliable.
/// </note>
String^	getIp			(const bool isPersistent);
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取 Subnet
/// Get Subnet.
/// </summary>
/// <param name="isPersistent">true 表示读取永久性的 Subnet。 true means to get persistent Subnet.</param>
/// <returns>
/// "" 错误返回空字符串。  return empty string when error happened.
/// </returns>
/// <remarks>
/// When getting IP, please follow the description here strictly;
/// The IP/Subnet include "Persistent IP/Subnet" and "Temporary IP/Subnet".
/// </remarks>
/// <returns>
/// - "" 错误返回空字符串。  return empty string when error happened.
/// </returns>
/// <remarks>
/// When getting IP, please follow the description here strictly;
/// The IP/Subnet include "Persistent IP/Subnet" and "Temporary IP/Subnet".
/// </remarks>
/// <par 永久性 IP/Subnet   Persisitent IP/Subnet.>
/// Camera must be connected or stopped. Please set "isPersistent" as true to get persistent IP.
/// </par>
/// <par 临时性 IP/Subnet  Force IP, temporary IP/Subnet.>
/// Camera must be disconnected. Please set "isPersistent" as false;
/// It will get temporary IP/Subnet
/// </par>
/// <note>
/// 相机处于 started 状态时，不可执行以上任何一种设置。
/// When camera is started, neither settings is avaliable.
/// </note>
String^	getSubNet		(const bool isPersistent);
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// MAC 地址支持任何相机状态下读取。MAC 地址不可修改，出厂固定。
/// MAC. can be queried in any camera status. MAC. is unchangable.
/// </summary>
/// <returns>
/// "" 错误返回空字符串。  return "" when error happened.
/// </returns>
String^	getMac			();
	
/// <summary>
/// 设备名格式： "SICKGigEVisionTL_DEV_" + mac + "_Ranger3"
/// Device name format : "SICKGigEVisionTL_DEV_" + mac + "_Ranger3"
/// </summary>
/// <returns>
/// "" 错误返回空字符串。  return "" when error happened.
/// </returns>
/// <remarks>
/// 支持任何相机状态下读取。
/// It can be queried in any camera status..
/// </remarks>
String^	getDeviceName	();
	
/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取 csv 文件中的相机参数，相机不可处于 started。【如需要在 started 时读取参数，可使用 getParameter 】
/// Get the device parameters shown in csv file.
/// </summary>
/// <param name="ParamterName">参数名，可以在 csv 文件中找到。Parameter name. It can be found in csv file.</param>
/// <returns>
/// "" 错误，返回空字符串。return empty string when error happened.
/// </returns>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// The camera must be connected or stopped when calling this.
/// </remarks>
String^ getParameterValue(const String^ ParamterName);

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 读取 csv 文件中的相机参数、Ranger3Studio 中的只读参数、指令等。支持包含多级 selector 的参数。
/// Get the device parameters.
/// </summary>
/// <param name="ParamterName">参数名，可以在 csv 文件中找到，或者根据 Ranger3Studio 中的所处位置进行推断。Parameter name. It can be found in csv file or Ranger3Studio.</param>
/// <returns>
/// "" 错误，返回空字符串。return empty string when error happened.
/// </returns>
/// <remarks>
/// 相机必须处于 connected 状态。
/// 特别地，这个函数支持在 started 状态下读取某些参数。某一参数是否支持在 started 状态读写，请根据实际情况测试为准。
/// 注意，Ranger3Studio 中的参数，根据功能，可以分为 3 种：这个函数支持 1 和 2。
/// - 1 数值型：整数、小数、字符串；Ranger3Studio 上体现为【输入框 / Editbox】。
/// - 2 使能型：布尔（使用 0，1 表示）；Ranger3Studio 上体现为【勾选框 / CheckBox】。
/// - 3 命令型：Ranger3Studio 上体现为【按钮 / Button】。如需使用此类型参数，请调用 excuteCommand()
/// </remarks>
/// <remarks>
/// 相机参数名的推导。
/// 例子一：
/// 在 Ranger3Studio 中的多级参数结构中，如果有如下的结构。
/// 1. 选择页面 ImageFormatControl
/// 2. 选择 RegionSelector 的下拉框，再选择 ComponentSelector 的下拉框，然后找到 ComponentEnable 勾选框（0 / 1）。
/// RegionSelector_Scan3dExtraction1 
/// | ____ComponentSelector_Reflectance 
/// | ____ComponentEnable 
/// 3. 仿造下面的结果，可以推导出适当的参数名。
/// 上例中，推导的参数名为：ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1 
/// 并且，勾选框的参数可选值为 0 / 1。
/// 
/// 例子二：
/// 在 Ranger3Studio 中的多级参数结构中，如果有如下的结构。
/// 1. 选择页面 DeviceControl \n
/// 2. 选择 DeviceScanType 下拉框。 
/// 3. 上例中，推导的参数名为：DeviceScanType。这里没有多级的 selector 。 
/// </remarks>
/// /// <returns>
/// - 字符串。
/// - "" 。表示错误，返回空字符串。return empty string when error happened.
/// </returns>
String^	getParameter(const String^ ParamterName);


////////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_CAL_IN_PC
/// <summary>
/// 启用 PC 标定，从相机端只获得原始数据，标定过程在PC端完成，需要设置标定文件。标定文件支持 json 和 xml。推荐使用 xml
/// </summary>
/// <remarks>
/// <para>Enable in-PC calibration, calibration file required.</para>
/// </remarks>
/// <param name="filePath">若 filePath 为空，则输出未标定的原始数据。<para>If filePath=="", output uncalibrated image.</para></param>
/// <param name="toDoRectify">控制是否进行矫正。<para>To rectify or not.</para></param>
/// <param name="rectificationWidth">矫正宽度。在PC上执行的情况下，支持设置范围（0-10000）。一般而言，该值越大，X方向的分辨率越高。但是算法得到的虚拟点也会影响真值，需要根据实际应用情况权衡。<para>Rectification Width.</para></param>
/// <param name="threads_number">设置标定函数将使用的线程数。如果设置为-1，则设置为 CPU 的逻辑核数，如此，则一般会达到 100% 的CPU使用率。如果希望降低 CPU 使用率，可以减少线程数，这样也会增加标定处理的使用时间。</param>
/// <returns>
/// <para>CAM_STATUS.All_OK：正常</para>
/// <para>CAM_STATUS.ERROR_CAM_IS_STARTED：相机处于 start 状态。<para>Camera is started.</para></para>
/// <para>CAM_STATUS.ERROR_CAM_IS_DISCONNECTED：相机处于 disconnect 状态。<para>Camera is disconnected.</para></para>
/// <para>CAM_STATUS.ERROR_CALIBRATION_PATH：标定参数文件的路径不正确。<para>Calibration file path is invalid.</para></para>
/// </returns>
CAM_STATUS	setCalibration_inPC(const String^ filePath, const bool toDoRectify, const int rectificationWidth, const int threads_number);

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 启用 PC 标定，从相机端只获得原始数据，标定过程在PC端完成，需要设置标定文件。标定文件支持 json 和 xml。推荐使用 xml
/// </summary>
/// <remarks>
/// <para>Enable in-PC calibration, calibration file required.</para>
/// </remarks>
/// <param name="filePath">若 filePath 为空，则输出未标定的原始数据。<para>If filePath=="", output uncalibrated image.</para></param>
/// <param name="toDoRectify">控制是否进行矫正。<para>To rectify or not.</para></param>
/// <param name="rectificationWidth">矫正宽度。在PC上执行的情况下，支持设置范围（0-10000）。一般而言，该值越大，X方向的分辨率越高。但是算法得到的虚拟点也会影响真值，需要根据实际应用情况权衡。<para>Rectification Width.</para></param>
/// <param name="threads_number">设置标定函数将使用的线程数。如果设置为-1，则设置为 CPU 的逻辑核数，如此，则一般会达到 100% 的CPU使用率。如果希望降低 CPU 使用率，可以减少线程数，这样也会增加标定处理的使用时间。</param>
/// <param name="runMode">定义 CPU 的优化模式。如果本机不支持 AVX ，必须设置为 0；-1 表示默认值。如果遇到 sheet_of_light.dll 报错，请先使用 cpu-z 查看 CPU 是否支持 AVX。</param>
/// <returns>
/// <para>CAM_STATUS.All_OK：执行成功</para>
/// <para>CAM_STATUS.ERROR_CAM_IS_STARTED：相机处于 start 状态。<para>Camera is started.</para></para>
/// <para>CAM_STATUS.ERROR_CAM_IS_DISCONNECTED：相机处于 disconnect 状态。<para>Camera is disconnected.</para></para>
/// <para>CAM_STATUS.ERROR_CALIBRATION_PATH：标定参数文件的路径不正确。<para>Calibration file path is invalid.</para></para>
/// </returns>
CAM_STATUS	setCalibration_inPC(const String^ filePath, const bool toDoRectify, const int rectificationWidth, const int threads_number, const int runMode);
#endif

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 启用相机内部标定（包含 Calibration + Rectification），标定由相机完成。这个是（推荐）做法。需要提前发送标定文件到相机中（一体式相机已在出厂时放置）。
/// Enable in-Device calibration.
/// </summary>
/// <param name="rectificationWidth">输出图像的宽度（最小160，最大4096，必须是32的倍数）。</param>
/// <param name="range_Uint16">In-Device 标定的原始输出数据是 uin16_t 类型。设置为 false，则将其转化成 float。</param>
/// <param name="enableOpenMP">当 range_Uint16 设置为 false，此参数有意义。true 表示使用 OpenMP 加速计算。这会增加 CPU 使用率。</param>
/// <returns>
/// CAM_STATUS::All_OK
/// CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。  Camera is started.
/// CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。  Camera is disconnected.
/// CAM_STATUS::ERROR_OPERATION_NOT_ALLOW	无法启用相机内部标定。原因可能是相机不支持此功能。详见说明文档中列举的支持相机型号。
/// </returns>
/// <remarks>
/// 相机内标定，最终图像的宽度，由 rectificationWidth 确定，也就是参数 [Scan3dRectificationWidth_Scan3dExtractionSelector_Scan3dExtraction1]。
/// 如果 [rangeTypeUint16] 设置为 true，请特别注意：标定后的 Range 数据要使用 DataNames::RAN_CAL_16 表示，是 uin16_t 类型。
/// </remarks>
CAM_STATUS	setCalibration_inDevice(const uint32_t rectificationWidth, const bool range_Uint16, const bool enableOpenMP)
{
	return static_cast<CAM_STATUS>(mp_R3->setCalibration_inDevice(static_cast<uint32_t>(rectificationWidth), range_Uint16, enableOpenMP));
}

/// <summary>
/// 【警告：旧接口。请优先使用 SwigRanger3 】
/// 启用相机内部标定（只做 Calibration，不做 Rectification），标定由相机完成。需要提前发送标定文件到相机中（一体式相机已在出厂时放置）。
/// Enable in-Device calibration.
/// </summary>
/// <param name="range_Uint16">In-Device 标定输出的 Range 数据 是 uin16_t 类型。设置为 false，则将其转化成 float。</param>
/// <param name="rangeX_Uint16">In-Device 标定输出的 RangeX 数据 是 uin16_t 类型。设置为 false，则将其转化成 float。</param>
/// <param name="enableOpenMP">当 range_Uint16 或者 rangeX_Uint16 其中至少一个设置为 false，此参数有意义。true 表示使用 OpenMP 加速计算。这会增加 CPU 使用率。</param>
/// <returns>
/// CAM_STATUS::All_OK
/// CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。  Camera is started.
/// CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。  Camera is disconnected.
/// CAM_STATUS::ERROR_OPERATION_NOT_ALLOW	无法启用相机内部标定。原因可能是相机不支持此功能。详见说明文档中列举的支持相机型号。
/// </returns>
/// <remarks>
/// 相机内标定，最终图像的宽度，由参数 [Width_RegionSelector_Region1] 控制。
/// 如果 [rangeTypeUint16] 设置为 true，请特别注意：标定后的 Range 数据要使用 DataNames::RAN_CAL_16 表示，是 uin16_t 类型。
/// </remarks>
CAM_STATUS	setCalibration_inDevice_CalibrateAC(const bool range_Uint16, const bool rangeX_Uint16, const bool enableOpenMP)
{ return static_cast<CAM_STATUS>(mp_R3->setCalibration_inDevice_CalibrateAC(range_Uint16, rangeX_Uint16, enableOpenMP)); }

/// <summary>
/// 禁用标定，从相机端只获得原始数据
/// Disable calibration.
/// </summary>
/// <returns>
/// CAM_STATUS::All_OK
/// CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。  Camera is started.
/// CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。  Camera is disconnected.
/// </returns>
/// <remarks>
/// 调用此函数将禁用标定，只能从相机端获取原始数据。
/// </remarks>
CAM_STATUS	setCalibration_No() { return static_cast<CAM_STATUS>(mp_R3->setCalibration_No()); }

/// <summary>
/// 导出相机的参数文件，csv 格式
/// Export Configuration File.
/// </summary>
/// <param name="filePath">csv 文件路径。csv file path.</param>
/// <returns>
/// CAM_STATUS::All_OK
/// CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。			Camera is started.
/// CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。	Camera is disconnected.
/// CAM_STATUS::ERROR_CSV_PATH				无效的文件路径。				Invalid file path.
/// </returns>
/// <remarks>
/// 调用此函数将导出相机的参数文件，以csv格式保存到指定路径。
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
CAM_STATUS exportConfiguretionFile  (const String^ filePath);
CAM_STATUS saveParameterToCSV		(const String^ do_not_use_any_more_replaced_by_exportConfiguretionFile);
	
/// <summary>
/// 加载相机的参数文件，csv 格式
/// import Configuretion File.
/// </summary>
/// <param name="filePath">csv 文件路径。若 value 为空，则使用相机的默认参数  csv file path. If value=="", using device inner parameters.</param>
/// <returns>
/// CAM_STATUS::All_OK
/// CAM_STATUS::ERROR_CAM_IS_STARTED		相机处于 start 状态。			Camera is started.
/// CAM_STATUS::ERROR_CAM_IS_DISCONNECTED	相机处于 disconnect 状态。	Camera is disconnected.
/// CAM_STATUS::ERROR_CSV_PATH				无效的文件路径。				Invalid file path.
/// </returns>
/// <remarks>
/// 调用此函数将加载相机的参数文件，以csv格式从指定路径导入。
/// 相机必须处于 connected 或者 stopped 状态。csv 参数文件必须和相机的固件版本相兼容。
/// 否则会导入失败。
/// </remarks>
CAM_STATUS importConfiguretionFile(const String ^ filePath);
CAM_STATUS loadParameterFrCSV	(const String^ do_not_use_any_more_replaced_by_importConfiguretionFile);


////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 更新相机参数。
/// Update device parameters.
/// </summary>
/// <returns>
/// true    更新完成。  Updating done.
/// false	更新失败。  Updating failed.
/// </returns>
/// <remarks>
/// 调用此函数将尝试更新相机的参数。
/// </remarks>
bool			updateParamtersInfo	();
	
/// <summary>
/// 读取参数的所有分类名。
/// Get categories of device parameters.
/// </summary>
/// <remarks>
/// updateParamtersInfo()必须先行调用。
/// Please call updateParamtersInfo() before this.
/// </remarks>
array<String^>^ paramCategoryList	();

/// <summary>
/// 读取指定分类名下的所有参数全名。
/// Get full-name of device parameters of given category.
/// </summary>
/// <remarks>
/// 全名是指 csv 文件中显示的参数名。 updateParamtersInfo()必须先行调用。
/// Full-name is shown in csv file. Please call updateParamtersInfo() before this.
/// </remarks>
array<String^>^ paramFullNameOfCategory(const String^ categoryName);
	
/// <summary>
/// 读取所有参数全名。
/// Get full-name of all device parameters.
/// </summary>
/// <remarks>
/// 全名是指 csv 文件中显示的参数名。 updateParamtersInfo()必须先行调用。
/// Full-name is shown in csv file. Please call updateParamtersInfo() before this.
/// </remarks>
array<String^>^ paramFullNameList	();
	
/// <summary>
/// 读取指定全名参数的当前设定值。
/// Get current value of given full-name.
/// </summary>
/// <remarks>
/// 全名是指 csv 文件中显示的参数名。 updateParamtersInfo()必须先行调用。
/// Full-name is shown in csv file. Please call updateParamtersInfo() before this.
/// </remarks>
String^			paramCurrentValue	(const String^ paramFullName);
	
/// <summary>
/// 读取指定全名参数的可选设定值。
/// Get optional value of given full-name.
/// </summary>
/// <param name="paramFullName">参数的全名，和csv文件中的一致。Fullname of parameter.</param>
/// <param name="currentValueId">当前参数为有可选项的，这个值才有意义。表示当前设定的可选项的序号，从0开始。ID of current selected optional values.</param>
/// <remarks>
/// 全名是指 csv 文件中显示的参数名。 updateParamtersInfo()必须先行调用。
/// Full-name is shown in csv file. Please call updateParamtersInfo() before this.
/// </remarks>
array<String^>^ paramOptionalValues	(const String^ paramFullName, int% currentValueId);

/// <summary>
/// 读取相机状态。
/// Get camera status.
/// </summary>
/// <returns>
/// CAM_STATUS::CAM_IS_CONNECTED	相机已连接 Connected
/// CAM_STATUS::CAM_IS_STARTED	相机开始采集 Start acquisition
/// CAM_STATUS::CAM_IS_STOPPED	相机停止采集 Stop acquisition
/// CAM_STATUS::CAM_IS_DISCONNECTED 相机未连接 Disconnected
/// CAM_STATUS::ERROR_NULL_PTR_DEV 类 Ranger3 的对象未初始化完成。 Class Ranger3 initialization undone.
/// </returns>
CAM_STATUS	getStatus	()	{ return static_cast<CAM_STATUS>(mp_R3->getStatus()); };
	
/// <summary>
/// 读取图像序号，也是当前取到的图像总数，stop 后清理。
/// Get image ID. The value is also the number of grabbed image. Reset to 0 when stopped.
/// </summary>
/// <returns>0初始。相机必须处于 connected 或者 stopped 状态。</returns>
int			getImgID	()	{ return static_cast<int>(mp_R3->getImgID()); }


////////////////////////////////////////////////////////////////////////////////

bool		isGrabbed	(const String^ do_not_use_any_more) { return false; /* mp_gt->isGrabbed(); */ }

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 发送文件到相机上的 100K 存储空间。
/// Sends a file to the device.
/// </summary>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// 该 100K 空间，默认用来存放标定文件。
/// 100K 空间只可以存放一个文件。后放的文件将覆盖之前的文件。该区域将始终只有一个文件。
/// </remarks>
/// <returns>
/// CAM_STATUS::ALL_OK - 操作成功。
/// CAM_STATUS::ERROR_USER_FILE_NOT_FOUND_IN_PC - 操作失败，无效的文件输入。
/// CAM_STATUS::ERROR_USER_FILE_SEND_FAILED - 操作失败，发送过程失败。
/// </returns>
CAM_STATUS	sendFileToCamera		(const String^ filePath);

/// <summary>
/// 获取相机 100K 存储空间内的文件。
/// Retrive a file from the device.
/// </summary>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
/// <returns>
/// 返回文本的内容。
/// </returns>
String^		retrieveFileFromCamera	();

/// <summary>
/// 删除 100K 存储空间内的文件。
/// Delete a file in the device.
/// </summary>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
/// <returns>
/// - CAM_STATUS::All_OK						成功删除
/// - CAM_STATUS::ERROR_USER_FILE_NOT_EXIST	删除失败，100K 空间内不存在文件
/// </returns>
CAM_STATUS	deleteFileFromCamera	();

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 加载（使能）相机开放的 5 组参数中的一个。
/// </summary>
/// <param name="id">可设置值为 1-5</param>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
/// <returns>
/// - CAM_STATUS::ALL_OK						操作成功。
/// - CAM_STATUS::ERROR_OPERATION_NOT_ALLOW	非法操作，一般为 id 给定的值超出1-5，或者 userSet 没有预设。
/// </returns>
CAM_STATUS	userSet_load			(const int id) { return static_cast<CAM_STATUS>(mp_R3->userSet_load(id)); }
	
/// <summary>
/// 保存当前设置到，相机开放的 5 组参数中的一个。
/// </summary>
/// <param name="id">可设置值为 1-5</param>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
/// <returns>
/// - CAM_STATUS::ALL_OK						操作成功。
/// - CAM_STATUS::ERROR_OPERATION_NOT_ALLOW	非法操作，一般为 id 给定的值超出1-5，或者 userSet 没有预设。
/// </returns>
CAM_STATUS	userSet_save			(const int id) { return static_cast<CAM_STATUS>(mp_R3->userSet_save(id)); }
	
/// <summary>
/// 将相机开放的 5 组参数中的一个，设置为默认启动加载项。
/// </summary>
/// <param name="id">可设置值为 1-5</param>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
/// <returns>
/// - CAM_STATUS::ALL_OK						操作成功。
/// - CAM_STATUS::ERROR_OPERATION_NOT_ALLOW	非法操作，一般为 id 给定的值超出1-5，或者 userSet 没有预设。
/// </returns>
CAM_STATUS	userSet_set_default		(const int id) { return static_cast<CAM_STATUS>(mp_R3->userSet_set_default(id)); }
	
/// <summary>
/// 为相机开放的 5 组参数中的一个，添加描述。
/// </summary>
/// <param name="id">可设置值为 1-5</param>
/// <param name="description">参数组的描述</param>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
/// <returns>
/// - CAM_STATUS::ALL_OK						操作成功。
/// - CAM_STATUS::ERROR_OPERATION_NOT_ALLOW	非法操作，一般为 id 给定的值超出1-5，或者 userSet 没有预设。
/// </returns>
CAM_STATUS	userSet_set_description	(const int id, const String^ description);
	
/// <summary>
/// 读取，相机开放的 5 组参数中的一个的描述。
/// </summary>
/// <param name="id">可设置值为 1-5</param>
/// <remarks>
/// 相机必须处于 connected 或者 stopped 状态。
/// </remarks>
String^		userSet_get_description	(const int id);

/// <summary>
/// 更新相机固件。此函数将会阻塞，请运行后重新扫描相机。
/// </summary>
/// <param name="firmwarePackagePath">固件包路径。</param>
/// <remarks>
/// 过程 1: Link/Data 闪烁，表示发送固件文件到相机。
/// 过程 2: State 闪烁，表示正在执行固件升级，请不要断开相机电源。
/// </remarks>
bool		updateFirmware(const String^ firmwarePackagePath);

////////////////////////////////////////////////////////////////////////////////
protected:



private:
R3 *  mp_R3;

////////////////////////////////////////////////////////////////////////////////

SickCam::Categories	* mp_cate;
SickCam::Params		* mp_map_Cata_ParamStr;
SickCam::AllParams	* mp_map_allPramPtr;
	
////////////////////////////////////////////////////////////////////////////////
Object^ locker_GetData;
#ifndef _DISABLE_GrabberThread

bool				m_enable_CPP_Grab_thread;
Thread^				m_Thread;
GrabberThread^ 		mp_gt;

Object^				locker_GetData;
Object^				locker_Start;

////////////////////////////////////////////////////////////////////////////////
#ifdef _CALLBACK_ON
CallbackEvent^		m_callBack_function;

GrabberCallback^	mp_cb;
#endif 

#endif 

};

////////////////////////////////////////////////////////////////////////////////

}

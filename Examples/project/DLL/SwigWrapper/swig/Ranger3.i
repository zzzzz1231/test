// 定义模块名称为 SwigRanger3, 后续C#中引用的C++ dll名字就是 SwigRanger3.dll
%module SwigRanger3

// Suppress warnings
#pragma SWIG nowarn=845  // Suppress warning about unhandled exceptions
#pragma SWIG nowarn=302  // Suppress warning about redefinition of identifiers

// 包含 Windows 特定的 SWIG 库，用于处理 Windows 平台相关的类型映射
%include "windows.i"

// 定义 EXPORT_TO_DLL 宏以避免 SWIG 中的语法错误
%define EXPORT_TO_DLL
%enddef

// 包含所需的 SWIG 库，用于标准容器和类型的映射
%include "std_string.i"   // 处理 C++ std::string 到 C# string 的映射
%include "std_vector.i"   // 处理 C++ std::vector 到 C# 数组/集合的映射
%include "std_map.i"      // 处理 C++ std::map 到 C# 字典的映射
%include "stdint.i"       // 处理 C++ 整数类型到 C# 对应类型的映射

// 为 uint32_t 类型添加类型映射，确保它被正确映射到 C# 的 uint 类型
%apply unsigned int { uint32_t };

// 尝试包含 std_function.i 来处理 std::function 类型
// 注意：不是所有 SWIG 版本都包含这个文件
#ifdef SWIG_STD_FUNCTION
%include "std_function.i"
#endif

%feature("autodoc", "1");

// 前置声明 CAM_STATUS 枚举，确保 SWIG 在处理 CameraShared 类时能够正确识别这个类型
namespace SickCam {
    enum CAM_STATUS {
        All_OK = 1,
        CAM_IS_CONNECTED,
        CAM_IS_STARTED,
        CAM_IS_STOPPED,
        CAM_IS_DISCONNECTED,
        ERROR_OPEN_CAM,
        ERROR_SCAN_EMPTY_DEVICE,
        ERROR_CTI_NOT_FOUND,
        ERROR_OPEN_CONSUMER,
        ERROR_OPEN_TL_HANDLE,
        ERROR_EMPTY_IF_ID,
        ERROR_EMPTY_IF_HANDLE,
        ERROR_CSV_PATH,
        ERROR_CALIBRATION_PATH,
        ERROR_EMPTY_PATH,
        ERROR_EMPTY_IMG,
        ERROR_SET_CAM,
        ERROR_CLO_CAM,				
        ERROR_GET_IMG,				
        ERROR_GET_SEN_IMG,			
        ERROR_RST_CAM,				
        ERROR_UPDATE_PARA,				
        ERROR_STOP_ACQUISITION,
        ERROR_START_ACQUISITION,
        ERROR_PARAMETER_FORMAT_DISMATCH,
        ERROR_PARAMETER_VALUE_DISMATCH,
        ERROR_PARAMETER_READ_ONLY,
        ERROR_PARAMETER_INVALID,
        ERROR_PARAMETER_VALUE_INVALID,
        ERROR_PARAMETERS_EMPTY,
        ERROR_SCAN_PARAMETERS,
        ERROR_COMMUNICATION,
        ERROR_SET_IP,
        ERROR_SET_SUBNET,
        ERROR_NULL_PTR_DEV,				
        ERROR_NULL_DEV_HANDLE,
        ERROR_NULL_DS_HANDLE,
        ERROR_OPEN_DATASTREAM,
        ERROR_CAM_NOT_CONNECT,
        ERROR_CAM_NOT_START,
        ERROR_CAM_NOT_STOP,
        ERROR_CAM_NOT_DISCONNECT,
        ERROR_CAM_IS_CONNECTED,
        ERROR_CAM_IS_STARTED,
        ERROR_CAM_IS_STOPPED,
        ERROR_CAM_IS_DISCONNECTED,
        ERROR_CAM_IS_UNREACHABLE,
        ERROR_CAM_IS_OCCUPIED,
        ERROR_CAM_IS_LOST,
        ERROR_IMAGE_NOT_READY,
        ERROR_USER_FILE_NOT_EXIST,
        ERROR_USER_FILE_SEND_FAILED,
        ERROR_USER_FILE_NOT_FOUND_IN_PC,
        ERROR_USER_SET_NOT_FOUND,
        ERROR_OPERATION_NOT_ALLOW,
        WARN_IMAGE_LOST,
        TIME_OUT,
        NO_CHANGE_IN_NETWORK, // not use anymore
        UNKNOWN,
        DEFAULT
    };
}


// 为回调函数参数添加类型映射
%typemap(cstype) std::function<void(std::string, std::string, std::string, std::string, void*)> "System.Action<string, string, string, string, System.IntPtr>"
%typemap(csin) std::function<void(std::string, std::string, std::string, std::string, void*)> "$csinput"
%typemap(csout, excode=SWIGEXCODE) std::function<void(std::string, std::string, std::string, std::string, void*)> {
    System.Action<string, string, string, string, System.IntPtr> ret = null;
    $excode
    return ret;
}

// 为其他可能的回调函数类型添加映射
%typemap(cstype) std::function<void(void*)> "System.Action<System.IntPtr>"
%typemap(csin) std::function<void(void*)> "$csinput"
%typemap(csout, excode=SWIGEXCODE) std::function<void(void*)> {
    System.Action<System.IntPtr> ret = null;
    $excode
    return ret;
}

// 为 ImgT 回调函数类型添加映射
%typemap(cstype) std::function<void(ImgT*, void*)> "System.Action<ImgT, System.IntPtr>"
%typemap(csin) std::function<void(ImgT*, void*)> "$csinput"
%typemap(csout, excode=SWIGEXCODE) std::function<void(ImgT*, void*)> {
    System.Action<ImgT, System.IntPtr> ret = null;
    $excode
    return ret;
}

// 为 SPtr<Ranger3Shared> 添加类型映射，使其映射到 SPtr<CameraShared>
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) SPtr<SickCam::Ranger3Shared> {
    $1 = SWIG_IsOK;
}

%typemap(in) SPtr<SickCam::Ranger3Shared> {
    $1 = *reinterpret_cast<SPtr<SickCam::Ranger3Shared>*>(&$input);
}

%typemap(out) SPtr<SickCam::Ranger3Shared> {
    *reinterpret_cast<SPtr<SickCam::Ranger3Shared>*>(&$result) = $1;
}

// 为 const SPtr<Ranger3Shared>& 添加类型映射，使其映射到 const SPtr<CameraShared>&
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) const SPtr<SickCam::Ranger3Shared>& {
    $1 = SWIG_IsOK;
}

%typemap(in) const SPtr<SickCam::Ranger3Shared>& {
    $1 = *reinterpret_cast<SPtr<SickCam::Ranger3Shared>*>(&$input);
}

%typemap(out) const SPtr<SickCam::Ranger3Shared>& {
    *reinterpret_cast<SPtr<SickCam::Ranger3Shared>*>(&$result) = $1;
}

// 为 void* 到 SPtr<CameraShared> 的转换添加类型映射
%typemap(in) SPtr<CameraShared> {
    $1 = *reinterpret_cast<SPtr<CameraShared>*>(&$input);
}

%typemap(out) SPtr<CameraShared> {
    *reinterpret_cast<SPtr<CameraShared>*>(&$result) = $1;
}

// 为函数指针类型添加类型映射
%typemap(cstype) void (*)(const char*, const char*, const char*, const char*, void*) "System.Action<string, string, string, string, System.IntPtr>"
%typemap(csin) void (*)(const char*, const char*, const char*, const char*, void*) "$csinput"
%typemap(csout, excode=SWIGEXCODE) void (*)(const char*, const char*, const char*, const char*, void*) {
    System.Action<string, string, string, string, System.IntPtr> ret = null;
    $excode
    return ret;
}

// 使用 SWIG 的内置支持来定义标准容器的模板实例
%template(StringVector) std::vector<std::string>;

// 在包含头文件之前定义 Str 和 cStr 类型
using Str = std::string;
typedef const Str cStr;


// 使用标准字符串类型映射
%include "std_string.i"

// 覆盖 std::string 的默认空值检查，避免使用 ! 操作符
%typemap(nullcheck, noblock=1) std::string {
    // 检查输入指针是否为空，而不是字符串本身
    if (!$input) {
        SWIG_exception(SWIG_NULLPTR, "null string");
    }
}

// 覆盖 const std::string& 的默认空值检查，避免使用 ! 操作符
%typemap(nullcheck, noblock=1) const std::string& {
    // 检查输入指针是否为空，而不是字符串引用
    if (!$input) {
        SWIG_exception(SWIG_NULLPTR, "null string");
    }
}

// 这些输入类型映射已经由 std_string.i 处理
// 我们只需要覆盖空值检查类型映射

// 映射 const std::string* 到 C# string，用于需要检查 null 的情况
%typemap(ctype) const std::string* "const std::string*"
%typemap(cstype) const std::string* "string"
%typemap(csin) const std::string* "$csinput == null ? null : new $csclassname($csinput)"
%typemap(csout, excode=SWIGEXCODE, canthrow=1) const std::string* {
    string ret = $imcall == null ? null : ($csclassname)$imcall;
    $excode
    return ret;
}

// 为 const std::string* 添加空值检查类型映射，避免 ! 操作符错误
%typemap(nullcheck, type="const std::string*") const std::string* {
    if (!$1) {
        SWIG_exception(SWIG_NULLPTR, "$1_type is null");
    }
}



// 映射 Str 类型到 std::string
%typemap(ctype) Str "std::string"
%typemap(cstype) Str "string"
%typemap(csin) Str "$csinput"
%typemap(csout, excode=SWIGEXCODE, canthrow=1) Str {
    string ret = (string)$imcall;
    $excode
    return ret;
}

// 映射 cStr 类型到 const std::string&
%typemap(ctype) cStr "const std::string&"
%typemap(cstype) cStr "string"
%typemap(csin) cStr "$csinput"
%typemap(csout, excode=SWIGEXCODE, canthrow=1) cStr {
    string ret = (string)$imcall;
    $excode
    return ret;
}

// 映射 uint32_t 到 C# uint
%typemap(ctype) uint32_t "uint32_t"
%typemap(cstype) uint32_t "uint"
%typemap(csin) uint32_t "$csinput"
%typemap(csout, excode=SWIGEXCODE, canthrow=1) uint32_t {
    uint ret = (uint)$imcall;
    $excode
    return ret;
}

// 映射 uint32_t* 到 C# uint
%typemap(ctype) uint32_t* "uint32_t"
%typemap(cstype) uint32_t* "uint"
%typemap(csin) uint32_t* "$csinput"
%typemap(csout, excode=SWIGEXCODE, canthrow=1) uint32_t* {
    uint ret = (uint)$imcall;
    $excode
    return ret;
}

// 映射 const uint32_t& 到 C# uint（用于 setCalibration_inDevice、setTimeOut 等方法）
%typemap(ctype) const uint32_t& "uint32_t"
%typemap(cstype) const uint32_t& "uint"
%typemap(csin) const uint32_t& "$csinput"
%typemap(csout, excode=SWIGEXCODE, canthrow=1) const uint32_t& {
    uint ret = (uint)$imcall;
    $excode
    return ret;
}

// 映射 uint 到 C# uint（如果需要）
%typemap(ctype) uint "uint32_t"
%typemap(cstype) uint "uint"
%typemap(csin) uint "(uint32_t)$csinput"
%typemap(csout, excode=SWIGEXCODE, canthrow=1) uint {
    uint ret = (uint)$imcall;
    $excode
    return ret;
}

// 为接受 const uint32_t& 的函数添加类型映射，确保我们不传递指针
%typemap(in) const uint32_t& (uint32_t temp) {
    temp = (uint32_t)$input;
    $1 = &temp;
}


// 启用 Doxygen 注释处理
%feature("autodoc", "1");


/////////////////////////////////

// 仅导出指定的枚举类型
#define EXPORT_TO_DLL   // 让 SWIG 看到空定义，不报错重定义
#undef  EXPORT_TO_DLL   // 防止冲突
%{
#include <iostream>
using namespace std;
%}






%ignore cout;
%ignore endl;
%ignore std::cout;
%ignore std::endl;

%include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/Typedef.h"


/////////////////------------------////////////////

%{
// 直接包含 SDK 头文件
#include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/Ranger3.h"

#include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/Trispector.h"

#include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/ImageTable.h"
using namespace SickCam;
%}


// 为 DeviceConnection 提供前向声明，以备其他地方需要
namespace SickCam {
    class DeviceConnection;
    class ImgT;
    enum DataNames;
}

#define DN DataNames

// 定义 Str 和 cStr 类型
using Str = std::string;
typedef const Str cStr;

// 忽略 SampleApiDll.lib 中没有实现的方法
%ignore _getGenistreamtraits;
%ignore SickCam::Ranger3::_getGenistreamtraits;
%ignore ::SickCam::Ranger3::_getGenistreamtraits;


// 忽略带有复杂内部类型的方法


%ignore SickCam::Ranger3::Ranger3; // 不使用原生的构造函数，参数复杂。

%ignore SickCam::Ranger3::getAllParameterInfo; // Ignore methods with AllParams/Categories/Params types
//%ignore SickCam::Ranger3::getRectMethos; // Ignore methods with SiCaliWrapper::RectMethod type
//%ignore SickCam::Ranger3::setRectMethos_inPC; // Ignore methods with SiCaliWrapper::RectMethod type
//%ignore SickCam::Ranger3::setRectMethos; // Ignore methods with SiCaliWrapper::RectMethod type

// 忽略带有 std::function 参数的方法，保留基本版本的方法
%ignore SickCam::Ranger3::connectCamera;
%ignore SickCam::Ranger3::startCamera;
%ignore SickCam::Ranger3::InitDevice;
%ignore SickCam::Ranger3::setHeartBeatEventLostEvent;



// 重新添加基本版本的 connectCamera 和 startCamera 方法
%rename("connectCamera") SickCam::Ranger3::connectCamera();
%rename("startCamera") SickCam::Ranger3::startCamera();
%ignore SickCam::Ranger3::m_RangeAxis;


// 使用 SWIG 标准的 int* OUTPUT 类型映射
%include "typemaps.i"
%apply int* OUTPUT { int& currentValueId };


// 为回调函数类型添加类型映射（使用 void*）
%typemap(ctype) std::function<void(SickCam::ImgT*, void*)> "void*"
%typemap(imtype) std::function<void(SickCam::ImgT*, void*)> "System.IntPtr"
%typemap(cstype) std::function<void(SickCam::ImgT*, void*)> "System.IntPtr"
%typemap(csin) std::function<void(SickCam::ImgT*, void*)> "$csinput"
%typemap(in) std::function<void(SickCam::ImgT*, void*)> {
    $1 = reinterpret_cast<std::function<void(SickCam::ImgT*, void*)>*>($input);
}
%typemap(out) std::function<void(SickCam::ImgT*, void*)> {
    $result = reinterpret_cast<void*>($1);
}

// 为 userData 参数添加完整的类型映射
%typemap(ctype) void* "void*"
%typemap(imtype) void* "System.IntPtr"
%typemap(cstype) void* "System.IntPtr"
%typemap(csin) void* "$csinput"
%typemap(in) void* {
    $1 = $input;
}

// 添加事件机制
%typemap(cscode) SickCam::Ranger3 
%{
    /// <summary>
    /// 图像接收事件，当相机捕获到新图像时触发。参数：（image，r3）
    /// </summary>
    /// <param name="ImgT">捕获到的图像对象（不拥有内存所有权）</param>
    /// <param name="Ranger3">相机实例对象，用于访问相机相关方法</param>
    public event System.Action<ImgT, Ranger3> ImageReceived;
    
    /// <summary>
    /// 心跳丢失事件，当相机心跳丢失时触发。参数：（name，ip，mac，msg，userData）
    /// </summary>
    /// <param name="name">设备名称</param>
    /// <param name="ip">设备 IP 地址</param>
    /// <param name="mac">设备 MAC 地址</param>
    /// <param name="msg">心跳丢失原因</param>
    /// <param name="userData">用户自定义数据，在连接相机时传入</param>
    public event System.Action<string, string, string, string, Ranger3> HeartBeatLost;

    /// <summary>
    /// 心跳丢失事件，当相机心跳丢失时触发。参数：（name，ip，mac，msg，userData）
    /// </summary>
    /// <param name="name">设备名称</param>
    /// <param name="ip">设备 IP 地址</param>
    /// <param name="mac">设备 MAC 地址</param>
    /// <param name="msg">心跳丢失原因</param>
    /// <param name="userData">用户自定义数据，在连接相机时传入</param>
    public event System.Action<string, string, string, string, Ranger3> HeartBeatLost_ReconnectOK;
    
    /// <summary>
    /// 心跳丢失事件，当相机心跳丢失时触发。参数：（name，ip，mac，msg，userData）
    /// </summary>
    /// <param name="name">设备名称</param>
    /// <param name="ip">设备 IP 地址</param>
    /// <param name="mac">设备 MAC 地址</param>
    /// <param name="msg">心跳丢失原因</param>
    /// <param name="userData">用户自定义数据，在连接相机时传入</param>
    public event System.Action<string, string, string, string, Ranger3> HeartBeatLost_ReconnectFailed;




    
    // 非托管回调委托类型（必须是非泛型的，才能获取函数指针）
    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
    private delegate void ImageCallbackDelegate(System.IntPtr imgPtr, System.IntPtr r3Ptr);
    
    // 心跳丢失回调委托类型
    [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
    private delegate void HeartBeatLostCallbackDelegate(
        System.IntPtr namePtr, System.IntPtr ipPtr, System.IntPtr macPtr, System.IntPtr msgPtr, System.IntPtr r3Ptr);
    
    // 保存委托引用以防止垃圾回收
    private ImageCallbackDelegate _callbackDelegate;
    private HeartBeatLostCallbackDelegate _heartBeatLostCallbackDelegate;
    private HeartBeatLostCallbackDelegate _heartBeatLostReconnectOKDelegate;
    private HeartBeatLostCallbackDelegate _heartBeatLostReconnectFailedDelegate;


    /// <summary>
    /// 包装方法：使用事件机制启动相机（带用户自定义数据）
    /// </summary>
    /// <param name="grab_sleep_time_ms">抓取时间间隔，单位毫秒</param>
    /// <param name="userData">用户自定义数据，在启动相机时传入</param>
    /// <returns>启动状态</returns>
    public CAM_STATUS StartCameraWithEvent(uint grab_sleep_time_ms) 
    {
        // 创建回调委托并保存引用（防止被垃圾回收）
        // 使用 lambda 捕获当前实例，实现实例级别的回调
        _callbackDelegate = (imgPtr, r3Ptr) => 
        {
            // 创建 ImgT 对象（注意：不拥有内存所有权，由 C++ 层管理）
            ImgT img = new ImgT(imgPtr, false);
            
            // 触发当前实例的事件
            this.ImageReceived?.Invoke(img, this);
        };
        
        // 获取函数指针（非泛型委托可以正常获取）
        System.IntPtr callbackPtr = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(_callbackDelegate);
        
        // 调用原始方法，用户数据完整传递
        return startCameraWithCallback_protected(callbackPtr, grab_sleep_time_ms);
    }
    
    /// <summary>
    /// 清理图像回调相关的资源（应在停止相机后调用）
    /// </summary>
    public void CleanupImageCallback() {
        _callbackDelegate = null;
    }
        


    /// <summary>
    /// 包装方法：使用事件机制连接相机（带心跳丢失回调）
    /// </summary>
    /// <returns>连接状态</returns>
    public CAM_STATUS connectCameraWithEvent(
		uint reconnect_TryTimes,
        uint reconnect_SleepIntervalMs,
		uint heartBeatMs) 
    {
        // 创建心跳丢失回调委托并保存引用（防止被垃圾回收）
        // 使用 lambda 捕获当前实例，实现实例级别的回调
        _heartBeatLostCallbackDelegate = (namePtr, ipPtr, macPtr, msgPtr, callbackUserData) => {
            string name = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(namePtr);
            string ip = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ipPtr);
            string mac = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(macPtr);
            string msg = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(msgPtr);
            
            // 触发当前实例的事件
            this.HeartBeatLost?.Invoke(name, ip, mac, msg, this);
        };
        
        // 获取函数指针（非泛型委托可以正常获取）
        System.IntPtr callbackPtr = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(_heartBeatLostCallbackDelegate);
        

        _heartBeatLostReconnectOKDelegate = (namePtr, ipPtr, macPtr, msgPtr, callbackUserData) => {
            string name = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(namePtr);
            string ip = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ipPtr);
            string mac = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(macPtr);
            string msg = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(msgPtr);
            
            // 触发当前实例的事件
            this.HeartBeatLost_ReconnectOK?.Invoke(name, ip, mac, msg, this);
        };
        
        System.IntPtr reconnectOKCallback = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(_heartBeatLostReconnectOKDelegate);

        _heartBeatLostReconnectFailedDelegate = (namePtr, ipPtr, macPtr, msgPtr, callbackUserData) => {
            string name = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(namePtr);
            string ip = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ipPtr);
            string mac = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(macPtr);
            string msg = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(msgPtr);
            
            // 触发当前实例的事件
            this.HeartBeatLost_ReconnectFailed?.Invoke(name, ip, mac, msg, this);
        };
        
        System.IntPtr reconnectFailedCallback = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(_heartBeatLostReconnectFailedDelegate);


        // 调用原始方法，用户数据完整传递
        return connectCameraWithCallback_protected(
            reconnect_TryTimes, reconnect_SleepIntervalMs, heartBeatMs, 
            callbackPtr, reconnectOKCallback, reconnectFailedCallback
        );

    }


    // 包装方法：使用事件机制连接相机（带心跳丢失回调）
    // <returns>连接状态</returns>
    // public CAM_STATUS ConnectCameraWithEvent() {
    //     return ConnectCameraWithEvent(1000, 1, System.IntPtr.Zero);
    // }
    
    // 包装方法：使用事件机制连接相机（带心跳丢失回调和自定义参数）
    // <param name="microSecond">超时时间，单位微微秒</param>
    // <param name="numCallbackThreadsInPool">回调线程池中的线程数</param>
    // <param name="userData">用户自定义数据，在连接相机时传入</param>
    // <returns>连接状态</returns>
    // public CAM_STATUS ConnectCameraWithEvent(uint microSecond, uint numCallbackThreadsInPool, System.IntPtr userData) {
    //     // 创建心跳丢失回调委托并保存引用（防止被垃圾回收）
    //     // 使用 lambda 捕获当前实例，实现实例级别的回调
    //     _heartBeatLostCallbackDelegate = (namePtr, ipPtr, macPtr, msgPtr, callbackUserData) => {
    //         string name = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(namePtr);
    //         string ip = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(ipPtr);
    //         string mac = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(macPtr);
    //         string msg = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(msgPtr);
            
    //         // 触发当前实例的事件
    //         this.HeartBeatLost?.Invoke(name, ip, mac, msg, callbackUserData);
    //     };
        
    //     // 获取函数指针（非泛型委托可以正常获取）
    //     System.IntPtr callbackPtr = System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate(_heartBeatLostCallbackDelegate);
        
    //     // 调用原始方法，用户数据完整传递
    //     return connectCameraWithCallback_protected(callbackPtr, microSecond, numCallbackThreadsInPool, userData);
    // }
%}

// 为回调函数类型添加类型映射
%typemap(cstype) std::function<void(SickCam::ImgT*, void*)> "System.Action<ImgT, System.IntPtr>"
%typemap(csin) std::function<void(SickCam::ImgT*, void*)> "$csinput"



// 包含 Ranger3.h 头文件，导入 Ranger3 类的定义
%include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/Ranger3.h"



// 使用 %extend 为 Ranger3 类添加 C# 友好的构造函数和方法
%extend SickCam::Ranger3 {
    // 添加直接接受字符串的 C# 友好构造函数
    static Ranger3* createFromIpMac(const std::string& ip_mac, bool useIP, CameraShared* cameraShared) {
        // Create a shared_ptr from the CameraShared instance
        SPtr<Ranger3Shared> sharedPtr(cameraShared);
        return new Ranger3(sharedPtr, ip_mac, useIP);
    }

    // 添加基本版本的 connectCamera 方法
    CAM_STATUS connectCamera() {
        return $self->connectCamera();
    }


    CAM_STATUS connectCameraWithCallback_protected(
        const uint32_t& reconnect_TryTimes, 
        const uint32_t& reconnect_SleepIntervalMs, 
        const uint32_t& heartBeatMs, 
        void* pCallback_Lost, 
        void* pCallback_ReConnectOK, 
        void* pCallback_ReConnectFailed
    ) 
    {
        // 定义函数指针类型，匹配 C# 端的委托签名
        typedef void (*HeartBeatCallbackFunc)(const char*, const char*, const char*, const char*, void*);
        
        // 创建 std::function，使用 lambda 包装 C# 函数指针调用
        std::function<void(std::string*, std::string*, std::string*, std::string*, void*)> func_Lost = 
            [pCallback_Lost,self](std::string* name, std::string* ip, std::string* mac, std::string* msg, void*) 
            {
                // 调用 C# 传递的函数指针
                HeartBeatCallbackFunc cb = reinterpret_cast<HeartBeatCallbackFunc>(pCallback_Lost);
                cb(name->c_str(), ip->c_str(), mac->c_str(), msg->c_str(), self);
            };

        std::function<void(std::string*, std::string*, std::string*, std::string*, void*)> func_ReConnectOK = 
            [pCallback_ReConnectOK, self](std::string* name, std::string* ip, std::string* mac, std::string* msg, void*) 
            {
                // 调用 C# 传递的函数指针
                HeartBeatCallbackFunc cb = reinterpret_cast<HeartBeatCallbackFunc>(pCallback_ReConnectOK);
                cb(name->c_str(), ip->c_str(), mac->c_str(), msg->c_str(), self);
            };

        std::function<void(std::string*, std::string*, std::string*, std::string*, void*)> func_ReConnectFailed = 
            [pCallback_ReConnectFailed, self](std::string* name, std::string* ip, std::string* mac, std::string* msg, void*) 
            {
                // 调用 C# 传递的函数指针
                HeartBeatCallbackFunc cb = reinterpret_cast<HeartBeatCallbackFunc>(pCallback_ReConnectFailed);
                cb(name->c_str(), ip->c_str(), mac->c_str(), msg->c_str(), self);
            };

        return $self->connectCamera(
            reconnect_TryTimes, reconnect_SleepIntervalMs, heartBeatMs, 
            func_Lost, func_ReConnectOK, func_ReConnectFailed
        );
    }


    // 添加基本版本的 startCamera 方法
    CAM_STATUS startCamera() {
        return $self->startCamera();
    }
    
    // 添加使用 C# 委托的 startCamera 方法
    CAM_STATUS startCameraWithCallback_protected(void* callback, 
        const uint32_t& grab_sleep_time_ms)
    {
        // 定义函数指针类型，匹配 C# 端的委托签名
        typedef void (*ImageCallbackFunc)(SickCam::ImgT*, void*);
        
        // 创建 std::function，使用 lambda 包装 C# 函数指针调用
        std::function<void(SickCam::ImgT*, void*)> func = 
            [callback](SickCam::ImgT* img, void*) {
                // 调用 C# 传递的函数指针
                ImageCallbackFunc cb = reinterpret_cast<ImageCallbackFunc>(callback);
                cb(img, nullptr);
            };
        
        return $self->startCamera(func, grab_sleep_time_ms);
    }


    // Add methods with std::string parameter for sendFileToCamera
    CAM_STATUS sendFileToCamera(const std::string& filePath) {
        return $self->sendFileToCamera(filePath);
    }
    
    // Add other missing methods
    std::string retrieveFileFromCamera() {
        return $self->retrieveFileFromCamera();
    }
    
    CAM_STATUS deleteFileFromCamera() {
        return $self->deleteFileFromCamera();
    }
    
    CAM_STATUS userSet_load(const int& id) {
        return $self->userSet_load(id);
    }
    
    CAM_STATUS userSet_save(const int& id) {
        return $self->userSet_save(id);
    }
    
    CAM_STATUS userSet_set_default(const int& id) {
        return $self->userSet_set_default(id);
    }
    
    CAM_STATUS userSet_set_description(const int& id, const std::string& description) {
        return $self->userSet_set_description(id, description);
    }
    
    std::string userSet_get_description(const int& id) {
        return $self->userSet_get_description(id);
    }
    
#ifdef CALLBACK_NEW
    uint64_t get_Heartbeat_Count() const {
        return $self->get_Heartbeat_Count();
    }
#endif

};



// 忽略带有 const std::string& 返回类型的有问题的 Trispector 方法
%ignore SickCam::Trispector::Trispector;
%ignore SickCam::Trispector::getDeviceName; // Ignore method with const std::string& return type
%ignore SickCam::Trispector::getDeviceSN; // Ignore method with const std::string& return type

%include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/Trispector.h"





%include "CameraShared.i"
%include "ImageTable.i"


%shared_ptr(SickCam::CustomerLog);
%ignore CAM_STATUS_str;
namespace std {
    %template(DeviceConnectionMap) map<string, std::shared_ptr<SickCam::DeviceConnection>>;
}



%include "Ver.i"

// todo : 有些StringVector 可以保留，CS 中可以很简单的就返回
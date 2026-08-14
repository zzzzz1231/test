

// 为返回 SickCam::Ranger3* 类型的方法添加 canthrow 属性
%typemap(csout, excode=SWIGEXCODE, canthrow=1) SickCam::Ranger3* {
    Ranger3 ret = new Ranger3($imcall, $owner);
    $excode
    return ret;
}



// 为返回 string 类型的方法添加 canthrow 属性
%typemap(csout, excode=SWIGEXCODE, canthrow=1) const std::string& {
    string ret = (string)$imcall;
    $excode
    return ret;
}

// 为返回 vector<string> 类型的方法添加 canthrow 属性
%typemap(csout, excode=SWIGEXCODE, canthrow=1) std::vector<std::string> {
    StringVector temp = new StringVector($imcall, $owner);
    string[] ret = temp.ToArray();
    $excode
    return ret;
}

// 为返回 SickCam::CAM_STATUS 类型的方法添加 canthrow 属性
%typemap(csout, excode=SWIGEXCODE, canthrow=1) SickCam::CAM_STATUS {
    CAM_STATUS ret = (CAM_STATUS)$imcall;
    $excode
    return ret;
}

// 为 CameraShared::scanDevice 方法添加专门的类型映射，确保返回值正确处理
%typemap(out) CAM_STATUS CameraShared::scanDevice {
    $result = reinterpret_cast<void *>(static_cast<int>($1));
}

%typemap(cstype) CAM_STATUS CameraShared::scanDevice "CAM_STATUS"

%typemap(csout, excode=SWIGEXCODE, canthrow=1) CAM_STATUS CameraShared::scanDevice {
    CAM_STATUS ret = (CAM_STATUS)$imcall;
    $excode
    return ret;
}

// 使用 SWIG 标准的 int* OUTPUT 类型映射
%include "typemaps.i"
%apply int* OUTPUT { int& numberDeviceNewFound };


// 再次包含标准库，确保类型映射正确
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "stdint.i"




// 为 vector<string> 方法添加自定义类型映射，使其返回 string[]
%typemap(cstype, out="string[]") std::vector<std::string> "string[]"


// 包含 std_shared_ptr.i 以支持智能指针
%include <std_shared_ptr.i>

// Add includes for CameraShared.h and Ranger3.h so SWIG understands the types
class DeviceConnection;


// 为 CameraShared 和 Ranger3 类型添加 shared_ptr 支持
%shared_ptr(SickCam::CameraShared)
%shared_ptr(SickCam::Ranger3)

// 首先忽略 CameraShared 中的所有方法
%ignore CameraShared::getConDevListMAC;
%ignore CameraShared::getConDevListIP;
%ignore CameraShared::getConDevListSN;

// 直接定义 CameraShared 类，使其在 C# 中可用
class CameraShared {
public:
    CameraShared( const std::string& logPath, 
                 const std::string& ctiPath="", 
                 const bool enShow = true,
                 const bool enWrite = true);
    ~CameraShared();

    // We'll need to handle deviceList type for C# compatibility
    // but we can still keep the class name the same
    const deviceList   getConDevList(bool available_device_only = false) const;
    const deviceList   getConDevListIP(bool available_device_only = false) const;
    const deviceList   getConDevListMAC(bool available_device_only = false) const;
    const deviceList   getConDevListSN(bool available_device_only = false) const;
    
    CAM_STATUS         scanDevice(int& numberDeviceNewFound);
    CAM_STATUS         scanDevice();
    CAM_STATUS         loadCtiFile(const std::string& ctiPath);
    
    bool               isAvaliable() const;
    bool               isCtiFound() const;
    bool               isDevFound() const;
    bool               isLogOutput() const;
    bool               isLogToFile() const;
    
    std::string        getLastErrorMessage();
    std::string        getVersion();
    std::string        getVersionTime();
    
    static std::string getPathToRanger3Producer();
    static std::string getPathToTrispectorProducer();
};

// 首先，隐藏返回 deviceList 的原始方法
%rename("@") CameraShared::getConDevListMAC;
%rename("@") CameraShared::getConDevListIP;
%rename("@") CameraShared::getConDevListSN;

// 然后，将我们的 C# 友好方法重命名为使用原始方法名称
%rename("getConDevListMAC") CameraShared::getConDevListMAC_CSharp;
%rename("getConDevListIP") CameraShared::getConDevListIP_CSharp;
%rename("getConDevListSN") CameraShared::getConDevListSN_CSharp;

// 使用 %extend 为 CameraShared 添加 C# 友好的设备列表访问方法
%extend CameraShared {
    // 添加返回 vector<string> 的 C# 友好设备列表访问方法
    std::vector<std::string> getConDevListMAC_CSharp() {
        std::vector<std::string> result;
        const deviceList& devices = $self->getConDevListMAC();
        for (const auto& pair : devices) {
            result.push_back(pair.first);
        }
        return result;
    }

    std::vector<std::string> getConDevListIP_CSharp() {
        std::vector<std::string> result;
        const deviceList& devices = $self->getConDevListIP();
        for (const auto& pair : devices) {
            result.push_back(pair.first);
        }
        return result;
    }
    
    std::vector<std::string> getConDevListSN_CSharp() {
        std::vector<std::string> result;
        const deviceList& devices = $self->getConDevListSN();
        for (const auto& pair : devices) {
            result.push_back(pair.first);
        }
        return result;
    }
};




// Add type mapping for void* to IntPtr in C#
%typemap(cstype) void* "System.IntPtr"
%typemap(csin) void* "$csinput"
%typemap(csout, excode=SWIGEXCODE) void* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}

// Add type mappings for specific pointer types used in helper methods
%typemap(ctype) uint8_t* "uint8_t*"
%typemap(cstype) uint8_t* "System.IntPtr"
%typemap(csin) uint8_t* "$csinput"
%typemap(out) uint8_t* {
    $result = $1;
}
%typemap(csout, excode=SWIGEXCODE) uint8_t* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}

%typemap(cstype) uint16_t* "System.IntPtr"
%typemap(csin) uint16_t* "$csinput"
%typemap(csout, excode=SWIGEXCODE) uint16_t* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}

%typemap(cstype) float* "System.IntPtr"
%typemap(csin) float* "$csinput"
%typemap(csout, excode=SWIGEXCODE) float* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}



%typemap(cstype) uint* "System.IntPtr"
%typemap(csin) uint* "$csinput"
%typemap(csout, excode=SWIGEXCODE) uint* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}

// Override the DataMap methods to handle void* -> System.IntPtr conversion
%ignore std::map<DataNames, void*>::Add;  // Ignore the default Add method
%ignore std::map<DataNames, void*>::operator[];  // Ignore the default indexer
%ignore std::map<DataNames, void*>::getitem;  // Ignore the default getitem method
%ignore std::map<DataNames, void*>::setitem;  // Ignore the default setitem method

// Add the correct Add method for IDictionary interface
%csmethodmodifiers std::map<DataNames, void*>::Add "public";
%extend std::map<DataNames, void*> {
  void Add(DataNames key, void* value) {
    (*$self)[key] = value;
  }

  // Add getitem and setitem methods that work with void*
  void* getitem(DataNames key) {
    return (*$self)[key];
  }
  
  void setitem(DataNames key, void* value) {
    (*$self)[key] = value;
  }
};



// Add type mapping for const void* to IntPtr in C#
%typemap(cstype) const void* "System.IntPtr"
%typemap(csin) const void* "$csinput"
%typemap(csout, excode=SWIGEXCODE) const void* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}

%typemap(ctype) uint32_t* "void*"
// Add type mapping for uint32_t* to C# System.IntPtr
%typemap(cstype) uint32_t* "System.IntPtr"
%typemap(csin) uint32_t* "$csinput"
%typemap(csout, excode=SWIGEXCODE) uint32_t* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}

// Add type mapping for const uint32_t* to C# System.IntPtr
%typemap(cstype) const uint32_t* "System.IntPtr"
%typemap(csin) const uint32_t* "$csinput"
%typemap(csout, excode=SWIGEXCODE) const uint32_t* {
    System.IntPtr ret = (System.IntPtr)$imcall;
    $excode
    return ret;
}

// Add more precise type mappings for string parameters in Ranger3 methods
%typemap(cstype) const std::string& "string"
%typemap(csin) const std::string& "$csinput"
%typemap(csout, excode=SWIGEXCODE) const std::string& {
    string ret = (string)$imcall;
    $excode
    return ret;
}

// Add type mapping for uint32_t parameter in Ranger3 methods
%typemap(cstype) uint32_t "uint"
%typemap(csin) uint32_t "$csinput"

// Add type mapping for unsigned char* to byte[] in C#
%typemap(ctype) unsigned char* "unsigned char*"
%typemap(cstype) unsigned char* "byte[]"
%typemap(csin) unsigned char* "$csinput"
%typemap(csout, excode=SWIGEXCODE) unsigned char* {
    byte[] ret = $imcall;
    $excode
    return ret;
}

// Add canthrow attribute to vector<DataNames> return type methods
%typemap(csout, excode=SWIGEXCODE, canthrow=1) std::vector<DataNames> {
    DataNamesVector temp = new DataNamesVector($imcall, $owner);
    DataNames[] ret = temp.ToArray();
    $excode
    return ret;
}

// Add canthrow attribute to vector<unsigned char> return type methods
%typemap(csout, excode=SWIGEXCODE, canthrow=1) std::vector<unsigned char> {
    UnsignedCharVector temp = new UnsignedCharVector($imcall, $owner);
    byte[] ret = temp.ToArray();
    $excode
    return ret;
}
%typemap(csout, excode=SWIGEXCODE) uint32_t {
    uint ret = (uint)$imcall;
    $excode
    return ret;
}

// Add type mapping for uint parameter in Ranger3 methods
%typemap(cstype) uint "uint"
%typemap(csin) uint "$csinput"
%typemap(csout, excode=SWIGEXCODE) uint {
    uint ret = (uint)$imcall;
    $excode
    return ret;
}


// Add typemaps for DataNames enum parameters
%typemap(ctype) SickCam::DataNames "int"
%typemap(cstype) SickCam::DataNames "DataNames"
%typemap(csin) SickCam::DataNames "(int)$csinput"
%typemap(csout, excode=SWIGEXCODE) SickCam::DataNames {
    DataNames ret = (DataNames)$imcall;
    $excode
    return ret;
}

// For output values, convert enum to int
%typemap(out) SickCam::DataNames {
    $result = static_cast<int>($1);
}

// For input values, convert int to enum
%typemap(in) SickCam::DataNames {
    $1 = static_cast<SickCam::DataNames>($input);
}

// Add input typemap for const references to DataNames enum
%typemap(ctype) const SickCam::DataNames& "int"
%typemap(cstype) const SickCam::DataNames& "DataNames"
%typemap(csin) const SickCam::DataNames& "(int)$csinput"
%typemap(in) const SickCam::DataNames& (SickCam::DataNames temp) {
    temp = static_cast<SickCam::DataNames>($input);
    $1 = &temp;
}

// Add input typemap for non-const references to DataNames enum
%typemap(ctype) SickCam::DataNames& "int"
%typemap(cstype) SickCam::DataNames& "DataNames"
%typemap(csin) SickCam::DataNames& "(int)$csinput"
%typemap(in) SickCam::DataNames& (SickCam::DataNames temp) {
    temp = static_cast<SickCam::DataNames>($input);
    $1 = &temp;
}

// Add output typemap for const references to DataNames enum
%typemap(out) const SickCam::DataNames& {
    $result = static_cast<int>(*$1);
}

// Add output typemap for non-const references to DataNames enum
%typemap(out) SickCam::DataNames& {
    $result = static_cast<int>(*$1);
}

// Now define templates that use DataNames
%template(DataNamesVector) std::vector<SickCam::DataNames>;
%template(UnsignedCharVector) std::vector<unsigned char>;


%ignore SickCam::ImgT::operator=;
%ignore SickCam::ImgT::operator==;


#pragma SWIG nowarn=845
%include "windows.i"
%include "std_map.i"
%include "std_string.i"


%ignore SickCam::ImgT::DN_StrName;
%ignore SickCam::ImgT::StrName_DN;


// Ignore the parameterless getData method to avoid SWIGTYPE
%ignore SickCam::ImgT::getData;
%ignore SickCam::ImgT::getData(const DN& _dn);


// Export ImgTData type
//typedef std::map<DataNames, void*> ImgTData;

namespace SiCaliWrapper 
{
    enum RectMethod
    {
        Mean,
        Top,
        Bottom,
        DARKEST,
        BRIGHTEST,
        NOT_DO,
    };
}


%typemap(cscode) SickCam::ImgT 
%{
    // 重载 == 运算符
    public static bool operator ==(ImgT left, ImgT right)
    {
        // 1. 先处理空值
        if (object.ReferenceEquals(left, null))
            return object.ReferenceEquals(right, null);
        if (object.ReferenceEquals(right, null))
            return false;

        // 2. 调用 IsEqual 方法比较内容
        return left.IsEqual(right);
    }

    // 重载 != 运算符
    public static bool operator !=(ImgT left, ImgT right)
    {
        return !(left == right);
    }

    // 配套重写 Equals 和 GetHashCode，是规范要求，也能让 Assert.AreEqual 工作
    public override bool Equals(object obj)
    {
        return obj is ImgT other && this.IsEqual(other);
    }

    public override int GetHashCode()
    {
        // 简单返回基类哈希，或者根据内部数据计算
        return base.GetHashCode();
    }


    /// <summary>
    /// 自动将指针转换为byte数组（长度=row*col）。
    /// 您需要确保您输入的 dn 和返回的数据类型是匹配的。否则，将返回 null。
    /// </summary>
    public unsafe byte[] getDataByte(DataNames dn, uint col) 
    {
        System.IntPtr ptr = getDataByte_protected(dn);
        if (ptr == System.IntPtr.Zero)
        {
            return null;  // 返回空
        }
        var row = get_rows();
        int len = (int)(row * col);
        byte[] arr = new byte[len];

        fixed (byte* pDest = arr)
        {
            System.Buffer.MemoryCopy(
                (void*)ptr,
                pDest,
                len,           // byte 每个元素 1 字节
                len
            );
        }
        return arr;
    }

    /// <summary>
    /// 自动将指针转换为ushort数组（长度=row*col）。
    /// 您需要确保您输入的 dn 和返回的数据类型是匹配的。否则，将返回 null。
    /// </summary>
    public unsafe ushort[] getDataWord(DataNames dn, uint col) 
    {
        System.IntPtr ptr = getDataWord_protected(dn);
        if (ptr == System.IntPtr.Zero)
        {
            return null;  // 返回空
        }

        var row = get_rows();
        int len = (int)(row * col);
        ushort[] arr = new ushort[len];

        fixed (ushort* pDest = arr)
        {
            System.Buffer.MemoryCopy(
                (void*)ptr,    // 源指针
                pDest,          // 目标数组
                len * 2,        // 最大字节数
                len * 2         // 要复制的字节数
            );
        }
        return arr;
    }

    /// <summary>
    /// 自动将指针转换为float数组（长度=row*col）。
    /// 您需要确保您输入的 dn 和返回的数据类型是匹配的。否则，将返回 null。
    /// </summary>
    public unsafe float[] getDataFloat(DataNames dn, uint col) 
    {
        System.IntPtr ptr = getDataFloat_protected(dn);
        if (ptr == System.IntPtr.Zero)
        {
            return null;  // 返回空
        }
        var row = get_rows();
        int len = (int)(row * col);
        float[] arr = new float[len];

        fixed (float* pDest = arr)
        {
            System.Buffer.MemoryCopy(
                (void*)ptr,
                pDest,
                len * 4,       // float 每个元素 4 字节
                len * 4
            );
        }
        return arr;
    }


    /// <summary>
    /// 自动将指针转换为uint数组（长度=row*5）。
    /// 您需要确保您输入的 dn 和返回的数据类型是匹配的。否则，将返回 null。
    /// </summary>
    public unsafe uint[] getDataMark()
    {
        System.IntPtr ptr = getDataMark_protected();
        if (ptr == System.IntPtr.Zero)
        {
            return null;  // 返回空
        }
        var row = get_rows();
        int len = (int)(row * 5); // ChunkData 长度 5 个 uint32_t
        uint[] arr = new uint[len];
        // System.Runtime.InteropServices.Marshal.Copy(ptr, arr, 0, len);
        // return arr;

        fixed (uint* pDest = arr)
        {
            System.Buffer.MemoryCopy(
                (void*)ptr,    // 源指针
                pDest,          // 目标数组
                len * 4,        // 最大字节数
                len * 4         // 要复制的字节数
            );
        }
        return arr;
    }

%}

// 启用 Doxygen 注释处理
%feature("autodoc", "1");
%include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/ImageTable.h"



// Use %extend to add helper methods to ImgT class
%extend SickCam::ImgT 
{
    uint8_t*  getDataByte_protected(DataNames dn)  { return (uint8_t*)$self->getData(dn); }
    uint16_t* getDataWord_protected(DataNames dn) { return (uint16_t*)$self->getData(dn); } 
    float*    getDataFloat_protected(DataNames dn) { return (float*)$self->getData(dn); }
    uint32_t* getDataMark_protected() { return (uint32_t*)$self->getData(DataNames::MAR); }
    
    // 把 == 重载变成普通函数
    bool IsEqual(const ImgT& other) 
    {
        return (*self) == other; // 调用 C++ 原生的 operator==
    }

    /// <summary>
    /// 深度拷贝赋值
    /// </summary>
    void DeepCopy(const ImgT& other) 
    {
        *self = other; // 调用 C++ 原生的 operator=
    }
};

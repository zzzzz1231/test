///////////////  Ver  ///////////////////////////////////////////////


%include "std_string.i"




%typemap(cscode) SickCam::VER 
%{
    /// <summary>
    /// SwigRanger3 包装器版本号
    /// </summary>
    public static string SwigRanger3_Version { get; } = "1.0.8";
    
    /// <summary>
    /// SwigRanger3 包装器版本发布时间
    /// </summary>
    public static string SwigRanger3_BuildTime { get; } = "2026-06-08";

    public static string SwigRanger3_VersionInfo() { 
        return "SICK GenICam SDK CS " + SwigRanger3_Version + " " + SwigRanger3_BuildTime;
    }

%}




%include "D:/Program Files/SICK_GCN/Ranger3Studio/SICK_GENICAM_SDK_FOLDER/SICK/include/apiDll/Version.h"

@echo off
chcp 65001

cd /d %~dp0												 
echo "| 												 
echo "| 												 
echo "|==================================================
echo "|    ___           _        _ _ _             	 
echo "|   |_ _|_ __  ___| |_ __ _| | (_)_ __   __ _ 	 
echo "|    | || '_ \/ __| __/ _` | | | | '_ \ / _` |	 
echo "|    | || | | \__ \ || (_| | | | | | | | (_| |	 
echo "|   |___|_| |_|___/\__\__,_|_|_|_|_| |_|\__, |	 
echo "|                                       |___/ 	 
echo "|==================================================
echo "| 												 
echo "| 												 
echo "|
echo *
echo *
set exePath=%~dp0
echo * Current path = "%exePath%"
echo *
echo *
if defined SICK_GENICAM_SDK (
  echo * 
  echo * Find SICK SDK environment veriables!
  echo * 
) else (
  echo * 
  echo * NO Find SICK SDK environment veriables!
  echo *
  install_X64_Windows.bat
  goto :END
)

set root_folder_name=%SICK_GENICAM_SDK%
echo *
echo * Delete : %SICK_GENICAM_SDK%\* , and re-install?
echo *
echo *


pause


del "%SICK_GENICAM_SDK%\*" /q /f /s
echo *


if exist "%root_folder_name%" (
	md "%root_folder_name%\GenICam"
	md "%root_folder_name%\tinyxml2"
	md "%root_folder_name%\opencv"
	md "%root_folder_name%\SICK\include"
	md "%root_folder_name%\SICK\cti"
	md "%root_folder_name%\SICK\dll\Debug"
	md "%root_folder_name%\SICK\dll\Release"
	md "%root_folder_name%\SICK\lib\Debug"
	md "%root_folder_name%\SICK\lib\Release"
	md "%root_folder_name%\Examples\Sample\ApiDllTest"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon"
) else (
  echo *
  echo * No found folder: %root_folder_name%\*
  echo *
  goto :NOPATH
)

set R3_sln_Path=%exePath%Examples\project


echo ---------- Copy help documents ----------

echo F | xcopy "%R3_sln_Path%\..\..\documents\API_help\C++\html\GenICam_Cpp_API.chm"  "%root_folder_name%\SICK\help\GenICam_Cpp_API.chm" /y
echo F | xcopy "%R3_sln_Path%\..\..\documents\API_help\CSharp\html\GenICam_CSharp_API.chm"  "%root_folder_name%\SICK\help\GenICam_CSharp_API.chm" /y
echo D | xcopy "%R3_sln_Path%\..\..\documents\TrispectorDoc\"  "%root_folder_name%\SICK\help\TrispectorDoc\" /y
echo D | xcopy "%R3_sln_Path%\..\..\documents\Ranger3Doc\"  "%root_folder_name%\SICK\help\Ranger3Doc\" /y
echo F | xcopy "%R3_sln_Path%\..\..\*.pdf"  "%root_folder_name%\Examples\project\" /y
echo F | xcopy "%R3_sln_Path%\..\..\*.pdf"  "%root_folder_name%\" /y
echo F | xcopy "%R3_sln_Path%\..\..\*.pdf"  "%root_folder_name%\SICK\help\" /y

echo ---------- Copy no compile dlls ----------

set TYPE=Debug
set SUFFIX=dll
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%R3_sln_Path%\..\..\ReleaseBatFiles\0-DLL\*.dll"  "%TO%\" /y
echo F | xcopy "%R3_sln_Path%\..\..\ReleaseBatFiles\0-OpenCV\*.dll"  "%TO%\" /y

set TYPE=Release
set SUFFIX=dll
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%R3_sln_Path%\..\..\ReleaseBatFiles\0-DLL\*.dll"  "%TO%\" /y
echo F | xcopy "%R3_sln_Path%\..\..\ReleaseBatFiles\0-OpenCV\*.dll"  "%TO%\" /y



echo ---------- Copy 3rd parties dlls ----------

set TYPE=Debug
set SUFFIX=dll
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%FROM%\*.%SUFFIX%"  "%TO%\" /y

set TYPE=Release
set SUFFIX=dll
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%FROM%\*.%SUFFIX%"  "%TO%\" /y

echo ---------- Copy dll ----------

set TYPE=Debug
set SUFFIX=dll
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%FROM%\*.%SUFFIX%"  "%TO%\" /y


set TYPE=Release
set SUFFIX=dll
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%FROM%\*.%SUFFIX%"  "%TO%\" /y


echo ---------- Copy lib ----------

set TYPE=Debug
set SUFFIX=lib
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%FROM%\*.lib"  "%TO%\" /y


set TYPE=Release
set SUFFIX=lib
set FROM=%R3_sln_Path%\x64\%TYPE%
set TO=%root_folder_name%\SICK\%SUFFIX%\%TYPE%
echo F | xcopy "%FROM%\*.lib"  "%TO%\" /y


echo ---------- Copy others ----------

echo D | xcopy "%R3_sln_Path%\..\Dependencies\cti" "%root_folder_name%\SICK\cti" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\..\Dependencies\GenICam" "%root_folder_name%\GenICam" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\..\Dependencies\tinyxml2" "%root_folder_name%\tinyxml2" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\..\Dependencies\opencv" "%root_folder_name%\opencv" /e /q /d /y

echo D | xcopy "%R3_sln_Path%\..\Sample\GenIRanger" "%root_folder_name%\SICK\include\GenIRanger" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\..\Sample\ApiDll" "%root_folder_name%\SICK\include\ApiDll" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\..\Sample\CalibrationWrapper" "%root_folder_name%\SICK\include\CalibrationWrapper" /e /q /d /y


echo ---------- Copy demos ----------

echo D | xcopy "%R3_sln_Path%\..\Sample\ApiDllTest"  "%root_folder_name%\Examples\Sample\ApiDllTest" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\..\Sample\QtDemo" "%root_folder_name%\Examples\Sample\QtDemo" /e /q /d /y

echo F | xcopy "%R3_sln_Path%\SICK_GENICAM_SDK.props"  "%root_folder_name%\Examples\project\SICK_GENICAM_SDK.props" /y
echo F | xcopy "%R3_sln_Path%\opencv341.props"  "%root_folder_name%\Examples\project\opencv341.props" /y
echo F | xcopy "%R3_sln_Path%\opencv341d.props"  "%root_folder_name%\Examples\project\opencv341d.props" /y
echo F | xcopy "%R3_sln_Path%\Halcon.props" "%root_folder_name%\Examples\project\Halcon.props" /y

echo F | xcopy "%R3_sln_Path%\Ranger3_CPP_Demo.sln"  "%root_folder_name%\Examples\project\Ranger3_CPP_Demo.sln" /y
echo F | xcopy "%R3_sln_Path%\Ranger3_CSharp_Demo.sln"  "%root_folder_name%\Examples\project\Ranger3_CSharp_Demo.sln" /y
echo F | xcopy "%R3_sln_Path%\Ranger3_CSharp_Demo_2Device.sln"  "%root_folder_name%\Examples\project\Ranger3_CSharp_Demo_2Device.sln" /y
echo F | xcopy "%R3_sln_Path%\Trispector_CPP_Demo.sln"  "%root_folder_name%\Examples\project\Trispector_CPP_Demo.sln" /y
echo F | xcopy "%R3_sln_Path%\Trispector_CSharp_Demo.sln"  "%root_folder_name%\Examples\project\Trispector_CSharp_Demo.sln" /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_Console\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_Console\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_Console\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console\" /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\" /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\X64_Only" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\Properties" /e /q /d /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_CMD\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_CMD\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\" /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_CMD\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\X64_Only" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_CMD\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\Properties" /e /q /d /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_2Device\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_2Device\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\" /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_2Device\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\X64_Only" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_CSharp_2Device\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\Properties" /e /q /d /y

echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\res" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\res" /e /q /d /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.h" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.cpp" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.bat" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.rc" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_MFC\*.aps" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_Qt\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_Qt\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Ranger3_Qt\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt\" /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_Console\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_Console\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_Console\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console\" /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\" /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\X64_Only" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\Properties" /e /q /d /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_CMD\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_CMD\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\" /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_CMD\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\X64_Only" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_CMD\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\Properties" /e /q /d /y

echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_Halcon\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\" /y
echo F | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_Halcon\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\" /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_Halcon\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\X64_Only" /e /q /d /y
echo D | xcopy "%R3_sln_Path%\DEMO\Demo_Trispector_CSharp_Halcon\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\Properties" /e /q /d /y


if defined SICK_GENICAM_SDK (
  echo * 
) else (
  echo * No environment variable!
  echo * 
  goto :NOPATH
)

color 2e
echo * 
echo * Done successfully!
goto :END


:NOPATH
color 4e
echo * 
echo * Abandon / Error / Exit.
goto :END


:END
pause

color 0f
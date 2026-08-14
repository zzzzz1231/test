@echo off
chcp 65001

cd /d %~dp0												 
echo "| 												 
echo "| 												 
echo "|==================================================
echo "| 
echo "| auto_make_sourcecode_package
echo "| 
echo "|==================================================
echo "| 												 
echo "| 												 
echo "|
echo *
echo *
set bat_Path=%~dp0
echo * Current path = "%bat_Path%"
echo *
echo * Where to save the package?
set install_path=%1
echo *
echo *
echo * The installing path you input is:
echo * "%install_path%"
echo *
set version=%2
set root_folder_name=SICK_GENICAM_SDK_SOURCE_CODE-%version%
echo *
echo * Delete : %install_path%\%root_folder_name%\* ?
echo *
echo *
del "%install_path%\%root_folder_name%\*" /q /f /s
echo *
echo * Press Y to continue the installation.
set Y_N=Y
if "%Y_N%" neq "Y" (
  goto :NOPATH
)


if exist "%install_path%" (
	cd /d "%install_path%"
	md "%root_folder_name%\documents"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_FW461"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD"
	md "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon"
	
	md "%root_folder_name%\Examples\project\DLL\CalibrationWrapper"
	md "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper"
	md "%root_folder_name%\Examples\project\DLL\GenIRanger"
	md "%root_folder_name%\Examples\project\DLL\SampleApiDll"
	md "%root_folder_name%\Examples\project\DLL\SampleApiDll_CSharp"
	md "%root_folder_name%\Examples\project\DLL\SwigWrapper"
	md "%root_folder_name%\Examples\project\DLL\SwigWrapper\VisualStudioSolution"
	md "%root_folder_name%\Examples\project\DLL\SwigWrapper\VisualStudioSolution\Ranger3SwigModule_cpp"
	md "%root_folder_name%\Examples\project\DLL\SwigWrapper\VisualStudioSolution\Ranger3SwigModule_cs"
	md "%root_folder_name%\Examples\project\DLL\SwigWrapper\VisualStudioSolution\Ranger3SwigModule_cs\SwigRanger3.Net7"
	md "%root_folder_name%\Examples\project\DLL\SwigWrapper\VisualStudioSolution\Ranger3SwigModule_cs\SwigRanger3.NetF472"
	

	md "%root_folder_name%\Examples\project\x64\Debug"
	md "%root_folder_name%\Examples\project\x64\Release"

) else (
  goto :NOPATH
)

echo *
cd /d "%install_path%"


echo ---------- Copy Root ----------
echo - "%bat_Path%.vscode"
echo D | xcopy "%bat_Path%.vscode" "%root_folder_name%\.vscode\" /e /q /d /y
echo D | xcopy "%bat_Path%.idea" "%root_folder_name%\.idea\" /e /q /d /y

echo ---------- Copy sub-file ----------
echo F | xcopy "%bat_Path%.gitattributes" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%.gitignore" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%buildAndRun_ARM64.sh" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%buildAndRun_X64.sh" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%buildAndRun_X64_CalibrateInPC.sh" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%CMakeLists.txt" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%CMakeLists.txt.user" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%install_X64_Windows_add_environmentVariables.bat" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%install_X64_Windows_developer.bat" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%auto_copy_dll_example_to_package.bat" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%make_package_auto.bat" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%auto_make_sourcecode_package.bat" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%readme.md" "%root_folder_name%\" /y
echo F | xcopy "%bat_Path%*.pdf" "%root_folder_name%\" /y

echo ---------- Copy sub-folder ----------
echo D | xcopy "%bat_Path%documents\TrispectorDoc" "%root_folder_name%\documents\TrispectorDoc\" /e /q /d /y
echo F | xcopy "%bat_Path%documents\API_help\C++\html\GenICam_Cpp_API.chm" "%root_folder_name%\documents\" /y
echo F | xcopy "%bat_Path%documents\API_help\CSharp\html\GenICam_CSharp_API.chm" "%root_folder_name%\documents\" /y
echo F | xcopy "%bat_Path%Examples\project\x64\Debug\help.PNG"  "%root_folder_name%\documents\" /y
echo F | xcopy "%bat_Path%Examples\project\x64\Debug\help2.PNG"  "%root_folder_name%\documents\" /y



::for /d %%i in ("%bat_Path%Examples\Dependencies\*") do (
::xcopy %%i\dir3 "%root_folder_name%\Examples\Dependencies\" /s /y /i)

:: Please note, the folder had better no more than 100M


echo ---------- Copy dll ----------

echo ---------- Copy lib ----------


echo ---------- Copy others ----------
echo D | xcopy "%bat_Path%Examples\Dependencies\cti" "%root_folder_name%\Examples\Dependencies\cti" /i /s /e /q /d /y
echo D | xcopy "%bat_Path%Examples\Dependencies\GenICam" "%root_folder_name%\Examples\Dependencies\GenICam" /i /s /e /q /d /y
echo D | xcopy "%bat_Path%Examples\Dependencies\opencv" "%root_folder_name%\Examples\Dependencies\opencv" /i /s /e /q /d /y
echo D | xcopy "%bat_Path%Examples\Dependencies\tinyxml2" "%root_folder_name%\Examples\Dependencies\tinyxml2" /i /s /e /q /d /y



echo D | xcopy "%bat_Path%Examples\Sample" "%root_folder_name%\Examples\Sample\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\LinuxQtDemo_Ranger3Studio" "%root_folder_name%\Examples\LinuxQtDemo_Ranger3Studio\" /e /q /d /y

echo D | xcopy "%bat_Path%Examples\project\VisualStudioPropertyPages" "%root_folder_name%\Examples\project\VisualStudioPropertyPages\" /e /q /d /y
echo F | xcopy "%bat_Path%Examples\project\opencv341.props" "%root_folder_name%\Examples\project\" /y
echo F | xcopy "%bat_Path%Examples\project\opencv341d.props" "%root_folder_name%\Examples\project\" /y
echo F | xcopy "%bat_Path%Examples\project\Halcon.props" "%root_folder_name%\Examples\project\" /y
echo F | xcopy "%bat_Path%Examples\project\SICK_GENICAM_SDK.props" "%root_folder_name%\Examples\project\" /y


@REM echo F | xcopy "%bat_Path%Examples\project\Ranger3_CPP.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Ranger3_CPP_Demo.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Ranger3_CSharp.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Ranger3_CSharp_Demo.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Ranger3_CSharp_Demo_2Device.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\SICK_GENICAM_SDK.props" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Trispector_CPP.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Trispector_CPP_Demo.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Trispector_CSharp.sln" "%root_folder_name%\Examples\project\" /y
@REM echo F | xcopy "%bat_Path%Examples\project\Trispector_CSharp_Demo.sln" "%root_folder_name%\Examples\project\" /y


echo F | xcopy "%bat_Path%Examples\project\*.sln" "%root_folder_name%\Examples\project\" /y
echo F | xcopy "%bat_Path%Examples\project\*.runsettings" "%root_folder_name%\Examples\project\" /y



echo F | xcopy "%bat_Path%Examples\project\x64\Debug\*.dll" "%root_folder_name%\Examples\project\x64\Debug\" /y
echo F | xcopy "%bat_Path%Examples\project\x64\Debug\*.lib" "%root_folder_name%\Examples\project\x64\Debug\" /y
echo F | xcopy "%bat_Path%Examples\project\x64\Release\*.dll" "%root_folder_name%\Examples\project\x64\Release\" /y
echo F | xcopy "%bat_Path%Examples\project\x64\Release\*.lib" "%root_folder_name%\Examples\project\x64\Release\" /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_Console\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_Console\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_Console\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Console\" /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp\Properties" /e /q /d /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_CMD\Properties" /e /q /d /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_2Device\Properties" /e /q /d /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_DotNET7.0\Properties" /e /q /d /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_FW461\Properties" /e /q /d /y


echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\*.cs" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_CSharp_SWIG_FW472\Properties" /e /q /d /y








echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\res" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\res" /e /q /d /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.h" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.cpp" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.bat" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.rc" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_MFC\*.aps" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_MFC\" /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_Qt\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_Qt\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Ranger3_Qt\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Ranger3_Qt\" /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_Console\*.vcxproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_Console\*.filters" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_Console\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_Console\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_Console\" /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp\Properties" /e /q /d /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_CMD\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_CMD\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_CMD\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_CMD\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_CMD\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_CMD\Properties" /e /q /d /y

echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\*.csproj" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\*.user" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\" /y
echo F | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\*.config" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\" /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\X64_Only" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\X64_Only" /e /q /d /y
echo D | xcopy "%bat_Path%Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\Properties" "%root_folder_name%\Examples\project\DEMO\Demo_Trispector_CSharp_Halcon\Properties" /e /q /d /y






echo F | xcopy "%bat_Path%\Examples\project\DLL\CalibrationWrapper\*.vcxproj" "%root_folder_name%\Examples\project\DLL\CalibrationWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CalibrationWrapper\*.filters" "%root_folder_name%\Examples\project\DLL\CalibrationWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CalibrationWrapper\*.user" "%root_folder_name%\Examples\project\DLL\CalibrationWrapper\" /y

echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.vcxproj" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.filters" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.user" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.aps" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.cpp" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.h" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.ico" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.rc" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper\*.txt" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper\" /y

echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.vcxproj" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.filters" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.user" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.aps" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.cpp" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.h" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.ico" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.rc" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.461\*.txt" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.461\" /y


echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.vcxproj" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.filters" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.user" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.aps" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.cpp" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.h" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.ico" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.rc" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\CSharpDllWrapper.Net6\*.txt" "%root_folder_name%\Examples\project\DLL\CSharpDllWrapper.Net6\" /y


echo F | xcopy "%bat_Path%\Examples\project\DLL\GenIRanger\*.vcxproj" "%root_folder_name%\Examples\project\DLL\GenIRanger\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\GenIRanger\*.filters" "%root_folder_name%\Examples\project\DLL\GenIRanger\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\GenIRanger\*.user" "%root_folder_name%\Examples\project\DLL\GenIRanger\" /y

echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll\*.vcxproj" "%root_folder_name%\Examples\project\DLL\SampleApiDll\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll\*.filters" "%root_folder_name%\Examples\project\DLL\SampleApiDll\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll\*.user" "%root_folder_name%\Examples\project\DLL\SampleApiDll\" /y

echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll.NoPCCalibrated.No380\*.vcxproj" "%root_folder_name%\Examples\project\DLL\SampleApiDll.NoPCCalibrated.No380\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll.NoPCCalibrated.No380\*.filters" "%root_folder_name%\Examples\project\DLL\SampleApiDll.NoPCCalibrated.No380\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll.NoPCCalibrated.No380\*.user" "%root_folder_name%\Examples\project\DLL\SampleApiDll.NoPCCalibrated.No380\" /y

echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll_CSharp\*.vcxproj" "%root_folder_name%\Examples\project\DLL\SampleApiDll_CSharp\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll_CSharp\*.filters" "%root_folder_name%\Examples\project\DLL\SampleApiDll_CSharp\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll_CSharp\*.user" "%root_folder_name%\Examples\project\DLL\SampleApiDll_CSharp\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll_CSharp\*.config" "%root_folder_name%\Examples\project\DLL\SampleApiDll_CSharp\" /y

echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll_NoPCCalibrated\*.vcxproj" "%root_folder_name%\Examples\project\DLL\SampleApiDll_NoPCCalibrated\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll_NoPCCalibrated\*.filters" "%root_folder_name%\Examples\project\DLL\SampleApiDll_NoPCCalibrated\" /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SampleApiDll_NoPCCalibrated\*.user" "%root_folder_name%\Examples\project\DLL\SampleApiDll_NoPCCalibrated\" /y

echo D | xcopy "%bat_Path%\Examples\project\DLL\SwigWrapper\cpp" "%root_folder_name%\Examples\project\DLL\SwigWrapper\cpp" /e /q /d /y
echo D | xcopy "%bat_Path%\Examples\project\DLL\SwigWrapper\lib" "%root_folder_name%\Examples\project\DLL\SwigWrapper\lib" /e /q /d /y
echo D | xcopy "%bat_Path%\Examples\project\DLL\SwigWrapper\swig" "%root_folder_name%\Examples\project\DLL\SwigWrapper\swig" /e /q /d /y
echo F | xcopy "%bat_Path%\Examples\project\DLL\SwigWrapper\swig.exe" "%root_folder_name%\Examples\project\DLL\SwigWrapper\swig.exe" /y


set swig_solution_path=Examples\project\DLL\SwigWrapper\VisualStudioSolution

echo F | xcopy "%bat_Path%%swig_solution_path%\Ranger3SwigModule_cpp\*.vcxproj" "%root_folder_name%\%swig_solution_path%\Ranger3SwigModule_cpp\" /y
echo F | xcopy "%bat_Path%%swig_solution_path%\Ranger3SwigModule_cpp\*.filters" "%root_folder_name%\%swig_solution_path%\Ranger3SwigModule_cpp\" /y
echo F | xcopy "%bat_Path%%swig_solution_path%\Ranger3SwigModule_cpp\*.user" "%root_folder_name%\%swig_solution_path%\Ranger3SwigModule_cpp\" /y

echo F | xcopy "%bat_Path%%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.Net7\*.csproj" "%root_folder_name%\%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.Net7\" /y

echo F | xcopy "%bat_Path%%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.NetF472\*.csproj" "%root_folder_name%\%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.NetF472\" /y
robocopy "%bat_Path%%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.NetF472\code" "%root_folder_name%\%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.NetF472\code" /E /IS /IT /NFL /NDL /NJH /NJS
robocopy "%bat_Path%%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.NetF472\Properties" "%root_folder_name%\%swig_solution_path%\Ranger3SwigModule_cs\SwigRanger3.NetF472\Properties" /E /IS /IT /NFL /NDL /NJH /NJS




echo * 
echo * Done successfully!
goto :END

:NOPATH
color 4e
echo * 
echo * Abandon / Error / Exit.
goto :END

:END
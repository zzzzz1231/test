# GenICam SDK

Doc | (GCN-VisionLab | SICK China Co., Ltd.) 

## Introduction

This SDK package can be used for Ranger3 and Trispector.


## Ranger3

|Users|Demo|
|-|-|
|X64 + Windows + C++|Examples/project/Ranger3_CPP.sln|
|X64 + Windows + C#|Examples/project/Ranger3_CSharp.sln|
|X64 + Ubuntu + C++|buildAndRun_X64.sh|
|ARM64 + Ubuntu + C++|buildAndRun_ARM64.sh|


---

## Trispector

|Users|Demo|
|-|-|
|X64 + Windows + C++|Examples/project/Trispector_CPP.sln|
|X64 + Windows + C#|Examples/project/Trispector_CSharp.sln|

---

## Maintain and Release

* Requirements
  * doxygen 1.9.1
  * 7Zip.exe
  * Python
  * Advanced Installer 10.6
* Update version in **Examples/Sample/ApiDll/Version.h**

``` C++
#define R3_VERSION_1 3  // Major version
#define R3_VERSION_2 3  // Minor version
#define R3_VERSION_3 2  // Patch version
#define R3_VERSION_4 18 // Build number 
#define R3_VERSION_TIME "2023.3" // Build Time
```

* Update version in **Visual Studio Project (SamepleApiDll):** SamepleApiDll->SamepleApiDll.rc->Version->VS_VERSION_INFO

``` Bash
FILEVERSION="3,3,2,0"
PRODUCTVERSION="3,3,2,0"
```

* Update version in **Visual Studio Project (SampleCalibrationWrapper):** SampleCalibrationWrapper->SampleCalibrationWrapper.rc->Version->VS_VERSION_INFO

``` Bash
FILEVERSION="3,3,2,0"
PRODUCTVERSION="3,3,2,0"
```


* Update version in **Visual Studio Project (SampleApiDll_CS):** SampleApiDll_CS->SampleApiDll_CS.rc->Version->VS_VERSION_INFO

``` Bash
FILEVERSION="3,3,2,0"
PRODUCTVERSION="3,3,2,0"
```


* Update version in **Visual Studio Project (SampleApiDllCSharpWrapper):** SampleApiDllCSharpWrapper->SampleApiDllCSharpWrapper.rc->Version->VS_VERSION_INFO

``` Bash
FILEVERSION="3,3,2,0"
PRODUCTVERSION="3,3,2,0"
```


* Update doc **Ranger3 & Trispector SDK工程说明.docx**
  * Add version to <img src="readme/word1.jpg" />
  * Add change log to <img src="readme/word2.jpg" />
  * Update contents <img src="readme/word3.jpg" />

* Export pdf **Ranger3 & Trispector SDK工程说明.pdf**
  <img src="readme/word4.jpg" />
  <img src="readme/P1.jpg" />

* into folder **/ReleaseBatFiles/**
* run **CompileAll_PackageDLL.py**
  
``` Bash
python CompileAll_PackageDLL.py
```

* when done

``` Bash

======
======
======
======
======
========================= 执行成功 / ALL DONE SUCCESSFULLY ==================================
请打开 D:\SICK\gitlab\GenICam-SDK\ReleaseBatFiles/make_exe/SICK_GENICAM_SDK.aip
完成 exe 的制作


DLL文件：
D:\SICK\gitlab\GenICam-SDK\ReleaseBatFiles/make_exe/tmp//SICK_GENICAM_SDK_FOLDER


DLL文件（no SheetOfLight.dll）：
D:\SICK\gitlab\GenICam-SDK\ReleaseBatFiles/make_exe/tmp//noSheetOfLight//SICK_GENICAM_SDK_FOLDER


原码文件：
D:\SICK\gitlab\GenICam-SDK\ReleaseBatFiles/make_exe/tmp//SICK_GENICAM_SOURCE_CODE3.3.2.0.7z

D:\SICK\gitlab\GenICam-SDK\ReleaseBatFiles>
```

* Open **D:\SICK\gitlab\GenICam-SDK\ReleaseBatFiles/make_exe/SICK_GENICAM_SDK.aip**
  * Update version information and add change log
  <img src="readme/i1.jpg" />
  * Update files. But normally it will update automaticly if the path is valid.
  <img src="readme/i2.jpg" />
  * Click and Run
  <img src="readme/i3.jpg" />


---

## How to switch GenICam DLL version

You can find v3.1 and v3.3 dll in **GenICam-SDK-GenICamDLL**

Please see the **\Examples\Dependencies\GenICam\readme.md** to know how to manage the dll from GenICam website.

---


## How to release no sheet-of-light.dll

### 1. C++, [SampeApiDll: typedef.h]

Modify Typedef.h, to enable DISABLE_CAL_IN_PC
```C++
#define DISABLE_CAL_IN_PC
```

### 2. C# , [SampleApiDllCSharpWinForm: OperaCamera.cs]

Modify C# Winform, OperaCamera.cs
```C#
//#define CAL_IN_PC
```

---


moxygen ./xml -o apidoc.md



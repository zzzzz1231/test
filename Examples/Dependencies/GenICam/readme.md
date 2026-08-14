# GenICam文件说明

[GenICam 官网](https://www.emva.org/)

[GenICam 文件下载地址](https://www.emva.org/standards-technology/genicam/genicam-downloads/)

---

## 文件说明

完整的 GenICam 依赖文件有下面的文件组成。

以 GenICamTM Package Version 2022.06 为例

* GenApi 3.4.0
* GenTL 1.6, SFNC 2.7
* GenTL SFNC 1.2
* GenCP 1.3
* GenDC 1.1
* CLProtocol 1.2
* FWUpdate 1.0

所以官方发布的总文件并没有具体的版本，只有给出一个发布时间。

一般地，大多数人会用 GenApi 的版本来代指 GenICam 依赖库的版本，这里特别说明。

一般来说，较新的文件，都可以从官网下载到一个整合包，会包含所有的依赖项。

---

## 从整合包怎么得到开发需要的文件

* 解压缩这个路径下的对应压缩包：GenICam_Package_2021.02\Reference Implementation
* 比如 Win-x64 开发，可以选择 GenICam_V3_3_0-Win64_x64_VC141-Release-SDK.zip
  * 头文件第一部分位于：
    * GenICam_Package_2021.02\Reference Implementation\GenICam_V3_3_0-Win64_x64_VC141-Release-**SDK**\library\CPP\include
  * 头文件第二部分位于：
    * GenICam_Package_2021.02\GenTL
  * lib 文件位于：
    * GenICam_Package_2021.02\Reference Implementation\GenICam_V3_3_0-Win64_x64_VC141-Release-**SDK**\library\CPP\lib
  * dll 文件位于：
    * GenICam_Package_2021.02\Reference Implementation\GenICam_V3_3_0-Win64_x64_VC141-Release-**Runtime**\bin\Win64_x64

---

## 本仓库中的依赖关系

以 GenApi 3.1.0 版本为例。

* 头文件1 GenICam\GenICam_V3_1_0\library\CPP\include
* 头文件2 GenICam\GenICam_V3_1_0\library\CPP\include\TLI\GenTL.h
  * 老版本使用 TLI 作为文件名，新版本使用原始文件夹名 GenTL

---
cmake_minimum_required(VERSION 3.5)
project(Ranger3Studio LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# OpenCV
find_package(OpenCV)
if(OpenCV_FOUND)
    message("OpenCV Found!")
else()
    message("Without OpenCV to compile the demo!")
endif()

# ==============================
# 自动判断系统：Linux / Windows
# ==============================
if(WIN32)
    message("Building for WINDOWS")
    add_compile_definitions(OS_WINDOWS)
else()
    message("Building for LINUX")
    add_compile_definitions(OS_LINUX)
    list(APPEND CMAKE_CXX_FLAGS "-pthread")
endif()

# 头文件目录（两个系统一样）
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${GENAPI_INCLUDE_DIR}
    ${GENIRANGER_INCLUDE_DIR}
    ${SAMPLE_COMMON_INCLUDE_DIR}
    ${CALIBRATION_WRAPPER_INCLUDE_DIR}
    ${ApiDll_INCLUDE_DIR}
    ${ApiDemo_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

# ==============================
# 库目录：自动切换
# ==============================
if(WIN32)
    link_directories(
        ${GENAPI_LIB_DIR}/win64
        ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_win/GenIRanger
        ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_win/Common
        ${CMAKE_BINARY_DIR}/Examples/Sample/CalibrationWrapper
        ${CMAKE_BINARY_DIR}/Examples/Sample/ApiDll
        ${OpenCV_BINARY_DIR}
    )
else()
    link_directories(
        ${GENAPI_LIB_DIR}/Linux64_x64
        ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/GenIRanger
        ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/Common
        ${CMAKE_BINARY_DIR}/Examples/Sample/CalibrationWrapper
        ${CMAKE_BINARY_DIR}/Examples/Sample/ApiDll
        ${OpenCV_BINARY_DIR}
    )
endif()

# 源文件（两个系统一样）
add_executable(Ranger3Studio
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    opencvHelper.cpp
    opencvHelper.h
    #../Sample/ApiDllTest/InterfaceOpenCV.cpp
    #../Sample/ApiDllTest/InterfaceOpenCV.h
    ${SOURCE}
)

# ==============================
# 链接库：自动切换 .so / .lib
# ==============================
if(WIN32)
    target_link_libraries(Ranger3Studio PRIVATE
        Qt6::Widgets
        GCBase_v3_1.lib
        GenApi_v3_1.lib
        GenIRanger.lib
        Common.lib
        CalibrationWrapper.lib
        ApiDll.lib
        ${OpenCV_LIBRARIES}
    )
else()
    target_link_libraries(Ranger3Studio PRIVATE
        Qt6::Widgets
        libGCBase_gcc42_v3_1.so
        libGenApi_gcc42_v3_1.so
        libGenIRanger.so
        libCommon.so
        libCalibrationWrapper.so
        libApiDll.so
        ${OpenCV_LIBRARIES}
    )
endif()
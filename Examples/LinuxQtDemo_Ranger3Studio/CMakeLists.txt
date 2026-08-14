cmake_minimum_required(VERSION 3.5)

project(Ranger3Studio LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

#set(TS_FILES Ranger3Studio_zh_CN.ts)

################################################################################################################
# Add Ranger3 dependencies
################################################################################################################

# Opencv
find_package(OpenCV)
if(OpenCV_FOUND)
    message("OpenCV Found!")
else()
    message("Without OpenCV to compile the demo!")
endif()

list(APPEND CMAKE_CXX_FLAGS "-pthread")

# Set path to headers
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
link_directories(
        ${GENAPI_LIB_DIR}/Linux64_x64
        ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/GenIRanger
        ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/Common
        ${CMAKE_BINARY_DIR}/Examples/Sample/CalibrationWrapper
        ${CMAKE_BINARY_DIR}/Examples/Sample/ApiDll
        ${OpenCV_BINARY_DIR}

)

################################################################################################################
# Origin Qt excutable file settings
################################################################################################################

add_executable(Ranger3Studio
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
        opencvHelper.cpp
        opencvHelper.h
        ../Sample/ApiDllTest/InterfaceOpenCV.cpp
        ../Sample/ApiDllTest/InterfaceOpenCV.h
    #${TS_FILES}
    ${SOURCE}
)

target_link_libraries(Ranger3Studio PRIVATE Qt5::Widgets
        libGCBase_gcc42_v3_1.so
        libGenApi_gcc42_v3_1.so
        libGenIRanger.so
        libCommon.so
        libCalibrationWrapper.so
        libApiDll.so
        ${OpenCV_LIBRARIES}
)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

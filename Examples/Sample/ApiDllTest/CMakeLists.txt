# Copyright 2023 SICK AG. All rights reserved.
# Build definitions for Ranger GenICam helper library
project(ApiDllTest)
cmake_minimum_required(VERSION 2.8)


message("[**SICK MSG ${PROJECT_NAME}**] Project: ${PROJECT_NAME}")

# Opencv
find_package(OpenCV)
if(${USE_OPENCV} STREQUAL "FALSE")
    set(OpenCV_FOUND FALSE)
endif()


if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    message("[**SICK MSG ${PROJECT_NAME}**] USE_PC_CALIBRATION == " ${USE_PC_CALIBRATION} ", enable calibration in PC.")
else()
    message("[**SICK MSG ${PROJECT_NAME}**] USE_PC_CALIBRATION == " ${USE_PC_CALIBRATION} ", disable calibration in PC.")
endif()


# List all sources files
set(SOURCE
    InterfaceOperation.cpp
    InterfaceOperation.h
    #Demo_R3_01_quick_start_callback.cpp
)
if(OpenCV_FOUND)
    message("[**SICK MSG ${PROJECT_NAME}**] OpenCV Found!")
    set(SOURCE
        InterfaceOpenCV.cpp
        InterfaceOpenCV.h
    )
else()
    message("[**SICK MSG ${PROJECT_NAME}**] Without OpenCV to compile the demo!")
endif()


list(APPEND CMAKE_CXX_FLAGS "-pthread")


# Set path to headers
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${GENAPI_INCLUDE_DIR}
    ${GENIRANGER_INCLUDE_DIR}
    ${SAMPLE_COMMON_INCLUDE_DIR}
    ${ApiDll_INCLUDE_DIR}
)

if(OpenCV_FOUND)
    include_directories(
        ${OpenCV_INCLUDE_DIRS}
    )
endif()

if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    message("[**SICK MSG ${PROJECT_NAME}**] USE_PC_CALIBRATION=${USE_PC_CALIBRATION}. Included directories: ${CALIBRATION_WRAPPER_INCLUDE_DIR}")
    include_directories(
        ${CALIBRATION_WRAPPER_INCLUDE_DIR}
    )
endif()


link_directories(
    ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/GenIRanger
    ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/Common
    ${CMAKE_BINARY_DIR}/Examples/Sample/ApiDll
)

if(OpenCV_FOUND)
    link_directories(
        ${OpenCV_BINARY_DIR}
    )
endif()

if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    message("[**SICK MSG ${PROJECT_NAME}**] USE_PC_CALIBRATION=${USE_PC_CALIBRATION}. Linked directories: ${CMAKE_BINARY_DIR}/Examples/Sample/CalibrationWrapper")

    link_directories(
        ${CMAKE_BINARY_DIR}/Examples/Sample/CalibrationWrapper
    )
endif()


set(EXECUTABLE_FILE_NAME "${PROJECT_NAME}${BUILD_TYPE_EXTENSION}")

# Create a executable
add_executable(${EXECUTABLE_FILE_NAME} 
    Demo_R3_01_quick_start_callback.cpp
    ${SOURCE})

link_libraries(pthread)

# link shared libraries
target_link_libraries(
    ${EXECUTABLE_FILE_NAME}
    
    ${GENAPI_PATH_LIB_libGCBase}
    ${GENAPI_PATH_LIB_libGenApi}
    ${GENAPI_PATH_LIB_libCLProtocol}
    ${GENAPI_PATH_LIB_libFirmwareUpdate}
    ${GENAPI_PATH_LIB_liblog4cpp}
    ${GENAPI_PATH_LIB_libLog}
    ${GENAPI_PATH_LIB_libMathParser}
    ${GENAPI_PATH_LIB_libNodeMapData}
    ${GENAPI_PATH_LIB_libXmlParser}
    libGenIRanger.so
    libCommon.so
    libApiDll.so
    #libApiDll_STATIC.a # if you need static library please reference to this example
)

if(OpenCV_FOUND)
    target_link_libraries(
        ${EXECUTABLE_FILE_NAME}

        ${OpenCV_LIBRARIES}
    )
endif()


if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    message("[**SICK MSG ${PROJECT_NAME}**] USE_PC_CALIBRATION=${USE_PC_CALIBRATION}. target_link_libraries: libCalibrationWrapper.so")

    target_link_libraries(
        ${EXECUTABLE_FILE_NAME}

        ${SickSheetOfLight_SO}

        libCalibrationWrapper.so
    )
endif()


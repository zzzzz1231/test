# Copyright 2023 SICK AG. All rights reserved.
# Build definitions for Ranger GenICam helper library
project(SICK_GENICAM_SDK)
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0054 OLD)

message("[**SICK MSG ${PROJECT_NAME}**] Project: ${PROJECT_NAME}")
message("[**SICK MSG ${PROJECT_NAME}**] Project source dir = ${PROJECT_SOURCE_DIR}")
message("[**SICK MSG ${PROJECT_NAME}**] PLATFORM = ${PLATFORM}")


set(SAMPLE_COMMON_DIR ${PROJECT_SOURCE_DIR}/Examples/Sample/GenIRanger_linux/Common)
set(SAMPLE_COMMON_INCLUDE_DIR ${SAMPLE_COMMON_DIR}/public)

if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    message("[**SICK MSG ${PROJECT_NAME}**] USE_PC_CALIBRATION = ${USE_PC_CALIBRATION}, set path variables")
    set(CALIBRATION_WRAPPER_DIR ${PROJECT_SOURCE_DIR}/Examples/Sample/CalibrationWrapper)
    set(CALIBRATION_WRAPPER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Examples/Sample/CalibrationWrapper)
endif()

set(ApiDll_DIR ${PROJECT_SOURCE_DIR}/Examples/Sample/ApiDll)
set(ApiDll_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Examples/Sample/ApiDll)

set(ApiDemo_DIR ${PROJECT_SOURCE_DIR}/Examples/Sample/ApiDllTest)

set(LinuxQtDemo_DIR ${PROJECT_SOURCE_DIR}/Examples/LinuxQtDemo_Ranger3Studio)


# Set path to GenApi so that it can be accessed by underlying projects
if(${PLATFORM} STREQUAL "ARM64")
    message("[**SICK MSG ${PROJECT_NAME}**] ARM64 specified!")
    message("[**SICK MSG ${PROJECT_NAME}**] If you want to set X64, please run \"cmake .. -DPLATFORM=X64 && make -j4\"\n")
    set(GENAPI_PATH ${PROJECT_SOURCE_DIR}/Examples/Dependencies/GenICam/GenICam_V3_1_0_ARM64/library/CPP)
    
    set(GENAPI_PATH_LIB ${GENAPI_PATH}/lib/Linux64_ARM)
    set(GENAPI_PATH_LIB_libGCBase      ${GENAPI_PATH_LIB}/libGCBase_gcc48_v3_1.so)
    #set(GENAPI_PATH_LIB_libCLAllSerial ${GENAPI_PATH_LIB}/libCLAllSerial_gcc42_v3_1.so.so)
    set(GENAPI_PATH_LIB_libCLProtocol  ${GENAPI_PATH_LIB}/libCLProtocol_gcc48_v3_1.so)
    set(GENAPI_PATH_LIB_libFirmwareUpdate ${GENAPI_PATH_LIB}/libFirmwareUpdate_gcc48_v3_1.so)
    set(GENAPI_PATH_LIB_libGenApi     ${GENAPI_PATH_LIB}/libGenApi_gcc48_v3_1.so)
    set(GENAPI_PATH_LIB_liblog4cpp    ${GENAPI_PATH_LIB}/liblog4cpp_gcc48_v3_1.so)
    set(GENAPI_PATH_LIB_libLog        ${GENAPI_PATH_LIB}/libLog_gcc48_v3_1.so)
    set(GENAPI_PATH_LIB_libMathParser ${GENAPI_PATH_LIB}/libMathParser_gcc48_v3_1.so)
    set(GENAPI_PATH_LIB_libNodeMapData${GENAPI_PATH_LIB}/libNodeMapData_gcc48_v3_1.so)
    set(GENAPI_PATH_LIB_libXmlParser  ${GENAPI_PATH_LIB}/libXmlParser_gcc48_v3_1.so)

    if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
        set(SickSheetOfLight_H  ${CALIBRATION_WRAPPER_INCLUDE_DIR}/RuntimeCalibration/include/arm/SickSheetOfLight.h)
        set(SickSheetOfLight_SO  ${CALIBRATION_WRAPPER_INCLUDE_DIR}/RuntimeCalibration/lib/Linux_arm64/libSickSheetOfLight.so)
    endif()

else()
    message("[**SICK MSG ${PROJECT_NAME}**] X64 specified!")
    message("[**SICK MSG ${PROJECT_NAME}**] If you want to set ARM64, please run \"cmake .. -DPLATFORM=ARM64 && make -j4\"\n")
    set(GENAPI_PATH ${PROJECT_SOURCE_DIR}/Examples/Dependencies/GenICam/GenICam_V3_1_0/library/CPP)

    set(GENAPI_PATH_LIB ${GENAPI_PATH}/lib/Linux64_x64)
    set(GENAPI_PATH_LIB_libGCBase      ${GENAPI_PATH_LIB}/libGCBase_gcc42_v3_1.so)
    #set(GENAPI_PATH_LIB_libCLAllSerial ${GENAPI_PATH_LIB}/libCLAllSerial_gcc42_v3_1.so.so)
    set(GENAPI_PATH_LIB_libCLProtocol  ${GENAPI_PATH_LIB}/libCLProtocol_gcc42_v3_1.so)
    set(GENAPI_PATH_LIB_libFirmwareUpdate ${GENAPI_PATH_LIB}/libFirmwareUpdate_gcc42_v3_1.so)
    set(GENAPI_PATH_LIB_libGenApi     ${GENAPI_PATH_LIB}/libGenApi_gcc42_v3_1.so)
    set(GENAPI_PATH_LIB_liblog4cpp    ${GENAPI_PATH_LIB}/liblog4cpp_gcc42_v3_1.so)
    set(GENAPI_PATH_LIB_libLog        ${GENAPI_PATH_LIB}/libLog_gcc42_v3_1.so)
    set(GENAPI_PATH_LIB_libMathParser ${GENAPI_PATH_LIB}/libMathParser_gcc42_v3_1.so)
    set(GENAPI_PATH_LIB_libNodeMapData${GENAPI_PATH_LIB}/libNodeMapData_gcc42_v3_1.so)
    set(GENAPI_PATH_LIB_libXmlParser  ${GENAPI_PATH_LIB}/libXmlParser_gcc42_v3_1.so)

    if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
        set(SickSheetOfLight_H  ${CALIBRATION_WRAPPER_INCLUDE_DIR}/RuntimeCalibration/include/SickSheetOfLight.h)
        set(SickSheetOfLight_SO  ${CALIBRATION_WRAPPER_INCLUDE_DIR}/RuntimeCalibration/lib/Linux_x64/libSickSheetOfLight.so)
    else()
        add_definitions(-DDISABLE_CAL_IN_PC)
    endif()
endif()

set(GENAPI_INCLUDE_DIR ${GENAPI_PATH}/include)
set(GENIRANGER_DIR ${PROJECT_SOURCE_DIR}/Examples/Sample/GenIRanger_linux/GenIRanger)
set(GENIRANGER_INCLUDE_DIR ${GENIRANGER_DIR}/public)

#Add projects
add_subdirectory(${GENIRANGER_DIR})
add_subdirectory(${SAMPLE_COMMON_DIR})

if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    message("[**SICK MSG ${PROJECT_NAME}**] To Build CALIBRATION_WRAPPER_DIR")
    add_subdirectory(${CALIBRATION_WRAPPER_DIR})
endif()

add_subdirectory(${ApiDll_DIR})

add_subdirectory(${ApiDemo_DIR})
add_dependencies(ApiDllTest ApiDll)
#add_subdirectory(${LinuxQtDemo_DIR})

message("[**SICK MSG ${PROJECT_NAME}**] Configure Ranger3 SDK done! You can run \"make .. -j4\" to compile/build the projects!")
message("[**SICK MSG ${PROJECT_NAME}**] After building projects, you can find the executable file in \"/build/Examples/Sample/ApiDllTest/ApiDllTest\" !")
message("[**SICK MSG ${PROJECT_NAME}**] If you want to run another demo, please modify the \"Examples/Sample/ApiDllTest/CMakeList.txt\" , and rebuild the project!")

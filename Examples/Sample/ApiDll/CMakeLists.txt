# Copyright 2023 SICK AG. All rights reserved.
# Build definitions for Ranger GenICam helper library
project(ApiDll)
cmake_minimum_required(VERSION 2.8)

message("[**SICK MSG ${PROJECT_NAME}**] Project: ${PROJECT_NAME}")

# List all sources files
set(SOURCE
        CameraShared.h
        CameraShared.cpp
        CustomerLog.cpp
        CustomerLog.h
        DeviceConnection.cpp
        DeviceConnection.h
        ImageTable.cpp
        ImageTable.h
        Parameters.cpp
        Parameters.h
        Ranger3.cpp
        Ranger3.h
        Typedef.h
        Version.cpp
        Version.h
        tinyxml2/tinyxml2.cpp
        tinyxml2/tinyxml2.h
        ColorImageRun.hpp
        )


source_group("public" FILES ${PUBLIC_SOURCE})

# Set path to headers
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${GENAPI_INCLUDE_DIR}
    ${GENIRANGER_INCLUDE_DIR}
    ${SAMPLE_COMMON_INCLUDE_DIR}
)

if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
message("[**SICK MSG ${PROJECT_NAME}**] USE_PC_CALIBRATION=${USE_PC_CALIBRATION}, add library CALIBRATION_WRAPPER_DIR")
include_directories(
    /${CALIBRATION_WRAPPER_DIR}
)
endif()
message("[**SICK MSG ${PROJECT_NAME}**] Set path to headers")


link_directories(
    ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/GenIRanger
    ${CMAKE_BINARY_DIR}/Examples/Sample/GenIRanger_linux/Common
)
if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
link_directories(
    ${CMAKE_BINARY_DIR}/Examples/Sample/CalibrationWrapper
)
endif()
message("[**SICK MSG ${PROJECT_NAME}**] Linked directories")




# Create a shared library
set(LIBRARY_FILE_NAME "${PROJECT_NAME}${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME} SHARED ${SOURCE})
target_link_libraries(
    ${LIBRARY_FILE_NAME}

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
)
if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    target_link_libraries(
        ${LIBRARY_FILE_NAME}

        ${SickSheetOfLight_SO}

        libCalibrationWrapper.so
    )
endif()
message("[**SICK MSG ${PROJECT_NAME}**] Added shared library:${LIBRARY_FILE_NAME}")




# Create a static library
set(LIBRARY_FILE_NAME_STATIC "${PROJECT_NAME}_STATIC${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME_STATIC} STATIC ${SOURCE})
target_link_libraries(
    ${LIBRARY_FILE_NAME_STATIC}

    ${GENAPI_PATH_LIB_libGCBase}
    ${GENAPI_PATH_LIB_libGenApi}
    ${GENAPI_PATH_LIB_libCLProtocol}
    ${GENAPI_PATH_LIB_libFirmwareUpdate}
    ${GENAPI_PATH_LIB_liblog4cpp}
    ${GENAPI_PATH_LIB_libLog}
    ${GENAPI_PATH_LIB_libMathParser}
    ${GENAPI_PATH_LIB_libNodeMapData}
    ${GENAPI_PATH_LIB_libXmlParser}

    libGenIRanger_STATIC.so
    libCommon_STATIC.so
)
if(${USE_PC_CALIBRATION} STREQUAL "TRUE")
    target_link_libraries(
        ${LIBRARY_FILE_NAME_STATIC}

        ${SickSheetOfLight_SO}

        libCalibrationWrapper_STATIC.a
    )
endif()
message("[**SICK MSG ${PROJECT_NAME}**] Added static library:${LIBRARY_FILE_NAME_STATIC}")

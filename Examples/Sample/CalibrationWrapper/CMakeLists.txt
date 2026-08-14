# Copyright 2016 SICK AG. All rights reserved.
# Build definitions for Ranger GenICam helper library
project(CalibrationWrapper)
cmake_minimum_required(VERSION 2.8)

message("[**SICK MSG ${PROJECT_NAME}**] Project: ${PROJECT_NAME}")
# List all sources files
set(SOURCE
        CalibrationWrapper.cpp
        CalibrationWrapper.h
        Ranger3RuntimeCalibration/include/CalibrationAPI.h
        Ranger3RuntimeCalibration/include/CalibrationFilter.h
        Ranger3RuntimeCalibration/include/CalibrationParametersFactory.h
        Ranger3RuntimeCalibration/include/Point2D.h
        Ranger3RuntimeCalibration/include/RectificationFilter.h
        Ranger3RuntimeCalibration/include/Settings.h
        Ranger3RuntimeCalibration/include/Transformations.h
        Ranger3RuntimeCalibration/src/CalibrationAPI.cpp
        Ranger3RuntimeCalibration/src/CalibrationFilter.cpp
        Ranger3RuntimeCalibration/src/CalibrationParametersFactory.cpp
        Ranger3RuntimeCalibration/src/Point2D.cpp
        Ranger3RuntimeCalibration/src/RectificationFilter.cpp
        Ranger3RuntimeCalibration/src/Transformations.cpp
        tinyxml2/tinyxml2.cpp
        tinyxml2/tinyxml2.h
        ${SickSheetOfLight_H}
        )


#set(LIBRARY_FILE_NAME "${PROJECT_NAME}${BUILD_TYPE_EXTENSION}")


# Create a shared library
set(LIBRARY_FILE_NAME "${PROJECT_NAME}${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME} SHARED ${SOURCE})
target_link_libraries(
        ${LIBRARY_FILE_NAME}

        ${SickSheetOfLight_SO}
)
message("[**SICK MSG ${PROJECT_NAME}**] Added shared library:${LIBRARY_FILE_NAME}")


# Create a static library
set(LIBRARY_FILE_NAME_STATIC "${PROJECT_NAME}_STATIC${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME_STATIC} STATIC ${SOURCE})
target_link_libraries(
        ${LIBRARY_FILE_NAME_STATIC}

        ${SickSheetOfLight_SO}
)
message("[**SICK MSG ${PROJECT_NAME}**] Added static library:${LIBRARY_FILE_NAME_STATIC}")

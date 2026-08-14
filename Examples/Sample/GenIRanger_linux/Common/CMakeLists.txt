# Copyright 2016 SICK AG. All rights reserved.        public/M

# Build definitions for Ranger GenICam helper library
project(Common)
cmake_minimum_required(VERSION 2.8)


message("[**SICK MSG ${PROJECT_NAME}**] Project: ${PROJECT_NAME}")

# List all sources files
set(PUBLIC_SOURCE
    public/BufferInfo.h
    public/ChunkAdapter.h
    public/Consumer.h
    public/DeviceSelector.h
    public/MetadataExtractor.h
    public/GenTLApi.h
    public/NodeHelpers.h
    public/SampleUtils.h
    public/SingleDeviceConsumer.h)

set(PRIVATE_SOURCE
    private/BufferInfo.cpp
    private/ChunkAdapter.cpp
    private/Consumer.cpp
    private/DeviceSelector.cpp
    private/MetadataExtractor.cpp
    private/GenTLApi.cpp
    private/NodeHelpers.cpp
    private/SampleUtils.cpp
    private/SingleDeviceConsumer.cpp)

# Add files to generated project
source_group("public" FILES ${PUBLIC_SOURCE})
source_group("private" FILES ${PRIVATE_SOURCE})

# Set path to headers
include_directories(
    ${PROJECT_SOURCE_DIR}/public
    ${GENAPI_INCLUDE_DIR}
    ${GENIRANGER_INCLUDE_DIR})

# Make output and includes available to sample project
set(GENIRANGER_LIB_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)


# Create a shared library (dll)
set(LIBRARY_FILE_NAME "${PROJECT_NAME}${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME} SHARED ${PRIVATE_SOURCE} ${PUBLIC_SOURCE})
target_link_libraries(
    ${LIBRARY_FILE_NAME} 

    ${GENAPI_PATH_LIB_libGCBase}
    ${GENAPI_PATH_LIB_libGenApi}
)
# Turn off automatic linkage of GenICam libs, we specify them ourselves above
target_compile_definitions(${LIBRARY_FILE_NAME} PRIVATE GENICAM_NO_AUTO_IMPLIB)
target_compile_definitions(${LIBRARY_FILE_NAME} PRIVATE GENIRANGER_LINKAGE_SHARED_EXPORT)
message("\n[**SICK MSG ${PROJECT_NAME}**] Added shared library:${LIBRARY_FILE_NAME}")


# Also create a static library
set(LIBRARY_FILE_NAME_STATIC "${PROJECT_NAME}_STATIC${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME_STATIC} STATIC ${PRIVATE_SOURCE} ${PUBLIC_SOURCE})
target_link_libraries(
    ${LIBRARY_FILE_NAME_STATIC} 

    GCBase GenApi
    ${GENAPI_PATH_LIB_libGCBase}
    ${GENAPI_PATH_LIB_libGenApi}
)

# Turn off automatic linkage of GenICam libs, we specify them ourselves above
target_compile_definitions(${LIBRARY_FILE_NAME_STATIC} PRIVATE GENICAM_NO_AUTO_IMPLIB)
target_compile_definitions(${LIBRARY_FILE_NAME_STATIC} PUBLIC GENIRANGER_LINKAGE_STATIC)
message("\n[**SICK MSG ${PROJECT_NAME}**] Added static library:${LIBRARY_FILE_NAME_STATIC}")

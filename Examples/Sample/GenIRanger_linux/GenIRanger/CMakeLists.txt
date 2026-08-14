# Copyright 2016 SICK AG. All rights reserved.
# Build definitions for Ranger GenICam helper library
project(GenIRanger)
cmake_minimum_required(VERSION 2.8)
#cmake_policy(SET CMP0054 OLD)


message("[**SICK MSG ${PROJECT_NAME}**] Project: ${PROJECT_NAME}")

# List all sources files
set(PUBLIC_SOURCE
    public/CustomCommands.h
    public/DeviceLogWriter.h
    public/Exceptions.h
    public/FileOperation.h
    public/GenIRanger.h
    public/GenIRangerDll.h
    public/GenIRangerDataTypes.h
    public/StreamData.h
    )

set(PRIVATE_SOURCE
    private/ConfigReader.cpp
    private/ConfigReader.h
    private/ConfigWriter.cpp
    private/ConfigWriter.h
    private/DatXmlWriter.cpp
    private/DatXmlWriter.h
    private/DatAndXmlFiles.cpp
    private/DatAndXmlFiles.h
    private/DeviceLogWriter.cpp
    private/Exceptions.cpp
    private/FileOperation.cpp
    private/GenIRanger.cpp
    private/GenIUtil.cpp
    private/GenIUtil.h
    private/NodeExporter.cpp
    private/NodeExporter.h
    private/NodeImporter.cpp
    private/NodeImporter.h
    private/NodeTraverser.cpp
    private/NodeTraverser.h
    private/NodeUtil.cpp
    private/NodeUtil.h
    private/SaveBuffer.cpp
    private/SaveBuffer.h
    private/SelectorSnapshot.cpp
    private/SelectorSnapshot.h
    private/XmlElement.cpp
    private/XmlElement.h
    private/NodeInfoVLB.cpp
    private/NodeInfoVLB.h
    )

# Add files to generated project
source_group("public" FILES ${PUBLIC_SOURCE})
source_group("private" FILES ${PRIVATE_SOURCE})

# Set path to headers
include_directories(
    ${PROJECT_SOURCE_DIR}/public
    ${GENAPI_INCLUDE_DIR})

# Make output and includes available to sample project
set(GENIRANGER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/public PARENT_SCOPE)
set(GENIRANGER_LIB_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)


#### Create a shared library
set(LIBRARY_FILE_NAME "${PROJECT_NAME}${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME} SHARED ${PRIVATE_SOURCE} ${PUBLIC_SOURCE})
target_link_libraries(
    ${LIBRARY_FILE_NAME} 
    
    ${GENAPI_PATH_LIB_libGCBase} 
    ${GENAPI_PATH_LIB_libGenApi}
    #${GENAPI_PATH_LIB_libCLAllSerial}
    ${GENAPI_PATH_LIB_libCLProtocol}
    ${GENAPI_PATH_LIB_libFirmwareUpdate}
    ${GENAPI_PATH_LIB_liblog4cpp}
    ${GENAPI_PATH_LIB_libLog}
    ${GENAPI_PATH_LIB_libMathParser}
    ${GENAPI_PATH_LIB_libNodeMapData}
    ${GENAPI_PATH_LIB_libXmlParser}
)

# Turn off automatic linkage of GenICam libs, we specify them ourselves above
target_compile_definitions(${LIBRARY_FILE_NAME} PRIVATE GENICAM_NO_AUTO_IMPLIB)
# Export DLL symbols
target_compile_definitions(${LIBRARY_FILE_NAME} PRIVATE GENIRANGER_LINKAGE_SHARED_EXPORT)
message("[**SICK MSG ${PROJECT_NAME}**] Added shared library:${LIBRARY_FILE_NAME}")



#### Create a static library
set(LIBRARY_FILE_NAME_STATIC "${PROJECT_NAME}_STATIC${BUILD_TYPE_EXTENSION}")
add_library(${LIBRARY_FILE_NAME_STATIC} STATIC ${PRIVATE_SOURCE} ${PUBLIC_SOURCE})
target_link_libraries(
    ${LIBRARY_FILE_NAME_STATIC} 

    GCBase GenApi
    ${LIBRARY_FILE_NAME_STATIC} 
    ${GENAPI_PATH_LIB_libGCBase} 
    ${GENAPI_PATH_LIB_libGenApi}
    #${GENAPI_PATH_LIB_libCLAllSerial}
    ${GENAPI_PATH_LIB_libCLProtocol}
    ${GENAPI_PATH_LIB_libFirmwareUpdate}
    ${GENAPI_PATH_LIB_liblog4cpp}
    ${GENAPI_PATH_LIB_libLog}
    ${GENAPI_PATH_LIB_libMathParser}
    ${GENAPI_PATH_LIB_libNodeMapData}
    ${GENAPI_PATH_LIB_libXmlParser}
)

# Turn off automatic linkage of GenICam libs, we specify them ourselves above
target_compile_definitions(${LIBRARY_FILE_NAME_STATIC} PRIVATE GENICAM_NO_AUTO_IMPLIB)
# Export DLL symbols, static
target_compile_definitions(${LIBRARY_FILE_NAME_STATIC} PUBLIC GENIRANGER_LINKAGE_STATIC)
message("[**SICK MSG ${PROJECT_NAME}**] Added static library:${LIBRARY_FILE_NAME_STATIC}")

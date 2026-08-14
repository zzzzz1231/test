// Copyright 2016-2019 SICK AG. All rights reserved.

#pragma once

#include "TLI/GenTL.h"

#include <stdexcept>
#include <map>
#include <memory>
#include <sstream>
#include <cstring>


#define  _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // avoid min/max macros

#if defined(_MSC_VER)
#include <windows.h>
#endif
#if defined(__GNUC__)
#include <dlfcn.h>
#endif

#define CLEAR_FUNCTION_PTR(FN) tl.FN = NULL;

#if defined(__GNUC__)
#define HMODULE void*
#endif

#if defined(_MSC_VER)
#define LOAD_LIBRARY(path) LoadLibrary(path)
#define FREE_LIBRARY(path) FreeLibrary(path)
#elif defined(__GNUC__)
#define LOAD_LIBRARY(path) dlopen(path, RTLD_LAZY)
#define FREE_LIBRARY(path) (dlclose(path) == 0)
#endif


// clang-format off
#define API_LIST(code)\
  code(GCGetInfo)\
  code(GCGetLastError)\
  code(GCInitLib)\
  code(GCCloseLib)\
  code(GCReadPort)\
  code(GCWritePort)\
  code(GCGetPortURL)\
  code(GCGetPortInfo)\
  code(GCRegisterEvent)\
  code(GCUnregisterEvent)\
  code(EventGetData)\
  code(EventGetDataInfo)\
  code(EventGetInfo)\
  code(EventFlush)\
  code(EventKill)\
  code(TLOpen)\
  code(TLClose)\
  code(TLGetInfo)\
  code(TLGetNumInterfaces)\
  code(TLGetInterfaceID)\
  code(TLGetInterfaceInfo)\
  code(TLOpenInterface)\
  code(TLUpdateInterfaceList)\
  code(IFClose)\
  code(IFGetInfo)\
  code(IFGetNumDevices)\
  code(IFGetDeviceID)\
  code(IFUpdateDeviceList)\
  code(IFGetDeviceInfo)\
  code(IFOpenDevice)\
  code(DevGetPort)\
  code(DevGetNumDataStreams)\
  code(DevGetDataStreamID)\
  code(DevOpenDataStream)\
  code(DevGetInfo)\
  code(DevClose)\
  code(DSAnnounceBuffer)\
  code(DSAllocAndAnnounceBuffer)\
  code(DSFlushQueue)\
  code(DSStartAcquisition)\
  code(DSStopAcquisition)\
  code(DSGetInfo)\
  code(DSGetBufferID)\
  code(DSClose)\
  code(DSRevokeBuffer)\
  code(DSQueueBuffer)\
  code(DSGetBufferInfo)\
  code(GCGetNumPortURLs)\
  code(GCGetPortURLInfo)\
  code(GCReadPortStacked)\
  code(GCWritePortStacked)\
  code(DSGetBufferChunkData)\
  code(IFGetParentTL)\
  code(DevGetParentIF)\
  code(DSGetParentDev)\
  code(DSGetNumBufferParts)\
  code(DSGetBufferPartInfo)
// clang-format on

class GenTLApi
{
public:
  GenTLApi(HMODULE module);
  ~GenTLApi();

#define FUNC_PTR(func) GenTL::P##func func;
  API_LIST(FUNC_PTR)
#undef FUNC_PTR

//private:
  HMODULE mModule;
};

struct LocalUrl
{
  std::string filename;
  uint64_t address;
  size_t length;
};

std::unique_ptr<GenTLApi> loadProducer(std::string ctiFile);

// Macro for checking if a call to GenTL succeeded
#define CC(tl, CALL)                                                           \
  if (GenTL::GC_ERR_SUCCESS != CALL)                                           \
  {                                                                            \
    char message[1024];                                                        \
    memset(message, 0, sizeof(message));                                       \
    size_t size = sizeof(message);                                             \
    GenTL::GC_ERROR errorCode;                                                 \
    tl->GCGetLastError(&errorCode, message, &size);                            \
    std::stringstream ss;                                                      \
    ss << "GenTL call failed: " << errorCode << ", Message: " << message;      \
    throw std::runtime_error(ss.str().c_str());                                    \
  }

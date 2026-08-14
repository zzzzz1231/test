// Copyright 2016-2019 SICK AG. All rights reserved.

#include "GenTLApi.h"

#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

GenTLApi::GenTLApi(HMODULE module)
  : mModule(module)
{
}

GenTLApi::~GenTLApi() {
    if (mModule == nullptr) {
        std::cout << "try to free GenTLApi\n";
        FREE_LIBRARY(mModule);
        mModule = nullptr;
    }
}

std::unique_ptr<GenTLApi> loadProducer(std::string ctiFile)
{
  HMODULE module = LOAD_LIBRARY(ctiFile.c_str());
  if (module == nullptr)
  {
    std::stringstream sstr;
    sstr << "Could not load: " << ctiFile;
    std::string errorMessage(sstr.str());
    std::cerr << errorMessage << std::endl;
    throw std::runtime_error(errorMessage.c_str());
  }
  std::unique_ptr<GenTLApi> tl(new GenTLApi(module));

#define LOAD_PROC_ADDRESS(func)                                                \
  tl->func = (GenTL::P##func)dlsym(module, #func);                    \
  assert(tl->func);

  API_LIST(LOAD_PROC_ADDRESS)
#undef LOAD_PROC_ADDRESS
  return tl;
}

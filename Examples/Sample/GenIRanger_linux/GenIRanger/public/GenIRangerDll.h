// Copyright 2017-2019 SICK AG. All rights reserved.
#if defined(_MSC_VER)
#ifdef GENIRANGER_LINKAGE_SHARED_EXPORT
#  define GENIRANGER_API __declspec(dllexport)
#elif GENIRANGER_LINKAGE_STATIC
#  define GENIRANGER_API
#else
#  define GENIRANGER_API __declspec(dllimport)
#endif
#elif defined(__GNUC__)
  #ifdef GENIRANGER_LINKAGE_SHARED_EXPORT
    #define GENIRANGER_API __attribute__((visibility("default")))
  #elif GENIRANGER_LINKAGE_STATIC
  #  define GENIRANGER_API
  #else
    #define GENIRANGER_API
  #endif
#endif

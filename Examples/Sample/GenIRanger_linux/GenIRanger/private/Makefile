###########################################################################
# Makefile
###########################################################################
#
# Project:      
# Author(s):    
# Description:  
# License:      
#
# (c) 2016-2017 by MVTec Software GmbH
#
###########################################################################

# Basic project definitions
# (BUILD_TYPE might be overriden on command line to debug)
PROJECT_NAME      := GenIRanger
OSNAME            := $(shell uname -s)
ARCH_CODE         := $(shell uname -m)
LIB_FILE          := lib$(PROJECT_NAME).a
BUILD_TYPE        := debug
GENICAM_ROOT      := /Prog/GenICam

# Build directories
INTERMEDIATE_DIR  :=  $(ARCH_CODE)/$(BUILD_TYPE)
OUT_DIR           :=  $(ARCH_CODE)/$(BUILD_TYPE)

# Tool definitions
LD                := $(CXX)

.PHONY: all clean install prerequisites

vpath %.o $(INTERMEDIATE_DIR)
vpath %.d $(INTERMEDIATE_DIR)
vpath %.a $(OUT_DIR)

# Source files

SOURCES = \
  ConfigReader.cpp \
  ConfigWriter.cpp \
  DatAndXmlFiles.cpp \
  DatXmlWriter.cpp \
  DeviceLogWriter.cpp \
  Exceptions.cpp \
  FileOperation.cpp \
  GenIRanger.cpp \
  GenIUtil.cpp \
  NodeExporter.cpp \
  NodeImporter.cpp \
  NodeTraverser.cpp \
  NodeUtil.cpp \
  SaveBuffer.cpp \
  SelectorSnapshot.cpp

# Object files - assumed 1:1 mapping with source files
OBJECTS = $(patsubst %.cpp,$(INTERMEDIATE_DIR)/%.o, $(patsubst %.c,$(INTERMEDIATE_DIR)/%.o,$(SOURCES)))

# Compiler and linker flags
INCLUDE_DIRS = -I../public/ -I$(GENICAM_ROOT)/library/CPP/include/

LIB_DIRS =

LIBS= 

CXXFLAGS = $(INCLUDE_DIRS) \
            -Wall -Wextra -Wformat=2 \
            -fPIC \
            -fvisibility=hidden \
            -Wno-unknown-pragmas

LDFLAGS  = -shared \
            $(LIBS) $(LIB_DIRS)

ifeq ($(OSNAME),Linux)
LDFLAGS  = -Wl,--fatal-warnings \
            -Wl,--no-undefined \
            -Wl,--export-dynamic
endif

# Build-type specific flags
ifeq ($(BUILD_TYPE),debug)
	CXXFLAGS = -D_DEBUG -g -O0
else
	CXXFLAGS = -DNDDEBUG -O2
endif


all: $(LIB_FILE)

prerequisites:
	@mkdir -p $(INTERMEDIATE_DIR)
	@mkdir -p $(OUT_DIR)
	@echo $(INCLUDE_DIRS)

# Automatic dependency calculation according (and slightly modified)
# go "Generating Prerequisites Automatically" from GNU make manual
# Improved approaches might be available, but for now this should do it
-include $(patsubst %.cpp,$(INTERMEDIATE_DIR)/%.d, $(patsubst %.c,$(INTERMEDIATE_DIR)/%.d,$(SOURCES)))

$(LIB_FILE): $(OBJECTS)
	$(AR) -rsc $(CXXFLAGS) $(OUT_DIR)/$@ $^

$(INTERMEDIATE_DIR)/%.o: %.cpp | prerequisites
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(INTERMEDIATE_DIR)/%.o: %.c | prerequisites
	$(CC) $(CXXFLAGS) -c $< -o $@

$(INTERMEDIATE_DIR)/%.d: %.cpp | prerequisites
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(INTERMEDIATE_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(INTERMEDIATE_DIR)/%.d: %.c | prerequisites
	@set -e; rm -f $@; \
	$(CC) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(INTERMEDIATE_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -f $(INTERMEDIATE_DIR)/*
	rm -f $(OUT_DIR)/*

install:
	$(error Installation support not yet available)

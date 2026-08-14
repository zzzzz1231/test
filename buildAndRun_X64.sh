#!/bin/bash
set ff=unix
cp Examples/Dependencies/GenICam/GenICam_V3_1_0/library/CPP/lib/Linux64_x64/lib* /usr/local/lib/
#cp Examples/Sample/CalibrationWrapper/RuntimeCalibration/lib/Linux_x64/libSickSheetOfLight.so /usr/local/lib/
cp Examples/Dependencies/cti/Ranger3/x64-ubuntu/sickGenTLProducer.cti /usr/local/lib/
cp Examples/LinuxQtDemo_Ranger3Studio/SICK.jpeg /usr/local/lib/


if [ -d "buildRanger3SDK" ];then
    echo "---- try to delete folder buildRanger3SDK"
    rm -r buildRanger3SDK
    echo "---- process ok"
else 
    echo "---- try to mkdir buildRanger3SDK"
fi

mkdir buildRanger3SDK
cd buildRanger3SDK
#cmake .. -DPLATFORM=X64 -DUSE_PC_CALIBRATION=TRUE
cmake .. -DPLATFORM=X64 -DUSE_PC_CALIBRATION=FALSE -DUSE_OPENCV=FALSE
make -j4
cp Examples/Sample/GenIRanger_linux/GenIRanger/libGenIRanger.so /usr/local/lib/
cp Examples/Sample/GenIRanger_linux/Common/libCommon.so /usr/local/lib/
#cp Examples/Sample/CalibrationWrapper/libCalibrationWrapper.so /usr/local/lib/
cp Examples/Sample/ApiDll/libApiDll.so /usr/local/lib/


chmod +x Examples/Sample/ApiDllTest/ApiDllTest
./Examples/Sample/ApiDllTest/ApiDllTest

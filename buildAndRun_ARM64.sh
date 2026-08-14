#!/bin/bash
set ff=unix
cp Examples/Dependencies/GenICam/GenICam_V3_1_0_ARM64/library/CPP/lib/Linux64_ARM/lib* /usr/local/lib/
cp Examples/Sample/CalibrationWrapper/RuntimeCalibration/lib/Linux_arm64/libSickSheetOfLight.so /usr/local/lib/
cp Examples/Sample/CalibrationWrapper/RuntimeCalibration/lib/Linux_arm64/libSickSheetOfLight.so /usr/local/lib/libsick_sheet_of_light.so
cp Examples/Dependencies/cti/Ranger3/arm-ubuntu/sickGenTLProducer.cti /usr/local/lib/


if [ -d "buildRanger3SDK_Arm64" ];then
    echo "---- try to delete folder buildRanger3SDK_Arm64"
    rm -r buildRanger3SDK_Arm64
    echo "---- process ok"
else 
    echo "---- try to mkdir buildRanger3SDK_Arm64"
fi

mkdir buildRanger3SDK_Arm64
cd buildRanger3SDK_Arm64
#cmake .. -DPLATFORM=ARM64 -DUSE_PC_CALIBRATION=TRUE
cmake .. -DPLATFORM=ARM64 -DUSE_PC_CALIBRATION=TRUE -DUSE_OPENCV=TRUE
make -j4
cp Examples/Sample/GenIRanger_linux/GenIRanger/libGenIRanger.so /usr/local/lib/
cp Examples/Sample/GenIRanger_linux/Common/libCommon.so /usr/local/lib/
cp Examples/Sample/CalibrationWrapper/libCalibrationWrapper.so /usr/local/lib/
cp Examples/Sample/ApiDll/libApiDll.so /usr/local/lib/


chmod +x Examples/Sample/ApiDllTest/ApiDllTest
./Examples/Sample/ApiDllTest/ApiDllTest

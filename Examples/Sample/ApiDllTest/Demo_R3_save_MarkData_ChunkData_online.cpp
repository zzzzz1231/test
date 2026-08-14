/**
*	@copyright	Copyright 2018-2023 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4474)

#include "Demo.h" // 此文件仅用于GTest，作为普通例程运行时，可以屏蔽或者将"Demo.h"文件放在此文件旁边
#include <Ranger3.h>			// 此头文件封装了 API 的主体。
#include "InterfaceOpenCV.h"
#include "InterfaceOperation.h"
#include <iostream>
#include <map>

void SICK_CALLBACK
on_callback_function_Demo_R3_save_MarkData_ChunkData_online(SickCam::ImgT * pImg, void * any)
{
    /*  一般，ChunkData 会保存在图像文件中。无需如此单独保存。 */

#ifdef _WIN32
    std::string path_folder = "D:/delete/";
#else
    std::string path_folder = "/home/alex/delete/";
#endif
    auto chunk_data_mark = pImg->get_ChunkInfo();
    std::ofstream file(path_folder + std::to_string(pImg->get_ID()) + ".txt", std::ios::app);
    file
            << "timestamp          "<<", "
            << "encoderValue       "<<", "
            << "overtriggerCount)  "<<", "
            << "frameTriggerActive "<<", "
            << "lineTriggerActive  "<<", "
            << "encoderResetActive "<<", "
            << "encoderA           "<<", "
            << "encoderB           "<<", \n";
    for(auto sub : chunk_data_mark)
    {
        file
            << sub.timestamp<<", "
            << sub.encoderValue<<", "
            << uint16_t (sub.overtriggerCount)<<", "
            << (sub.frameTriggerActive   ? ("true"):("false")) <<", "
            << (sub.lineTriggerActive    ? ("true"):("false")) <<", "
            << (sub.encoderResetActive   ? ("true"):("false")) <<", "
            << (sub.encoderA             ? ("true"):("false")) <<", "
            << (sub.encoderB             ? ("true"):("false")) <<", \n";
    }
    file.close();

    // save image
    pImg->SaveToIconFile(path_folder + std::to_string(pImg->get_ID()), {DN::RAN, DN::MAR});

    std::cout << "Get: " << pImg->get_ID() << std::endl;
}

bool SICK_GTEST_LIBRARY Demo_R3_save_MarkData_ChunkData_online()
{
    std::cout << "Demo Function Name: " << __func__ << std::endl;
    std::cout << "sdk version number:" << SickCam::VER::version_number << std::endl;
    std::cout << "sdk version time:" << SickCam::VER::version_time << std::endl;

    // --- Init Ranger3 environment
    auto pR3S = std::make_shared<SickCam::Ranger3Shared>(
            "",											/* 日志文件 */
            SickCam::Ranger3Shared::getPathToRanger3Producer(),	/* cti 文件 */
            false,										/* 是否打印信息到屏幕 */
            false										/* 是否打印信息到文件 */
    );

    // --- Check connected device
    std::vector<std::string> ip(0);
    if (pR3S->isAvaliable()) {
        auto devices = pR3S->getConDevListIP();
        std::cout << "Found: " << std::endl;
        for (auto sub : devices)
        {
            ip.push_back(sub.first);
            std::cout << "---- " << sub.first << std::endl;
        }
    }
    else
    {
        print_process_ng("Error: scan camera error. \n");
        return false;
    }

    //  --- Init Ranger3
    auto pCam1 = std::make_shared<SickCam::Ranger3>(pR3S, ip[0], true);

    // --- Connect Ranger3
    auto err = pCam1->connectCamera();
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Connect camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        print_process_ng("The camera may be connected by other progress!");
        return false;
    }


    pCam1->setParameterValue("DeviceScanType", "Linescan3D"); // sensor image. 若要 3D Range，请设置为 Linescan3D. Areascan
    pCam1->setParameterValue("ChunkModeActive", "1");

    // --- Start Ranger3
    err = pCam1->startCamera(on_callback_function_Demo_R3_save_MarkData_ChunkData_online);
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Start camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        print_process_ng("The camera may be connected by other progress!");
        return false;
    }

    // --- Hold on main thread
    //getchar();
    for (int i = 0; i < 10; ++i)
    {
        __sleep1MS(1000); // 1 sec
    }

    // --- Stop Ranger3
    err = pCam1->stopCamera();
    if (SickCam::CAM_STATUS::All_OK != err) {
        print_process_ng("Stop camera failed! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        print_process_ng("The camera may be connected by other progress!");
        return false;
    }

    // --- Disconnect Ranger3
    err = pCam1->disconnectCamera();
    if (SickCam::CAM_STATUS::All_OK != err)
    {
        print_process_ng("Disconnect camera! ErrorCode: " + SickCam::CAM_STATUS_str(err));
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifndef SICK_GTEST
int main(int argc, char* argv[])
{
    Demo_R3_save_MarkData_ChunkData_online();
    return 0;
}
#endif


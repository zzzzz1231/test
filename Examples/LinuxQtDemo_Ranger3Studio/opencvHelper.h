#pragma once
#include <opencv2/opencv.hpp>
#include <QtGui/QImage>
#include <Ranger3.h>
//#include <InterfaceOpenCV.h>
#include "../Sample/ApiDllTest/InterfaceOpenCV.h"

class cvHelper
{
public:
    cvHelper(){};

    static QImage mat2qimGray(const cv::Mat & mat);
    static QImage mat2qimRGB(const cv::Mat & mat);

};


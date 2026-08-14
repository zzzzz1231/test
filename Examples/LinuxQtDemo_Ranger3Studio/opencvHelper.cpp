#include "opencvHelper.h"


QImage
cvHelper::mat2qimGray(const cv::Mat & mat)
{
    //cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    //QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
    return qim;
}

QImage
cvHelper::mat2qimRGB(const cv::Mat & mat)
{
    cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    return qim;
}



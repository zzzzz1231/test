#include <fstream>
#include <iostream>
#include <cmath>
#include <ImageTable.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "centerline_extraction.hpp"
#include "ransac.hpp"
#include "unorded_points.hpp"
#include <bitset>

#define PI 3.1415926

cv::Mat 
LaserCenterline::Centerline::load_sick_data(const std::string &filename)
{
    using SickCam::DataNames;
    SickCam::ImgT sick_img;
    sick_img.loadFromIconFile(filename, 1.0, 1.0);
    cv::Mat pic = cv::Mat(sick_img.get_rows(), sick_img.get_cols(), CV_8UC1, (uint8_t *)sick_img.getData(SickCam::DataNames::REF_CAL));
    return pic;
}

std::pair<float, cv::Mat> 
LaserCenterline::Centerline::get_background(const cv::Mat &img)
{
    cv::Mat background;
    cv::Mat img_ = img.clone();
    //cv::cvtColor(img_, img_, cv::COLOR_BGR2GRAY);
    // cv::Mat img_mask = img.clone();
    std::pair<float, cv::Mat> pic_laser;
    cv::Mat mask = cv::Mat::zeros(img.size(), img.depth());
    cv::subtract(img, cv::Scalar(255, 255, 255), background, mask);
    std::vector<int> laser_width;
    int laser_width_sum = 0;
    int laser_sum = 0;
    for (size_t i = 0; i < img_.cols; ++i)
    {
        laser_sum = 0;
        for (size_t j = 0; j < img_.rows; ++j)
        {
            if (img_.at<uchar>(j, i) > 20)
            {
                laser_sum += 1;
            }
        }
        if (laser_sum > 0)
        {
            laser_width.push_back(laser_sum);
            laser_width_sum += laser_sum;
        }
    }
    if ((laser_width_sum / laser_width.size()) % 2 == 0)
        pic_laser.first = laser_width_sum / laser_width.size() + 1;
    else
        pic_laser.first = laser_width_sum / laser_width.size();
    pic_laser.second = background;
    return pic_laser;
}

std::vector<cv::Point2f> 
LaserCenterline::Centerline::maxi_pixels(cv::Mat &Line_img)
{
    cv::Mat Line_img_ = Line_img.clone();
    // 求解最亮的值
    Line_img_.convertTo(Line_img_, CV_32FC1); // 转换数据类型不同的mat，改变位深度，32位浮点型单通道
    GaussianBlur(Line_img_, Line_img_, cv::Size(0, 0), 3, 3);
    std::vector<cv::Point2f> maxi_pts;
    // Line_img_.convertTo(Line_img_, Line_img.depth());
    /*  cv::imwrite("./data/output/after_Gass.png", Line_img_);*/

    for (size_t i = 0; i < Line_img.cols; ++i)
    {
        int maxi = -1;
        cv::Point2d temp_maxi;
        for (size_t j = 0; j < Line_img.rows; ++j)
        {
            if (maxi < Line_img.at<uchar>(j, i))
            {
                maxi = Line_img.at<uchar>(j, i);
                temp_maxi = cv::Point2d(j, i);
            }
        }
        maxi_pts.push_back(temp_maxi);
    }
    return maxi_pts;
}

std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> 
LaserCenterline::Centerline::extract_laser_center(cv::Mat &Line_img, float &laser_width)
{
    cv::cvtColor(Line_img, Line_img, cv::COLOR_BGR2GRAY);
    cv::Mat Line_img_ = Line_img.clone();
    size_t W = int(laser_width);
    size_t Sig = int(W / sqrt(3));
    cv::GaussianBlur(Line_img_, Line_img_, cv::Size(W, W), Sig, Sig);
    Line_img_.convertTo(Line_img_, CV_32FC1);
    int kernelsize = W;
    float sigma = Sig;

    cv::Mat xGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat yGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));

    cv::Mat xxGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat xyGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat yyGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    // 构建高斯二阶偏导数模板代替普通卷积核
    for (int i = -kernelsize; i <= kernelsize; i++)
    {
        for (int j = -kernelsize; j <= kernelsize; j++)
        {
            // new
            xGauKernel.at<float>(i + kernelsize, j + kernelsize) = i * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            yGauKernel.at<float>(i + kernelsize, j + kernelsize) = j * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));

            xxGauKernel.at<float>(i + kernelsize, j + kernelsize) = (1 - (i * i) / (sigma * sigma)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            yyGauKernel.at<float>(i + kernelsize, j + kernelsize) = (1 - (j * j) / (sigma * sigma)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            xyGauKernel.at<float>(i + kernelsize, j + kernelsize) = ((i * j)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (1 / (2 * PI * pow(sigma, 6)));
        }
    }

    std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> center_pts_Evectors;
    std::vector<cv::Point2f> center_pts;
    std::vector<cv::Point2f> Evectors;
    // // // // 一阶偏导数
    cv::Mat dx, dy;
    cv::filter2D(Line_img_, dx, CV_32FC1, xGauKernel); // 卷积
    cv::filter2D(Line_img_, dy, CV_32FC1, yGauKernel);

    // // // // // 二阶偏导
    cv::Mat dxx, dyy, dxy;
    cv::filter2D(Line_img_, dxx, CV_32FC1, xxGauKernel);
    cv::filter2D(Line_img_, dyy, CV_32FC1, yyGauKernel);
    cv::filter2D(Line_img_, dxy, CV_32FC1, xyGauKernel);

    // 一阶偏导数
    // cv::Mat m1, m2;
    // m1 = (cv::Mat_<float>(1, 2) << 1, -1); // x偏导
    // m2 = (cv::Mat_<float>(2, 1) << 1, -1); // y偏导

    // cv::Mat dx, dy;
    // cv::filter2D(Line_img_, dx, CV_32FC1, m1);
    // cv::filter2D(Line_img_, dy, CV_32FC1, m2);

    // // 二阶偏导数
    // cv::Mat m3, m4, m5;
    // m3 = (cv::Mat_<float>(1, 3) << 1, -2, 1);     // 二阶x偏导
    // m4 = (cv::Mat_<float>(3, 1) << 1, -2, 1);     // 二阶y偏导
    // m5 = (cv::Mat_<float>(2, 2) << 1, -1, -1, 1); // 二阶xy偏导

    // cv::Mat dxx, dyy, dxy;
    // cv::filter2D(Line_img_, dxx, CV_32FC1, m3);
    // cv::filter2D(Line_img_, dyy, CV_32FC1, m4);
    // cv::filter2D(Line_img_, dxy, CV_32FC1, m5);

    // std::ofstream file("./data/TxT/T.txt");

    for (size_t i = 0; i < Line_img.cols; ++i)
    {
        for (size_t j = 0; j < Line_img.rows; ++j)
        {
            if (Line_img.at<uchar>(j, i) > 20)
            {
                cv::Mat hessian(2, 2, CV_32FC1);
                hessian.at<float>(0, 0) = dxx.at<float>(j, i);
                hessian.at<float>(0, 1) = dxy.at<float>(j, i);
                hessian.at<float>(1, 0) = dxy.at<float>(j, i);
                hessian.at<float>(1, 1) = dyy.at<float>(j, i);

                cv::Mat eValue, eVectors;
                cv::eigen(hessian, eValue, eVectors); // 求特征值与特征向量

                double nx, ny;
                double fmaxD = 0;
                if (fabs(eValue.at<float>(0, 0)) >= fabs(eValue.at<float>(1, 0))) // 求特征值最大时对应的特征向量
                {
                    nx = eVectors.at<float>(0, 0);
                    ny = eVectors.at<float>(0, 1);
                    fmaxD = eValue.at<float>(0, 0);
                }
                else
                {
                    nx = eVectors.at<float>(1, 0);
                    ny = eVectors.at<float>(1, 1);
                    fmaxD = eValue.at<float>(1, 0);
                }

                double t = -(nx * dx.at<float>(j, i) + ny * dy.at<float>(j, i)) /
                           (nx * nx * dxx.at<float>(j, i) + 2 * nx * ny * dxy.at<float>(j, i) + ny * ny * dyy.at<float>(j, i));

                if (fabs(t * nx) <= 0.5 && fabs(t * ny) <= 0.5)
                {

                    if (fmaxD < 0)
                        t = -t;
                    cv::Point2f tp;
                    tp.x = i + t * nx;
                    tp.y = j + t * ny;
                    center_pts.push_back(tp);
                    Evectors.push_back(cv::Point2f(t * nx, t * ny));
                }
            }
        }
    }

    center_pts_Evectors.first = center_pts;
    center_pts_Evectors.second = Evectors;
    return center_pts_Evectors;
}

LaserCenterline::Centerline::laserExtracted
LaserCenterline::Centerline::extract_laser_center(const cv::Mat& Line_img, const float& laser_width, const int& laser_threshold)
{
    //cv::cvtColor(Line_img, Line_img, cv::COLOR_BGR2GRAY);
    cv::Mat Line_img_ = Line_img.clone();
    size_t W = int(laser_width);
    size_t Sig = int(W / sqrt(3));
    cv::GaussianBlur(Line_img_, Line_img_, cv::Size(W, W), Sig, Sig);
    Line_img_.convertTo(Line_img_, CV_32FC1);
    int kernelsize = W;
    float sigma = Sig;

    cv::Mat xGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat yGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));

    cv::Mat xxGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat xyGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat yyGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    // Gassfuction
    for (int i = -kernelsize; i <= kernelsize; i++)
    {
        for (int j = -kernelsize; j <= kernelsize; j++)
        {
            // new
            xGauKernel.at<float>(i + kernelsize, j + kernelsize) = i * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            yGauKernel.at<float>(i + kernelsize, j + kernelsize) = j * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));

            xxGauKernel.at<float>(i + kernelsize, j + kernelsize) = (1 - (i * i) / (sigma * sigma)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            yyGauKernel.at<float>(i + kernelsize, j + kernelsize) = (1 - (j * j) / (sigma * sigma)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            xyGauKernel.at<float>(i + kernelsize, j + kernelsize) = ((i * j)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (1 / (2 * PI * pow(sigma, 6)));
        }
    }

    //std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> center_pts_Evectors;
    laserExtracted center_pts_Evectors;
    std::vector<cv::Point2f> center_pts;
    std::vector<cv::Point2f> Evectors;
    std::vector<double> vecTs;
    std::vector<cv::Point2f> center_ptsPixel;

    cv::Mat dx, dy;
    cv::filter2D(Line_img_, dx, CV_32FC1, xGauKernel);
    cv::filter2D(Line_img_, dy, CV_32FC1, yGauKernel);

    cv::Mat dxx, dyy, dxy;
    cv::filter2D(Line_img_, dxx, CV_32FC1, xxGauKernel);
    cv::filter2D(Line_img_, dyy, CV_32FC1, yyGauKernel);
    cv::filter2D(Line_img_, dxy, CV_32FC1, xyGauKernel);

    // cv::Mat m1, m2;
    // m1 = (cv::Mat_<float>(1, 2) << 1, -1);
    // m2 = (cv::Mat_<float>(2, 1) << 1, -1);

    // cv::Mat dx, dy;
    // cv::filter2D(Line_img_, dx, CV_32FC1, m1);
    // cv::filter2D(Line_img_, dy, CV_32FC1, m2);

    // // 二阶偏导数
    // cv::Mat m3, m4, m5;
    // m3 = (cv::Mat_<float>(1, 3) << 1, -2, 1);
    // m4 = (cv::Mat_<float>(3, 1) << 1, -2, 1);
    // m5 = (cv::Mat_<float>(2, 2) << 1, -1, -1, 1);

    // cv::Mat dxx, dyy, dxy;
    // cv::filter2D(Line_img_, dxx, CV_32FC1, m3);
    // cv::filter2D(Line_img_, dyy, CV_32FC1, m4);
    // cv::filter2D(Line_img_, dxy, CV_32FC1, m5);

    // std::ofstream file("./data/TxT/T.txt");


    for (size_t i = 0; i < Line_img.cols; ++i)
    {
        for (size_t j = 0; j < Line_img.rows; ++j)
        {
            // 一个列方向上只能保留一个点。// 前提假设是一列上只能够看到一个光带。
            if (Line_img.at<uchar>(j, i) > laser_threshold)
            {
                cv::Mat hessian(2, 2, CV_32FC1);
                hessian.at<float>(0, 0) = dxx.at<float>(j, i);
                hessian.at<float>(0, 1) = dxy.at<float>(j, i);
                hessian.at<float>(1, 0) = dxy.at<float>(j, i);
                hessian.at<float>(1, 1) = dyy.at<float>(j, i);

                cv::Mat eValue, eVectors;
                cv::eigen(hessian, eValue, eVectors);

                double nx, ny;
                double fmaxD = 0;
                if (fabs(eValue.at<float>(0, 0)) >= fabs(eValue.at<float>(1, 0)))
                {
                    nx = eVectors.at<float>(0, 0);
                    ny = eVectors.at<float>(0, 1);
                    fmaxD = eValue.at<float>(0, 0);
                }
                else
                {
                    nx = eVectors.at<float>(1, 0);
                    ny = eVectors.at<float>(1, 1);
                    fmaxD = eValue.at<float>(1, 0);
                }

                // t 肯定是表示距离，向量距离。但是这个计算过程是怎么确定的呢？
                double t = -(nx * dx.at<float>(j, i) + ny * dy.at<float>(j, i)) /
                    (nx * nx * dxx.at<float>(j, i) + 2 * nx * ny * dxy.at<float>(j, i) + ny * ny * dyy.at<float>(j, i));

                if (fabs(t * nx) <= 0.5 && fabs(t * ny) <= 0.5)
                {
                    cv::Point2f tp;
                    tp.x = i + t * nx;
                    tp.y = j + t * ny;
                    center_pts.push_back(tp);
                    Evectors.push_back(cv::Point2f(t * nx, t * ny));
                    vecTs.push_back(t);
                    center_ptsPixel.push_back(cv::Point2f(i, j));
                }
            }
        }
    }

    center_pts_Evectors.centerSubPixel = center_pts;
    center_pts_Evectors.centerEigenVector = Evectors;
    center_pts_Evectors.centerPixel = center_ptsPixel;
    center_pts_Evectors.centerT = vecTs;


    return center_pts_Evectors;
}


LaserCenterline::Centerline::laserExtracted
LaserCenterline::Centerline::extract_laser_center_xld(const cv::Mat& Line_img, const std::vector<int> Line_Y, const float& laser_width, const int& laser_threshold)
{
    //cv::cvtColor(Line_img, Line_img, cv::COLOR_BGR2GRAY);
    cv::Mat Line_img_ = Line_img.clone();
    size_t W = int(laser_width);
    size_t Sig = int(W / sqrt(3));
    cv::GaussianBlur(Line_img_, Line_img_, cv::Size(W, W), Sig, Sig);
    Line_img_.convertTo(Line_img_, CV_32FC1);
    int kernelsize = W;
    float sigma = Sig;

    cv::Mat xGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat yGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));

    cv::Mat xxGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat xyGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    cv::Mat yyGauKernel(2 * kernelsize + 1, 2 * kernelsize + 1, CV_32FC1, cv::Scalar::all(0));
    // Gassfuction
    for (int i = -kernelsize; i <= kernelsize; i++)
    {
        for (int j = -kernelsize; j <= kernelsize; j++)
        {
            // new
            xGauKernel.at<float>(i + kernelsize, j + kernelsize) = i * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            yGauKernel.at<float>(i + kernelsize, j + kernelsize) = j * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));

            xxGauKernel.at<float>(i + kernelsize, j + kernelsize) = (1 - (i * i) / (sigma * sigma)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            yyGauKernel.at<float>(i + kernelsize, j + kernelsize) = (1 - (j * j) / (sigma * sigma)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (-1 / (2 * PI * pow(sigma, 4)));
            xyGauKernel.at<float>(i + kernelsize, j + kernelsize) = ((i * j)) * exp(-1 * (i * i + j * j) / (2 * sigma * sigma)) * (1 / (2 * PI * pow(sigma, 6)));
        }
    }

    //std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> center_pts_Evectors;
    laserExtracted center_pts_Evectors;
    std::vector<cv::Point2f> center_pts;
    std::vector<cv::Point2f> Evectors;
    std::vector<double> vecTs;
    std::vector<cv::Point2f> center_ptsPixel;

    cv::Mat dx, dy;
    cv::filter2D(Line_img_, dx, CV_32FC1, xGauKernel);
    cv::filter2D(Line_img_, dy, CV_32FC1, yGauKernel);

    cv::Mat dxx, dyy, dxy;
    cv::filter2D(Line_img_, dxx, CV_32FC1, xxGauKernel);
    cv::filter2D(Line_img_, dyy, CV_32FC1, yyGauKernel);
    cv::filter2D(Line_img_, dxy, CV_32FC1, xyGauKernel);


    for (size_t i = 0; i < Line_img.cols; ++i)
    {
        if (Line_Y[i] == IDX_NULL)
            continue;

        size_t j = Line_Y[i];
        //for (size_t j = 0; j < Line_img.rows; ++j)
        {
            // 一个列方向上只能保留一个点。// 前提假设是一列上只能够看到一个光带。
            if (Line_img.at<uchar>(j, i) > laser_threshold)
            {
                cv::Mat hessian(2, 2, CV_32FC1);
                hessian.at<float>(0, 0) = dxx.at<float>(j, i);
                hessian.at<float>(0, 1) = dxy.at<float>(j, i);
                hessian.at<float>(1, 0) = dxy.at<float>(j, i);
                hessian.at<float>(1, 1) = dyy.at<float>(j, i);

                cv::Mat eValue, eVectors;
                cv::eigen(hessian, eValue, eVectors);

                double nx, ny;
                double fmaxD = 0;
                if (fabs(eValue.at<float>(0, 0)) >= fabs(eValue.at<float>(1, 0)))
                {
                    nx = eVectors.at<float>(0, 0);
                    ny = eVectors.at<float>(0, 1);
                    fmaxD = eValue.at<float>(0, 0);
                }
                else
                {
                    nx = eVectors.at<float>(1, 0);
                    ny = eVectors.at<float>(1, 1);
                    fmaxD = eValue.at<float>(1, 0);
                }

                // t 肯定是表示距离，向量距离。但是这个计算过程是怎么确定的呢？
                double t = -(nx * dx.at<float>(j, i) + ny * dy.at<float>(j, i)) /
                    (nx * nx * dxx.at<float>(j, i) + 2 * nx * ny * dxy.at<float>(j, i) + ny * ny * dyy.at<float>(j, i));

                //if (fabs(t * nx) <= 0.5 && fabs(t * ny) <= 0.5)
                {
                    cv::Point2f tp;
                    tp.x = i + t * nx;
                    tp.y = j + t * ny;
                    center_pts.push_back(tp);
                    Evectors.push_back(cv::Point2f(t * nx, t * ny));
                    vecTs.push_back(t);
                    center_ptsPixel.push_back(cv::Point2f(i, j));
                }
            }
        }
    }

    center_pts_Evectors.centerSubPixel = center_pts;
    center_pts_Evectors.centerEigenVector = Evectors;
    center_pts_Evectors.centerPixel = center_ptsPixel;
    center_pts_Evectors.centerT = vecTs;


    return center_pts_Evectors;
}

int extract_Max_Idx(const std::vector<int>& values)
{
    int vMax = IDX_NULL;
    int idx = 0;
    for (int i=0; i< values.size(); ++i)
    {
        // vMax = vMax < values[i] ? values[i] : vMax;

        if (vMax < values[i])
        {
            vMax = values[i];
            idx = i;
        }

    }
    return idx;
}

int extract_Center_Of_Gravity_Idx(const std::vector<int>& values, const int& laser_threshold)
{
    int sum1 = 0;
    int sum2 = 0;

    for (int i = 0; i < values.size(); ++i)
    {
        if(values[i] > laser_threshold)
        {
            sum1 += values[i]*i;
            sum2 += values[i];
        }
    }

    int vCOG = IDX_NULL;
    if (sum2 > 0)
    {
        vCOG = sum1 / sum2;
    }

    return vCOG;
}

int
extract_Center_Of_Range_Idx(const std::vector<int>& values, const int& laser_threshold)
{
    int idx1 = 0;
    int idx2 = 0;

    for (int i = 0; i < values.size(); ++i)
    {
        if (values[i] > laser_threshold)
        {
            idx1 = i;
            break;
        }
    }

    for (int i = values.size()-1; i >0; --i)
    {
        if (values[i] > laser_threshold)
        {
            idx2 = i;
            break;
        }
    }

    int idx = (idx1+ idx2)/2;

    return idx;
}



// todo: 列数据预处理。对于多峰状态下的中值滤波处理 --- 



std::vector<bool> vector_and(const std::vector<bool>& a, const std::vector<bool>& b)
{
    if (a.size() != b.size())
        return std::vector<bool>();

    std::vector<bool> res;
    for (int i=0; i<a.size(); ++i)
    {
        res.push_back(a[i] && b[i]);
    }

    return res;
}

std::vector<bool> vector_or(const std::vector<bool>& a, const std::vector<bool>& b)
{
    if (a.size() != b.size())
        return std::vector<bool>();

    std::vector<bool> res;
    for (int i = 0; i < a.size(); ++i)
    {
        res.push_back(a[i] || b[i]);
    }

    return res;
}

// id 对应的值应该都是1
std::vector<std::pair<int, int>> findPeaksRange(const std::vector<bool>& peaks)
{
    std::vector<std::pair<int, int>> peaksRange;

    std::pair<int, int> PRange = {-1, -1};
    bool isInRange = false;

    // peaks[0] is 1
    if (peaks[0] == 1)
    {
        PRange.first = 0;
        isInRange = true;
    }
    for (int i = 1; i < peaks.size(); ++i)
    {
        // peaks[0] is 0
        if (!isInRange)
        {
            if (!peaks[i - 1] && peaks[i]) // find 0,1
            {
                PRange.first = i;
                isInRange = true;
            }
        }
        else
        {
            if (peaks[i - 1] && !peaks[i]) // find 1,0
            {
                PRange.second = i - 1;
                isInRange = false;

                // save result
                peaksRange.push_back(PRange);
            }

            // peaks[last] is 0
            // pass

            // peaks[last] is 1
            if (i == peaks.size() - 1)
            {
                PRange.second = i - 1;
                isInRange = false;

                // save result
                peaksRange.push_back(PRange);
            }
        }
    }

    return peaksRange;
}


std::vector<int>
grayColumnPreProcess(const std::vector<int>& _values, const int& move_step, const std::vector<bool>& kernel)
{
    if (kernel.size() % 2 != 1)
    {
        return std::vector<int>();
    }

    const int n = kernel.size() / 2;
    std::vector<bool> ker; // 一半的 kernel
    for (int i = 0; i < n; ++i)
    {
        ker.push_back(kernel[i]);
    }
    std::vector<int> values = _values;

    // 补头尾
    values.insert(values.begin(), n, 255);
    values.insert(values.end(), n, 255);

    // 数组的 Shrink
    std::vector<bool> resultShrink(0);
    for (int i = n; i < values.size()-n; ++i)
    {
        bool isKeep = true;

        // 判定 kernel 上的对应点数据
        for (int j = 0; j < kernel.size(); ++j)
        {
            // 1 才需要判断
            if (kernel[j])
            {
                // 区域内有一个点 等于 0，就 不要 这个点
                if ((values[i - n + j] == 0))
                {
                    isKeep = false;
                    break;
                }
            }
        }
        resultShrink.push_back(isKeep);
    }

    // 数组的 Expand
    std::vector<bool> resultExpand(0);
    for (int i = n; i < values.size()-n; ++i)
    {
        bool isKeep = false;

        // 判定 kernel 上的对应点数据
        for (int j=0; j<kernel.size(); ++j)
        {
            // 1 才需要判断
            if (kernel[j])
            {
                // 区域内有一个点 大于 0，就 要 这个点
                if ((values[i - n + j] > 0))
                {
                    isKeep = true;
                    break;
                }
            }
        }
        resultExpand.push_back(isKeep);
    }

    // 数据整合
    std::vector<int> valuesSelected = _values;
    std::vector<bool> valuesKept;

    auto sPeaks = findPeaksRange(resultShrink);
    if (sPeaks.size() == 0)
    {
        return _values;
    }
    else if (sPeaks.size() == 1)
    {
        valuesKept = vector_and(resultExpand, resultShrink);
    }
    else
    {
        valuesKept = vector_or(resultExpand, resultShrink);
    }

    // 选择数据
    for (int j = 0; j < _values.size(); ++j)
    {
        if (!valuesKept[j])
        {
            valuesSelected[j] = 0;
        }
    }

    return valuesSelected;
}




// todo: 基于列方向的灰度变化特征，计算激光线中心 ---
std::vector<int>
LaserCenterline::Centerline::extract_laser_center_px(const cv::Mat& Line_img, const Algorithm& Algo, const int& laser_threshold)
{
    // to speed up, transpose image.
    cv::Mat src_t = Line_img.t();

    std::vector<int> results(src_t.rows, -1);
    for (int j = 0; j < src_t.rows; ++j)
    {
        auto pd = src_t.ptr<uchar>(j);
        std::vector<int> valuesRough(src_t.cols, 0);
        for (int i = 0; i < src_t.cols; ++i)
        {
            if (pd[i] > laser_threshold)
            {
                valuesRough[i] = pd[i];
            }
        }

        // 对数据进行 SICK 滤波
        auto values = grayColumnPreProcess(valuesRough, 1, {1,1,1});


        // 默认单峰
        int center = IDX_NULL;
        if (Algo == Algorithm::Max)
        {
            center = extract_Max_Idx(values);
        }
        else if (Algo == Algorithm::Center_Of_Gravity)
        {
            center = extract_Center_Of_Gravity_Idx(values, laser_threshold);
        }
        else if (Algo == Algorithm::Center_Of_Range)
        {
            center = extract_Center_Of_Range_Idx(values, laser_threshold);
        }
        else if (Algo == Algorithm::First_Peak)
        {

        }
        else if (Algo == Algorithm::Last_Peak)
        {

        }

        // log result
        results[j] = center;
    }

    return results;
}


std::vector<cv::Point> 
LaserCenterline::Centerline::slecet_center(std::vector<cv::Point> &center_pts, const cv::Mat &line_img, std::vector<int>& idx)
{
    //double best_error = std::numeric_limits<double>::max();
    std::vector<cv::Point> new_center = Unorded::removeDuplicates(center_pts);
    std::vector<cv::Point> select_center = Unorded::select_index(center_pts, line_img, idx);
    return select_center;
}

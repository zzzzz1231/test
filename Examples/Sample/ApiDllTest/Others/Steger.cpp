
#include "Steger.h"


//std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> /*LaserCenterline::Centerline::*/
laserExtracted
extract_laser_center(const cv::Mat &Line_img, const float &laser_width, const int& laser_threshold)
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


// 优化方向：粗提取先行，这个算法需要给定一个点，然后精确的确定它的亚像素位置。
// 对于较为粗的激光线，或者噪声很大的数据有很多问题
// 连线算法也是很重要的一点

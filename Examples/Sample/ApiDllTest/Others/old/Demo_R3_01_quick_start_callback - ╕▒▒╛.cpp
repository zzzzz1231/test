#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main2()
{
    Mat img = imread("F://1.png", 0);
    Mat binImg;
    threshold(img, binImg, 128, 255, THRESH_BINARY);

    std::vector<std::vector<Point>> contours;
    findContours(binImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Mat skeleton(binImg.size(), CV_8UC1, Scalar(0));
    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(skeleton, contours, i, Scalar(255), 1);
    }

    // 细化
    Mat skel(binImg.size(), CV_8UC1, Scalar(0));
    Mat temp;
    Mat eroded;
    Mat element = getStructuringElement(MORPH_CROSS, Size(5, 5));

    bool done;
    do
    {
        erode(skeleton, eroded, element);
        dilate(eroded, temp, element);
        subtract(skeleton, temp, temp);
        bitwise_or(skel, temp, skel);
        eroded.copyTo(skeleton);

        done = (countNonZero(skeleton) == 0);
    } while (!done);

    // 输出骨架坐标
    cv::Mat src = imread("F://1.png", IMREAD_UNCHANGED);
    for (int r = 0; r < skel.rows; ++r)
    {
        for (int c = 0; c < skel.cols; ++c)
        {
            if (skel.at<uchar>(r, c) != 0)
            {
                cv::circle(src, cv::Point(c, r), 0, cv::Scalar(255, 0, 0, 255), 1);
                //std::cout << "Skeleton point: (" << c << ", " << r << ")" << std::endl;
            }
        }
    }

    imshow("Input Image", src);
    return 0;
}

int main()
{
    Mat src = imread("F://1.png", IMREAD_GRAYSCALE); // 读入灰度图像
    Mat dst;
    threshold(src, dst, 127, 255, THRESH_BINARY); // 二值化

    Mat skel(src.size(), CV_8UC1, Scalar(0)); // 创建空白图像，用于存储骨架像素的坐标

    Mat temp;
    Mat eroded;

    while (true)
    {
        erode(dst, eroded, Mat()); // 腐蚀操作
        dilate(eroded, temp, Mat()); // 膨胀操作
        subtract(dst, temp, temp); // 差分操作，得到骨架像素

        bitwise_or(skel, temp, skel);


        erode(dst, dst, Mat());

        if (countNonZero(dst) == 0) // 如果所有像素都处理完毕，则退出循环
            break;
    }

    // 输出骨架像素的坐标
    cv::Mat src2 = imread("F://1.png", IMREAD_UNCHANGED);
    for (int i = 0; i < skel.rows; i++)
    {
        for (int j = 0; j < skel.cols; j++)
        {
            if (skel.at<uchar>(i, j) != 0)
            {
                cv::circle(src2, cv::Point(j, i), 0, cv::Scalar(255, 0, 0, 255), 1);
                //std::cout << "(" << j << ", " << i << ")" << std::endl;
            }
        }
    }

    return 0;
}
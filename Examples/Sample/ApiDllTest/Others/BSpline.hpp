#pragma once
#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <opencv2/opencv.hpp>

namespace BSpline_center
{
    enum Type // B样条类型
    {
        uniform, // 均匀
        quniform // 准均匀
    };
    class Bspline // B样条曲线
    {
    public:
        Bspline(int _k, int _type, std::vector<cv::Point2f> _p, bool _bDelayShow);
        ~Bspline();
        double BsplineBfunc(int i, int k, double uu); // 计算每个u和每个i对应的B样条
        std::vector<cv::Point2f> creatBspline();      // 计算整个的B样条

    public:
        int k;                  // 阶数
        int n;                  // 控制点数-1
        int type;               // B样条类型
        std::vector<double> u;  // 自变量
        double delta_u = 0.002; // 自变量间隔
        double uBegin;
        double uEnd;
        std::vector<cv::Point2f> p;      // 控制点
        std::vector<cv::Point2f> pTrack; // 轨迹点
        bool bDelayShow = true;          // 是否显示曲线生成的过程
    };
}

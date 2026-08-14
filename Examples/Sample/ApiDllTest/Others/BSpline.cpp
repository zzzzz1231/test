#include <iostream>
#include <optional>
#include <vector>
#include <opencv2/opencv.hpp>
#include "BSpline.hpp"

BSpline_center::Bspline::Bspline(int _k, int _type, std::vector<cv::Point2f> _p, bool _bDelayShow)
{
    bDelayShow = _bDelayShow;
    k = _k;
    n = _p.size() - 1;
    if (k > n + 1 || _p.empty()) // k必需<=n+1， 不能一个控制点都没有
    {
        std::cout << "error!" << std::endl;
    }

    type = _type;
    p = _p;

    double u_tmp = 0.0;
    u.push_back(u_tmp); // 先把0.0存入

    if (type == uniform) // 均匀
    {
        double dis_u = 1.0 / (k + n);
        for (int i = 1; i < n + k + 1; i++) // n + k + 1个分段
        {
            u_tmp += dis_u;
            u.push_back(u_tmp);
        }
    }
    else if (type == quniform) // 准均匀
    {
        int j = 3; // 重复度
        double dis_u = 1.0 / (k + n - (j - 1) * 2);
        for (int i = 1; i < j; i++)
        {
            u.push_back(u_tmp);
        }
        for (int i = j; i < n + k - j + 2; i++)
        {
            u_tmp += dis_u;
            u.push_back(u_tmp);
        }
        for (int i = n + k - j + 2; i < n + k + 1; i++) // n + k + 1个分段
        {
            u.push_back(u_tmp);
        }
    }

    // std::cout << "" << k << ", 控制点数:" << n + 1 << std::endl;
    // std::cout << "delta_u= " << delta_u << ", u的序列为:";
    // for (int i = 0; i < u.size(); i++)
    // {
    //     std::cout << u[i] << ", ";
    // }
    // std::cout << std::endl;

    uBegin = u[k - 1];
    uEnd = u[n + 1]; // 计算u的区间
}

BSpline_center::Bspline::~Bspline()
{
    p.clear();
    u.clear();
    pTrack.clear();
}

double BSpline_center::Bspline::BsplineBfunc(int i, int k, double uu) // 计算每个u和每个i对应的B样条
{
    // cout << "****************i= " << i << endl;
    /*if (i == n + 1)
    {
        return 0.0;//防止越界
    }*/

    double Bfunc = 0.0;

    if (k == 1) // 递归退出的条件
    {
        if (u[i] <= uu && uu < u[i + 1])
        {
            Bfunc = 1.0;
        }
        else
        {
            Bfunc = 0.0;
        }
    }
    else if (k >= 2)
    {
        double A = 0.0;
        double B = 0.0;

        if (u[i + k - 1] - u[i] == 0.0)
        {
            // cout << "A = 0.0; u[i+k-1]= " << u[i + k - 1] << ", u[i]= " << u[i] << endl;
            A = 0.0; // 约定分母为0时，整个除式为0
        }
        else
        {
            A = (uu - u[i]) / (u[i + k - 1] - u[i]);

            /*if (A <= 0.0)
            {
                cout << "A < 0.0; A= " << A << ", uu= " << uu << ", u[i]= " << u[i] << ", u[i + k - 1]= " << u[i + k - 1] << ", i= " << i << ", k= " << k << endl;
            }*/
        }

        if (u[i + k] - u[i + 1] == 0.0)
        {
            // cout << "B = 0.0; u[i + k]= " << u[i + k] << ", u[i + 1] " << u[i + 1] << endl;
            B = 0.0; // 约定分母为0时，整个除式为0
        }
        else
        {
            B = (u[i + k] - uu) / (u[i + k] - u[i + 1]);

            /*if (B <= 0.0)
            {
                cout << "B < 0.0; B= " << B << ", uu= " << uu << ", u[i]= " << u[i] << ", u[i + k]= " << u[i + k] << ", u[i + 1]= " << u[i + 1] << ", i= " << i << ", k= " << k << endl;
            }*/
        }

        Bfunc = A * BsplineBfunc(i, k - 1, uu) + B * BsplineBfunc(i + 1, k - 1, uu); // 递归
    }

    // cout << "Bfunc= " << Bfunc << endl;
    return Bfunc;
}
std::vector<cv::Point2f> BSpline_center::Bspline::creatBspline() // 计算整个的B样条
{
    // int r = 0;
    std::vector<cv::Point2f> BSpline_results;
    for (double uu = uBegin; uu <= uEnd; uu += delta_u) // u的循环放外层，对应每个u，去遍历所有控制点
    {
        cv::Point2f Pu = {0.0, 0.0};    // 每轮循环初始化
        for (int i = 0; i < n + 1; i++) // i从0到n，每个控制点
        {
            double xtmp = p[i].x;
            double ytmp = p[i].y;
            double BfuncTmp = BsplineBfunc(i, k, uu);
            Pu.x += xtmp * BfuncTmp;
            Pu.y += ytmp * BfuncTmp; // 累加
        }
        pTrack.push_back(Pu); // 轨迹点
        BSpline_results.push_back(Pu);
    }

    // std::cout << "track point: " << pTrack.size() << std::endl;
    // for (auto it = pTrack.begin(); it != pTrack.end(); it++)
    // {
    //     std::cout << "(" << it->x << ", " << it->y << ") ";
    // }
    return BSpline_results;
}

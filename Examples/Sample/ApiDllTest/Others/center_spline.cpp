
#include <iostream>
#include <algorithm>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2\core\core.hpp>

#include "center_spline.hpp"
#include <iostream>

std::pair<std::vector<float>, std::vector<float>> Spline_center::InitData(std::vector<cv::Point> &new_center, const cv::Mat &Line_img)
{
    std::vector<float> X;
    std::vector<float> Y;
    std::pair<std::vector<float>, std::vector<float>> points;
    for (size_t i = 0; i < new_center.size(); ++i)
    {
        X.push_back(new_center[i].x);
        Y.push_back(new_center[i].y);
    }
    points.first = X;
    points.second = Y;
    return points;
}
Eigen::VectorXf Spline_center::FitterLeastSquareMethod(std::vector<float> &X, std::vector<float> &Y, uint8_t orders)
{
    // abnormal input verification
    if (X.size() < 2 || Y.size() < 2 || X.size() != Y.size() || orders < 1)
        exit(EXIT_FAILURE);

    // map sample data from STL vector to eigen vector
    Eigen::Map<Eigen::VectorXf> sampleX(X.data(), X.size());
    Eigen::Map<Eigen::VectorXf> sampleY(Y.data(), Y.size());

    Eigen::MatrixXf mtxVandermonde(X.size(), orders + 1); // Vandermonde matrix of X-axis coordinate vector of sample data
    Eigen::VectorXf colVandermonde = sampleX;             // Vandermonde column

    // construct Vandermonde matrix column by column
    for (size_t i = 0; i < orders + 1; ++i)
    {
        if (0 == i)
        {
            mtxVandermonde.col(0) = Eigen::VectorXf::Constant(X.size(), 1, 1);
            continue;
        }
        if (1 == i)
        {
            mtxVandermonde.col(1) = colVandermonde;
            continue;
        }
        colVandermonde = colVandermonde.array() * sampleX.array();
        mtxVandermonde.col(i) = colVandermonde;
    }

    // calculate coefficients vector of fitted polynomial
    Eigen::VectorXf result = (mtxVandermonde.transpose() * mtxVandermonde).inverse() * (mtxVandermonde.transpose()) * sampleY;

    return result;
}

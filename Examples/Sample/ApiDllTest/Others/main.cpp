#include <iostream>
#include <opencv2/opencv.hpp>
#include "Others\centerline_extraction.hpp"
#include "Others\center_spline.hpp"
#include "Others\medianFilter.hpp"
#include "Others\BSpline.hpp"

cv::Mat draw_res(const cv::Mat &img, const std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> &center_pts_evctors, int a = 10)
{
    auto points = center_pts_evctors.first;
    auto normals = center_pts_evctors.second;

    cv::Mat img_out;
    cv::resize(img, img_out, img.size() * a, (0, 0), (0, 0), cv::INTER_NEAREST);
    auto convert_point = [&](const cv::Point2f &p)
    {
        cv::Point2i point;
        int x = std::max(0, int(p.x * a));
        int y = std::max(0, int(p.y * a));
        x = std::min(x, img_out.cols - 1);
        y = std::min(y, img_out.rows - 1);
        point = {x, y};
        return point;
    };

    for (int i = 0; i < points.size(); i++)
    {
        auto p = convert_point(points[i]);
        auto begin_p = convert_point(points[i] - normals[i]);
        cv::arrowedLine(img_out, begin_p, p, {0, 0, 255}, 1);
        img_out.at<cv::Vec3b>(p.y, p.x) = {0, 255, 0};
        img_out.at<cv::Vec3b>(begin_p.y, begin_p.x) = {255, 0, 0};
    }
    return img_out;
}

// a0 + a1*X + a2*X^2 + a3*X^3 + ... + ai*X^i
float polynomial(const float X, const std::vector<float>& As)
{
    // results[0] + results[1] * least_square.first[i] + results[2] * pow(least_square.first[i], 2) + results[3] * pow(least_square.first[i], 3);

    float res = 0.0f;

    for (int i=0; i< As.size(); ++i)
    {
        res += (As[i] * pow(X, i));
    }

    return res;

}

int main()
{
    LaserCenterline::Centerline Line;
    //std::string filename = "./data/filename.txt";
    //std::ifstream fin(filename, std::ios::in);
    //std::string filenames;
    std::vector<cv::Mat> laser_img;
    //while (fin >> filenames)
    //{
    //    cv::Mat src = cv::imread(filenames);
    //    cv::Mat temp = src.clone();
    //    laser_img.push_back(temp);
    //}
    laser_img.push_back(cv::imread("F:\\gitlab\\GlassSix\\liu\\img\\7.png", cv::IMREAD_COLOR));
    for (size_t i = 0; i < laser_img.size(); ++i)
    {
        cv::Mat src_img = laser_img[i];
        cv::Mat img_ = src_img.clone();
        std::pair<float, cv::Mat> laser_img = Line.get_background(src_img);
        cv::Mat Line_img;
        cv::addWeighted(laser_img.second, -1, src_img, 1, 0, Line_img);

        std::pair<std::vector<cv::Point2f>, std::vector<cv::Point2f>> center_pts_evctors = Line.extract_laser_center(Line_img, laser_img.first);
        // // std::cout << laser_img.first << std::endl;
        std::vector<cv::Point2f> center_pts = center_pts_evctors.first;
        std::cout << center_pts.size() << std::endl;
        // // 超分辨率画图
        // //  cv::Mat imgout = draw_res(src_img, center_pts_evctors, 10);
        // //  cv::imwrite("./data/output/evctors_one_gass1.png", imgout);
        std::vector<cv::Point> Gass;
        for (size_t i = 0; i < center_pts.size(); ++i)
        {
            auto &rgb = img_.at<cv::Vec3b>(center_pts[i].y, center_pts[i].x);
            cv::Point rpt;
            rpt.x = static_cast<int>(std::round(center_pts[i].x));
            rpt.y = static_cast<int>(std::round(center_pts[i].y));
            // cv::circle(img_, rpt, 0.1, cv::Scalar(rgb[0], rgb[1], 255));
            Gass.push_back(rpt);
        }
        // cv::imwrite("./data/output/laser_" + std::to_string(i + 1) + "_Gass.png", img_);
        std::vector<int> idx;
        std::vector<cv::Point> new_center = Line.slecet_center(Gass, Line_img, idx);
        std::cout << new_center.size() << std::endl;

        // for (size_t i = 0; i < new_center.size(); ++i)
        // {
        //     cv::circle(img_, cv::Point(new_center[i].x, new_center[i].y), 0.1, cv::Scalar(255, 0, 0));
        // }
        // cv::imwrite("./data/output/laser_" + std::to_string(i + 1) + "_Gass_unique.png", img_);

        // std::vector<cv::Point2f> maxi_pts = Line.maxi_pixels(Line_img);

        std::vector<cv::Point> median_center = MedianFilter::median_filter(new_center, 5);

        std::vector<cv::Point> avg_center = MedianFilter::avg_filter(new_center, 5);
        for (size_t i = 0; i < median_center.size(); ++i)
        {
            auto &rgb = img_.at<cv::Vec3b>(median_center[i].y, median_center[i].x);
            cv::Point rpt;
            rpt.x = static_cast<int>(std::round(median_center[i].x));
            rpt.y = static_cast<int>(std::round(median_center[i].y));
            cv::circle(img_, rpt, 0.1, cv::Scalar(rgb[0], rgb[1], 255));
        }
        //cv::imwrite("./data/output/laser_" + std::to_string(i + 1) + "_Gass_Median.png", img_);

        ////最小二乘拟合求解多项式
        std::pair<std::vector<float>, std::vector<float>> least_square = Spline_center::InitData(median_center, Line_img);
        Eigen::VectorXf results = Spline_center::FitterLeastSquareMethod(least_square.first, least_square.second, 5);
        std::vector<float> As;
        for (int residx = 0; residx<results.size(); ++residx)
        {
            As.push_back(results[residx]);
        }
        std::cout << results[0] << " " << results[1] << " " << results[2] << " " << results[3] << std::endl;
        std::vector<cv::Point2f> finals;
        for (size_t i = 0; i < least_square.first.size(); ++i)
        {
            //float y = results[0] + results[1] * least_square.first[i] + results[2] * pow(least_square.first[i], 2) + results[3] * pow(least_square.first[i], 3);
            float y = polynomial(least_square.first[i], As);
            finals.push_back(cv::Point2f(least_square.first[i], y));
        }
        // 在原图上显示提取结果
        std::vector<cv::Point> fan;
        for (int k = 0; k < finals.size(); k++)
        {
            cv::Point rpt;
            rpt.x = static_cast<int>(std::round(finals[k].x));
            rpt.y = static_cast<int>(std::round(finals[k].y));
            cv::circle(img_, rpt, 0.5, cv::Scalar(0, 255, 0), 1);
            fan.push_back(rpt);
        }

        //cv::imwrite("./data/output/laser_" + std::to_string(i + 1) + "_Gass_unique_median_LM_F.png", img_);
    }
    return 0;
}

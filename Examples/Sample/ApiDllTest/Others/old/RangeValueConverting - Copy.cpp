#include <Ranger3.h>		 // 此头文件封装了，API 的主体。 

#include <opencv2/opencv.hpp> // OpenCV 仅作为本示例程序中的显示之用，API 并不依赖该库。
#include <iostream>


////////////////////////////////////////////////////////////////////////////////

std::vector<int> getY_From_Range_To_Sensor(
	const std::vector<uint16_t> & vRange_A_Line, 
	const int & offsetY,
	const int & height_region1,
	const bool rangeAxis_is_Standard)
{
	std::vector<int> res;
	for (auto sub : vRange_A_Line)
	{
		if (rangeAxis_is_Standard)
		{
			int y_in_sensor = cvRound(sub / 16 + offsetY);
			res.push_back(y_in_sensor);
		}
		else
		{
			int offsetY_reversed = 832 - offsetY - height_region1;
			int y_in_sensor = cvRound(832 - sub / 16 - offsetY_reversed);
			res.push_back(y_in_sensor);
		}

	}

	return res;
}

int main(int argc, char* argv[])
{
	{
		// ------ Convert
		SickCam::ImgT iconRange;
		iconRange.loadFromIconFile("D:\\support\\extra\\1", 1.0);
		uint16_t* pBeg = (uint16_t*)iconRange.getData(SickCam::DataNames::RAN);
		std::vector<uint16_t> aLine(pBeg, pBeg + 2560);
		std::vector<int> res = getY_From_Range_To_Sensor(aLine, 0, 256, false);


		// ------ Show result
		SickCam::ImgT iconSensor;
		iconSensor.loadFromIconFile("D:\\support\\extra\\sen", 1.0);
		cv::Mat senIm(iconSensor.getS_rows(), iconSensor.getS_cols(), CV_8UC1, iconSensor.getData(SickCam::DataNames::SEN));
		cv::Mat senIm3;
		std::vector<cv::Mat> vSenImg(3, senIm);
		cv::merge(vSenImg, senIm3);
		for (size_t i = 0; i < 2560; ++i)
		{
			cv::circle(senIm3, cv::Point(i, res[i]), 1, cv::Scalar(0, 0, 255), -1);
		}
	}

	{
		// ------ Convert
		SickCam::ImgT iconRange;
		iconRange.loadFromIconFile("D:\\support\\extra\\2", 1.0);
		uint16_t* pBeg = (uint16_t*)iconRange.getData(SickCam::DataNames::RAN);
		std::vector<uint16_t> aLine(pBeg, pBeg + 2560);
		std::vector<int> res = getY_From_Range_To_Sensor(aLine, 0, 832, false);


		// ------ Show result
		SickCam::ImgT iconSensor;
		iconSensor.loadFromIconFile("D:\\support\\extra\\sen", 1.0);
		cv::Mat senIm(iconSensor.getS_rows(), iconSensor.getS_cols(), CV_8UC1, iconSensor.getData(SickCam::DataNames::SEN));
		cv::Mat senIm3;
		std::vector<cv::Mat> vSenImg(3, senIm);
		cv::merge(vSenImg, senIm3);
		for (size_t i = 0; i<2560; ++i)
		{
			cv::circle(senIm3, cv::Point(i, res[i]), 1, cv::Scalar(0, 0, 255), -1);
		}
	}

	return 0;
}


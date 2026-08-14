#include <Ranger3.h>		 // 此头文件封装了，API 的主体。 

#include <opencv2/opencv.hpp> // OpenCV 仅作为本示例程序中的显示之用，API 并不依赖该库。
#include <iostream>
#include <omp.h>

////////////////////////////////////////////////////////////////////////////////

cv::Mat draw_Range_To_Sensor(
	const std::string & sensorPath,
	const std::vector<std::string> & rangePath,
	const std::vector<cv::Scalar> & colors,
	const int & subPixelNumber
	)
{
	// load and resize sensor image 
	SickCam::ImgT sensor;
	sensor.loadFromIconFile(sensorPath, 1.0);
	cv::Mat _senIm(sensor.get_rows(), sensor.get_cols(), CV_8UC1, sensor.getData(SickCam::DataNames::SEN));
	cv::Mat senIm = _senIm;

	cv::Mat senIm3;
	std::vector<cv::Mat> vSenImg(3, senIm);
	cv::merge(vSenImg, senIm3);
	cv::resize(senIm3, senIm3, cv::Size(), subPixelNumber, subPixelNumber, CV_INTER_NN);

	// 
	for (int ran_id=0; ran_id<rangePath.size(); ++ran_id)
	{
		SickCam::ImgT range_raw;
		range_raw.loadFromIconFile(rangePath[ran_id], 1.0);
		uint16_t* pBeg = (uint16_t*)range_raw.getData(SickCam::DataNames::RAN);
		int id_col_to_draw = range_raw.get_rows()/2;
		pBeg += (id_col_to_draw * range_raw.get_cols());
		std::vector<uint16_t> vRange_A_Line(pBeg, pBeg + range_raw.get_cols());

		// get value in sensor
		std::vector<int> id;
		for (auto sub : vRange_A_Line)
		{
			if (range_raw.getRange_info().m_RangeAxis == RA::STANDARD)
			{
				int y_in_sensor = sub + (range_raw.getRange_info().aoiOffsetY + 1) * 16;
				id.push_back(y_in_sensor);
			}
			else
			{
				int offsetY_reversed = 832 - range_raw.getRange_info().aoiOffsetY - range_raw.getRange_info().aoiHeight;
				int y_in_sensor = 832 * 16 - sub - offsetY_reversed * 16 + 16;
				id.push_back(y_in_sensor);
			}
		}
#pragma omp parallel for
		for (size_t i = subPixelNumber; i < senIm3.cols; i += subPixelNumber)
		{
			cv::line(senIm3, cv::Point(i - subPixelNumber + subPixelNumber/2, id[i / subPixelNumber - 1]), cv::Point(i + subPixelNumber / 2, id[i / subPixelNumber]), colors[ran_id]);
		}
	}

	return senIm3;
}


int main(int argc, char* argv[])
{
	cv::Mat result = draw_Range_To_Sensor(
		//"F:\\gitlab\\GlassSix\\New folder\\sensor",
		"F:\\gitlab\\GlassSix\\Region_sensor\\u10.1",
		//"H:\\TestData\\111\\sensor",
		{
			//"D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\Reverse",
			//"D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\stand",
			"F:\\gitlab\\GlassSix\\New folder\\3DrANGE",
			"F:\\gitlab\\GlassSix\\New folder\\3DrANGE",
			//"H:\\TestData\\111\\WarmSize_small\\RangeAxis_Reverse__SerachDirection_Reverse",
			//"H:\\TestData\\111\\WarmSize_small\\RangeAxis_Reverse__SerachDirection_Standard",
			//"H:\\TestData\\111\\WarmSize_small\\RangeAxis_Standard__SerachDirection_Reverse",
			//"H:\\TestData\\111\\WarmSize_small\\RangeAxis_Standard__SerachDirection_Standard",
			//"H:\\TestData\\111\\WarmSize_small\\RangeAxis_Standard__SerachDirection_Standard__Large",
			//"H:\\TestData\\111\\WarmSize_small\\RangeAxis_Standard__SerachDirection_Standard__Normal",
		},
		{ 
			//cv::Scalar(255,0,0), 
			cv::Scalar(0,0,255)
		},
		16);

	cv::imwrite("F:\\gitlab\\GlassSix\\New folder\\sensor.sick.jpg", result);

	return 0;
}


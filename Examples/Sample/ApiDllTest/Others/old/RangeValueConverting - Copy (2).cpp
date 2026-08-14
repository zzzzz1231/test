#include <Ranger3.h>		 // 此头文件封装了，API 的主体。 

#include <opencv2/opencv.hpp> // OpenCV 仅作为本示例程序中的显示之用，API 并不依赖该库。
#include <iostream>


////////////////////////////////////////////////////////////////////////////////

std::vector<int> getY_From_Range_To_Sensor(
	const std::vector<uint16_t> & vRange_A_Line, 
	const int & OffsetY_RegionSelector_Region1,
	const int & Height_RegionSelector_Region1,
	const bool rangeAxis_is_Standard)
{
	std::vector<int> res;
	for (auto sub : vRange_A_Line)
	{
		if (rangeAxis_is_Standard)
		{
			int y_in_sensor = cvRound(sub / 16 + OffsetY_RegionSelector_Region1);
			res.push_back(y_in_sensor);
		}
		else
		{
			int offsetY_reversed = 832 - OffsetY_RegionSelector_Region1 - Height_RegionSelector_Region1;
			int y_in_sensor = cvRound(832 - sub / 16 - offsetY_reversed);
			res.push_back(y_in_sensor);
		}
	}

	return res;
}

cv::Mat getY_From_Range_To_Sensor(
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
	cv::Mat senIm = _senIm;// .colRange(0, 500);

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
		int id_col_to_draw = 500;
		pBeg += (500 * range_raw.get_cols());
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

		for (size_t i = subPixelNumber; i < senIm3.cols; i += subPixelNumber)
		{
			cv::line(senIm3, cv::Point(i - subPixelNumber + subPixelNumber/2, id[i / subPixelNumber - 1]), cv::Point(i + subPixelNumber / 2, id[i / subPixelNumber]), colors[ran_id]);
		}
	}

	return senIm3;
}


cv::Mat getY_From_Range_To_Sensor(
	SickCam::ImgT &sensor,
	SickCam::ImgT &range_raw,
	const int & OffsetY_RegionSelector_Region1,
	const int & Height_RegionSelector_Region1,
	const bool  rangeAxis_is_Standard,
	const int & subPixelNumber
)
{
	// resize image
	cv::Mat _senIm(sensor.get_rows(), sensor.get_cols(), CV_8UC1, sensor.getData(SickCam::DataNames::SEN));
	cv::Mat senIm = _senIm;// .colRange(0, 500);

	cv::Mat senIm3;
	std::vector<cv::Mat> vSenImg(3, senIm);
	cv::merge(vSenImg, senIm3);
	cv::resize(senIm3, senIm3, cv::Size(), subPixelNumber, subPixelNumber, CV_INTER_NN);


	// 
	uint16_t* pBeg = (uint16_t*)range_raw.getData(SickCam::DataNames::RAN);
	std::vector<uint16_t> vRange_A_Line(pBeg, pBeg + 2560);

	// get value in sensor
	std::vector<int> id;
	for (auto sub : vRange_A_Line)
	{
		if (rangeAxis_is_Standard)
		{
			int y_in_sensor = cvRound(sub + OffsetY_RegionSelector_Region1 * 16);
			id.push_back(y_in_sensor);
		}
		else
		{
			int offsetY_reversed = 832 - OffsetY_RegionSelector_Region1 - Height_RegionSelector_Region1;
			int y_in_sensor = cvRound(832 * 16 - sub - offsetY_reversed * 16);
			id.push_back(y_in_sensor);
		}
	}

	for (size_t i = subPixelNumber; i < senIm3.cols; i += subPixelNumber)
	{
		cv::line(senIm3, cv::Point(i - subPixelNumber, id[i / subPixelNumber - 1]), cv::Point(i, id[i / subPixelNumber]), cv::Scalar(0, 0, 255));
	}

	return senIm3;
}

int main(int argc, char* argv[])
{
	cv::Mat result = getY_From_Range_To_Sensor(
		"D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\sensor", 
		{ "D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\Reverse", "D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\stand" },
		{cv::Scalar(255,0,0), cv::Scalar(0,0,255)},
		16);

	cv::imwrite("D:/sss.jpg", result);

	return 0;

#if 0	
	// ------ Convert
	SickCam::ImgT iconRange;
	//iconRange.loadFromIconFile("D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\Reverse", 1.0);
	iconRange.loadFromIconFile("D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\stand", 1.0);
	//uint16_t* pBeg = (uint16_t*)iconRange.getData(SickCam::DataNames::RAN);
	//std::vector<uint16_t> aLine(pBeg, pBeg + iconRange.getRange_info().aoiWidth);
	//std::vector<int> res = getY_From_Range_To_Sensor(aLine, iconRange.getRange_info().aoiOffsetX, iconRange.getRange_info().aoiHeight, iconRange.getRange_info().m_RangeAxis==RA::STANDARD);


	// ------ Show result
	SickCam::ImgT iconSensor;
	iconSensor.loadFromIconFile("D:\\SSZNvsSICK\\SICK\\09-LaserSearchDirection\\sensor", 1.0);
	/*cv::Mat senIm(iconSensor.get_rows(), iconSensor.get_cols(), CV_8UC1, iconSensor.getData(SickCam::DataNames::SEN));
	cv::Mat senIm3;
	std::vector<cv::Mat> vSenImg(3, senIm);
	cv::merge(vSenImg, senIm3);
	for (size_t i = 0; i < 2560; ++i)
	{
		//cv::circle(senIm3, cv::Point(i, res[i]), 1, cv::Scalar(0, 0, 255), -1);
		senIm3.at<cv::Vec3b>(res[i], i) = cv::Vec3b(0, 0, 255);
	}*/

	cv::Mat result = getY_From_Range_To_Sensor(iconSensor, iconRange, iconRange.getRange_info().aoiOffsetY, iconRange.getRange_info().aoiHeight, iconRange.getRange_info().m_RangeAxis == RA::STANDARD, 16);
	cv::imwrite("D:/stand.jpg", result);
	
/*
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
	*/
	return 0;
#endif
}


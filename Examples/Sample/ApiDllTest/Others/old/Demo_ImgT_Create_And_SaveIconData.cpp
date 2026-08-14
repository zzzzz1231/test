/**
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include <ImageTable.h>
#include <opencv2/opencv.hpp>

using SickCam::DataNames;

int main(int argc, char* argv[])
{
	SickCam::ImgT img;
	SickCam::RangeInfo rInfo;

	rInfo.cols;				/**< 3D 图像宽度。 Width of range image. */
	rInfo.rows;				/**< 3D 图像高度。 Height of range image.*/
	rInfo.aoiOffsetX;			/**< 在 sensor 上设置的 AOI 的左上 X 坐标，像素。[ICON | XML 参数]->["origin x"]。[相机参数]->[OffsetX_RegionSelector_Region1]。*/
	rInfo.aoiOffsetY;			/**< 在 sensor 上设置的 AOI 的左上 Y 坐标，像素。[ICON | XML 参数]->["origin z"]。[相机参数]->[OffsetY_RegionSelector_Region1]。*/
	rInfo.aoiHeight;			/**< 在 sensor 上设置的 AOI 的高度，像素。[ICON | XML 参数]->[("fov z2" + 1) / 16.0]。[相机参数]->[Height_RegionSelector_Region1]。*/
	rInfo.aoiWidth;			/**< 在 sensor 上设置的 AOI 的宽度，像素。[ICON | XML 参数]->["fov x1" - "fov x0" + 1]。[相机参数]->[Width_RegionSelector_Region1]。*/
	rInfo.m_RangeAxis;		/**< 3D 图像的 R 轴方向。激光竖直架设，R 就是 Z 轴。3D 图像的 R 轴对应于 sensor 上的 Y 轴。支持将 Y 轴取反。"STANDARD" 表示 sensor 上到下。*/

	// XML paras - sensorrangetraits
	rInfo.xml_fov_x0 = 0.0f ;							/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左上，X 坐标），像素，始终为0。*/
	rInfo.xml_fov_x1 = rInfo.aoiHeight - 1.0f;			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右上，X 坐标），像素，aoiHeight - 1。*/
	rInfo.xml_fov_x2 = 0.0f ;							/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左下，X 坐标），像素，始终为0。*/
	rInfo.xml_fov_x3 = rInfo.aoiWidth - 1.0f;			/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右下，X 坐标），像素，aoiWidth - 1。*/
	rInfo.xml_fov_z0 = 1.0f;							/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左上，Y 坐标），像素，始终为1。*/
	rInfo.xml_fov_z1 = rInfo.aoiHeight * 16.f - 1.0f;	/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右上，Y 坐标），像素，aoiHeight * 16 - 1 。*/
	rInfo.xml_fov_z2 = 1.0f;							/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（左下，Y 坐标），像素，始终为1。*/
	rInfo.xml_fov_z3 = rInfo.xml_fov_z1;				/**< [ICON | XML 参数] 在 sensor 上设置的 AOI 的矩形区参数（右下，Y 坐标），像素，aoiHeight * 16 - 1 。*/
	rInfo.xml_origin_x	= 0.0f;						/**< 在 sensor 上设置的 AOI 的左上 X 坐标，像素。[ICON | XML 参数]->["origin x"]。[相机参数]->[OffsetX_RegionSelector_Region1]。*/
	rInfo.xml_scale_x	= 1.0f;					/**< 提取激光线算法，在 sensor 图像的 X 坐标上的亚像素分辨率。始终为 1.0。*/
	rInfo.xml_origin_z	= 0.0f;					/**< 在 sensor 上设置的 AOI 的左上 Y 坐标，像素。[ICON | XML 参数]->["origin z"]。[相机参数]->[OffsetY_RegionSelector_Region1]。*/
	rInfo.xml_scale_z	= 0.0625f;

	img.setRangeInfo();

	img.SaveToIconFile();

	return 0;
}


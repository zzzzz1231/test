using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using SICK3D;
using System.Timers;
using System.Collections.Concurrent;
using System.IO;

namespace SampleApiDllCSharpConsole
{
	class Program
	{

		//static ColorPalette m_palette = null;

		static void BuiltGrayBitmap(byte[] rawValues, uint width, uint height, ref Bitmap bitmap)
		{
			if (bitmap == null || bitmap.Width != width || bitmap.Height != height)
			{
				bitmap = new Bitmap(Convert.ToInt32(width), Convert.ToInt32(height), PixelFormat.Format8bppIndexed);
			}


			BitmapData bmpData = bitmap.LockBits(
				new Rectangle(0, 0, Convert.ToInt32(width), Convert.ToInt32(height)),
				 ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);

			IntPtr ptr = bmpData.Scan0;
			int scanBytes = bmpData.Stride * bmpData.Height;

			Marshal.Copy(rawValues, 0, ptr, scanBytes);
			bitmap.UnlockBits(bmpData);

			ColorPalette m_palette = null;
			if (m_palette == null)
			{
				using (Bitmap bmp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
				{
					m_palette = bmp.Palette;
				}

				for (int i = 0; i < 256; ++i)
				{
					m_palette.Entries[i] = Color.FromArgb(i, i, i);
				}
			}

			bitmap.Palette = m_palette;
		}

		// 将 RGB 三个通道的数据合成为彩色 Bitmap
		static Bitmap CombineRGBToBitmap(byte[] RImageBytes, byte[] GImageBytes, byte[] BImageBytes,
									 int width, int height)
		{
			// 确保输入数据有效
			if (RImageBytes == null || GImageBytes == null || BImageBytes == null ||
				RImageBytes.Length != GImageBytes.Length ||
				GImageBytes.Length != BImageBytes.Length ||
				width <= 0 || height <= 0 
				)
			{
				throw new ArgumentException("输入参数无效，长度不匹配或尺寸错误");
			}

			// 创建 24 位彩色 Bitmap（每个像素占 3 字节，无 alpha 通道）
			Bitmap bitmap = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format24bppRgb);

			// 锁定位图数据以提高访问速度
			var bitmapData = bitmap.LockBits(
				new Rectangle(0, 0, width, height),
				System.Drawing.Imaging.ImageLockMode.WriteOnly,
				bitmap.PixelFormat);

			try
			{
				// 获取位图数据的指针
				IntPtr scan0 = bitmapData.Scan0;
				int stride = bitmapData.Stride; // 每行的字节数（可能大于 width*3，因内存对齐）

				// 分配缓冲区存储像素数据
				byte[] pixelData = new byte[stride * height];

				// 组合 RGB 通道数据（注意：Bitmap 中是 BGR 顺序）
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						int index = y * width + x; // 三个通道共用的索引
						int pixelOffset = y * stride + x * 3; // 像素在缓冲区中的位置

						// 注意：.NET Bitmap 中像素格式是 BGR 顺序，不是 RGB
						pixelData[pixelOffset] = BImageBytes[index];   // 蓝色通道
						pixelData[pixelOffset + 1] = GImageBytes[index]; // 绿色通道
						pixelData[pixelOffset + 2] = RImageBytes[index];  // 红色通道
					}
				}

				// 将像素数据复制到位图
				System.Runtime.InteropServices.Marshal.Copy(pixelData, 0, scan0, pixelData.Length);
			}
			finally
			{
				// 解锁位图
				bitmap.UnlockBits(bitmapData);
			}

			return bitmap;
		}

		static void Main(string[] args)
		{
			CSImgT img1 = new CSImgT();
			img1.loadFromIconFile("F:\\111\\test", 1.0, 1.0, false);
			img1.SaveRawImagesToIconFile("F:\\11\\test55", true, true);

			CSImgT img2 = new CSImgT();
			img2.loadFromIconFile("F:\\20250109\\222\\raw", 1.0, 1.0, false);

			CSImgT img3 = new CSImgT();
			img3.loadFromIconFile("F:\\20250109\\222\\raw", 1.0, 1.0, false);


			// --> 测试多次扫描不完整后，需要首尾拼接多个图像的情况
			CSImgT img_12 = img1.Append(img2);
			CSImgT img_123 = img_12.Append(img3);


			////////////////////////////////////////////////////////////////////////////


			// 应用高度图标定
			int calibration_handle_id = 0;

			// --> 可以多次调用
			var imgC = img_123.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, true, 1.2, 2560, 2560, 1);
			//var imgC = img.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, true, 1.2, 2560, 2560, 1);
			//var imgC = img.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, true, 1.2, 2560, 2560, 1);




			////////////////////////////////////////////////////////////////////////////


			// 应用彩色对齐标定。仅需要首次调用，初始化一些静态资源
			int color_handle_id = 8;

			// --> 彩色对齐
			CSImgT imgColorCal = img_123.ApplyCalibrationColor(
				color_handle_id, 
				"F:\\20250109\\222\\Calibration5120json.json",
				"F:\\20250109\\222\\2025-04-14-13-14-38_config.xml",
				200, // 假如工件需要 2 张或更多图才能扫描完整，为了对齐就扫 3 张，并且在这里剪除指定行数，即可获得完整的数据。
				//fcc强制启用
				//建议使用15.0/边界数据丢失严重的情况
				true, 3, 3.0, 5120, 5120, 1);

			//// --> 可以多次调用
			//img.ApplyCalibrationColor(color_handle_id, "F:\\20250109\\222\\Calibration5120json.json", "F:\\20250109\\222\\2025-04-14-13-14-38_config.xml", 0, true, 3, 1.2, 5120, 5120, 1);
			//img.ApplyCalibrationColor(color_handle_id, "F:\\20250109\\222\\Calibration5120json.json", "F:\\20250109\\222\\2025-04-14-13-14-38_config.xml", 0, true, 3, 1.2, 5120, 5120, 1);
			//img.ApplyCalibrationColor(color_handle_id, "F:\\20250109\\222\\Calibration5120json.json", "F:\\20250109\\222\\2025-04-14-13-14-38_config.xml", 0, true, 3, 1.2, 5120, 5120, 1);
			//img.ApplyCalibrationColor(color_handle_id, "F:\\20250109\\222\\Calibration5120json.json", "F:\\20250109\\222\\2025-04-14-13-14-38_config.xml", 0, true, 3, 1.2, 5120, 5120, 1);
			//img.ApplyCalibrationColor(color_handle_id, "F:\\20250109\\222\\Calibration5120json.json", "F:\\20250109\\222\\2025-04-14-13-14-38_config.xml", 0, true, 3, 1.2, 5120, 5120, 1);


			// 演示获得红色图像的方法
			var RangeComponent = imgColorCal.getDataFloat(DataNames.RAN_CAL);
			var RImageBytes = imgColorCal.getDataByte(DataNames.R);
			var GImageBytes = imgColorCal.getDataByte(DataNames.G);
			var BImageBytes = imgColorCal.getDataByte(DataNames.B);
			uint width = 5120;
			uint height = imgColorCal.get_rows();

			Bitmap bitmap = null;
			BuiltGrayBitmap(RImageBytes, width, height, ref bitmap);

			// --> 演示彩色图像
			Bitmap bitmap3 = CombineRGBToBitmap(RImageBytes, GImageBytes, BImageBytes, (int)width, (int)height);
			bitmap3.Save("F:\\20250109\\color.bmp");


			// --> 演示转化点云
			double xOffset = imgColorCal.getC_offsetX();
			double yOffset = imgColorCal.getC_offsetY();
			double xScale = imgColorCal.getC_scaleX();
			double yScale = imgColorCal.getC_scaleY();

			for (int j = 0; j < height; ++j)
			{
				double y = yOffset + j * yScale;
				for (int i = 0; i < width; ++i)
				{
					double x = xOffset + i * xScale;
					double z = RangeComponent[j * width + i];
					byte r = RImageBytes[j * width + i];
					byte g = GImageBytes[j * width + i];
					byte b = BImageBytes[j * width + i];

					// 点云 x y z r g b 
					Console.WriteLine($"{x},{y},{z},{r},{g},{b},");
				}

				if (j > 10) // 仅显示 10 行数据
					break;

			}





			// 仅需要最后一次调用，释放静态资源
			CSImgT.DeleteCalibration(calibration_handle_id);
			// 仅需要最后一次调用，释放静态资源
			CSImgT.DeleteCalibrationColor(color_handle_id);




		}

	}
}

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
using static System.Net.Mime.MediaTypeNames;

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

		static List<CSImgT> AlignColorImageArray(
			List<CSImgT> imageArray, int appendNumber/*必须大于1*/,
			string colorCalibrationJson,
			string colorConfigXML,
			int color_handle_id,
			bool  doFCC,
			int colorInterpolationStride,
			double rectificationSpread,
			int rawColoreResizedWidth,
			int rectificationWidth,
			int rectMethod,
			bool doCalibration
		)
		{
			List<CSImgT> alignedImages = new List<CSImgT>();
			if (appendNumber <= 1)
			{
				return alignedImages;
			}

			// ID = 1，比如 AppendNumber = 4，下面要合并的是 1 2 3 4 5 其中的 5 是重合冗余，后续去除。输出 1 2 3 4
			var image1 = new CSImgT(imageArray[0]);
			for (int i = 1; i <= appendNumber; ++i)
			{
				image1.Append(imageArray[i]);
			}

			int oneImageHeight = (int)(imageArray[0].get_rows());

			var imgCal = image1.ApplyCalibrationColor(
				color_handle_id,
				colorCalibrationJson,
				colorConfigXML,
				oneImageHeight, // 去除最后一张
				doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod, 0,
				doCalibration
			);
			alignedImages.Add(imgCal);


			// 下面要合并的是 4 5 6 7 8 9 其中的 4 9 是重合冗余，后续去除。输出 5 6 7 8
			for (int lp = appendNumber; lp < imageArray.Count(); lp += appendNumber)
			{
				image1 = imageArray[lp - 1];
				for (int i = lp; i <= lp + appendNumber && i < imageArray.Count(); ++i)
				{
					image1.Append(imageArray[i]);
				}

				imgCal = image1.ApplyCalibrationColor(
					color_handle_id,
					colorCalibrationJson,
					colorConfigXML,
					oneImageHeight, // 去除最后一张
					doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod,
					oneImageHeight, // 去除头部一张
					doCalibration
				);
				alignedImages.Add(imgCal);
			}

			// 处理最后一张
			image1 = imageArray[imageArray.Count() - 2];
			image1.Append(imageArray[imageArray.Count() - 1]);

			imgCal = image1.ApplyCalibrationColor(
				color_handle_id,
				colorCalibrationJson,
				colorConfigXML,
				0, // 
				doFCC, colorInterpolationStride, rectificationSpread, rawColoreResizedWidth, rectificationWidth, rectMethod,
				oneImageHeight, // 去除头部一张
				doCalibration
			);
			alignedImages.Add(imgCal);

			return alignedImages;
		}

		static void Main(string[] args)
		{

			// 读取图像
			List<CSImgT> imageArray = new List<CSImgT>();
			for (int i = 0; i < 20; ++i)
			{
				CSImgT img = new CSImgT();
				img.loadFromIconFile("F:\\111\\data\\img-" + i, 1.0, 1.0, false);
				imageArray.Add(img);
			}

			// 处理
			int calibration_handle_id = 0;
			bool doCalibration = true;
			var result = AlignColorImageArray(imageArray, 4,
				"F:\\111\\Calibration.json",
				"F:\\111\\2025-12-19-11-06-01_config.xml",
				calibration_handle_id, false, 3, 1.2, 5120, 5120, 0, doCalibration
			);

			// 分开存
			for (int i = 0; i < result.Count(); ++i)
			{
				result[i].SaveCalibratedImagesToIconFile("F:\\111\\result\\res-" + i, true, true);
			}

			// 存成一张
			CSImgT resultAll = result[0];
			for (int i = 1; i < result.Count(); ++i)
			{
				resultAll.Append(result[i]);
			}
			resultAll.SaveCalibratedImagesToIconFile("F:\\111\\result\\res-all3", true, false);


		}

	}
}

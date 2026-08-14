using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace SampleApiDllCSharpWinForm
{
	public partial class frmMain : Form
	{
		// 复用调色板对象，避免每次创建（大图像优化）
		private static ColorPalette s_grayPalette = null;
		private static readonly object s_paletteLock = new object();

		/// <summary>
		/// 获取灰度调色板（单例模式，复用）
		/// </summary>
		private ColorPalette GetGrayPalette()
		{
			if (s_grayPalette == null)
			{
				lock (s_paletteLock)
				{
					if (s_grayPalette == null)
					{
						using (Bitmap bmp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
						{
							s_grayPalette = bmp.Palette;
							for (int i = 0; i < 256; ++i)
							{
								s_grayPalette.Entries[i] = Color.FromArgb(i, i, i);
							}
						}
					}
				}
			}
			return s_grayPalette;
		}

		private void BuiltGrayBitmap(float[] rawValues, uint width, uint height, ref Bitmap bitmap)
		{
			// 优化：直接遍历原始数组，避免创建中间数组
			// 先找到有效值的最大值和最小值
			float vmax = float.MinValue;
			float vmin = float.MaxValue;
			bool hasValidValue = false;

			for (int i = 0; i < rawValues.Length; i++)
			{
				if (!float.IsNaN(rawValues[i]))
				{
					if (rawValues[i] > vmax) vmax = rawValues[i];
					if (rawValues[i] < vmin) vmin = rawValues[i];
					hasValidValue = true;
				}
			}

			if (!hasValidValue)
			{
				return;
			}

			float vRange = vmax - vmin;

			// 避免除以零的错误
			if (vRange == 0)
			{
				vRange = 1.0f;
			}

			// 创建字节数组
			byte[] arrayByte = new byte[rawValues.Length];

			// 并行计算灰度值
			Parallel.For(0, rawValues.Length, i =>
			{
				if (!float.IsNaN(rawValues[i]))
				{
					arrayByte[i] = (byte)((rawValues[i] - vmin) / vRange * 255);
				}
			});

			BuiltGrayBitmap(arrayByte, width, height, ref bitmap);
		}



		private void BuiltGrayBitmap(ushort[] rawValues, uint width, uint height, ref Bitmap bitmap)
		{
			// 直接在 rawValues 上找到最大值和最小值
			ushort vmax = rawValues.Max();
			ushort vmin = rawValues.Min();
			ushort vRange = (ushort)(vmax - vmin);

			if (vRange == 0)
				vRange = 1; // 避免除以零的错误

			byte[] arrayByte = new byte[rawValues.Length];
			
			// 大图像优化：使用并行处理提升性能（2560*5000 约 1280 万像素）
			Parallel.For(0, rawValues.Length, i =>
			{
				// 使用位运算直接计算字节值
				arrayByte[i] = (byte)((rawValues[i] - vmin) * 255 / vRange);
			});

			BuiltGrayBitmap(arrayByte, width, height, ref bitmap);
		}


		//public ColorPalette m_palette = null;

		private void BuiltGrayBitmap(byte[] rawValues, uint width, uint height, ref Bitmap bitmap)
		{
			//if (bitmap == null || bitmap.Width != width || bitmap.Height != height)
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
			
			// 使用复用的调色板，避免每次创建
			bitmap.Palette = GetGrayPalette();
		}



		// 将 RGB 三个通道的数据合成为彩色 Bitmap
		private Bitmap CombineRGBToBitmap(byte[] RImageBytes, byte[] GImageBytes, byte[] BImageBytes,
									 int width, int height)
		{
			// 确保输入数据有效
			if (RImageBytes == null || GImageBytes == null || BImageBytes == null ||
				RImageBytes.Length != GImageBytes.Length ||
				GImageBytes.Length != BImageBytes.Length ||
				width <= 0 || height <= 0)
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
				// 大图像优化：使用并行处理提升性能（2560*5000 约 1280 万像素）
				Parallel.For(0, height, y =>
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
				});

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

	}

}



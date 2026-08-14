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
		private void BuiltGrayBitmap(float[] rawValues, uint width, uint height, ref Bitmap bitmap)
		{
			// 过滤掉 NaN 值
			float[] filteredValues = rawValues.Where(v => !float.IsNaN(v)).ToArray();

			if (filteredValues.Length == 0)
			{
				return;
			}

			// 计算最大值和最小值
			float vmax = filteredValues.Max();
			float vmin = filteredValues.Min();
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
			for (int i = 0; i < rawValues.Length; i++)
			{
				// 使用位运算直接计算字节值
				arrayByte[i] = (byte)((rawValues[i] - vmin) * 255 / vRange);
			}

			BuiltGrayBitmap(arrayByte, width, height, ref bitmap);
		}


		public ColorPalette m_palette = null;

		private void BuiltGrayBitmap(byte[] rawValues, uint width, uint height, ref Bitmap bitmap)
		{
			if (bitmap == null || bitmap.Width!= width || bitmap.Height != height)
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


	}

}



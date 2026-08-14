using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Drawing.Drawing2D;

namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {

        private static Bitmap BuiltGrayBitmap(float[] rawValues, uint width, uint height)
        {
            var l = new List<float>(rawValues);
            l.RemoveAll(data => ( float.IsNaN(data)));
			l.RemoveAll(data => ( data == -1.0e6f ));
            var vmax = l.Max<float>();
            var vmin = l.Min<float>();
            var vRange = vmax - vmin;

            if (0 == vRange)
                vRange = 1.0f; // Avoid error "devide by zero".

            byte[] arrayByte = new byte[rawValues.Length];
            for (var i = 0; i < rawValues.Length; ++i)
               if(!float.IsNaN(rawValues[i]) && rawValues[i]!= -1.0e6f)
                    arrayByte[i] = Convert.ToByte((rawValues[i] - vmin) / vRange * 255);

            return BuiltGrayBitmap(arrayByte, width, height);
        }
		
		private static Bitmap BuiltGrayBitmap_OLD(float[] rawValues, uint width, uint height)
        {
            var l = new List<float>(rawValues);
            l.RemoveAll(data => ( float.IsNaN(data)));
            var vmax = l.Max<float>();
            var vmin = l.Min<float>();
            var vRange = vmax - vmin;

            if (0 == vRange)
                vRange = 1.0f; // Avoid error "devide by zero".

            byte[] arrayByte = new byte[rawValues.Length];
            for (var i = 0; i < rawValues.Length; ++i)
               if(!float.IsNaN(rawValues[i]))
                    arrayByte[i] = Convert.ToByte((rawValues[i] - vmin) / vRange * 255);

            return BuiltGrayBitmap(arrayByte, width, height);
        }

        private static Bitmap BuiltGrayBitmap(ushort[] rawValues, uint width, uint height)
        {
            var l = new List<ushort>(rawValues);
            var vmax = l.Max();
            var vmin = l.Min();
            var vRange = vmax - vmin;

            if (0 == vRange)
                vRange = 1; // Avoid error "devide by zero".

            byte[] arrayByte = new byte[rawValues.Length];
            for (var i=0; i< rawValues.Length; ++i)
                arrayByte[i] = Convert.ToByte((double)(rawValues[i] - vmin) / vRange * 255);

            return BuiltGrayBitmap(arrayByte, width, height);
        }

        // from: https://www.cnblogs.com/lonelyxmas/p/7727303.html
        private static Bitmap BuiltGrayBitmap(byte[] rawValues, uint width, uint height)
        {
            Bitmap bitmap = new Bitmap(Convert.ToInt32(width), Convert.ToInt32(height), PixelFormat.Format8bppIndexed);
            BitmapData bmpData = bitmap.LockBits(new Rectangle(0, 0, Convert.ToInt32(width), Convert.ToInt32(height)),
                 ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);

            int offset = bmpData.Stride - bmpData.Width;        
            IntPtr ptr = bmpData.Scan0;                         
            int scanBytes = bmpData.Stride * bmpData.Height;    

            Marshal.Copy(rawValues, 0, ptr, scanBytes);
            bitmap.UnlockBits(bmpData); 

            ColorPalette palette;
            using (Bitmap bmp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
            {
                palette = bmp.Palette;
            }

            for (int i = 0; i < 256; ++i)
            {
                palette.Entries[i] = Color.FromArgb(i, i, i);
            }
            bitmap.Palette = palette;

            return bitmap;
        }

        /// <summary>
        /// Resize the image to the specified width and height.
        /// </summary>
        /// <param name="image">The image to resize.</param>
        /// <param name="width">The width to resize to.</param>
        /// <param name="height">The height to resize to.</param>
        /// <returns>The resized image.</returns>
        public static Bitmap ResizeImage(Image image, int width, int height)
        {
            var destRect = new Rectangle(0, 0, width, height);
            var destImage = new Bitmap(width, height);

            destImage.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            using (var graphics = Graphics.FromImage(destImage))
            {
                graphics.CompositingMode = CompositingMode.SourceCopy;
                graphics.CompositingQuality = CompositingQuality.HighQuality;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;

                using (var wrapMode = new ImageAttributes())
                {
                    wrapMode.SetWrapMode(WrapMode.TileFlipXY);
                    graphics.DrawImage(image, destRect, 0, 0, image.Width, image.Height, GraphicsUnit.Pixel, wrapMode);
                }
            }

            return destImage;
        }


    }

}



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

		static void Main(string[] args)
		{
			{

				// 读取图像
				//List<CSImgT> imageArray = new List<CSImgT>();
				for (int i = 0; i < 2000; ++i)
				{
					CSImgT img = new CSImgT();
					img.loadFromIconFile("F:\\111\\data\\img-0", 1.0, 1.0, false);
					//imageArray.Add(img);
					CSImgT data = new CSImgT();
					data.Append(img);
					data.Append(img);
					data.Append(img);
					data.Append(img);
					data.Append(img);
					
					img.Dispose();
					data.Dispose();
					//GC.Collect();
					Console.WriteLine($"img-{i}");
				}

			}


		}

	}
}

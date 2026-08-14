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

namespace SampleApiDllCSharpConsole
{
	/// <summary>
	/// 示例程序：图像采集类
	/// 这里定义了两种图像采集方式，1 轮询 + 自定义图像处理线程；2 轮询 + callback
	/// V 1.3
	/// </summary>
	class ImgGrabber
	{
		public ImgGrabber() { }

		/// <summary>
		/// 图像缓存。直接从这里拿图像即可。 CSImgT imgTable = ImageBuffer.Dequeue();
		/// ConcurrentQueue 线程安全。
		/// Queue 非线程安全，单相机，测试 22K 行频，图像1000行，可行。
		/// 
		/// **设计提示：一般认为，在高并发环境下 ConcurrentQueue 的性能优于 Queue+lock。
		///   但是ConcurrentQueue内部是链表，Queue 的内部是数组，所以前者内存开销更大。
		/// </summary>
		public ConcurrentQueue<CSImgT> ImageBuffer = new ConcurrentQueue<CSImgT>();
		//public Queue<CSImgT> ImageBuffer = new Queue<CSImgT>();


		public void ClearImageBuffer()
		{
			_ = new CSImgT();
			while (ImageBuffer.LongCount<CSImgT>() > 0)
			{
				ImageBuffer.TryDequeue(out _);
			}
			//ImageBuffer.Clear();
		}


		/// <summary>
		/// 启动线程，后台运行，不阻塞
		/// </summary>
		/// <param name="r3"></param>
		/// <returns></returns>
		public bool ThreadStart_Background(Ranger3 r3)
		{
			m_IsRequstStop = false;
			//LoopStartCount = 0;
			//LoopEndCount = 0;


			m_GrabbingThread = new Thread(() => onGrabImage_Loop(r3))
			{
				IsBackground = true // 设置为后台线程
			};
			m_GrabbingThread.Start();

			return true;
		}

		/// <summary>
		/// 会抛出异常，一定要catch并处理。
		/// </summary>
		/// <returns></returns>
		/// <exception cref="Exception"></exception>
		public bool ThreadStop()
		{
			if (m_IsStop)
				return true;

			// send requirement
			if (m_IsRequstStop == false)
				m_IsRequstStop = true;

			// wait, timeout

			int stop_timeout = 200;
			while (m_IsRequstStop)
			{
				Thread.Sleep(5);
				if (--stop_timeout < 0)
				{
					throw new Exception("经过等待，仍然无法停止图像采集线程！");
					//break;
				}
			};

			return true;
		}
		public bool Thread_IsStop()
		{
			return m_IsStop;
		}


		/// <summary>
		/// 图像回调事件的函数签名
		/// </summary>
		/// <param name="imgT"></param>
		public delegate void ImageCallbackEvent(CSImgT imgT);

		/// <summary>
		/// 图像回调事件的接口 += 
		/// </summary>
		public event ImageCallbackEvent OnImageCallback;

		/////////////////////////////////////////////////////////////////////////////////////
		private Thread m_GrabbingThread = null;

		private bool m_IsRequstStop = false;
		private bool m_IsStop = true;


		//public int LoopStartCount = 0;
		//public int LoopEndCount = 0;


		private void onGrabImage_Loop(Ranger3 r3)
		{
			Console.WriteLine(DateTime.Now + " ：Grabber start!");
			//LoopStartCount++;
			m_IsStop = false;
			while (true)
			{
				// 响应停止请求
				if (m_IsRequstStop)
				{
					break;
				}

				if (r3.getStatus() == CAM_STATUS.CAM_IS_STARTED)
				{
					CSImgT imgT = new CSImgT();
					if (r3.getImageData(imgT) == CAM_STATUS.All_OK)
					{
						// 选择处理方案

						if (OnImageCallback != null)
						{
							// 用户指定了回调事件，调用之，并且直接返回。不阻塞当前轮询。用户须保证回调事件不会抛出异常。
							Task.Run(() => OnImageCallback(imgT));
							//Console.WriteLine(DateTime.Now + " c：" + imgT.get_ID().ToString() + " : count=" + (m_ImageCounter).ToString() + " : ImageBuffer=" + ImageBuffer.LongCount().ToString());
						}
						else
						{
							// 入队，用户自行从 ImageBuffer 中取数据
							ImageBuffer.Enqueue(new CSImgT(imgT));
							imgT.Dispose();
							GC.Collect();
							GC.WaitForPendingFinalizers();
							//Console.WriteLine(DateTime.Now + " a：" + imgT.get_ID().ToString() + " : count=" + (m_ImageCounter).ToString() + " : ImageBuffer=" + ImageBuffer.LongCount().ToString());
						}
					}
				}

				// 响应停止请求
				if (m_IsRequstStop)
				{
					break;
				}
			}

			//LoopEndCount++;
			m_IsRequstStop = false;
			m_IsStop = true;
			Console.WriteLine(DateTime.Now + " ：Grabber stop!");
		}


	} // class ImgGrabber


	/////////////////////////////////////////////////////////////////////////////////////
	class Program
	{
		/// <summary>
		/// 统计收到的图像张数，用于和 FrameID 比对判断有无丢帧。
		/// </summary>
		static int ImageCounter = 0;


		/// <summary>
		/// 示例：使用自定义线程处理图像。注意，当处理完成后，需要使用gc回收内存。
		/// </summary>
		/// <param name="imgGrabber">要不要都行，函数自定义，参数自定义</param>
		static void MyImageProcessLoop(ref ImgGrabber imgGrabber)
		{
			Console.WriteLine(DateTime.Now + " ：ImageProcessor start!");
			while (true)
			{
				CSImgT imgT;
				if(false == imgGrabber.ImageBuffer.TryDequeue(out imgT))
				{
					continue;
				}

				if (imgGrabber.Thread_IsStop())
				{
					Console.WriteLine(DateTime.Now + " ：ImageProcessor will stop now!");
					imgGrabber.ClearImageBuffer();
					break;
				}


				Console.WriteLine(
				DateTime.Now + " b：" + imgT.get_ID().ToString()
				+ " : count=" + (ImageCounter).ToString()
				+ " : ImageBuffer=" + imgGrabber.ImageBuffer.Count().ToString());

				// 内存回收
				imgT.Dispose();
				GC.Collect();
				GC.WaitForPendingFinalizers();

			}
			Console.WriteLine(DateTime.Now + " ：ImageProcessor stop!");
		}



		/// <summary>
		/// 示例：使用 callback 
		/// </summary>
		/// <param name="imgT"></param>
		static void MyImageCallback(CSImgT imgT)
		{
			Console.WriteLine(DateTime.Now + " ：" + imgT.get_ID().ToString() + " : count=" + (++ImageCounter).ToString());
			// 内存回收
			imgT.Dispose();
			GC.Collect();
			GC.WaitForPendingFinalizers();

		}

		static void Main(string[] args)
		{
			string logPath = "D:\\log.txt";
			bool isPrintLog = true;
			bool isWriteLog = false;
			GenICam3D r3s = new GenICam3D(logPath, SICK3D.GenICam3D.getPathToRanger3Producer(), isPrintLog, isWriteLog);

			string[] IPs = r3s.getConDevListIP();
			var scanRes = r3s.scanDevice();
			if (IPs.Length == 0)
			{
				Console.WriteLine("找不到相机");
				return;
			}

			// --- test CSR3. (Please note: "CS" = C_Sharp, "R3" = Ranger3.)
			Ranger3 r3 = new Ranger3(IPs[0], true);
			ImgGrabber imgGrabber = new ImgGrabber();

			// 设置图像的 callback 函数
			imgGrabber.OnImageCallback += MyImageCallback;

			if (r3.isReady() != CAM_STATUS.All_OK)
			{
				Console.WriteLine("相机不可用");
				return;
			}

			var camStatus = r3.connectCamera();
			if (camStatus != CAM_STATUS.All_OK)
			{
				Console.WriteLine("相机连接失败");
				return;
			}


			// --- test CSR3 setting paramters info
			//r3.importConfiguretionFile("path-to_csv-file"); // 加载参数文件

			r3.setParameterValue("DeviceScanType", "Linescan3D");    /// 3D Range image
			//r3.setParameterValue("DeviceScanType", "Areascan");    /// 2D Sensor image
			
			r3.setCalibration_No();
			//r3.setCalibration_inDevice();
			
			r3.setBufferCount(20); // 设置 C++ 底层的图像缓存




			camStatus = r3.startCamera();
			if (camStatus != CAM_STATUS.All_OK)
			{
				Console.WriteLine("相机开始失败");
				return;
			}

			imgGrabber.ThreadStart_Background(r3);

			// 开始执行自定义的图像处理函数 
			//Task.Run(() => MyImageProcessLoop(ref imgGrabber));


			// 阻塞主线程，等待按键输入结束程序
			Console.ReadKey();

			// 结束处理，准备停止采集，停止相机
			try
			{
				if (false == imgGrabber.ThreadStop())
					return;
			}
			catch
			{
				Console.WriteLine("停止采集图像失败");
				return;
			}


			// --- Memory using check: around X Mb after stopping.
			camStatus = r3.stopCamera();
			if (camStatus != CAM_STATUS.All_OK)
			{
				Console.WriteLine("相机停止失败");
				return;
			}

			camStatus = r3.disconnectCamera();
			if (camStatus != CAM_STATUS.All_OK)
			{
				Console.WriteLine("相机断开失败");
				return;
			}

			// --- delete variable.
			r3s.Dispose();
		}

	}
}

//#define CAL_IN_PC

using System;
using System.Windows.Forms;
using System.Threading;
using SICK3D;
using System.Collections.Concurrent;
using System.Linq;
using System.Threading.Tasks;

namespace SampleApiDllCSharpWinForm
{


	public class HeartThread
	{
		private Thread m_thread;
		private bool m_stopRequested = false;
		private bool m_stoped = true;
		private int m_loopSleep = 10;
        string m_ip;

		public delegate void MainTaskDelegate(string ip); // 定义主任务委托
		MainTaskDelegate m_task;

		public HeartThread(MainTaskDelegate mainTask, string ip, int loopSleepMS)
		{
            m_task = mainTask;
			m_loopSleep = loopSleepMS;
			m_ip = ip;
		}

		public void HeartBeatON()
		{
            if (m_stoped)
            {
			    m_stopRequested = false;
				m_thread = new Thread(() => Run(m_task));
				m_thread.Name = "WorkThread";
				m_thread.IsBackground = true; // 将线程设置为后台线程
				m_thread.Start();
			}

		}

		public void RequestHeartBeatOFF()
		{
			m_stopRequested = true;
		}

		public void WaitHeartBeatOFF(int sleepMS)
		{
			while (!m_stoped)
            {
                Thread.Sleep(sleepMS);
			}
		}

		private void Run(MainTaskDelegate mainTask)
		{
            m_stoped = false;
			while (!m_stopRequested)
			{
				// 执行主任务
				mainTask?.Invoke(m_ip);

				Thread.Sleep(m_loopSleep);
			}
			m_stoped = true;
		}
	}


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




	class LoopThread
	{
		public LoopThread() { }

		/// <summary>
		/// 启动线程，后台运行，不阻塞
		/// </summary>
		/// <param name="r3"></param>
		/// <returns></returns>
		public bool Start_Background()
		{
			if (!m_IsStop)
				return true;

			m_IsRequstStop = false;
			//LoopStartCount = 0;
			//LoopEndCount = 0;

			m_Thread = new Thread(() => onLoop())
			{
				IsBackground = true // 设置为后台线程
			};
			m_Thread.Start();

			return true;
		}

		/// <summary>
		/// 会抛出异常，一定要catch并处理。
		/// </summary>
		/// <returns></returns>
		/// <exception cref="Exception"></exception>
		public bool Stop()
		{
			if(m_IsStop) 
				return true;

			// send requirement
			if(m_IsRequstStop == false)
				m_IsRequstStop = true;

			// wait, timeout
			int stop_timeout = 200;

			while (m_IsRequstStop)
			{
				//int cc = LoopStartCount;
				Thread.Sleep(5);
				if (--stop_timeout < 0)
				{
					//throw new Exception($"经过等待，仍然无法停止Loop线程！停止状态 {m_IsRequstStop}。循环计数: 停止发出时 {cc}, 停止超时时 {LoopStartCount}");
					throw new Exception($"经过等待，仍然无法停止Loop线程！停止状态 {m_IsRequstStop}。");
					//break;
				}
			};


			//stop_timeout = 200;
			//while (LoopStartCount != LoopEndCount)
			//{
			//	Thread.Sleep(5);
			//	if (--stop_timeout < 0)
			//	{
			//		throw new Exception($"LoopThread: LoopStartCount {LoopStartCount} != LoopEndCount {LoopEndCount}");
			//	}
			//};

			return true;
		}
		public bool IsStop()
		{
			return m_IsStop;
		}

		/// <summary>
		/// 需要循环执行的操作委托定义
		/// </summary>
		/// <param name="imgT"></param>
		public delegate void DoSomethingInLoop();

		/// <summary>
		/// 需要循环执行的操作
		/// </summary>
		public event DoSomethingInLoop ToDoSomethingInLoop;

		/// <summary>
		/// 线程的间隔，毫秒
		/// </summary>
		public int interval = 10;
		//public int LoopStartCount = 0;
		//public int LoopEndCount = 0;

		/////////////////////////////////////////////////////////////////////////////////////
		private Thread m_Thread = null;

		/// <summary>
		/// 0_req, 1_init/finish/break, 2_running 
		/// </summary>
		private bool m_IsRequstStop = false;
		private bool m_IsStop = true;
		private void onLoop()
		{
			//LoopStartCount++;
			m_IsStop = false;
			while (true)
			{
				// 响应停止请求
				if (m_IsRequstStop)
				{
					break;
				}
				
				ToDoSomethingInLoop();

				// 响应停止请求
				if (m_IsRequstStop)
				{
					break;
				}
				Thread.Sleep(interval);
			}
			//LoopEndCount++;
			m_IsRequstStop = false;
			m_IsStop = true;
		}
	} // class ImgGrabber


	public partial class frmMain : Form
    {
        private Ranger3 R3(String ip)
        {
            if (m_list_r3.ContainsKey(ip))
                return m_list_r3[ip];

            return new Ranger3("", true); // Invalid Ranger3 object. Please note, do not return null
        }

        private void FindCamera()
        {
            CAM_STATUS scanRes = CAM_STATUS.DEFAULT;
            if (m_r3s.isCtiFound())
            {
                scanRes = m_r3s.scanDevice();
            }

            if (scanRes == CAM_STATUS.All_OK)
            {
                TreeNode nodeRoot = new TreeNode("Ranger3");
                treeView1.Nodes.Add(nodeRoot);
                string[] IPs = m_r3s.getConDevListIP();
                for (int i = 0; i < IPs.Length; i++)
                {
                    TreeNode nodeChild = new TreeNode(IPs[i]);
                    nodeRoot.Nodes.Add(nodeChild);

                    if (!m_list_r3.ContainsKey(IPs[i]))
                    {
                        m_list_r3.Add(IPs[i], new Ranger3(IPs[i], true));
                        m_list_r3_HT.Add(IPs[i], new HeartThread(onThreadHeartBeat, IPs[i], 1000));
						m_list_r3_HTC.Add(IPs[i], new int());

						// init heartbeat parameters
                        m_list_r3_HTC[IPs[i]] = 0;
					}

					wFlog(IPs[i] + "，已加载。");
				}

				m_ip = (IPs.Length > 0) ? IPs[0] : "" ;
				m_ip1 = m_ip;

				if (IPs.Length >= 2)
                {
                    m_ip2 = IPs[1];
                }
            }
        }

        private void Connection()
        {
            var status = _Connection();
            if (status != CAM_STATUS.All_OK)
            {
                MessageBox.Show(status.ToString());
                return;
            }

            if (false == R3(m_ip).updateParamtersInfo())
            {
                MessageBox.Show("更新相机参数错误！！\n\n\nUpdate paramters failed!!");
                Disconnection();
                return;
            }
            // expand propertyGrids 
            updateParamPropertyGrid();

        }

        private void Disconnection()
        {
            var status = _Disconnection(); if (status != CAM_STATUS.All_OK) MessageBox.Show(status.ToString());
        }

        private void Start()
        {
            var status = _Start(); if (status != CAM_STATUS.All_OK) MessageBox.Show(status.ToString());
        }

        private void Stop()
        {
            var status = _Stop(); if (status != CAM_STATUS.All_OK) MessageBox.Show(status.ToString());
        }

        private CAM_STATUS _Connection()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            status = R3(m_ip).connectCamera(false);
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);
            if (status != CAM_STATUS.All_OK) return status;

            m_list_r3_HT[m_ip].HeartBeatON();
			m_list_r3_HTC[m_ip] = 0;

			// some gui setup
			var scanType = R3(m_ip).getParameterValue("DeviceScanType");
            toolStripComboBoxMode.SelectedIndex = scanType == "Areascan" ? (int)DataNames.SEN : (int)DataNames.RAN;

            textBoxSleepTime.Enabled = true;
            textBoxSleepTime.Text = R3(m_ip).getGrabSleepTime().ToString();

			R3(m_ip).setCalibration_No();

			return status;
        }

        private CAM_STATUS _Disconnection()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED)
            {
                MessageBox.Show("Please Stop Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_STARTED;
            }

			m_list_r3_HT[m_ip].RequestHeartBeatOFF();
			m_list_r3_HT[m_ip].WaitHeartBeatOFF(10);

			m_list_r3_HTC[m_ip] = 0;

			status = R3(m_ip).disconnectCamera();
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);

            textBoxSleepTime.Enabled = false;

            return status;
       }

        private CAM_STATUS _Start()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                MessageBox.Show("Please Connect Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_DISCONNECTED;
            }

            textBoxSleepTime.Text = R3(m_ip).getGrabSleepTime().ToString();
            status = R3(m_ip).startCamera();

			if (m_ip == m_ip1)
            {
                ImageCounter1 = 0;
				imgGrabber1.ClearImageBuffer();
				imgGrabber1.ThreadStart_Background(R3(m_ip));
				wFlog(m_ip + ", Started.");
			}
			else if (m_ip == m_ip2)
            {
                ImageCounter2 = 0;
				imgGrabber2.ClearImageBuffer();
				imgGrabber2.ThreadStart_Background(R3(m_ip));
				wFlog(m_ip + ", Started.");
			}
			else
			{
				wFlog(m_ip + ", 这个IP不匹配。");
			}

			m_UI_Thread.Start_Background();
			

			btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);
            return status;
        }

        private CAM_STATUS _Stop()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            if (m_ip == m_ip1)
            {
                try
                {
                    if (false == imgGrabber1.ThreadStop())
                    {
                        return CAM_STATUS.ERROR_CAM_NOT_STOP;
                    }
                }
                catch
                {
                    Console.WriteLine("停止采集图像失败");
                    return CAM_STATUS.ERROR_CAM_NOT_STOP;
                }

				status = R3(m_ip).stopCamera();
				//IsUINeedUpdating1 = false;
				imgGrabber1.ClearImageBuffer();
			}
			else if (m_ip == m_ip2)
            {
				try
				{
					if (false == imgGrabber2.ThreadStop())
					{
						return CAM_STATUS.ERROR_CAM_NOT_STOP;
					}
				}
				catch
				{
					Console.WriteLine("停止采集图像失败");
					return CAM_STATUS.ERROR_CAM_NOT_STOP;
				}

				status = R3(m_ip).stopCamera();
				IsUINeedUpdating2 = false;
				imgGrabber2.ClearImageBuffer();
            }
            else
            {
                wFlog(m_ip + ", 这个IP不匹配。");
				return CAM_STATUS.UNKNOWN;
			}

			btnCameraStatus(CAM_STATUS.CAM_IS_STOPPED);
			wFlog(m_ip + ", Stoped.");

			//if(imgGrabber1.Thread_IsStop() && imgGrabber2.Thread_IsStop())
			//	m_UI_Thread.Stop();

			return status;
		}

        private CAM_STATUS ImportConfigrationFile()
        {
            string strConfigPath = string.Empty;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Please select a configuration file / 请选择相机配置文件";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "csv|*.csv";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return CAM_STATUS.ERROR_CSV_PATH;

            var res = R3(m_ip).setParametPath(strConfigPath);
			// expand propertyGrids 
			updateParamPropertyGrid();
            return res;
		}

        private CAM_STATUS ExportConfigrationFile()
        {
            string strConfigPath = string.Empty;
            SaveFileDialog ofd = new SaveFileDialog();
            ofd.Title = "Please select a path to save configuration file / 请选择要保存相机配置文件的路径";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "csv|*.csv";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return CAM_STATUS.ERROR_CSV_PATH;

            return R3(m_ip).exportConfiguretionFile(strConfigPath);
        }

        private CAM_STATUS ImportCalibrationFile()
        {
#if CAL_IN_PC
            string strConfigPath = string.Empty;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Please select a calibration file / 请选择标定文件";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "Calibration|*.xml;*.json";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return CAM_STATUS.ERROR_CALIBRATION_PATH;

            return R3(m_ip).setCalibration_inPC(strConfigPath, true, 2560, 4); // 假如认为标定太慢，可以增加 threads_number，不过也会增加CPU的使用率
#else
            return CAM_STATUS.All_OK;
#endif
        }

        private bool SaveImage()
        {
            _Stop();

            string strConfigPath = string.Empty;
            SaveFileDialog ofd = new SaveFileDialog();
            ofd.Title = "Please select a path to save image / 请选择保存路径";
            ofd.InitialDirectory = Application.StartupPath;
            //ofd.Filter = "*.*";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return false;
            if (m_lastImg.has(SICK3D.DataNames.SEN))
            {
                MessageBox.Show("保存图像 - sensor");
                return m_lastImg.SaveSensorImageToIconFile(strConfigPath, false);
            }
            else if (m_lastImg.has(SICK3D.DataNames.RAN_CAL) || m_lastImg.has(SICK3D.DataNames.RAN_CAL_16) 
                    || m_lastImg.has(SICK3D.DataNames.RAN_X) || m_lastImg.has(SICK3D.DataNames.RAN_X_16) )
            {
                MessageBox.Show("保存图像 - calibrated range");
                return m_lastImg.SaveCalibratedImagesToIconFile(strConfigPath, true, false);
            }
            else if (m_lastImg.has(SICK3D.DataNames.RAN))
            {
                MessageBox.Show("保存图像 - raw range");
                return m_lastImg.SaveRawImagesToIconFile(strConfigPath, true, false);
            }
            else
            {
                MessageBox.Show("保存图像异常！没有检测到图像。");
                return false;
            }
        }

        private CSImgT LoadImage()
        {
            // This is just a demo
            MessageBox.Show("这里仅以代码形式演示如何加载 ICON(dat|xml) 类型的图像数据。");

            string strConfigPath = string.Empty;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Please select the path / 请选择加载路径";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "xml|*.xml";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return new CSImgT();

            CSImgT img = new CSImgT();
            if (false == img.loadFromIconFile(strConfigPath, 1.0, 1.0, true))
                MessageBox.Show("加载失败！！ \n\n\nLoad Image failed!!");

            return img;
        }

        private DataNames GetGrabMode()
        {
            return m_GrabMode;
        }

        private CAM_STATUS _checkR3()
        {
            if(!m_r3s.isAvaliable())
                return CAM_STATUS.ERROR_NULL_DEV_HANDLE;

            if (m_ip == "" || R3(m_ip) == null)
            {
                MessageBox.Show("Please select a camera first!!");
                return CAM_STATUS.ERROR_NULL_PTR_DEV;
            };

            if (R3(m_ip).isReady() != CAM_STATUS.All_OK)
            {
                MessageBox.Show("Camera is not ready!!");
                return CAM_STATUS.ERROR_NULL_PTR_DEV;
            }
            return CAM_STATUS.All_OK;
        }

        private CAM_STATUS _checkR3_is_connected_or_stopped()
        {
            if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                MessageBox.Show("Please Connect Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_DISCONNECTED;
            }

            if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED)
            {
                MessageBox.Show("Please Stop Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_STARTED;
            }

            return CAM_STATUS.All_OK;
        }


        private CAM_STATUS _sendFileToCamera()
        {
            var status = _checkR3();                        if (status != CAM_STATUS.All_OK) return status;
            status = _checkR3_is_connected_or_stopped();    if (status != CAM_STATUS.All_OK) return status;

            //
            string strPath = string.Empty;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Please select the path / 请选择文件路径";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "(*.*)|*.*";
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                strPath = ofd.FileName;
            }
          
            status = R3(m_ip).sendFileToCamera(strPath);
            return status;
        }

        private CAM_STATUS _retrieveFileFromCamera()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            status = _checkR3_is_connected_or_stopped(); if (status != CAM_STATUS.All_OK) return status;

            //
            string str =  R3(m_ip).retrieveFileFromCamera();
            MessageBox.Show(str);
            return CAM_STATUS.All_OK;
        }

        private CAM_STATUS _deleteFileFromCamera()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            status = _checkR3_is_connected_or_stopped(); if (status != CAM_STATUS.All_OK) return status;

            //
            return R3(m_ip).deleteFileFromCamera();
        }

#region userSet

        private CAM_STATUS _userSet_load(int id)
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            status = _checkR3_is_connected_or_stopped(); if (status != CAM_STATUS.All_OK) return status;

            status = m_list_r3[m_ip].userSet_load(id);
            if (status == CAM_STATUS.All_OK)
                updateParamPropertyGrid();

            return status;
        }

        private CAM_STATUS _userSet_save(int id)
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            status = _checkR3_is_connected_or_stopped(); if (status != CAM_STATUS.All_OK) return status;

            return m_list_r3[m_ip].userSet_save(id);
        }

        private CAM_STATUS _userSet_set_default(int id)
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            status = _checkR3_is_connected_or_stopped(); if (status != CAM_STATUS.All_OK) return status;

            return m_list_r3[m_ip].userSet_set_default(id);
        }

        private CAM_STATUS _userSet_set_description(int id, string str)
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            status = _checkR3_is_connected_or_stopped(); if (status != CAM_STATUS.All_OK) return status;

            return m_list_r3[m_ip].userSet_set_description(id, str);
        }

        private string _userSet_get_description(int id)
        {
            var status = _checkR3();
            if (status != CAM_STATUS.All_OK) return "";
            status = _checkR3_is_connected_or_stopped(); if (status != CAM_STATUS.All_OK) return "";

            return m_list_r3[m_ip].userSet_get_description(id);
        }

#endregion
    }

}

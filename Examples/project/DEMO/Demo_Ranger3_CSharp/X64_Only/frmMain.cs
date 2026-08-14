// 单元测试 
//#define UnitTest

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using SICK3D;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using System.Linq;

namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {

#region // Global variables. 全局变量

        // 状态位常量
        const int STATUS_BIT_ENABLE         = 30;
        const int STATUS_BIT_ENCODER_A      = 28;
        const int STATUS_BIT_ENCODER_B      = 27;
        const int STATUS_BIT_OVERTRIG       = 16;
        const int STATUS_BIT_LINE_TRIGGER   = 25;
        const int STATUS_BIT_ENCODER_RESET  = 24;

        // 配置常量
        private const int DEFAULT_BUFFER_COUNT = 20;
        private const int DEFAULT_IMAGE_WIDTH = 2560;
        private const int DEFAULT_CALIBRATION_WIDTH = 2560;
        private const int DEFAULT_CALIBRATION_WIDTH_5120 = 5120;



        public GenICam3D    m_r3s       = new GenICam3D("logPath", GenICam3D.getPathToRanger3Producer(), false, true);
        public string       m_ip        = "";                                   // IP of Current selected Ranger3.

        public Dictionary<string, Ranger3>   
                            m_list_r3   = new Dictionary<string, Ranger3>();    // Key=IP, Value=CSR3

        public DataNames    m_GrabMode  = DataNames.SEN;                  

		// Heartbeat
		public Dictionary<string, WorkThread>
							m_list_r3_HB = new Dictionary<string, WorkThread>();    // Key=IP, Value=

		// Heartbeat count
		public Dictionary<string, int> 
                            m_list_r3_HBC = new Dictionary<string, int>();    // Key=IP, Value=

        CSImgT              m_lastImg   = new CSImgT();
        XProps              m_PropsParam= new XProps();

        //private readonly object m_imgLocker = new object();
        //private readonly object m_logLocker = new object();

        bool                m_isSaveChnukData = false;

		ImgGrabber imgGrabber = new ImgGrabber();

#endregion


		public frmMain()
        {
            InitializeComponent();

#if UnitTest
            unitTestToolStripMenuItem.Enabled = true;
#else
			unitTestToolStripMenuItem.Enabled = false;
#endif

			imgGrabber.OnImageCallback += MyImageCallback;

		}

        /// <summary>
        /// 回收内存
        /// </summary>
        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hObject);


		/// <summary>
		/// 统计收到的图像张数，用于和 FrameID 比对判断有无丢帧。
		/// </summary>
		private int _imageCounter = 0;

		Bitmap imgGray;

		// 大图像高速刷新优化：帧率限制
		private int _frameSkipCounter = 0;
		private const int FRAME_SKIP = 2; // UI 更新跳帧数，每 2 帧更新一次 UI
		
		// 日志记录优化：减少日志频率
		private int _logCounter = 0;
		private const int LOG_INTERVAL = 10; // 每 10 帧记录一次日志
		private string LOG_STR = "";


		/// <summary>
		/// 示例：使用 callback 
		/// 优化：针对大图像（2560*5000）高速刷新场景
		/// </summary>
		/// <param name="imgT"></param>
		void MyImageCallback(CSImgT imgTable)
		{
			try
			{
				// 线程安全的计数器
				int currentCount = System.Threading.Interlocked.Increment(ref _imageCounter);
				
				// 日志记录优化：减少日志频率，避免影响性能
                LOG_STR += (DateTime.Now + " ：" + imgTable.get_ID().ToString() + " : count=" + currentCount.ToString() + "\r\n");
				_logCounter++;
				if (_logCounter >= LOG_INTERVAL)
				{
					_logCounter = 0;
                    wFlog(LOG_STR);
                    LOG_STR = "";
				}



				// 帧率限制：跳过部分帧以减少 UI 更新压力
				_frameSkipCounter++;
				if (_frameSkipCounter < FRAME_SKIP)
				{
					return; // 跳过显示，但继续处理以保持计数准确
				}
				_frameSkipCounter = 0;

				// 读取图片 convert image to show
				var mode = GetGrabMode();
				if (!imgTable.has(mode))
				{
					wFlog(DateTime.Now + " 没有这个图像：" + mode.ToString());
					return;
				}

				// 释放旧的 Bitmap 以避免内存泄漏
				var oldImgGray = imgGray;

				switch (mode)
				{
					case DataNames.SEN:
						BuiltGrayBitmap(
							imgTable.getDataByte(mode),
							imgTable.get_cols(),
							imgTable.get_rows(), ref imgGray
							);
						break;
					case DataNames.REF:
					case DataNames.REF_CAL:
					case DataNames.SCA_8:
					case DataNames.SCA_8_CAL:
					case DataNames.REF2:
					case DataNames.REF2_CAL:
					case DataNames.SCA2_8:
					case DataNames.SCA2_8_CAL:
						BuiltGrayBitmap(
							imgTable.getDataByte(mode),
							imgTable.get_cols(),
							imgTable.get_rows(), ref imgGray
							);
						break;
					case DataNames.SCA_16:
					case DataNames.SCA_16_CAL:
					case DataNames.SCA2_16:
					case DataNames.SCA2_16_CAL:
					case DataNames.RAN:
					case DataNames.RAN2:
					case DataNames.RAN_X_16:
					case DataNames.RAN_CAL_16:
						BuiltGrayBitmap(
							imgTable.getDataWord(mode),
							imgTable.get_cols(),
							imgTable.get_rows(), ref imgGray
							);
						break;
					case DataNames.RAN_X:
					case DataNames.RAN_CAL:
					case DataNames.RAN2_CAL:
						BuiltGrayBitmap(
							imgTable.getDataFloat(mode),
							imgTable.get_cols(),
							imgTable.get_rows(), ref imgGray
							);
						break;
					default:
						imgTable.Dispose();
						return;
				}

				// 释放旧的 Bitmap（在创建新 Bitmap 之后）
				oldImgGray?.Dispose();

				// save mark info if needed
				if (m_isSaveChnukData)
				{
					m_isSaveChnukData = false;
					if (imgTable.has(SICK3D.DataNames.MAR))
					{
						uint[] mark = imgTable.getDataMark();
						// 使用应用程序目录保存文件，避免硬编码路径
						var fileName = $"chunkdat_{imgTable.get_ID()}.txt";
						var path = System.IO.Path.Combine(Application.StartupPath, fileName);
						using (StreamWriter sw = new StreamWriter(path))
						{
							sw.WriteLine("encodervalue, overtriggerCounts, frameTriggerActive, lineTriggerActive, encoderResetActive, encoderA, encoderB, timestamp");
							for (int i = 0; i < imgTable.get_rows(); ++i)
							{
								var encodervalue = mark[0 + i * 5];
								var overtriggerCounts = mark[1 + i * 5] >> 16 & 0xff;

								var frameTriggerActive = (mark[1 + i * 5] >> STATUS_BIT_ENABLE & 1) == 1;
								var lineTriggerActive = (mark[1 + i * 5] >> STATUS_BIT_LINE_TRIGGER & 1) == 1;
								var encoderResetActive = (mark[1 + i * 5] >> STATUS_BIT_ENCODER_RESET & 1) == 1;
								var encoderA = (mark[1 + i * 5] >> STATUS_BIT_ENCODER_A & 1) == 1;
								var encoderB = (mark[1 + i * 5] >> STATUS_BIT_ENCODER_B & 1) == 1;

								var timestamp = ((UInt64)mark[3 + i * 5] << 32) + (UInt64)mark[2 + i * 5];

								sw.Write(encodervalue); sw.Write(",");
								sw.Write(overtriggerCounts); sw.Write(",");

								sw.Write(frameTriggerActive); sw.Write(",");
								sw.Write(lineTriggerActive); sw.Write(",");
								sw.Write(encoderResetActive); sw.Write(",");
								sw.Write(encoderA); sw.Write(",");
								sw.Write(encoderB); sw.Write(",");

								sw.Write(timestamp); sw.Write(",");

								sw.WriteLine();
							}
						}
						MessageBox.Show("保存每个剖面的编码器/时间戳到：" + path);
					}
					else
					{
						MessageBox.Show("请使能参数： ActiveChunData！");
					}

				}

				// UI 更新：每次创建新的 Bitmap，避免 PictureBox 内部状态冲突
				// 对于大图像，缩放操作是主要开销，但必须每次创建新实例
				pictureBox1.BeginInvoke(new Action(() =>
				{
					// 检查控件状态，避免在控件已释放时更新
					if (pictureBox1.IsDisposed || !pictureBox1.IsHandleCreated || imgGray == null)
						return;

					try
					{
						// 每次创建新的 Bitmap，避免 PictureBox 复用 Bitmap 时的状态冲突
						// 虽然会重复缩放，但这是 PictureBox 的限制
						var displaySize = new Size(pictureBox1.Width, pictureBox1.Height);
						Bitmap displayBitmap = new Bitmap(imgGray, displaySize);

						// 释放旧的显示图像，避免内存泄漏
						var oldImage = pictureBox1.Image;
						pictureBox1.Image = displayBitmap;
						oldImage?.Dispose();
					}
					catch (Exception ex)
					{
						// 记录错误但不中断程序
						System.Diagnostics.Debug.WriteLine($"UI 更新错误: {ex.Message}");
					}
				}));
			}
			finally
			{
				// 确保资源释放
				imgTable.Dispose();
				//GC.Collect();
				//GC.WaitForPendingFinalizers();
			}
		}


        /// <summary>
        /// 示例，缩放图像
        /// </summary>
		public Image doResizeImage(ref Image image, int width, int height)
		{
			// 创建一个目标大小的空画布
			Bitmap resizedImage = new Bitmap(width, height);

			// 创建一个绘图对象，用于在目标画布上绘制图像
			using (Graphics graphics = Graphics.FromImage(resizedImage))
			{
				// 设置绘图质量
				graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
				graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;

				// 绘制图像到目标画布上并进行缩放
				graphics.DrawImage(image, 0, 0, width, height);
			}

			// 返回缩放后的图像
			return resizedImage;
		}

		private void onThreadHeartBeat(string ip)
        {
			try
			{
				if (!m_list_r3.ContainsKey(ip))
				{
					wFlog(ip + "并未识别到。");
					return;
				}

				if (!R3(ip).getHeartBeat())
				{
					const int MAX_RECONNECT_ATTEMPTS = 500; // 尝试次数
					const int RECONNECT_INTERVAL_MS = 300;
					for (int i = 0; i < MAX_RECONNECT_ATTEMPTS; ++i)
					{
						try
						{
							if (SICK3D.CAM_STATUS.All_OK == R3(ip).reconnectCamera())
							{
								// 重连成功
								wFlog(ip + " 重新连接成功！");

								// 重置心跳计数
								if (m_list_r3_HBC.ContainsKey(ip))
								{
									m_list_r3_HBC[ip] = 0;
								}

								return;
							}
							else 
							{
								// 继续尝试
								wFlog(i.ToString() + "/" + MAX_RECONNECT_ATTEMPTS.ToString() + "，继续尝试重新连接。");
							}
						}
						catch (Exception ex)
						{
							wFlog($"重连尝试 {i} 时发生异常: {ex.Message}");
						}
						Thread.Sleep(RECONNECT_INTERVAL_MS);
					}

					// 尝试次数用尽，重连失败，相机永远丢失。清空主程序变量。
					if (m_list_r3.ContainsKey(ip))
					{
						m_list_r3.Remove(ip);
					}
					if (m_list_r3_HB.ContainsKey(ip))
					{
						m_list_r3_HB.Remove(ip);
					}
					if (m_list_r3_HBC.ContainsKey(ip))
					{
						m_list_r3_HBC.Remove(ip);
					}
					return;
				}
				else
				{
					// 心跳正常
					if (m_list_r3_HBC.ContainsKey(ip))
					{
						m_list_r3_HBC[ip] += 1;
					}
					//wFlog(ip + "，获得心跳，" + m_list_r3_HBC[ip].ToString());
				}
			}
			catch (Exception ex)
			{
				// 记录异常但不中断心跳线程
				wFlog($"心跳检查异常 ({ip}): {ex.Message}");
				System.Diagnostics.Debug.WriteLine($"Heartbeat exception for {ip}: {ex}");
			}
		}

		private void frmMain_Load(object sender, EventArgs e)
        {
            FindCamera();
            if (R3(m_ip) != null && R3(m_ip).isReady() == CAM_STATUS.All_OK)
            {
                toolStripComboBoxMode.SelectedIndex = 1;
            }
            else
            {
                textBoxSleepTime.Text = "0";
                textBoxSleepTime.Enabled = false;

                textBoxRectSpread.Text = "1.2";
                textBoxRectSpread.Enabled = false;
            }
        }

        private void clearPropertyGrid()
        {
            m_PropsParam.Clear();
            propertyGrid1.SelectedObject = m_PropsParam;
        }
        private void addPropertyGrid(String cate, String name, Type type, object value, String desc, String[] conv, bool readOnly)
        {
            // write the parameter property page automaticly. 
            XProp xprop = new XProp();
            xprop.Category = cate;
            xprop.Name = name;
            xprop.ProType = type;
            xprop.Value = value;
            xprop.Description = desc;
            xprop.Options = conv;
            if (conv.Length > 0)
            {
                // 性能优化：使用 string.Join 替代循环拼接
                String optStr = string.Join(",", conv);
                xprop.Converter = new MyComboItemConvert(optStr);
            }
            xprop.ReadOnly = readOnly;

            m_PropsParam.Add(xprop);
            propertyGrid1.SelectedObject = m_PropsParam;
        }
        private void updateParamPropertyGrid()
        {
            clearPropertyGrid();
            if (R3(m_ip).updateParamtersInfo())
            {
                String[] cates = R3(m_ip).paramCategoryList();
                foreach (var cat in cates)
                {
                    var paras = R3(m_ip).paramFullNameOfCategory(cat);

                    foreach (var par in paras)
                    {
                        String cValue = R3(m_ip).paramCurrentValue(par);
                        int cValueId = 0; // for the numerical parameter only
                        String[] opts = R3(m_ip).paramOptionalValues(par, ref cValueId);
                        if (opts.Length == 0)
                        {
                            addPropertyGrid(cat, par, typeof(String), cValue, "", opts, false);
                        }
                        else
                        {
                            addPropertyGrid(cat, par, typeof(MyComboItemConvert), cValueId, "", opts, false);
                        }

                    }
                }
            }
        }

        private void toolStripButtonFindCamera_Click(object sender, EventArgs e)
        {
            {
                treeView1.Nodes.Clear();
                m_list_r3.Clear();
				m_list_r3_HB.Clear();
				m_list_r3_HBC.Clear();
			}
            FindCamera();
            if (/*R3(m_ip) != null && */R3(m_ip).isReady() == CAM_STATUS.All_OK)
            {
                toolStripComboBoxMode.SelectedIndex = Convert.ToInt32(DataNames.SEN);
            }
            else
                MessageBox.Show("没有找到相机！ No Cameras found!");
        }

        private void toolStripButtonConnection_Click(object sender, EventArgs e)
        {
            Connection();
        }

        private void toolStripButtonDisconnection_Click(object sender, EventArgs e)
        {
            Disconnection();
        }

        private void toolStripButtonStart_Click(object sender, EventArgs e)
        {
            Start(); 
        }

        private void toolStripButtonStop_Click(object sender, EventArgs e)
        {
            Stop(); 
        }


        // if return false, caller need to keep toolStripComboBoxMode.SelectedIndex avaliable.
        // if return true, toolStripComboBoxMode.SelectedIndex is avalibale. But it may be changed in this function.
        private bool checkGrabMode()
        {
            if (!(R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_CONNECTED || R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STOPPED))
                return false;

            // 检查 Grab Mode 和参数设置是否匹配。以参数设置为标准，应该调整 Grab Mode 以适应当前的参数设置。
            var scanType = R3(m_ip).getParameterValue("DeviceScanType");
            if (scanType == "Areascan") // senseor
            {
                var mode = GetGrabMode();
                if (mode != DataNames.SEN)
                {
                    MessageBox.Show("检测到 \"Grab Mode\" 是 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + "与参数 <DeviceScanType = Areascan> 不匹配，关闭窗口后将会强制设置 \"Grab Mode\" 为 [Sensor]。 \n"
                        + "请检查：\n"
                        + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                        + "2. 若想起用 \"Grab Mode\" 中的设置，请设置 <DeviceScanType = Linescan3D> \n\n\n"
                        + "\"Grab Mode\" is [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + " that mismatch the parameter <DeviceScanType = Areascan>. After closing window it will set \"Grab Mode\" as [Sensor] automaticly. "
                        + "Please check:\n"
                        + "1. Have updated parameters? Please click <Set Param> to enabel setting. \n"
                        + "2. If you want to keep the setting of \"Grab Mode\", please set <DeviceScanType = Linescan3D> \n\n\n"
                        , "WARNING");
                    toolStripComboBoxMode.SelectedIndex = (int)DataNames.SEN;
                    //m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
                    return true;
                }
            }
            else if (scanType == "Linescan3D") // range
            {
                var mode = GetGrabMode();

                // 检查 Grab Mode 不能是 2D 模式
                if (mode == DataNames.SEN)
                {
                    MessageBox.Show("检测到 \"Grab Mode\" 是 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + "与参数 <DeviceScanType = Linescan3D> 不匹配，关闭窗口后将会强制设置 \"Grab Mode\" 为 [Range]。 \n"
                        + "请检查：\n"
                        + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n\n\n"
                        + "\"Grab Mode\" is [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + " that mismatch the parameter <DeviceScanType = Linescan3D>. After closing window it will set \"Grab Mode\" as [Range] automaticly. " + "Please check:\n"
                        + "Please check:\n"
                        + "1. Have updated parameters? Please click <Set Param> to enabel setting."
                        , "WARNING");
                    toolStripComboBoxMode.SelectedIndex = (int)DataNames.RAN;
                    return true;
                }
                else
                {
                    // Grab Mode 是 3D 模式

                    // Refelctance1 使能设置检查
                    if (mode == DataNames.REF || mode == DataNames.REF_CAL)
                    {
                        if ("1" != R3(m_ip).getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"))
                        {
                            MessageBox.Show("检测到 \"Grab Mode\" 设置值 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] 不匹配当前参数设置。\n"
                                + "该 Grab Mode 需要相机使能 <Reflectance (激光强度图)> 。\n"
                                + "请检查：\n"
                                + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                                + "2. 请将 <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1> 设置为 [1] \n"
                                + "3. 请将 \"Grab Mode\" 设置 [Range] \n\n\n"
                                + "\"Grab Mode\" = [" + toolStripComboBoxMode.SelectedItem.ToString() + "] mismatch current parameter setting. \n"
                                + "It need enable <Reflectance> \n"
                                + "Please check:\n"
                                + "1. Have updated parameters? Please click <Set Param> to enabel setting.\n"
                                + "2. Please set <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1> = [1]\n"
                                + "3. Please set \"Grab Mode\" as [Range]. \n\n\n"
                                , "ERROR");
                            return false;
                        }
                    }

                    // Range2 使能设置检查
                    if (mode == DataNames.RAN2 || mode == DataNames.RAN2_CAL)
                    {
                        var ran2 = R3(m_ip).getParameterValue("ComponentEnable_ComponentSelector_Range_RegionSelector_Scan3dExtraction2");
                        if (ran2 == "0")
                        {
                            MessageBox.Show("检测到 \"Grab Mode\" 设置值 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] 不匹配当前参数设置。\n"
                                + "该 Grab Mode 需要相机使能【Extra 算法 Scan3dExtraction2】中的 <Range (激光强度图)> 。\n"
                                + "请检查：\n"
                                + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                                + "2. 请将 <ComponentEnable_ComponentSelector_Range_RegionSelector_Scan3dExtraction2> 设置为 [1] \n"
                                + "3. 请将 \"Grab Mode\" 设置 [Range] \n\n\n"
                                + "\"Grab Mode\" = [" + toolStripComboBoxMode.SelectedItem.ToString() + "] mismatch current parameter setting. \n"
                                + "It need enable <Range of Scan3dExtraction2>\n"
                                + "Please check:\n"
                                + "1. Have updated parameters? Please click <Set Param> to enabel setting.\n"
                                + "2. Please set <ComponentEnable_ComponentSelector_Range_RegionSelector_Scan3dExtraction2> = [1]\n"
                                + "3. Please set \"Grab Mode\" as [Range]. \n\n\n"
                                , "ERROR");
                            return false;
                        }
                        else if (ran2 == "")
                        {
                            MessageBox.Show("不支持当前设置。 \n\n\nNot Support Grab Mode yet!", "ERROR");
                            return false;
                        }

                    }

                    // Refelctance2 使能设置检查
                    if (mode == DataNames.REF2 || mode == DataNames.REF2_CAL)
                    {
                        var ref2 = R3(m_ip).getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2");
                        if ("0" == ref2)
                        {
                            MessageBox.Show("检测到 \"Grab Mode\" 设置值 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] 不匹配当前参数设置。\n"
                                + "该 Grab Mode 需要相机使能【Extra 算法 Scan3dExtraction2】中的 <Reflectance (激光强度图)> 。\n"
                                + "请检查：\n"
                                + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                                + "2. 请将 <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2> 设置为 [1] \n"
                                + "3. 请将 \"Grab Mode\" 设置 [Range] \n\n\n"
                                + "\"Grab Mode\" = [" + toolStripComboBoxMode.SelectedItem.ToString() + "] mismatch current parameter setting. \n"
                                + "It need enable <Reflectance of Scan3dExtraction2>\n"
                                + "Please check:\n"
                                + "1. Have updated parameters? Please click <Set Param> to enabel setting.\n"
                                + "2. Please set <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2> = [1]\n"
                                + "3. Please set \"Grab Mode\" as [Range]. \n\n\n"
                                , "ERROR");
                            return false;
                        }
                        else if (ref2 == "")
                        {
                            MessageBox.Show("不支持当前设置。 \n\n\nNot Support Grab Mode yet!", "ERROR");
                            return false;
                        }
                    }

                    if (mode == DataNames.INV || mode == DataNames.SCA_8 || mode == DataNames.SCA_8_CAL || mode == DataNames.SCA2_8 || mode == DataNames.SCA2_8_CAL || mode == DataNames.REF_CAL_F)
                    {
                        MessageBox.Show("不支持当前设置。 \n\n\nNot Support Grab Mode yet!", "ERROR");
                        return false;
                    }
                }
            }
            else
            {
                MessageBox.Show("读取参数 <DeviceScanType> 失败！！\n\n\nGet value of parameter <DeviceScanType> failed!!", "ERROR");
                return false;
            }

            return true;
        }
        private void toolStripComboBoxMode_Click(object sender, EventArgs e)
        {
            var status = R3(m_ip).getStatus();
            if (status == CAM_STATUS.CAM_IS_STARTED)
                Stop();
            if (status == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                toolStripComboBoxMode.SelectedIndex = (int)m_GrabMode; /// revert to last avalible value
            }

            // check if Grab Mode match parameter setting.
            // Grab Mode is under control by parameter setting.
            var lstGrabMode = m_GrabMode;

            m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
            if (checkGrabMode())
            {
                //var selectedId = toolStripComboBoxMode.SelectedIndex;
                //m_GrabMode = (DataNames)selectedId;
                m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;


                //if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STOPPED) /// does not support connected on purpose! 
                //    Start();
            }
            else
            {
                toolStripComboBoxMode.SelectedIndex = (int)lstGrabMode; /// revert to last avalible value
            }

        }


        private void btnImportCameraConfigration_Click(object sender, EventArgs e)
        {
            _on_btn_Click(0);
        }

        private void btnExportCameraConfigration_Click(object sender, EventArgs e)
        {
            _on_btn_Click(1);
        }

        private void btnImportCalibrationFile_Click(object sender, EventArgs e)
        {
            if(CAM_STATUS.All_OK == _on_btn_Click(2))
                textBoxRectSpread.Enabled = true;
        }

        private void frmMain_FormClosed(object sender, FormClosedEventArgs e)
        {
            _Stop();
            _Disconnection();
            
            // 清理 Bitmap 资源
            imgGray?.Dispose();
            imgGray = null;
        }

        private void treeView1_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            var str = e.Node.Text;
            if (str == "Ranger3") return;

            m_ip = str;

            // recover tree view icon
            for (int i = 0; i < treeView1.Nodes[0].Nodes.Count; ++i)
                treeView1.Nodes[0].Nodes[i].ForeColor = treeView1.Nodes[0].Nodes[i].Text == m_ip ? Color.Red : Color.Black;

            // change status to stop or disconnect
            var sta = R3(m_ip).getStatus();
            if (sta == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                var status = _Connection();
                if (status != CAM_STATUS.All_OK) { MessageBox.Show(status.ToString()); return; }
            }
            if (sta == CAM_STATUS.CAM_IS_STARTED)
            {
                var status = _Stop();
                if (status != CAM_STATUS.All_OK) { MessageBox.Show(status.ToString()); return; }
            }
            btnCameraStatus(R3(m_ip).getStatus());

            // expand propertyGrids 
            updateParamPropertyGrid();
        }

        private void btnCameraStatus(CAM_STATUS sta)
        {
            toolStripButtonConnection.ForeColor = Color.Black;
            toolStripButtonDisconnection.ForeColor = Color.Black;
            toolStripButtonStart.ForeColor = Color.Black;
            toolStripButtonStop.ForeColor = Color.Black;

            if (sta == CAM_STATUS.CAM_IS_CONNECTED)
            {
                toolStripButtonConnection.ForeColor     = Color.Red;
            }
            else if (sta == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                toolStripButtonDisconnection.ForeColor = Color.Red;
            }
            else if (sta == CAM_STATUS.CAM_IS_STARTED)
            {
                toolStripButtonStart.ForeColor = Color.Red;
				//received_image_counter = 0;
            }
            else if (sta == CAM_STATUS.CAM_IS_STOPPED)
            {
                toolStripButtonStop.ForeColor = Color.Red;
            }
        }

        // id == 0 ImportConfigrationFile(); id == 1 ExportConfigrationFile(); id == 2 ImportCalibrationFile();
        private CAM_STATUS _on_btn_Click(int id)
        {
            var status = _Stop();
            if (CAM_STATUS.All_OK != status) { MessageBox.Show("Stop Camera failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }

            switch (id)
            {
                case 0:
                    status = ImportConfigrationFile();
                    if (CAM_STATUS.All_OK != status) { MessageBox.Show("Import Configration File failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }
                    break;
                case 1:
                    status = ExportConfigrationFile();
                    if (CAM_STATUS.All_OK != status) { MessageBox.Show("Export Configration File failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }
                    break;
                case 2:
                    status = ImportCalibrationFile();
                    if (CAM_STATUS.All_OK != status) { MessageBox.Show("Import Calibration File failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }
                    break;
            }
            MessageBox.Show("Processing OK !!");
            //status = _Start();
            //if (CAM_STATUS.All_OK != status) { MessageBox.Show("Start Camera failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }

            return CAM_STATUS.All_OK;
        }

        private void unitTestToolStripMenuItem_Click(object sender, EventArgs e)
        {
#if UnitTest
            _UnitTest();
#endif
        }


		/// <summary>
		/// 注意，在线程中区更新UI控件会比较消耗CPU，所以原则上，要少，要集中到几个UI操作到一个 BeginInvoke 中。
		/// 一些情况下，不允许跨线程直接操作 UI 控件，请使用 textBox1.BeginInvoke
		/// </summary>
		/// <param name="str"></param>
		private void wFlog(string str)
        {
            textBox1.BeginInvoke(new EventHandler(delegate
            {
                textBox1.AppendText(str + "\r\n\r\n"); // 日志的添加方式必须使用 AppendText
                //textBox1.Text = textBox1.Text + str + "\r\n\r\n"; // 这种方式设计到全部文本数据的读取、拼接、赋值，会随着text增加越来越耗时。绝对不能！！！用这种方式更新日志文本。
                //将导致 CPU 使用率逐渐上升，影响其他进程线程，甚至导致cpu无法即使响应网卡中断，导致图像丢帧。
                textBox1.Select(textBox1.TextLength, 0);
                textBox1.ScrollToCaret();
            })); 
        }

        private void btnSaveBuffer_Click(object sender, EventArgs e)
        {
            if (!SaveImage())
                MessageBox.Show("Save as ICON failed! 保存失败！");
            else
                MessageBox.Show("Save as ICON ok! 保存成功！");
        }

        private void propertyGrid1_Click(object sender, EventArgs e)
        {

        }

        private void 打开APIHelpToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start(@"GenICam_CSharp_API.chm");
        }

        private void 使用指南ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Bitmap src = new Bitmap(@"help.PNG");
            IntPtr pp = new IntPtr();

            Form f = new Form();
            f.Text = "使用指南 | User Guide";
            f.BackgroundImage = src.GetThumbnailImage(src.Width, src.Height, null, pp);
            f.Size = new Size(src.Width, src.Height + 30);
            f.MaximumSize = new Size(src.Width, src.Height+30);
            f.Show();
        }

        private void toolStripButtonSetParam_Click(object sender, EventArgs e)
        {
            // camera status check
            var sta = R3(m_ip).getStatus();
            if (sta == CAM_STATUS.CAM_IS_STARTED)
                Stop();
            if (sta == CAM_STATUS.CAM_IS_DISCONNECTED)
                Connection();

            sta = R3(m_ip).getStatus();
            if (!(sta == CAM_STATUS.CAM_IS_STOPPED || sta == CAM_STATUS.CAM_IS_CONNECTED))
            {
                MessageBox.Show("相机状态不合理! ErrorCode: " + sta.ToString() 
                    + "Camera status invalid, ErrorCode: " + sta.ToString(), "ERROR");
                return;
            }

            // set para
            foreach (var prop in m_PropsParam)
            {
                if (prop.Update)
                {
                    var name = prop.Name;
                    String value = "";
                    if (prop.Options.Length > 0) // has options
                    {
                        int valueId = Convert.ToInt32(prop.Value.ToString());
                        if (valueId >= prop.Options.Length)
                        {
                            MessageBox.Show(CAM_STATUS.ERROR_PARAMETER_VALUE_INVALID.ToString(), "ERROR");
                            return;
                        }    
                        value = prop.Options[valueId];
                    }
                    else // numerical parameter
                        value = prop.Value.ToString();

                    var status = R3(m_ip).setParameterValue(name, value);
                    if (status != CAM_STATUS.All_OK)
                    {
                        MessageBox.Show("尝试设置 <" + name + "> = [" + value + "] 失败！ ErrorCode: " + status.ToString() 
                            + "Try to set <" + name + "> as [" + value + "] Failed! ErrorCode: " + status.ToString(), "ERROR");
                        return;
                    }
                }

            }

            if(checkGrabMode())
                MessageBox.Show("设置成功！\n\nSetting Done!", "OK");
        }

		private void btnLoadBuffer_Click(object sender, EventArgs e)
		{
			CSImgT img = LoadImage();
			if (!img.isEmpty())
			{
				var dnList = img.getAvalibleDataNames();
				foreach (var dn in dnList)
				{
					Form f = new Form();
					Bitmap src = new Bitmap(1, 1);

					if (dn == DataNames.SEN)
					{
						f.Text = "激光线图像 | Sensor Image";
						BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(), img.get_rows(), ref src);
					}
					else if (dn == DataNames.RAN)
					{
						f.Text = "高度图像 | Range Image";
						BuiltGrayBitmap(img.getDataWord(dn), img.get_cols(), img.get_rows(), ref src);
					}
					else if (dn == DataNames.RAN_CAL)
					{
						f.Text = "标定后的高度图像 | Calibrated Range Image";
						BuiltGrayBitmap(img.getDataFloat(dn), img.get_cols(), img.get_rows(), ref src);
					}
					else if (dn == DataNames.REF)
					{
						f.Text = "激光强度图像 | Reflectance Image";
						BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(), img.get_rows(), ref src);
					}
					else if (dn == DataNames.REF_CAL)
					{
						f.Text = "标定后的激光强度图像 | Calibrated Reflectance Image";
						BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(), img.get_rows(), ref src);
					}

					IntPtr pp = new IntPtr();
					Size imgSize = new Size(src.Width / 3, src.Height / 3);
					f.BackgroundImage = src.GetThumbnailImage(imgSize.Width, imgSize.Height, null, pp);
					f.Size = new Size(imgSize.Width, imgSize.Height + 30);
					f.MaximumSize = new Size(imgSize.Width, imgSize.Height + 30);
					f.Show();

					DeleteObject(pp);
					src.Dispose();
				}
			}
			img.Dispose();
		}

		private void 常见问题ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Bitmap src = new Bitmap(@"help2.PNG");
            IntPtr pp = new IntPtr();

            Form f = new Form();
            f.Text = "常见问题 | Q & A";
            f.BackgroundImage = src.GetThumbnailImage(src.Width, src.Height, null, pp);
            f.Size = new Size(src.Width, src.Height + 30);
            f.MaximumSize = new Size(src.Width, src.Height + 30);
            f.Show();
        }

        private void 版本VersionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var apiVersion = new SICK3D.VER();
            var vers = apiVersion.__Version__();
            MessageBox.Show(vers, "SDK Version Information");
        }

        private void btnSetSleepTime_Click(object sender, EventArgs e)
        {
            var sta = R3(m_ip).getStatus();
            if (textBoxSleepTime.Enabled && ( sta == CAM_STATUS.CAM_IS_CONNECTED || sta == CAM_STATUS.CAM_IS_STARTED || sta == CAM_STATUS.CAM_IS_STOPPED))
            {
                // 异常处理优化：使用 TryParse 替代 Parse
                if (uint.TryParse(textBoxSleepTime.Text, out uint time))
                {
                    R3(m_ip).setGrabSleepTime(time);
                }
                else
                {
                    MessageBox.Show("请输入有效的数字！\n\nPlease enter a valid number!", "ERROR");
                    return;
                }
            }
            else
            {
                MessageBox.Show("请先连接(connect)相机！否则无法设置轮询采集线程的sleep时间。 ErrorCode: " + sta.ToString()
                    + "\n\nPls. connect Camera first! ErrorCode: " + sta.ToString(), "ERROR");
                return;
            }
        }

#region File_Operation
        private void sendToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_sendFileToCamera() == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void retriveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _retrieveFileFromCamera();
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_deleteFileFromCamera() == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }
#endregion

#region User_Set

        // default
        private void loadToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_load(0) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void getDescriptionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show(_userSet_get_description(0));
        }

        // userSet 1
        private void saveFromCurrentToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_save(1) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void loadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_load(1) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void setAsStartUpToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_set_default(1) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void getDescriptionToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            MessageBox.Show(_userSet_get_description(1));
        }

        private void setDescriptionToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            string strText = string.Empty;
            X64_Only.frmInput.Show(out strText);
            MessageBox.Show((_userSet_set_description(1, strText) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        // userSet 2
        private void toolStripMenuItem5_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_save(2) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void loadToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_load(2) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void setAsDefaultToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_set_default(2) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void getDescriptionToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            MessageBox.Show(_userSet_get_description(2));
        }

        private void setDescriptionToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            string strText = string.Empty;
            X64_Only.frmInput.Show(out strText);
            MessageBox.Show((_userSet_set_description(2, strText) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        // userSet 3
        private void saveFromCurrentToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_save(3) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void loadToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_load(3) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void setAsDefaultToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_set_default(3) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void getDescriptionToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            MessageBox.Show(_userSet_get_description(3));
        }

        private void setDescriptionToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            string strText = string.Empty;
            X64_Only.frmInput.Show(out strText);
            MessageBox.Show((_userSet_set_description(3, strText) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        // userSet 4
        private void saveFromCurrentToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_save(4) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void loadToolStripMenuItem4_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_load(4) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void setAsDefaultToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_set_default(4) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void getDescriptionToolStripMenuItem4_Click(object sender, EventArgs e)
        {
            MessageBox.Show(_userSet_get_description(4));
        }

        private void setDescriptionToolStripMenuItem4_Click(object sender, EventArgs e)
        {
            string strText = string.Empty;
            X64_Only.frmInput.Show(out strText);
            MessageBox.Show((_userSet_set_description(4, strText) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        // userSet 5
        private void saveFromCurrentToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_save(5) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void loadToolStripMenuItem5_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_load(5) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void setAsDefaultToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            MessageBox.Show((_userSet_set_default(5) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }

        private void getDescriptionToolStripMenuItem5_Click(object sender, EventArgs e)
        {
            MessageBox.Show(_userSet_get_description(5));
        }

        private void setDescriptionToolStripMenuItem5_Click(object sender, EventArgs e)
        {
            string strText = string.Empty;
            X64_Only.frmInput.Show(out strText);
            MessageBox.Show((_userSet_set_description(5, strText) == CAM_STATUS.All_OK ? "Operation done!" : "Failed!"));
        }
#endregion

        private void btnSetRectSpread_Click(object sender, EventArgs e)
        {
            /*
            // 从3.4.0.0之后版本，默认不含 in-PC 标定功能。可以使用更方便的 in-device 标定。
            var sta = R3(m_ip).getStatus();
            if (textBoxRectSpread.Enabled && (sta == CAM_STATUS.CAM_IS_CONNECTED || sta == CAM_STATUS.CAM_IS_STOPPED))
            {
                double time = double.Parse(textBoxRectSpread.Text); // if test is not a number, it will crash!!
                if (R3(m_ip).setRectSpread(time) == CAM_STATUS.All_OK)
                {
                    MessageBox.Show("设置成功！", "MSG");
                }
            }*/
            MessageBox.Show("请使用相机内部（in-device Calibration）标定！", "MSG");
		}

		private void 保存ChunkData时间戳编码器ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_isSaveChnukData = true;
        }

		private void importParameterscsvToolStripMenuItem_Click(object sender, EventArgs e)
		{
			_on_btn_Click(0);
		}

		private void exportParameterscsvToolStripMenuItem_Click(object sender, EventArgs e)
		{
			_on_btn_Click(1);
		}

		private void btnCalibrateInDevice_Click(object sender, EventArgs e)
		{
			var sta = R3(m_ip).getStatus();
			if (!(sta == CAM_STATUS.CAM_IS_STOPPED || sta == CAM_STATUS.CAM_IS_CONNECTED))
			{
				MessageBox.Show("相机状态不合理! ErrorCode: " + sta.ToString()
					+ "Camera status invalid, ErrorCode: " + sta.ToString(), "ERROR");
				return;
			}

            if (btnCalibrateInDevice.Text == "CalibrateInDevice(No)")
            {
                btnCalibrateInDevice.Text = "CalibrateInDevice(Yes, rectifyC, range uint16)";
                R3(m_ip).setCalibration_inDevice(DEFAULT_CALIBRATION_WIDTH, true, false);
            }
            else if (btnCalibrateInDevice.Text == "CalibrateInDevice(Yes, rectifyC, range uint16)")
            {
                btnCalibrateInDevice.Text = "CalibrateInDevice(Yes, rectifyC, range float)";
                R3(m_ip).setCalibration_inDevice(DEFAULT_CALIBRATION_WIDTH, false, false);
            }
			else if (btnCalibrateInDevice.Text == "CalibrateInDevice(Yes, rectifyC, range float)")
			{
				btnCalibrateInDevice.Text = "CalibrateInDevice(Yes, calibratedAC, range uint16)";
				R3(m_ip).setCalibration_inDevice_CalibrateAC(true, true, false);
			}
			else
            {
                btnCalibrateInDevice.Text = "CalibrateInDevice(No)";
                R3(m_ip).setCalibration_No();
			}

		}

		private void importCalibrationjsonToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (CAM_STATUS.All_OK == _on_btn_Click(2))
				textBoxRectSpread.Enabled = true;
		}

		private void 加载Ranger380数据并对齐ToolStripMenuItem_Click(object sender, EventArgs e)
		{
            var res =  MessageBox.Show("请修改这个函数中使用到的路径，并且提供相关的文件，才可以继续！！确认修改正确后，请点击 OK", "", MessageBoxButtons.OKCancel);
            if (res != DialogResult.OK)
                return;


			CSImgT img1 = new CSImgT();
			img1.loadFromIconFile("F:\\20250109\\222\\raw", 1.0, 1.0, false);

			CSImgT img2 = new CSImgT();
			img2.loadFromIconFile("F:\\20250109\\222\\raw", 1.0, 1.0, false);

			CSImgT img3 = new CSImgT();
			img3.loadFromIconFile("F:\\20250109\\222\\raw", 1.0, 1.0, false);


			// --> 测试多次扫描不完整后，需要首尾拼接多个图像的情况
			//CSImgT img_12 = img1.Append(img2);
			//CSImgT img_123 = img_12.Append(img3);

			img1.Append(img2);
			img1.Append(img3);
            CSImgT img_123 = img1;
			////////////////////////////////////////////////////////////////////////////


			// 应用高度图标定
			int calibration_handle_id = 0;

			// --> 可以多次调用
			img_123.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, true, 1.2, 2560, 2560, 1, 8);
			//img.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, true, 1.2, 2560, 2560, 1);
			//img.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, true, 1.2, 2560, 2560, 1);
			//img.ApplyCalibration(calibration_handle_id, "F:\\20250109\\222\\Calibration2560.json", 0, true, 1.2, 2560, 2560, 1);




			////////////////////////////////////////////////////////////////////////////


			// 应用彩色对齐标定。仅需要首次调用，初始化一些静态资源
			int color_handle_id = 8;

			// --> 彩色对齐
			CSImgT imgColorCal = img_123.ApplyCalibrationColor(color_handle_id, "F:\\20250109\\222\\Calibration5120json.json",
				"F:\\20250109\\222\\2025-04-14-13-14-38_config.xml",
				200, // 假如工件需要 2 张或更多图才能扫描完整，为了对齐就扫 3 张，并且在这里剪除指定行数，即可获得完整的数据。
                true, 3, 3.0, 5120, 5120, 1, 0, true, 8);

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

            //for (int j = 0; j < height; ++j)
            //{
            //	double y = yOffset + j * yScale;
            //	for (int i = 0; i < width; ++i)
            //	{
            //		double x = xOffset + i * xScale;
            //		double z = RangeComponent[j * width + i];
            //		byte r = RImageBytes[j * width + i];
            //		byte g = GImageBytes[j * width + i];
            //		byte b = BImageBytes[j * width + i];

            //		// 点云 x y z r g b 
            //	Console.WriteLine($"{x},{y},{z},{r},{g},{b},");
            //	}

            //	if (j > 10) // 仅显示 10 行数据
            //		break;

            //}


			pictureBox1.Image = new Bitmap(bitmap3, new Size(pictureBox1.Width, pictureBox1.Height));




			// 仅需要最后一次调用，释放静态资源
			CSImgT.DeleteCalibration(calibration_handle_id);
			// 仅需要最后一次调用，释放静态资源
			CSImgT.DeleteCalibrationColor(color_handle_id);



		}
	}



}

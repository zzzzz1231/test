using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using SwigRanger3;

namespace Demo.R3.SWIG.FW472.Winform
{
	public partial class Form1 : Form
	{
		private CameraShared m_cameraShared = null;

		public class ChannelInfo
	{
		public int ChannelIndex;
		public Ranger3 Camera;
		public bool IsRunning;
		public bool IsConnected;
		public string IpAddress;
	}

		private ChannelInfo[] m_channels = new ChannelInfo[4];
		private Dictionary<string, int> m_scannedCameras = new Dictionary<string, int>();
		private static ColorPalette s_grayPalette = null;
		private int m_selectedChannel = -1;

		public Form1()
		{
			InitializeComponent();

			for (int i = 0; i < 4; i++)
			{
				m_channels[i] = new ChannelInfo
				{
					ChannelIndex = i,
					Camera = null,
					IsRunning = false,
					IsConnected = false,
					IpAddress = ""
				};
			}
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			SetStatus("就绪 - 点击【扫描相机】开始");
			ArrangeCameraPanels();
		}

		private void Form1_FormClosing(object sender, FormClosingEventArgs e)
		{
			try
			{
				for (int i = 0; i < 4; i++)
				{
					StopCamera(i);
					DisconnectCamera(i);
				}

				if (m_cameraShared != null)
				{
					m_cameraShared.Dispose();
					m_cameraShared = null;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("清理资源时出错: " + ex.Message);
			}
		}

		private void panelCameras_Resize(object sender, EventArgs e)
		{
			ArrangeCameraPanels();
		}

		private void ArrangeCameraPanels()
		{
			int panelWidth = (panelCameras.ClientSize.Width - 15) / 2;
			int panelHeight = (panelCameras.ClientSize.Height - 15) / 2;

			Panel[] panels = { panelCamera0, panelCamera1, panelCamera2, panelCamera3 };
			int[] xPositions = { 5, 10 + panelWidth, 5, 10 + panelWidth };
			int[] yPositions = { 5, 5, 10 + panelHeight, 10 + panelHeight };

			for (int i = 0; i < 4; i++)
			{
				panels[i].Location = new Point(xPositions[i], yPositions[i]);
				panels[i].Size = new Size(panelWidth, panelHeight);
			}
		}

		private void buttonScan_Click(object sender, EventArgs e)
		{
			try
			{
				buttonScan.Enabled = false;
				SetStatus("正在扫描相机...");

				if (m_cameraShared == null)
				{
					m_cameraShared = new CameraShared(
						Application.StartupPath + "\\log.txt",
						CameraShared.getPathToRanger3Producer(),
						true,
						false);
				}

				var scanResult = m_cameraShared.scanDevice();
				string[] ips = m_cameraShared.getConDevListIP();

				// 调试：输出扫描结果
				System.Diagnostics.Debug.WriteLine("scanResult: " + scanResult);
				System.Diagnostics.Debug.WriteLine("ips count: " + ips.Length);
				foreach (var ip in ips)
				{
					System.Diagnostics.Debug.WriteLine("IP: " + ip);
				}

				m_scannedCameras.Clear();

				foreach (var ch in m_channels)
				{
					if (ch.IsConnected && !string.IsNullOrEmpty(ch.IpAddress))
					{
						m_scannedCameras[ch.IpAddress] = ch.ChannelIndex;
					}
				}

				treeViewCameras.Nodes.Clear();
				TreeNode rootNode = new TreeNode("Ranger3 相机 (" + ips.Length + " 台)");
				rootNode.Name = "rootNode";  // 给根节点命名，便于调试
				treeViewCameras.Nodes.Add(rootNode);

				// 调试：检查 TreeView 节点数量
				System.Diagnostics.Debug.WriteLine("After Clear, Nodes.Count: " + treeViewCameras.Nodes.Count);

				foreach (string ip in ips)
				{
					if (!m_scannedCameras.ContainsKey(ip))
					{
						m_scannedCameras[ip] = -1;
					}

					string status = m_scannedCameras[ip] >= 0
						? "[已连接-通道" + (m_scannedCameras[ip] + 1) + "]"
						: "[空闲]";

					TreeNode node = new TreeNode(ip + "  " + status);
					node.Tag = ip;
					rootNode.Nodes.Add(node);
					System.Diagnostics.Debug.WriteLine("Added node: " + ip);
				}

				rootNode.Expand();

				// 调试：检查最终 TreeView 状态
				System.Diagnostics.Debug.WriteLine("Final Nodes.Count: " + treeViewCameras.Nodes.Count);
				System.Diagnostics.Debug.WriteLine("Final rootNode.Nodes.Count: " + rootNode.Nodes.Count);
				System.Diagnostics.Debug.WriteLine("TreeView Visible: " + treeViewCameras.Visible);
				System.Diagnostics.Debug.WriteLine("TreeView Enabled: " + treeViewCameras.Enabled);

				labelScanInfo.Text = "发现 " + ips.Length + " 台";
				labelScanInfo.ForeColor = ips.Length > 0 ? Color.DarkGreen : Color.Gray;

				SetStatus("扫描完成，发现 " + ips.Length + " 台相机");
			}
			catch (Exception ex)
			{
				MessageBox.Show("扫描相机出错: " + ex.Message, "错误",
					MessageBoxButtons.OK, MessageBoxIcon.Error);
				SetStatus("扫描失败: " + ex.Message);
			}
			finally
			{
				buttonScan.Enabled = true;
			}
		}

		private void treeViewCameras_DoubleClick(object sender, EventArgs e)
		{
			TreeNode selectedNode = treeViewCameras.SelectedNode;
			if (selectedNode == null || selectedNode.Tag == null)
				return;

			string ip = selectedNode.Tag as string;
			if (string.IsNullOrEmpty(ip))
				return;

			if (m_scannedCameras.ContainsKey(ip) && m_scannedCameras[ip] >= 0)
			{
				int ch = m_scannedCameras[ip];
				m_selectedChannel = ch;
				UpdateParamPanel();
				SetStatus("已选中通道 " + (ch + 1) + " - " + ip);
				return;
			}

			int freeChannel = -1;
			for (int i = 0; i < 4; i++)
			{
				if (!m_channels[i].IsConnected)
				{
					freeChannel = i;
					break;
				}
			}

			if (freeChannel < 0)
			{
				MessageBox.Show("所有通道都已占用，请先断开一台相机。", "提示",
					MessageBoxButtons.OK, MessageBoxIcon.Warning);
				return;
			}

			ConnectCamera(freeChannel, ip);
		}

		private bool ConnectCamera(int channelIndex, string ip)
		{
			try
			{
				SetStatus("通道 " + (channelIndex + 1) + " 正在连接 " + ip + "...");

				if (m_cameraShared == null)
				{
					m_cameraShared = new CameraShared(
						Application.StartupPath + "\\log.txt",
						CameraShared.getPathToRanger3Producer(),
						true,
						false);
				}

				Ranger3 camera = Ranger3.createFromIpMac(ip, true, m_cameraShared);

				if (camera == null)
				{
					MessageBox.Show("创建相机对象失败！", "错误",
						MessageBoxButtons.OK, MessageBoxIcon.Error);
					return false;
				}

				if (camera.isReady() != CAM_STATUS.All_OK)
				{
					MessageBox.Show("相机未就绪: " + camera.isReady().ToString(), "错误",
						MessageBoxButtons.OK, MessageBoxIcon.Error);
					return false;
				}

				CAM_STATUS status = camera.connectCameraWithEvent(60, 1000, 1000);
				if (status != CAM_STATUS.All_OK)
				{
					MessageBox.Show("连接相机失败: " + status.ToString(), "错误",
						MessageBoxButtons.OK, MessageBoxIcon.Error);
					return false;
				}

				camera.enableCallbackThreadsInPool(4, true);

				try
				{
					camera.setParameter("DeviceScanType", "Linescan3D");
					camera.setCalibration_No();
					camera.setBufferCount(20);
				}
				catch (Exception ex)
				{
					Console.WriteLine("设置默认参数警告: " + ex.Message);
				}

				int idx = channelIndex;
				camera.ImageReceived += (img, r3) => OnImageReceived(idx, img);

				m_channels[channelIndex].Camera = camera;
				m_channels[channelIndex].IsConnected = true;
				m_channels[channelIndex].IpAddress = ip;

				m_scannedCameras[ip] = channelIndex;

				foreach (TreeNode root in treeViewCameras.Nodes)
				{
					foreach (TreeNode node in root.Nodes)
					{
						if ((node.Tag as string) == ip)
						{
							node.Text = ip + "  [已连接-通道" + (channelIndex + 1) + "]";
						}
					}
				}

				UpdateChannelUI(channelIndex);

				m_selectedChannel = channelIndex;
				UpdateParamPanel();

				SetStatus("通道 " + (channelIndex + 1) + " 已连接 - " + ip);
				return true;
			}
			catch (Exception ex)
			{
				MessageBox.Show("连接相机时出错: " + ex.Message, "错误",
					MessageBoxButtons.OK, MessageBoxIcon.Error);
				SetStatus("连接失败: " + ex.Message);
				return false;
			}
		}

		private void DisconnectCamera(int channelIndex)
		{
			try
			{
				var ch = m_channels[channelIndex];
				if (!ch.IsConnected) return;

				if (ch.IsRunning)
				{
					StopCamera(channelIndex);
				}

				if (ch.Camera != null)
				{
					try
					{
						ch.Camera.disconnectCamera();
					}
					catch (Exception ex)
					{
						Console.WriteLine("断开相机异常: " + ex.Message);
					}
				}

				if (!string.IsNullOrEmpty(ch.IpAddress) && m_scannedCameras.ContainsKey(ch.IpAddress))
				{
					m_scannedCameras[ch.IpAddress] = -1;
				}

				foreach (TreeNode root in treeViewCameras.Nodes)
				{
					foreach (TreeNode node in root.Nodes)
					{
						if ((node.Tag as string) == ch.IpAddress)
						{
							node.Text = ch.IpAddress + "  [空闲]";
						}
					}
				}

				ch.Camera = null;
				ch.IsConnected = false;
				ch.IpAddress = "";

				if (m_selectedChannel == channelIndex)
				{
					m_selectedChannel = -1;
					UpdateParamPanel();
				}

				UpdateChannelUI(channelIndex);
				SetStatus("通道 " + (channelIndex + 1) + " 已断开");
			}
			catch (Exception ex)
			{
				MessageBox.Show("断开相机时出错: " + ex.Message, "错误",
					MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void StartCamera(int channelIndex)
		{
			try
			{
				var ch = m_channels[channelIndex];
				if (!ch.IsConnected || ch.Camera == null || ch.IsRunning) return;

				SetStatus("通道 " + (channelIndex + 1) + " 正在开始采集...");

				CAM_STATUS status = ch.Camera.StartCameraWithEvent(10);
				if (status != CAM_STATUS.All_OK)
				{
					MessageBox.Show("开始采集失败: " + status.ToString(), "错误",
						MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				ch.IsRunning = true;
				UpdateChannelUI(channelIndex);
				SetStatus("通道 " + (channelIndex + 1) + " 采集进行中");
			}
			catch (Exception ex)
			{
				MessageBox.Show("开始采集时出错: " + ex.Message, "错误",
					MessageBoxButtons.OK, MessageBoxIcon.Error);
				m_channels[channelIndex].IsRunning = false;
			}
		}

		private void StopCamera(int channelIndex)
		{
			try
			{
				var ch = m_channels[channelIndex];
				if (!ch.IsRunning) return;

				ch.IsRunning = false;

				if (ch.Camera != null)
				{
					try
					{
						ch.Camera.stopCamera();
					}
					catch (Exception ex)
					{
						Console.WriteLine("停止相机异常: " + ex.Message);
					}
				}

				UpdateChannelUI(channelIndex);
				SetStatus("通道 " + (channelIndex + 1) + " 已停止");
			}
			catch (Exception ex)
			{
				Console.WriteLine("停止采集异常: " + ex.Message);
			}
		}

		private void OnImageReceived(int channelIndex, ImgT img)
		{
			try
			{
				var ch = m_channels[channelIndex];
				if (ch == null || !ch.IsRunning) return;

				Bitmap bitmap = ConvertImgToBitmap(img);
				if (bitmap != null)
				{
					// 获取图像的 ID
					ulong imageId = img.get_ID();
					UpdatePictureBox(channelIndex, bitmap, imageId);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("通道 " + (channelIndex + 1) + " 图像处理异常: " + ex.Message);
			}
		}

		private Bitmap ConvertImgToBitmap(ImgT img)
		{
			try
			{
				uint width = img.get_cols();
				uint height = img.get_rows();

				if (width == 0 || height == 0) return null;

				byte[] data = null;

				if (img.has(DataNames.REF))
				{
					data = img.getDataByte(DataNames.REF, width);
				}

				if ((data == null || data.Length == 0) && img.has(DataNames.RAN))
				{
					ushort[] rangeData = img.getDataWord(DataNames.RAN, width);
					if (rangeData != null && rangeData.Length > 0)
					{
						data = ConvertUshortToByte(rangeData);
					}
				}

				if ((data == null || data.Length == 0) && img.has(DataNames.SEN))
				{
					try
					{
						data = img.getDataByte(DataNames.SEN, width);
					}
					catch
					{
						ushort[] senData = img.getDataWord(DataNames.SEN, width);
						if (senData != null && senData.Length > 0)
						{
							data = ConvertUshortToByte(senData);
						}
					}
				}

				if (data == null || data.Length == 0) return null;

				int expectedSize = (int)(width * height);
				if (data.Length < expectedSize)
				{
					byte[] newData = new byte[expectedSize];
					Array.Copy(data, 0, newData, 0, Math.Min(data.Length, expectedSize));
					data = newData;
				}

				return BuiltGrayBitmap(data, width, height);
			}
			catch (Exception ex)
			{
				Console.WriteLine("图像转换异常: " + ex.Message);
				return null;
			}
		}

		private byte[] ConvertUshortToByte(ushort[] data)
		{
			if (data == null || data.Length == 0) return null;

			ushort maxVal = 0;
			ushort minVal = ushort.MaxValue;
			int sampleCount = Math.Min(data.Length, 2000);
			for (int i = 0; i < sampleCount; i++)
			{
				if (data[i] > maxVal) maxVal = data[i];
				if (data[i] < minVal) minVal = data[i];
			}

			byte[] result = new byte[data.Length];
			if (maxVal == minVal)
			{
				for (int i = 0; i < data.Length; i++)
					result[i] = 128;
			}
			else
			{
				float scale = 255.0f / (maxVal - minVal);
				for (int i = 0; i < data.Length; i++)
				{
					result[i] = (byte)((data[i] - minVal) * scale);
				}
			}

			return result;
		}

		private Bitmap BuiltGrayBitmap(byte[] rawValues, uint width, uint height)
		{
			if (rawValues == null || rawValues.Length == 0) return null;

			try
			{
				Bitmap bitmap = new Bitmap((int)width, (int)height, PixelFormat.Format8bppIndexed);

				if (s_grayPalette == null)
				{
					using (Bitmap temp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
					{
						s_grayPalette = temp.Palette;
					}
					for (int i = 0; i < 256; i++)
					{
						s_grayPalette.Entries[i] = Color.FromArgb(i, i, i);
					}
				}
				bitmap.Palette = s_grayPalette;

				BitmapData bmpData = bitmap.LockBits(
					new Rectangle(0, 0, (int)width, (int)height),
					ImageLockMode.WriteOnly,
					PixelFormat.Format8bppIndexed);

				int copySize = Math.Min(rawValues.Length, bmpData.Stride * (int)height);
				Marshal.Copy(rawValues, 0, bmpData.Scan0, copySize);

				bitmap.UnlockBits(bmpData);
				return bitmap;
			}
			catch (Exception ex)
			{
				Console.WriteLine("创建Bitmap异常: " + ex.Message);
				return null;
			}
		}

		private void UpdatePictureBox(int channelIndex, Bitmap bitmap, ulong imageId)
		{
			PictureBox pb = null;
			switch (channelIndex)
			{
				case 0: pb = pictureBox0; break;
				case 1: pb = pictureBox1; break;
				case 2: pb = pictureBox2; break;
				case 3: pb = pictureBox3; break;
			}

			if (pb == null || pb.IsDisposed) return;

			if (pb.InvokeRequired)
			{
				pb.BeginInvoke(new Action(() => UpdatePictureBoxSafe(pb, bitmap, channelIndex, imageId)));
			}
			else
			{
				UpdatePictureBoxSafe(pb, bitmap, channelIndex, imageId);
			}
		}

		private void UpdatePictureBoxSafe(PictureBox pb, Bitmap bitmap, int channelIndex, ulong imageId)
		{
			try
			{
				Panel emptyPanel = null;
				switch (channelIndex)
				{
					case 0: emptyPanel = panelEmpty0; break;
					case 1: emptyPanel = panelEmpty1; break;
					case 2: emptyPanel = panelEmpty2; break;
					case 3: emptyPanel = panelEmpty3; break;
				}
				if (emptyPanel != null) emptyPanel.Visible = false;

				Image oldImage = pb.Image;
				pb.Image = bitmap;
				if (oldImage != null && oldImage != bitmap)
				{
					oldImage.Dispose();
				}

				Label infoLabel = null;
				switch (channelIndex)
				{
					case 0: infoLabel = labelCamera0Info; break;
					case 1: infoLabel = labelCamera1Info; break;
					case 2: infoLabel = labelCamera2Info; break;
					case 3: infoLabel = labelCamera3Info; break;
				}

				if (infoLabel != null)
				{
					var ch = m_channels[channelIndex];
					infoLabel.Text = "通道" + (channelIndex + 1) + " [" + ch.IpAddress + "] #" + imageId;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("更新PictureBox异常: " + ex.Message);
			}
		}

		private void UpdateChannelUI(int channelIndex)
		{
			Button startBtn, stopBtn, disconnectBtn;
			Label infoLabel;
			Panel emptyPanel;

			switch (channelIndex)
			{
				case 0:
					startBtn = buttonStart0; stopBtn = buttonStop0;
					disconnectBtn = buttonDisconnect0; infoLabel = labelCamera0Info;
					emptyPanel = panelEmpty0;
					break;
				case 1:
					startBtn = buttonStart1; stopBtn = buttonStop1;
					disconnectBtn = buttonDisconnect1; infoLabel = labelCamera1Info;
					emptyPanel = panelEmpty1;
					break;
				case 2:
					startBtn = buttonStart2; stopBtn = buttonStop2;
					disconnectBtn = buttonDisconnect2; infoLabel = labelCamera2Info;
					emptyPanel = panelEmpty2;
					break;
				case 3:
					startBtn = buttonStart3; stopBtn = buttonStop3;
					disconnectBtn = buttonDisconnect3; infoLabel = labelCamera3Info;
					emptyPanel = panelEmpty3;
					break;
				default:
					return;
			}

			var ch = m_channels[channelIndex];

			if (this.InvokeRequired)
			{
				this.BeginInvoke(new Action(() =>
				{
					startBtn.Enabled = ch.IsConnected && !ch.IsRunning;
					stopBtn.Enabled = ch.IsConnected && ch.IsRunning;
					disconnectBtn.Enabled = ch.IsConnected;

					if (!ch.IsConnected)
					{
						infoLabel.Text = "通道" + (channelIndex + 1) + " [空闲]";
						emptyPanel.Visible = true;
					}
					else if (!ch.IsRunning)
					{
						infoLabel.Text = "通道" + (channelIndex + 1) + " [" + ch.IpAddress + "] [已连接]";
						emptyPanel.Visible = true;
					}
				}));
			}
			else
			{
				startBtn.Enabled = ch.IsConnected && !ch.IsRunning;
				stopBtn.Enabled = ch.IsConnected && ch.IsRunning;
				disconnectBtn.Enabled = ch.IsConnected;

				if (!ch.IsConnected)
				{
					infoLabel.Text = "通道" + (channelIndex + 1) + " [空闲]";
					emptyPanel.Visible = true;
				}
				else if (!ch.IsRunning)
				{
					infoLabel.Text = "通道" + (channelIndex + 1) + " [" + ch.IpAddress + "] [已连接]";
					emptyPanel.Visible = true;
				}
			}
		}

		private void buttonStart0_Click(object sender, EventArgs e) { StartCamera(0); }
		private void buttonStart1_Click(object sender, EventArgs e) { StartCamera(1); }
		private void buttonStart2_Click(object sender, EventArgs e) { StartCamera(2); }
		private void buttonStart3_Click(object sender, EventArgs e) { StartCamera(3); }

		private void buttonStop0_Click(object sender, EventArgs e) { StopCamera(0); }
		private void buttonStop1_Click(object sender, EventArgs e) { StopCamera(1); }
		private void buttonStop2_Click(object sender, EventArgs e) { StopCamera(2); }
		private void buttonStop3_Click(object sender, EventArgs e) { StopCamera(3); }

		private void buttonDisconnect0_Click(object sender, EventArgs e) { DisconnectCamera(0); }
		private void buttonDisconnect1_Click(object sender, EventArgs e) { DisconnectCamera(1); }
		private void buttonDisconnect2_Click(object sender, EventArgs e) { DisconnectCamera(2); }
		private void buttonDisconnect3_Click(object sender, EventArgs e) { DisconnectCamera(3); }

		private void contextMenuStripTree_Opening(object sender, CancelEventArgs e)
		{
			TreeNode selectedNode = treeViewCameras.SelectedNode;
			if (selectedNode == null || selectedNode.Tag == null)
			{
				e.Cancel = true;
				return;
			}

			string ip = selectedNode.Tag as string;
			if (string.IsNullOrEmpty(ip))
			{
				e.Cancel = true;
				return;
			}

			bool isConnected = m_scannedCameras.ContainsKey(ip) && m_scannedCameras[ip] >= 0;
			menuItemOpen.Enabled = !isConnected;
			menuItemClose.Enabled = isConnected;
		}

		private void menuItemOpen_Click(object sender, EventArgs e)
		{
			treeViewCameras_DoubleClick(sender, EventArgs.Empty);
		}

		private void menuItemClose_Click(object sender, EventArgs e)
		{
			TreeNode selectedNode = treeViewCameras.SelectedNode;
			if (selectedNode == null || selectedNode.Tag == null) return;

			string ip = selectedNode.Tag as string;
			if (string.IsNullOrEmpty(ip)) return;

			if (m_scannedCameras.ContainsKey(ip) && m_scannedCameras[ip] >= 0)
			{
				DisconnectCamera(m_scannedCameras[ip]);
			}
		}

		private void UpdateParamPanel()
		{
			if (m_selectedChannel >= 0 && m_selectedChannel < 4 && m_channels[m_selectedChannel].IsConnected)
			{
				var ch = m_channels[m_selectedChannel];
				labelParamTitle.Text = "相机参数 - 通道" + (m_selectedChannel + 1) + " (" + ch.IpAddress + ")";
				buttonRefreshParam.Enabled = true;
				buttonApplyParam.Enabled = false;

				// 创建参数容器用于显示所有相机参数
				CameraParameterBag paramBag = new CameraParameterBag();

				try
				{
					// 使用 SDK 接口动态获取相机参数
					ch.Camera.updateParamtersInfo();
					StringVector categories = ch.Camera.paramCategoryList();
					System.Diagnostics.Debug.WriteLine(string.Format("发现 {0} 个参数组", categories.Count));

					foreach (string category in categories)
					{
						try
						{
							StringVector paramNames = ch.Camera.paramFullNameOfCategory(category);
							System.Diagnostics.Debug.WriteLine(string.Format("参数组 [{0}] 包含 {1} 个参数", category, paramNames.Count));

							foreach (string paramName in paramNames)
							{
								try
								{
									string value = ch.Camera.paramCurrentValue(paramName);
									System.Diagnostics.Debug.WriteLine(string.Format("  {0} = {1}", paramName, value));

									// 获取参数的可选值
									List<string> options = null;
									try
									{
										int retVal = 0;
										StringVector optionValues = ch.Camera.paramOptionalValues(paramName, out retVal);
										if (optionValues != null && optionValues.Count > 0)
										{
											options = new List<string>();
											foreach (string opt in optionValues)
											{
												options.Add(opt);
											}
											System.Diagnostics.Debug.WriteLine(string.Format("    可选值: {0}", string.Join(", ", options)));
										}
									}
									catch
									{
										// 某些参数可能没有可选值，忽略异常
									}

									// 添加到参数容器（保留原始参数名，按类别分组）
									paramBag.AddParameter(category, paramName, value, options);
								}
								catch (Exception ex)
								{
									System.Diagnostics.Debug.WriteLine(string.Format("    获取参数值失败: {0}", ex.Message));
								}
							}
						}
						catch (Exception ex)
						{
							System.Diagnostics.Debug.WriteLine(string.Format("获取参数组 {0} 失败: {1}", category, ex.Message));
						}
					}
				}
				catch (Exception ex)
				{
					System.Diagnostics.Debug.WriteLine(string.Format("动态获取参数失败: {0}", ex.Message));
				}

				// 显示所有相机参数
				propertyGridParams.SelectedObject = paramBag;
			}
			else
			{
				labelParamTitle.Text = "相机参数（请双击打开相机）";
				propertyGridParams.SelectedObject = null;
				buttonRefreshParam.Enabled = false;
				buttonApplyParam.Enabled = false;
			}
		}

		private void propertyGridParams_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
		{
			if (propertyGridParams.SelectedObject is CameraParameterBag)
			{
				buttonApplyParam.Enabled = true;
			}
		}

		private void buttonRefreshParam_Click(object sender, EventArgs e)
		{
			UpdateParamPanel();
			SetStatus("参数面板已刷新");
		}

		private void buttonApplyParam_Click(object sender, EventArgs e)
		{
			if (m_selectedChannel < 0 || !m_channels[m_selectedChannel].IsConnected)
				return;

			var paramBag = propertyGridParams.SelectedObject as CameraParameterBag;
			if (paramBag == null) return;

			try
			{
				var ch = m_channels[m_selectedChannel];
				SetStatus("通道 " + (m_selectedChannel + 1) + " 正在应用参数...");

				// 获取所有参数并应用
				var parameters = paramBag.GetParameters();
				foreach (var param in parameters)
				{
					try
					{
						ch.Camera.setParameter(param.Key, param.Value);
					}
					catch (Exception ex)
					{
						Console.WriteLine("设置参数 " + param.Key + " 失败: " + ex.Message);
					}
				}

				buttonApplyParam.Enabled = false;
				SetStatus("通道 " + (m_selectedChannel + 1) + " 参数已应用");
				MessageBox.Show("参数已应用。如需生效，请重启采集。", "提示",
					MessageBoxButtons.OK, MessageBoxIcon.Information);
			}
			catch (Exception ex)
			{
				MessageBox.Show("应用参数时出错: " + ex.Message, "错误",
					MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void SetStatus(string message)
		{
			if (this.InvokeRequired)
			{
				this.BeginInvoke(new Action(() => toolStripStatusLabel.Text = message));
			}
			else
			{
				toolStripStatusLabel.Text = message;
			}
		}
	}

	public class CameraParams
	{
		public CameraParams()
		{
			AdditionalParams = "";
		}

		[Category("自定义参数")]
		[DisplayName("自定义参数")]
		[Description("每行一个参数，格式: 参数名=值。例如:\nWidth=2560\nHeight=500\nGain=1.5")]
		public string AdditionalParams { get; set; }
	}

	public class CameraParameterBag : System.ComponentModel.ICustomTypeDescriptor
	{
		private Dictionary<string, string> parameters = new Dictionary<string, string>();
		private Dictionary<string, string> categories = new Dictionary<string, string>();
		private Dictionary<string, List<string>> optionalValues = new Dictionary<string, List<string>>();

		public void AddParameter(string category, string name, string value, List<string> options = null)
		{
			string fullName = name;
			parameters[fullName] = value;
			categories[fullName] = category;
			if (options != null && options.Count > 0)
			{
				optionalValues[fullName] = options;
			}
		}

		public string this[string name]
		{
			get { return parameters.ContainsKey(name) ? parameters[name] : string.Empty; }
			set { parameters[name] = value; }
		}

		public Dictionary<string, string> GetParameters()
		{
			return parameters;
		}

		public List<string> GetOptionalValues(string name)
		{
			return optionalValues.ContainsKey(name) ? optionalValues[name] : null;
		}

		#region ICustomTypeDescriptor Members

		public System.ComponentModel.AttributeCollection GetAttributes()
		{
			return new System.ComponentModel.AttributeCollection(null);
		}

		public string GetClassName()
		{
			return "相机参数";
		}

		public string GetComponentName()
		{
			return null;
		}

		public System.ComponentModel.TypeConverter GetConverter()
		{
			return null;
		}

		public System.ComponentModel.EventDescriptor GetDefaultEvent()
		{
			return null;
		}

		public System.ComponentModel.PropertyDescriptor GetDefaultProperty()
		{
			return null;
		}

		public object GetEditor(Type editorBaseType)
		{
			return null;
		}

		public System.ComponentModel.EventDescriptorCollection GetEvents(System.Attribute[] attributes)
		{
			return new System.ComponentModel.EventDescriptorCollection(null);
		}

		public System.ComponentModel.EventDescriptorCollection GetEvents()
		{
			return new System.ComponentModel.EventDescriptorCollection(null);
		}

		public System.ComponentModel.PropertyDescriptorCollection GetProperties(System.Attribute[] attributes)
		{
			List<System.ComponentModel.PropertyDescriptor> props = new List<System.ComponentModel.PropertyDescriptor>();

			foreach (var param in parameters)
			{
				string category = categories.ContainsKey(param.Key) ? categories[param.Key] : "其他";
				List<string> options = optionalValues.ContainsKey(param.Key) ? optionalValues[param.Key] : null;
				props.Add(new DynamicPropertyDescriptor(param.Key, param.Value, category, options));
			}

			return new System.ComponentModel.PropertyDescriptorCollection(props.ToArray());
		}

		public System.ComponentModel.PropertyDescriptorCollection GetProperties()
		{
			return GetProperties(null);
		}

		public object GetPropertyOwner(System.ComponentModel.PropertyDescriptor pd)
		{
			return this;
		}

		#endregion
	}

	public class DynamicPropertyDescriptor : System.ComponentModel.PropertyDescriptor
	{
		private string value;
		private string category;
		private List<string> options;

		public DynamicPropertyDescriptor(string name, string value, string category, List<string> options = null)
			: base(name, null)
		{
			this.value = value;
			this.category = category;
			this.options = options;
		}

		public override bool CanResetValue(object component)
		{
			return false;
		}

		public override Type ComponentType
		{
			get { return typeof(CameraParameterBag); }
		}

		public override System.ComponentModel.TypeConverter Converter
		{
			get
			{
				if (options != null && options.Count > 0)
				{
					return new ParameterOptionsConverter(options);
				}
				return base.Converter;
			}
		}

		public override object GetValue(object component)
		{
			CameraParameterBag bag = component as CameraParameterBag;
			return bag != null ? bag[this.Name] : string.Empty;
		}

		public override bool IsReadOnly
		{
			get { return false; }
		}

		public override Type PropertyType
		{
			get { return typeof(string); }
		}

		public override void ResetValue(object component)
		{
		}

		public override void SetValue(object component, object value)
		{
			CameraParameterBag bag = component as CameraParameterBag;
			if (bag != null)
			{
				bag[this.Name] = value != null ? value.ToString() : string.Empty;
			}
		}

		public override bool ShouldSerializeValue(object component)
		{
			return true;
		}

		public override string Category
		{
			get { return category; }
		}

		public override string Description
		{
			get { return "相机参数: " + this.Name; }
		}
	}

	public class ParameterOptionsConverter : System.ComponentModel.StringConverter
	{
		private List<string> options;

		public ParameterOptionsConverter(List<string> options)
		{
			this.options = options;
		}

		public override bool GetStandardValuesSupported(System.ComponentModel.ITypeDescriptorContext context)
		{
			return true;
		}

		public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(System.ComponentModel.ITypeDescriptorContext context)
		{
			return new System.ComponentModel.TypeConverter.StandardValuesCollection(options);
		}

		public override bool GetStandardValuesExclusive(System.ComponentModel.ITypeDescriptorContext context)
		{
			return true;
		}
	}
}

// 单元测试 
//#define UnitTest

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using SICK3D;
using SICK3D.TRI;


namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {
        
        #region // Global variables. 全局变量

        public GenICam3D_Tri
                            m_TriShared = new GenICam3D_Tri("", GenICam3D_Tri.getPathToTrispectorProducer(), false, false);
        public String       m_ip        = "";                                   // IP of Current selected Trispector.

        public Dictionary<String, Trispector>   
                            m_list_tris   = new Dictionary<String, Trispector>();    // Key=SN, Value=Trispector

        public DataNames    m_GrabMode  = DataNames.RAN_CAL;                  
        public System.Timers.Timer 
                            myTimer     = new System.Timers.Timer();

        int                 m_reqStop   = 2;                                     // 0_req, 1_finish, 2_init 
        CSImgT              m_lastImg   = new CSImgT();

        private readonly Object m_imgLocker = new object();
        private readonly Object m_logLocker = new object();

        #endregion


        public frmMain()
        {
            InitializeComponent();
            myTimer.Interval = 10;
            myTimer.Elapsed += GrabImageTimer;

        }

        // In order to release image data
        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hObject);

        private void GrabImageTimer(object sender, System.Timers.ElapsedEventArgs e)
        {
            // 响应停止请求
            if (m_reqStop == 0)
            {
                m_reqStop = 1;
                myTimer.Enabled = false;
            }

            if (Tri(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED)
            {
                CSImgT imgTable = new CSImgT();
                if (Tri(m_ip).getImageData(imgTable) == CAM_STATUS.All_OK)
                {

                    Bitmap imgGray;

                    // 读取图片 convert image to show
                    var mode = GetGrabMode();
                    if (!imgTable.has(mode))
                        return;

                    switch (mode)
                    {
                        case DataNames.REF_CAL:
                            imgGray = BuiltGrayBitmap(
                                imgTable.getDataByte(mode),
                                imgTable.get_cols(),
                                imgTable.get_rows()
                                );
                            break;
                        case DataNames.RAN_CAL:
                            imgGray = BuiltGrayBitmap(
                                imgTable.getDataFloat(mode),
                                imgTable.get_cols(),
                                imgTable.get_rows()
                                );
                            break;
                        default:
                            imgTable.Dispose();
                            return;
                    }

                    // 显示图片 Show Image
                    var hBitmap = imgGray.GetHbitmap();
                    System.Drawing.Image imgShow = System.Drawing.Image.FromHbitmap(hBitmap);
                    pictureBox1.BackgroundImage = imgShow.GetThumbnailImage(pictureBox1.Size.Width, pictureBox1.Size.Height, null, IntPtr.Zero);

                    DeleteObject(hBitmap);
                    imgShow.Dispose();
                    imgGray.Dispose();

                    lock (m_imgLocker)
                    {
                        m_lastImg._deepCopy(imgTable);
                    }

                    // 响应停止请求 On stopping require.
                    if (m_reqStop == 0)
                    {
                        m_reqStop = 1;
                        myTimer.Enabled = false;
                    }

                    // show ID to start button
                    updateIDToButton(imgTable.get_ID().ToString());

                }
                imgTable.Dispose();
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            FindCamera();
            if (Tri(m_ip) != null && Tri(m_ip).isReady() == CAM_STATUS.All_OK)
            {
                toolStripComboBoxMode.SelectedIndex = 1;
            }
        }

        private void toolStripButtonFindCamera_Click(object sender, EventArgs e)
        {
            {
                treeView1.Nodes.Clear();
                m_list_tris.Clear();
            }
            FindCamera();
            if (Tri(m_ip) != null && Tri(m_ip).isReady() == CAM_STATUS.All_OK)
            {
                toolStripComboBoxMode.SelectedIndex = Convert.ToInt32(DataNames.RAN_CAL);
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
        
        private void toolStripComboBoxMode_Click(object sender, EventArgs e)
        {
            var status = Tri(m_ip).getStatus();
            if (status == CAM_STATUS.CAM_IS_STARTED)
                Stop();

            if (status == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                if(m_GrabMode == DataNames.REF_CAL)
                    toolStripComboBoxMode.SelectedIndex = 2;
                else
                    toolStripComboBoxMode.SelectedIndex = 1; 
            }

            // check if Grab Mode match parameter setting.
            // Grab Mode is under control by parameter setting.
            var lstGrabMode = m_GrabMode;

            m_GrabMode = (DataNames)(toolStripComboBoxMode.SelectedIndex + 6);
            if (m_GrabMode == DataNames.REF_CAL || m_GrabMode == DataNames.RAN_CAL)
            {
                if (Tri(m_ip).getStatus() == CAM_STATUS.CAM_IS_STOPPED) /// does not support connected on purpose! 
                    Start();
            }
            else
            {
                toolStripComboBoxMode.SelectedIndex = (int)lstGrabMode; /// revert to last avalible value
            }

        }

        private void frmMain_FormClosed(object sender, FormClosedEventArgs e)
        {
            _Stop();
            _Disconnection();
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
            }
            else if (sta == CAM_STATUS.CAM_IS_STOPPED)
            {
                toolStripButtonStop.ForeColor = Color.Red;
            }
        }

        private void wFlog(String str)
        {
            lock (m_logLocker)
            {
                textBox1.Invoke(new EventHandler(delegate
                {
                    textBox1.Text = textBox1.Text + str + "\r\n\r\n";
                    textBox1.Select(textBox1.TextLength, 0);
                    textBox1.ScrollToCaret();
                }));
            }

        }

        private void updateIDToButton(String str)
        {
            lock (m_logLocker)
            {
                toolStrip1.Invoke(new EventHandler(delegate
                {
                    toolStripButtonStart.Text = "Start(" + str + ")";
                }));
            }

        }

        private void btnSaveBuffer_Click(object sender, EventArgs e)
        {
            if (!SaveImage())
                MessageBox.Show("Save as ICON failed! 保存失败！");
            else
                MessageBox.Show("Save as ICON ok! 保存成功！");
        }

        private void 打开APIHelpToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start(@"GenICam_CSharp_API.chm");
        }

        private void btnLoadBuffer_Click(object sender, EventArgs e)
        {
            CSImgT img = LoadImage();
            if (!img.isEmpty())
            {
                var dnList = img.getAvalibleDataNames();
                foreach(var dn in dnList)
                {
                    Form f = new Form();
                    Bitmap src = new Bitmap(1, 1);

                    if (dn == DataNames.SEN)
                    {
                        f.Text = "激光线图像 | Sensor Image";
                        src = BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(), img.get_rows());
                    }
                    else if (dn == DataNames.RAN)
                    {
                        f.Text = "高度图像 | Range Image";
                        src = BuiltGrayBitmap(img.getDataWord(dn), img.get_cols(), img.get_rows());
                    }
                    else if (dn == DataNames.RAN_CAL)
                    {
                        f.Text = "标定后的高度图像 | Calibrated Range Image";
                        src = BuiltGrayBitmap(img.getDataFloat(dn), img.get_cols(), img.get_rows());
                    }
                    else if (dn == DataNames.REF)
                    {
                        f.Text = "激光强度图像 | Reflectance Image";
                        src = BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(), img.get_rows());
                    }
                    else if (dn == DataNames.REF_CAL)
                    {
                        f.Text = "标定后的激光强度图像 | Calibrated Reflectance Image";
                        src = BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(), img.get_rows());
                    }
                    else
                    {
                        continue;
                    }

                    IntPtr pp = new IntPtr();
					bool is_world_rate = false;
                    Size imgSize = new Size();
					if(is_world_rate)
					{
                        /*
						// 缩放图像到物理世界的长宽比例
	                    double imgXres = img.getC_scaleX();
	                    double imgYres = ; // 请输入正确的Y分辨率
	                    int width = src.Width;
	                    int height = src.Height;
	                    int radio = 1;
	                    int radio2 = 1;
	                    if (img.getC_scaleX()>=img.getC_scaleY())
	                    {

	                        width = (int)Math.Round((img.getC_scaleX() * src.Width) / imgYres);
	                        radio2=(int)Math.Ceiling(Math.Max((float)width / 1366, (float)height / 768));
	                        height/=radio2;
	                        width /=radio2;
	                    }
	                    else
	                    {
	                        height = (int)Math.Round((img.getC_scaleY() * src.Height) / imgYres);
	                        radio2 = (int)Math.Ceiling(Math.Max((float)width / 1366, (float)height / 768));
	                        height /= radio2;
	                        width /= radio2;
	                    }
                        imgSize = new Size(width, height);
                        */
					}
					else
					{
	                    // 按照像素比例显示
						imgSize = new Size(src.Width/3, src.Height/3);
					}
					
					// 显示
                    f.BackgroundImage = src.GetThumbnailImage(imgSize.Width, imgSize.Height, null, pp);
                    f.Size = new Size(imgSize.Width+30, imgSize.Height + 30);
                    f.MaximumSize = new Size(imgSize.Width+30, imgSize.Height + 30);
                    f.Show();

                    DeleteObject(pp);
                    src.Dispose();
                    GC.Collect();
                    GC.WaitForPendingFinalizers();
                }
            }
            img.Dispose();
            GC.Collect();
            GC.WaitForPendingFinalizers();
        }

        private void 版本VersionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var apiVersion = new SICK3D.VER();
            var vers = apiVersion.__Version__();
            MessageBox.Show(vers, "SDK Version Information");
        }

    }


}

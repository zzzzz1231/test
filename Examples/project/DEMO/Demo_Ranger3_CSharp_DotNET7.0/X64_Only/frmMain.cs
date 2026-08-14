// 单元测试 
//#define UnitTest

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using SICK3D;
using System.IO;

namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {

        #region // Global variables. 全局变量

        const int STATUS_BIT_ENABLE         = 30;
        const int STATUS_BIT_ENCODER_A      = 28;
        const int STATUS_BIT_ENCODER_B      = 27;
        const int STATUS_BIT_OVERTRIG       = 16;
        const int STATUS_BIT_LINE_TRIGGER   = 25;
        const int STATUS_BIT_ENCODER_RESET  = 24;



        public GenICam3D    m_r3s       = new GenICam3D("", GenICam3D.getPathToRanger3Producer(), false, false);
        public String       m_ip        = "";                                   // IP of Current selected Ranger3.

        public Dictionary<String, Ranger3>   
                            m_list_r3   = new Dictionary<String, Ranger3>();    // Key=IP, Value=CSR3

        public DataNames    m_GrabMode  = DataNames.SEN;                  
        public System.Timers.Timer 
                            myTimer     = new System.Timers.Timer();

        int                 m_reqStop   = 2;                                     // 0_req, 1_finish, 2_init 
        CSImgT              m_lastImg   = new CSImgT();
        XProps              m_PropsParam= new XProps();

        private readonly Object m_imgLocker = new object();
        private readonly Object m_logLocker = new object();

        bool                m_isSaveChnukData = false;


        #endregion


        public frmMain()
        {
            InitializeComponent();
            myTimer.Interval = 10;
            myTimer.Elapsed += GrabImageTimer;

#if UnitTest
            unitTestToolStripMenuItem.Enabled = true;
#else
            unitTestToolStripMenuItem.Enabled = false;
#endif
        }

        // In order to release image data
        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hObject);

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

		private void GrabImageTimer(object sender, System.Timers.ElapsedEventArgs e)
        {
            // 响应停止请求
            if (m_reqStop == 0)
            {
                m_reqStop = 1;
                myTimer.Enabled = false;
            }
            lock (m_imgLocker)
            {
                if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED)
                {
                    CSImgT imgTable = new CSImgT();
                    if (R3(m_ip).getImageData(imgTable) == CAM_STATUS.All_OK)
                    {
                        Bitmap imgGray;

                        // 读取图片 convert image to show
                        var mode = GetGrabMode();
                        if (!imgTable.has(mode))
                        {
                            wFlog(DateTime.Now + " 没有这个图像：" + mode.ToString());
                            return;
                        }

                        switch (mode)
                        {
                            case DataNames.SEN:
                                imgGray = BuiltGrayBitmap(
                                    imgTable.getDataByte(mode),
                                    imgTable.get_cols(),
                                    imgTable.get_rows()
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
                                imgGray = BuiltGrayBitmap(
                                    imgTable.getDataByte(mode),
                                    imgTable.get_cols(),
                                    imgTable.get_rows()
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
                                imgGray = BuiltGrayBitmap(
                                    imgTable.getDataWord(mode),
                                    imgTable.get_cols(),
                                    imgTable.get_rows()
                                    );
                                break;
                            case DataNames.RAN_X:
                            case DataNames.RAN_CAL:
                            case DataNames.RAN2_CAL:
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


                        // save mark info if needed
                        if (m_isSaveChnukData)
                        {
                            m_isSaveChnukData = false;
                            if (imgTable.has(SICK3D.DataNames.MAR))
                            {
                                uint[] mark = imgTable.getDataMark();
                                var path = "D:/chunkdat_" + imgTable.get_ID().ToString() + ".txt";
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


                        // 显示图片 Show Image
                        var hBitmap = imgGray.GetHbitmap();
                        System.Drawing.Image imgShow = System.Drawing.Image.FromHbitmap(hBitmap);


                        pictureBox1.BackgroundImage = imgShow.GetThumbnailImage(
                            pictureBox1.Size.Width,
                            pictureBox1.Size.Height,
                            null, IntPtr.Zero);

                        //double Y_X = imgTable.getC_scaleY() / imgTable.getC_scaleX(); // 分辨率之比
                        //pictureBox1.BackgroundImage = imgShow.GetThumbnailImage(
                        //    pictureBox1.Size.Width, // 假定以显示框的宽度为基准
                        //    Convert.ToInt32(Y_X * Convert.ToDouble(pictureBox1.Size.Height)), // 给定显示框的宽度，计算对应的高度
                        //    null, IntPtr.Zero);


                        DeleteObject(hBitmap);
                        imgShow.Dispose();
                        imgGray.Dispose();

                        //lock (m_imgLocker)
                        //{
                        m_lastImg._deepCopy(imgTable);
                        //}

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
                String optStr = "";
                for (int i = 0; i < conv.Length; ++i)
                {
                    optStr = optStr + conv[i];
                    if (i + 1 < conv.Length)
                        optStr = optStr + ",";
                }

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
            }
            FindCamera();
            if (/*R3(m_ip) != null && */R3(m_ip).isReady() == CAM_STATUS.All_OK)
            {
                toolStripComboBoxMode.SelectedIndex = Convert.ToInt32(DataNames.SEN);
                //m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
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
            // Check Grab Mode and parameters. The Grab Mode shoud be modified to match current parameters.
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

                    // 检查标定文件
                    // Check calibraton file
                    //if (mode == DataNames.REF_CAL || mode == DataNames.RAN_CAL || mode == DataNames.REF2_CAL || mode == DataNames.RAN2_CAL)
                    //{
                    //    if (R3(m_ip).getCalibraPath() == "")
                    //    {
                    //        MessageBox.Show("没找到标定文件，请手动导入！ \n\n\nNot Calibration File found!!!", "ERROR");
                    //        return false;
                    //    }
                    //}

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
                uint time = uint.Parse(textBoxSleepTime.Text); // if test is not a number, it will crash!!
                R3(m_ip).setGrabSleepTime(time);
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
                R3(m_ip).setCalibration_inDevice(2560, true, false);
            }
            else if (btnCalibrateInDevice.Text == "CalibrateInDevice(Yes, rectifyC, range uint16)")
            {
                btnCalibrateInDevice.Text = "CalibrateInDevice(Yes, rectifyC, range float)";
                R3(m_ip).setCalibration_inDevice(2560, false, false);
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
	}


}

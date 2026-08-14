//#define CAL_IN_PC

using System;
using System.Windows.Forms;
using System.Threading;
using SICK3D;

namespace SampleApiDllCSharpWinForm
{
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
                String[] IPs = m_r3s.getConDevListIP();
                for (int i = 0; i < IPs.Length; i++)
                {
                    TreeNode nodeChild = new TreeNode(IPs[i]);
                    nodeRoot.Nodes.Add(nodeChild);

                    m_list_r3.Add(IPs[i], new Ranger3(IPs[i], true));
                }

                m_ip = (IPs.Length > 0) ? IPs[0] : "" ;
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

			// todo: enableCorrectRawImageEdgeEffect() ---
			R3(m_ip).enableCorrectRawImageEdgeEffect(false);
            
            status = R3(m_ip).startCamera();
			myTimer.Enabled = status == CAM_STATUS.All_OK;
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);
            return status;
        }

        private CAM_STATUS _Stop()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            m_reqStop = 0; // send requirement
            while (m_reqStop == 0 && myTimer.Enabled) { Thread.Sleep(5); };
            m_reqStop = 2;

            status = R3(m_ip).stopCamera();
            myTimer.Enabled = !(status == CAM_STATUS.All_OK);
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);
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

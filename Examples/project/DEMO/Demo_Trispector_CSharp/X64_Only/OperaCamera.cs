using System;
using System.Windows.Forms;
using System.Threading;
using SICK3D;
using SICK3D.TRI;

namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {
        private Trispector Tri(String sn)
        {
            if (m_list_tris.ContainsKey(sn))
                return m_list_tris[sn];

            return null; // Invalid Trispector object. Please note, do not return null
        }

        private void FindCamera()
        {
            CAM_STATUS scanRes = CAM_STATUS.DEFAULT;
            if (m_TriShared.isCtiFound())
            {
                scanRes = m_TriShared.scanDevice();
            }

            if (scanRes == CAM_STATUS.All_OK)
            {
                TreeNode nodeRoot = new TreeNode("Trispector");
                treeView1.Nodes.Add(nodeRoot);
                String[] SNs = m_TriShared.getConDevListSN();
                for (int i = 0; i < SNs.Length; i++)
                {
                    TreeNode nodeChild = new TreeNode(SNs[i]);
                    nodeRoot.Nodes.Add(nodeChild);

                    m_list_tris.Add(SNs[i], new Trispector(SNs[i]));
                }

                m_ip = (SNs.Length > 0) ? SNs[0] : "" ;
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

            status = Tri(m_ip).connectCamera(true);
            btnCameraStatus(Tri(m_ip).getStatus());
            Thread.Sleep(5);
            if (status != CAM_STATUS.All_OK) return status;

            return status;
        }

        private CAM_STATUS _Disconnection()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            if (Tri(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED)
            {
                MessageBox.Show("Please Stop Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_STARTED;
            }
            status = Tri(m_ip).disconnectCamera();
            btnCameraStatus(Tri(m_ip).getStatus());
            Thread.Sleep(5);

            return status;
       }

        private CAM_STATUS _Start()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            if (Tri(m_ip).getStatus() == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                MessageBox.Show("Please Connect Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_DISCONNECTED;
            }

            status = Tri(m_ip).startCamera();
            myTimer.Enabled = status == CAM_STATUS.All_OK;
            btnCameraStatus(Tri(m_ip).getStatus());
            Thread.Sleep(5);
            return status;
        }

        private CAM_STATUS _Stop()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            m_reqStop = 0; // send requirement
            while (m_reqStop == 0 && myTimer.Enabled) { Thread.Sleep(5); };
            m_reqStop = 2;

            status = Tri(m_ip).stopCamera();
            myTimer.Enabled = !(status == CAM_STATUS.All_OK);
            btnCameraStatus(Tri(m_ip).getStatus());
            Thread.Sleep(5);
            return status;
        }

        private bool SaveImage()
        {
            _Stop();

            string strConfigPath = string.Empty;
            SaveFileDialog ofd = new SaveFileDialog();
            ofd.Title = "Please select a path to save image / 请选择保存路径";
            ofd.InitialDirectory = Application.StartupPath;
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return false;

            //return m_lastImg.SaveToIconFile(strConfigPath);
            return m_lastImg.SaveCalibratedImagesToIconFile(strConfigPath, false, false);
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
            if (false == img.loadFromIconFile(strConfigPath, 1.0, 1.0, false))
                MessageBox.Show("加载失败！！ \n\n\nLoad Image failed!!");

            return img;
        }

        private DataNames GetGrabMode()
        {
            return m_GrabMode;
        }

        private CAM_STATUS _checkR3()
        {
            if(!m_TriShared.isAvaliable())
                return CAM_STATUS.ERROR_NULL_DEV_HANDLE;

            if (m_ip == "" || Tri(m_ip) == null)
            {
                MessageBox.Show("Please select a camera first!!");
                return CAM_STATUS.ERROR_NULL_PTR_DEV;
            };

            if (Tri(m_ip).isReady() != CAM_STATUS.All_OK)
            {
                MessageBox.Show("Camera is not ready!!");
                return CAM_STATUS.ERROR_NULL_PTR_DEV;
            }
            return CAM_STATUS.All_OK;
        }

        private CAM_STATUS _checkR3_is_connected_or_stopped()
        {
            if (Tri(m_ip).getStatus() == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                MessageBox.Show("Please Connect Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_DISCONNECTED;
            }

            if (Tri(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED)
            {
                MessageBox.Show("Please Stop Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_STARTED;
            }

            return CAM_STATUS.All_OK;
        }
    }


}

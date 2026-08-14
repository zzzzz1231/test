using SICK3D;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SampleApiDllCSharpWinForm
{
    public partial class frmVpro : Form
    {
        Cognex.VisionPro.CogImage16Grey Image16Grey;
        Cognex.VisionPro.CogImage8Grey Image8Grey;
        Cognex.VisionPro.CogImage16Range Image16Range;

        public frmVpro()
        {
            InitializeComponent();
            this.cogDisplayStatusBarV21.Display = this.cogRecordDisplay3;
        }        
        private void button1_Click(object sender, EventArgs e)
        {
            CSImgT img = LoadImage();
            int height = img.get_rows(DataNames.RAN_CAL);
            int width = img.get_cols(DataNames.RAN_CAL);
            float LowerBoundR = (float)img.getC_lower_bound_r();
            float UpperBoundR = (float)img.getC_upper_bound_r();
            float LowerBoundX = (float)img.getC_lower_bound_x();
            float UpperBoundX = (float)img.getC_upper_bound_x();
            double factx = width / (UpperBoundX - LowerBoundX);
            double facty = 1.0 / 0.04;  //这里是取y分辨率的倒数，把0.04改为自己的y分辨率
            double factz = UInt16.MaxValue / (UpperBoundR - LowerBoundR);
            double b = UInt16.MaxValue - (factz * UpperBoundR);

            //  获取SICK Range 数据
            float[] RangeRawData = img.getDataFloat(DataNames.RAN_CAL);            
            // 由float转换为Uint16
            UInt16[] RangeUInt16 = new UInt16[RangeRawData.Length];
            for (int i = 0; i < RangeUInt16.Length; i++)
            {             
                if (RangeRawData[i] == -1.0e6)
                    RangeUInt16[i] = 0;  //把Sick missing data 转化为Cognex CogImageRange16的missing data
                else
                    RangeUInt16[i] = (UInt16)(RangeRawData[i] * factz + b);
            }
            //Cognex 16位灰度图像
            byte[] RangeByte = new byte[width * height * sizeof(UInt16)];
            Buffer.BlockCopy(RangeUInt16, 0, RangeByte, 0, RangeByte.Length);            
            Image16Grey = new Cognex.VisionPro.CogImage16Grey(width, height);
            IntPtr Image16GreyIntPtr = Image16Grey.Get16GreyPixelMemory(Cognex.VisionPro.CogImageDataModeConstants.Read, 0, 0, width, height).Scan0;            
            System.Runtime.InteropServices.Marshal.Copy(RangeByte, 0, Image16GreyIntPtr, RangeByte.Length);
            //图像显示
            cogRecordDisplay2.Image = Image16Grey;
            cogRecordDisplay2.Fit();

            // 获取SICK reflectance图像数据
            Byte[] refData = img.getDataByte(DataNames.REF_CAL);
            // Cognex 8位灰度图像
            Image8Grey = new Cognex.VisionPro.CogImage8Grey(width, height);
            IntPtr Image8GreyIntPtr = Image8Grey.Get8GreyPixelMemory(Cognex.VisionPro.CogImageDataModeConstants.Read, 0, 0, width, height).Scan0;            
            System.Runtime.InteropServices.Marshal.Copy(refData, 0, Image8GreyIntPtr, refData.Length);
            cogRecordDisplay1.Image = Image8Grey;
            cogRecordDisplay1.Fit();

            //Cognex 16位深度图像
            Cognex.VisionPro3D.Cog3DMatrix3x3 matrix3x3 = new Cognex.VisionPro3D.Cog3DMatrix3x3(factx, 0.0, 0.0, 0.0, facty, 0.0, 0.0, 0.0, factz);
            Cognex.VisionPro3D.Cog3DVect3 vect3D = new Cognex.VisionPro3D.Cog3DVect3(0.0, 0.0, b);
            Cognex.VisionPro3D.Cog3DTransformLinear trans = new Cognex.VisionPro3D.Cog3DTransformLinear(matrix3x3, vect3D);            
            Image16Range = new Cognex.VisionPro.CogImage16Range(Image16Grey, 0, trans);
            cogRecordDisplay3.Image = Image16Range;
            cog3DDisplayHost1.HostedDisplay.Image = Image16Range;
            cog3DDisplayHost1.HostedDisplay.Fit(true);
        }
        private CSImgT LoadImage()
        {
            // This is just a demo   
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
            if (false == img.loadFromIconFile(strConfigPath, 1.0))
                MessageBox.Show("加载失败！！ \n\n\nLoad Image failed!!");
            return img;
        }

        private void frmVpro_Load(object sender, EventArgs e)
        {

        }
    }
}

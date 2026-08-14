using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using VisionShop;
using System.Net.Sockets;
using System.Diagnostics;

//using DatabaseShop;

using System.Threading;

using Tri = SICK3D.TRI;

namespace SampleApiDllCSharpConsole
{
    public class ImageBuffer
    {
        public bool Has3D = false;
        public int Width = -1;
        public int Height = -1;


        public byte[] IntensityBuffer;
        public float XResolution = 0.0f;
        public float XOffset = 0.0f;
        public float YResolution = 0.0f;
        public float ZResolution = 0.0f;
        public float ZOffset = 0.0f;
    }


    //[Serializable]
    public class Trispector55// : IDeviceBase
    {
        public /*override*/ string DeviceBrand { get { return "SICK_Trispector"; } }
        public int ImageID = 0;

        public /*override*/ string DeviceName { get; set; } = "Trispector";
        public /*override*/ string DeviceSN { get; set; } = "SN";
        public /*override*/ string IPAddress { get; set; } = "192.168.1.10";
        public /*override*/ string ConfigSetting { get; set; }
        public /*override*/ string CalibrationFile { get; set; }
        //public /*override*/ DeviceStatus Status { get; set; }
        //public /*override*/ VisionShop.ImageBuffer ImageBuffer { get; set; }
        public /*override*/ ImageBuffer ImageBuffer { get; set; }
        public /*override*/ int Port1 { get; set; }
        public /*override*/ int Port2 { get; set; }
        public /*override*/ int Port3 { get; set; }

        //[field: NonSerialized]
        public /*override*/ object DeviceScanner { get; set; } // 最好不要在这里定义 scan 的动作
        // DeviceScanner 这个动作最好在 GenICam3D_Tri 这个基础环境中进行
        // SDK 的基本结构是，GenICam3D_Tri 是基础环境，负责所有设备的查找。
        //                  Trispector 是操作单个相机的类，负责取图。
        //                  在 Trispector 中扫描设备，必须保证其他设备都没有再被使用。scan 会断开网口上的所有 Trispector，重新扫描。这本来设计的就是基础环境 GenICam3D_Tri 的操作。


        //[field: NonSerialized]

        public Tri.Trispector _cam;

        public /*override*/ bool Connect()
        {
            try
            {
                //Database.Instance.UpdateLogRuntime(DatabaseShop.LogLevel.Info, "尝试连接相机 SN: " + DeviceSN ); ;
                if (_cam == null)
                {
                    _cam = new Tri.Trispector(DeviceSN);
                }

                var sts = _cam.getStatus();
                if (sts == SICK3D.CAM_STATUS.CAM_IS_DISCONNECTED)
                {
                    _cam.connectCamera(false); // 使用 false，外部已经有了轮训线程，就不需要dll里面处理。
                    _cam.setTimeOut(20000);
                }



                sts = _cam.getStatus();

                if (sts == SICK3D.CAM_STATUS.CAM_IS_CONNECTED)
                {
                    //Status = DeviceStatus.Connected;
                    //Database.Instance.UpdateLog(new List<LogMessageClass>() { new LogMessageClass(LogType.Runtime, LogLevel.Info, "鐩告満杩炴帴鎴愬姛 SN: " + DeviceSN) }); ;
                    return true;
                }
                else
                {
                    //Status = DeviceStatus.Disconnected;
                    //Database.Instance.UpdateLog(new List<LogMessageClass>() { new LogMessageClass(LogType.Runtime, LogLevel.Info, "鐩告満杩炴帴澶辫触 SN: " + DeviceSN) }); ;
                    return false;
                }
            }
            catch (Exception ex)
            {
                //Status = DeviceStatus.NA;
                //Database.Instance.UpdateLog(new List<LogMessageClass>() { new LogMessageClass(LogType.Runtime, LogLevel.Info, "鐩告満杩炴帴澶辫触 SN: " + DeviceSN + ex.Message) }); ;
                return false;
            }

        }

        public /*override*/ bool Disconnect()
        {

            try
            {
                Stop();
                _cam.disconnectCamera();
                //Status = DeviceStatus.Disconnected;
            }
            catch (Exception ex)
            {
                throw ex;
            }
            return true;
        }



        public /*override*/ bool Grab(int timeout)
        {
            try
            {
                //ImageBuffer = new VisionShop.ImageBuffer();
                ImageBuffer = new ImageBuffer();
                ImageBuffer.Has3D = true;

                SICK3D.CSImgT imagedata = new SICK3D.CSImgT();


                int count = timeout / 50;

                try
                {
                    while (true)
                    {
                        if (count <= 0)
                        {
                            imagedata = null;
                            break;
                        }

                        var camStatus = _cam.getImageData(imagedata);
                        Console.WriteLine(DeviceSN  + ", get data return: "+ camStatus.ToString());
                    
                        if (imagedata.isEmpty())
                            continue;

                        if (imagedata.has(SICK3D.DataNames.RAN_CAL))
                        {
                            ImageID = Convert.ToInt32(imagedata.get_ID());
                            Console.WriteLine(DeviceSN  + ", "+ imagedata.get_ID().ToString());
                            break;
                        }
                        count--;
                        Thread.Sleep(50);
                    }

                    //Console.WriteLine(timeout / 50 - count);
                }
                catch (Exception ex)
                { //

                    Stop();
                    throw ex;
                }


                if (imagedata == null)
                {
                    throw new Exception("采集数据为空");
                }

                if (imagedata.getAvalibleDataNames().Length <= 0)
                {

                    throw new Exception("采集数据为空");
                }



                ImageBuffer.Width = (int)imagedata.get_cols();
                ImageBuffer.Height = (int)imagedata.get_rows();

                //ImageBuffer.IntensityBuffer = imagedata.getDataByte(SICK3D.DataNames.REF_CAL);

                var dns = imagedata.getAvalibleDataNames();

                var rangedata = imagedata.getDataFloat(SICK3D.DataNames.RAN_CAL);

                float zresolution=0.04f;
                float zoffset=0.0f;
                float rangeMax = rangedata.Max();
                float rangeMin = rangedata.Min();

                // 这里的 ImageBuffer 在外部是有可能被读写的。
                // 这里将 imagedata 赋值给 ImageBuffer 最好要加锁。

                //ImageBuffer.RangerBuffer = ManagedWrapper.ConvertRealToUshort(rangedata, rangeMax, rangeMin, 1000, out zresolution, out zoffset);


                ImageBuffer.XResolution = (float)imagedata.getC_scaleX();
                ImageBuffer.XOffset = (float)imagedata.getC_offsetX();
                ImageBuffer.YResolution = (float)imagedata.getC_scaleY();
                ImageBuffer.ZResolution = zresolution;
                ImageBuffer.ZOffset = zoffset;

                //imagedata.Dispose();
                //GC.Collect();
                //GC.WaitForPendingFinalizers();
            }
            catch (Exception ex)
            {
                Stop();
                throw ex;
            }
            return true;
        }

        public /*override*/ bool Init()
        {
            return true;
        }

        public /*override*/ bool Release()
        {
            return true;
        }

        public /*override*/ bool Start(int scanCount, double scanInterval) // connnect 之后，不要再执行 scan
        {
            try
            {
                _cam.startCamera();
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine(DeviceSN + ", start exception = " + ex.ToString());
                throw ex;
            }

        }

        public /*override*/ bool Stop()
        {
            try
            {

                //if (_cam.getStatus() == SICK3D.CAM_STATUS.All_OK)// 应该是 started
                if (_cam.getStatus() == SICK3D.CAM_STATUS.CAM_IS_STARTED)
                {
                    _cam.stopCamera();
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine(DeviceSN + ", stop exception = " + ex.ToString());
                throw ex;
            }
            return true;
        }

        public /*override*/ bool SwitchJob(object args)
        {
            try
            {
                try
                {
                    Stop();
                }
                catch (Exception)
                {
                    //throw;
                }

                using (TcpClient client = new TcpClient())
                {
                    client.Connect(System.Net.IPAddress.Parse(IPAddress), Port1);
                    string cmd = (string)args;
                    cmd = ASCIIEncoding.UTF8.GetString(new byte[] { 0x02 }) + cmd +
                        ASCIIEncoding.UTF8.GetString(new byte[] { 0x03 });
                    if (client.Connected)
                    {
                        client.Client.Send(ASCIIEncoding.UTF8.GetBytes(cmd));
                        client.Client.ReceiveTimeout = 5000;
                        byte[] temp = new byte[1024];
                        int length = client.Client.Receive(temp);

                        byte[] ret = new byte[length];

                        Array.Copy(temp, ret, ret.Length);

                        string str = ASCIIEncoding.UTF8.GetString(ret);

                        if (!str.Contains("OK"))
                        {
                            client.Close();
                            throw new Exception("鍙傛暟璁惧畾澶辫触");
                        }

                        try
                        {
                            Disconnect();
                            Connect();
                        }
                        catch (Exception)
                        {

                            throw;
                        }

                    }

                    client.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                throw ex;
            }
            return true;
        }
    }
}

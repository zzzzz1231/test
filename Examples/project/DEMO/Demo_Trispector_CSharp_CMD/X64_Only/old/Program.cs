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
using SICK3D.TRI;
using System.Diagnostics;

namespace SampleApiDllCSharpConsole
{
    class Program
    {

        static void unit_test_print(bool isPrintToScream, string log, Object obj)
        {
            if (isPrintToScream)
                Console.WriteLine(log, obj);
        }
        static void unit_test_print(bool isPrintToScream, string log)
        {
            unit_test_print(isPrintToScream, log, "");
        }

        /////////////////////////////////////////////////////////////////////////////////////
        public static List<Trispector> devicesVLB = new List<Trispector>();
        public static List<Stopwatch> swVLB = new List<Stopwatch>();
        public static List<bool> isDoneVLB = new List<bool>();
        static void grabImageVLB(object id)
        {
            Trispector dev = devicesVLB[(int)id];
            {
                dev.startCamera();
                Thread.Sleep(10);
                SICK3D.CSImgT img = new SICK3D.CSImgT();
                while (true)
                {
                    if (SICK3D.CAM_STATUS.All_OK == dev.getImageData(img))
                    {
                        var IntensityBuffer = img.getDataByte(SICK3D.DataNames.REF_CAL);
                        var rangedata = img.getDataFloat(SICK3D.DataNames.RAN_CAL);
                        break; // 
                    }
                    Thread.Sleep(10);
                }
                dev.stopCamera();
                swVLB[(int)id].Stop();
                isDoneVLB[(int)id] = true;
                Console.WriteLine(dev.getDeviceName() + ", grab id=" + img.get_ID() + "; time(ms)=" + swVLB[(int)id].ElapsedMilliseconds.ToString());
                img.Dispose();
                GC.Collect();
                GC.WaitForPendingFinalizers();
                Thread.Sleep(100);
            }
        }
        static bool test_CS_basic_functions (bool isPrintToScream = true)
        {
            // --- test trispector shared environment. 
            bool isPrintLog = isPrintToScream;
            GenICam3D_Tri tri_shared = new GenICam3D_Tri("", GenICam3D_Tri.getPathToTrispectorProducer(), false, false);

            String[] SNs = tri_shared.getConDevListSN();
            if (SNs.Length == 0)
            {
                unit_test_print(isPrintToScream, "No camera found!");
                return false;
            }

            // --- test DEV init. 
            foreach (var sn in SNs)
            {
                Trispector dev = new Trispector(sn);
                dev.connectCamera(false);
                
                devicesVLB.Add(dev);
                swVLB.Add(new Stopwatch());
                isDoneVLB.Add(false);
            }


            for (int j = 0; j < 100; ++j)
            {
                foreach (var sw in swVLB)
                    sw.Restart();

                for (int i_b = 0; i_b< isDoneVLB.Count(); ++i_b)
                    isDoneVLB[i_b] = false;

                for (int i = 0; i < devicesVLB.Count(); ++i)
                {
                    Thread th_grabImage = new Thread(new ParameterizedThreadStart(grabImageVLB));
                    th_grabImage.Start(i);
                }
                while (true)
                {
                    bool ok = true;
                    for (int i_b = 0; i_b < isDoneVLB.Count(); ++i_b)
                        ok &= isDoneVLB[i_b];

                    if (ok)
                        break;

                    Thread.Sleep(10);
                }
                Console.WriteLine(j + " done!");
            }

            return true;
        }

        /////////////////////////////////////////////////////////////////////////////////////
        public static List<Trispector55> devices = new List<Trispector55>();
        public static List<Stopwatch> sw = new List<Stopwatch>();
        public static List<bool> isDone = new List<bool>();

        static object l = new object();
        static void grabImage(object id)
        {
            Trispector55 dev = devices[(int)id];
            //for (int i = 0; i < 100; ++i)
            {
                //Console.WriteLine(dev.DeviceSN + ", grab " + i + "; ");

                //lock (l)
                {
                    dev.Start(0, 0);
                }

                dev.Grab(2000);
                sw[(int)id].Stop();
                dev.Stop();


                isDone[(int)id] = true;
                Console.WriteLine(dev.DeviceSN + ", grab id=" + dev.ImageID + "; time(ms)="+ sw[(int)id].ElapsedMilliseconds.ToString());
                //Console.WriteLine(dev.DeviceSN + ", grab id=" + dev.ImageID + "; time(ms)=");
                Thread.Sleep(100);
            }
        }
        static bool test_liyang_4_trispector(bool isPrintToScream = true)
        {
            // --- test trispector shared environment. 
            GenICam3D_Tri tri_shared = new GenICam3D_Tri("", GenICam3D_Tri.getPathToTrispectorProducer(), false, false);

            var SNs = tri_shared.getConDevListSN();
            if (SNs.Length == 0)
            {
                unit_test_print(isPrintToScream, "No camera found!");
                return false;
            }

            // 
            
            foreach (var sn in SNs)
            {
                Trispector55 dev = new Trispector55();
                dev.DeviceSN = sn;
                dev.Connect();

                devices.Add(dev);

                sw.Add(new Stopwatch());
                isDone.Add(false);
            }


            for (int j = 0; j < 5000; ++j)
            {
                //Console.WriteLine("Please input:");
                //Console.ReadKey();
                //Console.WriteLine("-----------------");
                foreach (var sw in sw)
                    sw.Restart();

                for (int i_b = 0; i_b < isDone.Count(); ++i_b)
                    isDone[i_b] = false;
                for (int i = 0; i < devices.Count(); ++i)
                {
                    Thread th_grabImage = new Thread(new ParameterizedThreadStart(grabImage));
                    th_grabImage.Start(i);
                }
                while (true)
                {
                    bool ok = true;
                    for (int i_b = 0; i_b < isDone.Count(); ++i_b)
                        ok &= isDone[i_b];

                    if (ok)
                        break;

                    Thread.Sleep(10);
                }
                Console.WriteLine(j + " done!");
            }

            Thread.Sleep(15000);

            return true;
        }


        static bool test_liyang_4_trispector2(bool isPrintToScream = true)
        {
            // --- test trispector shared environment. 
            GenICam3D_Tri tri_shared = new GenICam3D_Tri("", GenICam3D_Tri.getPathToTrispectorProducer(), false, false);

            var SNs = tri_shared.getConDevListSN();
            if (SNs.Length == 0)
            {
                unit_test_print(isPrintToScream, "No camera found!");
                return false;
            }

            // 

            foreach (var sn in SNs)
            {
                Trispector55 dev = new Trispector55();
                dev.DeviceSN = sn;
                dev.Connect();

                devices.Add(dev);

                sw.Add(new Stopwatch());
                isDone.Add(false);
            }


            for (int j = 0; j < 100; ++j)
            {
                sw[0].Restart();
                sw[1].Restart();
                sw[2].Restart();
                sw[3].Restart();
                isDone[0] = false;
                isDone[1] = false;
                isDone[2] = false;
                isDone[3] = false;


                foreach (var dev in devices)
                {
                    dev.Start(1, 1);
                }

                for (int i = 0; i < devices.Count(); ++i)
                {
                    Thread th_grabImage = new Thread(new ParameterizedThreadStart(grabImage));
                    th_grabImage.Start(i);
                }
                while (!isDone[0] || !isDone[1] || !isDone[2] || !isDone[3])
                    Thread.Sleep(10);



                foreach (var dev in devices)
                {
                    dev.Stop();
                }


                Console.WriteLine(j + " done!");
            }

            Thread.Sleep(15000);

            return true;
        }


        /////////////////////////////////////////////////////////////////////////////////////

        static void Main(string[] args)
        {
            var isOK = false;

            //isOK = test_CS_basic_functions(true);
            isOK = test_liyang_4_trispector(true);
            unit_test_print(true, "test_CS_basic_functions    {0}", isOK);
        }


    }
}

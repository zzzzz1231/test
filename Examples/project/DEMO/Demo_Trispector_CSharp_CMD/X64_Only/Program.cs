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

        static bool test_CS_basic_functions (bool isPrintToScream = true)
        {
            bool enable_CPP_Grab_thread = true;
            unit_test_print(isPrintToScream, "*---* Start!");

            // --- test trispector shared environment. 
            String logPath = "";
            String ctiPath = GenICam3D_Tri.getPathToTrispectorProducer();
            bool isPrintLog = isPrintToScream;
            GenICam3D_Tri tri_shared = new GenICam3D_Tri(logPath, ctiPath, isPrintLog, false);

            String[] SNs = tri_shared.getConDevListSN();

            // --- test DEV init. 
            String sn = SNs[0];
            Trispector tri = new Trispector(sn);

            if (tri.isReady() != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera is not ready!");
                return false;
            }

            // --- test DEV connection. 
            var camStatus = tri.connectCamera(enable_CPP_Grab_thread);
            if (camStatus != CAM_STATUS.All_OK)
            {
                unit_test_print(isPrintToScream, "Camera connecting failed!");
                return false;
            }

            // --- test DEV start
            camStatus = tri.startCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                 unit_test_print(isPrintToScream, "Camera starting failed!");
                return false;
            }

            // --- test DEV acquisition
            CSImgT imgT = new CSImgT();
            while (true) 
            {
                camStatus = tri.getImageData(imgT);
                unit_test_print(isPrintToScream, camStatus.ToString());
                if (camStatus != CAM_STATUS.All_OK)
                {
                    unit_test_print(isPrintToScream, "---------- Not receive image yet! Waitting...");
                    Thread.Sleep(1000);
                    continue;
                }

                if (imgT.has(DataNames.RAN_CAL))
                    break;
            }

            // --- test CSImgT data info
            uint C_cols          = imgT.get_cols();
            uint C_rows          = imgT.get_rows();
            double C_offsetX    = imgT.getC_offsetX();
            double C_offsetY    = imgT.getC_offsetY();
            double C_scaleX     = imgT.getC_scaleX();
            double C_scaleY     = imgT.getC_scaleY();
            double C_lower_bound_x = imgT.getC_lower_bound_x();
            double C_upper_bound_x = imgT.getC_upper_bound_x();
            double C_lower_bound_r = imgT.getC_lower_bound_r();
            double C_upper_bound_r = imgT.getC_upper_bound_r();

            // --- test CSImgT data saving
            ulong id = imgT.get_ID();
            DataNames[] comp = { DataNames.RAN_CAL, DataNames.REF_CAL};
            bool saveRacOk = imgT.SaveToIconFile("D:\\rac", comp, true);
            unit_test_print(isPrintToScream, saveRacOk ? "Camera grabbing image OK!": "Camera grabbing image failed!");


            // --- test DEV stop. 
            camStatus = tri.stopCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                 unit_test_print(isPrintToScream, "Camera stopping failed!");
                return false;
            }

            // --- test DEV disconnection. 
            camStatus = tri.disconnectCamera();
            if (camStatus != CAM_STATUS.All_OK)
            {
                 unit_test_print(isPrintToScream, "Camera disconnecting failed!");
                return false;
            }
            return true;
        }

        /////////////////////////////////////////////////////////////////////////////////////


        static void Main(string[] args)
        {
            var isOK = false;

            isOK = test_CS_basic_functions(true);
            unit_test_print(true, "test_CS_basic_functions    {0}", isOK);
        }


    }
}

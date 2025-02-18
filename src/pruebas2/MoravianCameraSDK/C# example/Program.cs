using System;
using System.Diagnostics;
using System.Text;
using System.Threading;

// Here you can chose which DLL is used
// You need to copy the chosen DLL next
// to the resulting executable

// CXUSB variant
using Camera = MoravianCameraSDK.Cxusb;
// GXUSB variant
//using Camera = MoravianCameraSDK.Gxusb;
// GXETH variant
//using Camera = MoravianCameraSDK.Gxeth;

namespace MoravianCameraSDK
{
    class Program
    {
        // This example uses only one camera. In case of multiple cameras connected,
        // last one enumerated is used.
        private static uint cameraId = 0;

        private static void EnumCamera_CB(uint EnumId)
        {
            cameraId = EnumId;
        }

        static void Main(string[] args)
        {
            // This will open GUI dialog to enter ETH Adapter IP, which is then saved to registry.
            // In the next calls of the Enumerate and Initialize functions this IP is loaded,
            // so Configure only needs to be called once when changing the IP.
            // Gxeth only!
            //Camera.Configure(UIntPtr.Zero, Process.GetCurrentProcess().MainWindowHandle);

            Camera.EnumCallBack cb = new Camera.EnumCallBack(EnumCamera_CB);
            Camera.Enumerate(cb);

            if (cameraId == 0)
            {
                Console.WriteLine("No cameras detected");
                return;
            }
            Console.WriteLine($"Using camera id:\t{cameraId}");

            UIntPtr cameraHandle = Camera.Initialize(cameraId);
            if ((long)cameraHandle == -1)
            {
                Console.WriteLine("Couldn't connect to the camera!");
                return;
            }

            StringBuilder desc = new StringBuilder(128);
            Camera.GetStringParameter(cameraHandle, Camera.gspCameraDescription, desc.Capacity - 1, desc);
            Console.WriteLine("Camera description:\t{0}", desc.ToString().Trim());

            Camera.GetIntegerParameter(cameraHandle, Camera.gipFirmwareMajor, out int major);
            Camera.GetIntegerParameter(cameraHandle, Camera.gipFirmwareMinor, out int minor);
            Camera.GetIntegerParameter(cameraHandle, Camera.gipFirmwareBuild, out int build);
            Console.WriteLine($"Camera FW version:\t{major}.{minor}.{build}");

            Camera.GetValue(cameraHandle, Camera.gvChipTemperature, out float valf);
            Console.WriteLine("Camera chip temp:\t{0:F} °C", valf);

            Camera.GetValue(cameraHandle, Camera.gvSupplyVoltage, out valf);
            Console.WriteLine("Camera supply voltage:\t{0:F} V", valf);

            int Index = 0;
            byte rv;
            do
            {
                rv = Camera.EnumerateReadModes(cameraHandle, Index, desc.Capacity - 1, desc);
                if (rv != 0)
                {
                    Console.WriteLine("Read mode #{0}:\t\t{1}", Index, desc.ToString().Trim());
                    Index++;
                }
            } while (rv != 0);

            // set last read mode
            Camera.SetReadMode(cameraHandle, Index - 1);

            // start exposure
            const double expTime = 0.3; // 300 ms
            Camera.GetIntegerParameter(cameraHandle, Camera.gipChipW, out int chipW);
            Camera.GetIntegerParameter(cameraHandle, Camera.gipChipD, out int chipD);

            Console.WriteLine("Starting exposure of light frame with params:\nExp. time:\t\t{0:F}" +
                " sec\nWidth:\t\t\t{1} px\nHeight:\t\t\t{2} px", expTime, chipW, chipD);

            Camera.StartExposure(cameraHandle, expTime, 1, 0, 0, chipW, chipD);
            
            // sleep during exposure
            Thread.Sleep((int)(expTime * 1000));

            byte ready = 0;
            while (ready == 0)
            {
                Camera.ImageReady(cameraHandle, out ready);
            }

            // read whole frame
            uint imageLen = (uint)(chipW * chipD * 2);
            ushort[] imageData = new ushort[imageLen];
            rv = Camera.ReadImage(cameraHandle, imageLen, imageData);
            if (rv == 0)
            {
                Console.WriteLine("Error reading image!");
            }
            else
            {
                Console.WriteLine($"Downloaded image size:\t{imageLen} bytes");
            }

            // disconnect
            Camera.Release(cameraHandle);
        }
    }
}

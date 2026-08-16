using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace case_4424
{
    class Program
    {
        static public KAYA.Lib kyLib;
        static public KAYA.IDevice grabber;


        class Options
        {
            public bool unattended;
            public int deviceIndex;
            public bool help;

            public void parseArgs(string[] args)
            {
                for (int i = 0; i < args.Length; i++)
                {
                    if (args[i] == "--unattended")
                    { this.unattended = true; }
                    else if (args[i] == "--help")
                    { this.help = true; }
                    else if (args[i] == "--deviceIndex")
                    { this.deviceIndex = Convert.ToInt16(args[i + 1]); }
                    else { Console.WriteLine($"Unexpected argument {args[i]} skipped"); }
                }
            }
            public void printArguments()
            {
                Console.WriteLine($"--unattended {this.unattended}");
                Console.WriteLine($"--deviceIndex {this.deviceIndex}");
            }
            public void printHelp()
            {
                Console.WriteLine("--help               print Help");
                Console.WriteLine("--unattended         Enable unattended mode");
                Console.WriteLine("--deviceIndex        Grabber index");
            }
        }
        static int Main(string[] args)
        {
            int infosize = 0;

            Options arguments = new Options();
            arguments.parseArgs(args);

            if (arguments.help) { arguments.printHelp(); return 0; }
            arguments.printArguments();
            int grabber_idx = arguments.deviceIndex;
            kyLib = new KAYA.Lib();
            KAYA.InitParameters initParams = new KAYA.InitParameters();
            initParams.version = 1;
            kyLib.Initialize(initParams);

            KAYA.SOFTWARE_VERSION soft_ver = kyLib.GetSoftwareVersion();
            //Console.WriteLine("Reserved:{0}", soft_ver.Reserved);
            Console.WriteLine("Running software version: {0}.{1}.{2}\n", soft_ver.Major, soft_ver.Minor, soft_ver.SubMinor);

            // Scan for available devices (frame grabbers)
            infosize = kyLib.Scan();

            Console.WriteLine("Number of scan results: {0}\n", infosize);

            for (int i = 0; i < infosize; i++)
            {
                // Get device's info
                KAYA.DEVICE_INFO deviceInfo = kyLib.DeviceInfo(i);
                Console.WriteLine("[{0}] {1} on PCI slot [{2}:{3}:{4}]: Protocol {5}, Generation {6}\n", i, deviceInfo.DeviceName,
                                    deviceInfo.Bus, deviceInfo.Slot, deviceInfo.Function, deviceInfo.Protocol, deviceInfo.DeviceGeneration);
            }

            Console.WriteLine("\n");

            // Choose the grabber


            if (!arguments.unattended)
            {
                Console.Write("Which grabber you'd like to use: ");
                grabber_idx = Convert.ToInt32(Console.ReadLine());
            }
            // Get info about the chosen device
            KAYA.DEVICE_INFO dev_info = kyLib.DeviceInfo(grabber_idx);
            Console.WriteLine("\nGetting Info about the device:");
            Console.WriteLine("DeviceName:\t{0}", dev_info.DeviceName);
            Console.WriteLine("Bus:\t\t{0}", dev_info.Bus);
            Console.WriteLine("Slot:\t\t{0}", dev_info.Slot);
            Console.WriteLine("Function:\t{0}", dev_info.Function);
            Console.WriteLine("DevicePID:\t{0}", dev_info.DevicePID);
            Console.WriteLine("isVirtual:\t{0}", dev_info.isVirtual);
            try
            {
                grabber = kyLib.Open(grabber_idx);
            }
            catch (KAYA.KYFGLibException ex)
            {
                Console.WriteLine();
                Console.WriteLine("Unable to open frame grabber, {0}", ex.Message);
                Console.WriteLine("Press some key to exit script...");
                if (!arguments.unattended) { Console.Read(); }
                return 1;
            }
            UInt64 address = 0x402024;
            UInt32 size = 4;
            Byte[] writeBuffer = {0x0, 0x0, 0x1, 0x0};
            grabber.DeviceDirectHardwareWrite(address, writeBuffer);
            Byte[] resultBuffer = grabber.DeviceDirectHardwareRead(address, size);
            Console.WriteLine(resultBuffer);
            bool testResult = true;
            if (resultBuffer.Length != size) { Console.Write("resultBuffer.Length != write buffer length\nTestFiled"); return 1; }
            for (int i = 0; i < resultBuffer.Length; i++)
            {
                Console.WriteLine(resultBuffer[i]);
                if (resultBuffer[i] != writeBuffer[i]) { testResult = false; }
            }
           
            Console.WriteLine("Press some key to exit script...");
            if(!arguments.unattended) Console.ReadKey();
            int returCode;
            if (testResult) { returCode = 0; } else { returCode = 0; }
            return returCode;
        }
    }
}


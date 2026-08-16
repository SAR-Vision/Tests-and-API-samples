using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;


namespace KYFGLib.NET_Example
{
    public class Program
    {
        static public KAYA.Lib kyLib;
        static public KAYA.IDevice grabber;
        static public KAYA.ICamera camera;
        static public KAYA.IStream stream;

        
        public static void Device_event_callback_func(Object userContext, Object eventObject)
        {
            System.Type eventObjectType = eventObject.GetType();
            //Console.Write("\n\nDevice Event has been occurred. Type: {0}\n", eventObjectType.ToString());

            if (eventObjectType == typeof(KAYA.DEVICE_EVENT_CAMERA_CONNECTION_LOST))
            {
                Console.Write("Processing DEVICE_EVENT_CAMERA_CONNECTION_LOST event\n");
                KAYA.DEVICE_EVENT_CAMERA_CONNECTION_LOST linkLossEvent = (KAYA.DEVICE_EVENT_CAMERA_CONNECTION_LOST)eventObject;
                Console.Write("Disconnected Camera Link: {0}\n", linkLossEvent.CameraLink);
                Console.Write("Disconnected Device Link: {0}\n", linkLossEvent.DeviceLink);
            }
            else if (eventObjectType == typeof(KAYA.DEVICE_EVENT_CAMERA_START))
            {
                Console.Write("Processing DEVICE_EVENT_CAMERA_START event\n");
            }
            else
            {
                Console.Write("Processing another event\n");
            }
        }


        public static void Stream_callback_func(KAYA.IStreamBuffer stream_buffer, Object userContext)
        {
            int buffIndex =0 ;

            if (stream_buffer == null)
            {
                return;
            }

            // Get current buffer(frame) index
            buffIndex = stream_buffer.GetFrameIndex();

            //Get buffer info
            IntPtr infoBase = (IntPtr)stream_buffer.GetInfo(KAYA.KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_BASE);
            UInt64 buffsize = (UInt64)stream_buffer.GetInfo(KAYA.KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_SIZE);
            IntPtr userPTR = (IntPtr)stream_buffer.GetInfo(KAYA.KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_USER_PTR);
            UInt64 timestamp = (UInt64)stream_buffer.GetInfo(KAYA.KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_TIMESTAMP);
            Double instantFPS = (Double)stream_buffer.GetInfo(KAYA.KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_INSTANTFPS);
            UInt32 infoID = (UInt32)stream_buffer.GetInfo(KAYA.KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_ID);

            Console.Write("\rGood callback, current index:{0}, timestamp:{1}, FPS:{2}, buffer size:{3}", buffIndex, timestamp, instantFPS, buffsize);
        }
        class Options
        {
            public bool unattended;
            public int deviceIndex;
            public int cameraIndex;
            public int streamDuration;
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
                    else if (args[i] == "--cameraIndex")
                    { this.cameraIndex = Convert.ToInt16(args[i + 1]); }
                    else if (args[i] == "--streamDuration")
                    { this.streamDuration = Convert.ToInt32(args[i + 1]); }
                    else { Console.WriteLine($"Unexpected argument {args[i]} skipped"); }
                }
            }
            public void printArguments()
            {
                Console.WriteLine($"--unattended {this.unattended}");
                Console.WriteLine($"--deviceIndex {this.deviceIndex}");
                Console.WriteLine($"--cameraIndex {this.cameraIndex}");
                Console.WriteLine($"--streamDuration {this.streamDuration}");
            }
            public void printHelp()
            {
                Console.WriteLine("--help               print Help" );
                Console.WriteLine("--unattended         Enable unattended mode");
                Console.WriteLine("--deviceIndex        Grabber index");
                Console.WriteLine("--cameraIndex        Camera Index");
                Console.WriteLine("--streamDuration     duration of stream");
            }
        }
        static void Main(string[] args)
        {

            Options arguments = new Options();
            arguments.parseArgs(args);
            
            if (arguments.help) { arguments.printHelp(); return; }
            arguments.printArguments();
            Main1(arguments); // if we reinit once again SDK then problem occur
            Main1(arguments);
            Main1(arguments);

            Console.WriteLine("\nPress some key to exit script...");
            if (!arguments.unattended) { Console.ReadKey(); }
        }
        static void Main1(Options args)
        {

            int infosize = 0;
            Console.WriteLine("Starting Test Script\n");

            // Get the Root object to Library
            kyLib = new KAYA.Lib();

            // Initialize
            // Optional call
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
            Console.Write("Which grabber you'd like to use: ");
            int grabber_idx;
            if (args.unattended)
            {
                grabber_idx = args.deviceIndex;
            }
            else { grabber_idx = Convert.ToInt32(Console.ReadLine()); }
            

            // Get info about the chosen device
            KAYA.DEVICE_INFO dev_info = kyLib.DeviceInfo(grabber_idx);
            Console.WriteLine("\nGetting Info about the device:");
            Console.WriteLine("DeviceName:\t{0}", dev_info.DeviceName);
            Console.WriteLine("Bus:\t\t{0}", dev_info.Bus);
            Console.WriteLine("Slot:\t\t{0}", dev_info.Slot);
            Console.WriteLine("Function:\t{0}", dev_info.Function);
            Console.WriteLine("DevicePID:\t{0}", dev_info.DevicePID);
            Console.WriteLine("isVirtual:\t{0}", dev_info.isVirtual);

            // Open the device
            // Example of handling an exception after function call
            try
            {
                grabber = kyLib.Open(grabber_idx);
            }
            catch (KAYA.KYFGLibException ex)
            {
                Console.WriteLine();
                Console.WriteLine("Unable to open frame grabber, {0}", ex.Message);
                Console.WriteLine("Press some key to exit script...");
                Console.ReadKey();
                return;
            }

            GetAndPrintParameter(grabber, "PoCXP0");
            GetAndPrintParameter(grabber, "DeviceTemperature");
            GetAndPrintParameter(grabber, "PulseMessageMode");

            // Connect to camera(s)r
            Console.Write("Hit 'Enter' to scan and connect camera(s): ");
            if (!args.unattended) { Console.ReadKey(); }

            Console.WriteLine("\n\nSearching for available cameras...");

            // Scan the device for available cameras
            System.Collections.Generic.List<KAYA.ICamera> camHandles;
            //camHandles = grabber.CameraScan(); // deprecated
            camHandles = grabber.UpdateCameraList();
            

            Console.WriteLine("Found {0} cameras connected to Frame Grabber", camHandles.Count);

            // If no available cameras connected to device --> return
            if (camHandles.Count == 0)
            {
                Console.WriteLine("\nNo cameras found. Press some key to exit script...");
                if (!args.unattended) {Console.ReadKey(); }
                
                return;
            }
            // Get the object for the first found camera (index 0)
            camera = camHandles[0];

            // Open the camera connection
            camera.Open(null);
            Console.WriteLine("Camera 0 was connected successfully\n");
            KAYA.CAMERA_INFO cam_info = camera.CameraInfo();
            Console.WriteLine("Getting Info about the cam:");
            Console.WriteLine("master_link:\t\t{0}", cam_info.master_link);
            Console.WriteLine("link_mask:\t\t{0}", cam_info.link_mask);
            Console.WriteLine("link_speed:\t\t{0}", cam_info.link_speed);
            Console.WriteLine("stream_id:\t\t{0}", cam_info.stream_id);
            Console.WriteLine("deviceVersion:\t\t{0}", cam_info.deviceVersion);
            Console.WriteLine("deviceVendorName:\t{0}", cam_info.deviceVendorName);
            Console.WriteLine("deviceManufacturerInfo:\t{0}", cam_info.deviceManufacturerInfo);
            Console.WriteLine("deviceModelName:\t{0}", cam_info.deviceModelName);
            Console.WriteLine("deviceID:\t\t{0:x}", cam_info.deviceID);
            Console.WriteLine("deviceUserID:\t\t{0}", cam_info.deviceUserID);
            Console.WriteLine("outputCamera:\t\t{0}", cam_info.outputCamera); // TODO: IsOutputCamera()
            Console.WriteLine("virtualCamera:\t\t{0}", cam_info.virtualCamera); // TODO: IsVirtualCamera
            Console.WriteLine("\n");

            GetAndPrintParameter(camera, "PixelFormat");


            Tuple<byte[], KAYA.KYBOOL> xml_string_tuple = camera.GetXML();
            if (xml_string_tuple.Item2 == KAYA.KYBOOL.KY_TRUE)
            {
                //System.IO.File.WriteAllBytes("C:\\Users\\PC-01\\Desktop\\cameras_xml.zip", xml_string_tuple.Item1);
            }
            else
            {
                //System.IO.File.WriteAllBytes("C:\\Users\\PC-01\\Desktop\\cameras_xml.xml", xml_string_tuple.Item1);
            }
            //Console.WriteLine("XML file has been saved");

            // Set the Width of the frame (Int)
            /*
            camera.SetValue("Width", 640);
            Object cam_width = camera.GetValue("Width");
            Console.WriteLine("Set camera width to: {0}", Convert.ToInt64(cam_width));
             */

            // Set the Height of the frame (Int)
            /*
            camera.SetValue("Height", 480);
            Object cam_height = camera.GetValue("Height");
            Console.WriteLine("Set camera height to: {0}", Convert.ToInt64(cam_height));
            */

            // Set camera exposure time value (Double)
            /*
            camera.SetValue("ExposureTime", 625.0);
            Object cam_exposure_time = camera.GetValue("ExposureTime");
            Console.WriteLine("Set camera exposure time to: {0}", Convert.ToDouble(cam_exposure_time));
            */

            // Set camera exposure time value (String)
            /*
            camera.SetValue("DeviceUserID", "Tester Name");
            Object cam_device_user_id = camera.GetValue("DeviceUserID");
            Console.WriteLine("Set Device User ID to: {0}", Convert.ToString(cam_device_user_id));
            */

            // Sets BF_AutoLevelAdjust. If enabled -> the camera tries to adjust the video level before calibrating. (Boolean)
            /*
            camera.SetValue("BF_AutoLevelAdjust", true);
            Object cam_AutoLevelAdjust = camera.GetValue("BF_AutoLevelAdjust");
            Console.WriteLine("Set BF_AutoLevelAdjust to: {0}", Convert.ToBoolean(cam_AutoLevelAdjust));
            */

            // Set gain selector value (Enum)
            /*
            Object cam_GainSelector_orig = GetAndPrintParameter(camera, "GainSelector");
            try
            {
                camera.SetValue("GainSelector", "All");
            }
            catch (KAYA.KYFGLibException ex)
            {
                Console.WriteLine();
                Console.WriteLine("Cannot set parameter \"GainSelector\"");
            }
            GetAndPrintParameter(camera, "GainSelector");
            */

            // Set proper pixel format (Not all the cameras support 'Mono8') (String)
            /*
            camera.SetValue("PixelFormat", "Mono8");
            */

            // Allocate the object to stream
            stream = camera.StreamCreateAndAlloc(16);

            // Declare the delegate to the StreamBufferCallback function
            KAYA.StreamBufferCallback del = new KAYA.StreamBufferCallback(Stream_callback_func);
            // Register the delegate for future callbacks
            stream.BufferCallbackRegister(del, null);


            // Declare the delegate for loss connection event
            KAYA.DeviceEventCallBack devEventDelegator = new KAYA.DeviceEventCallBack(Device_event_callback_func);
            // Register the delegate for future callbacks
            grabber.EventCallBackRegister(devEventDelegator, null);

            // Start the camera
            // After this line we will get continuous calls to our callback function (that we registered earlier) ...
            camera.Start(stream, 0);

            // ... until some key is pressed
            Console.WriteLine("Press some key to continue...");
            if (args.unattended)
            {
                Thread.Sleep(args.streamDuration*1000);
            }
            else {Console.ReadKey(); }

            // Stop the camera
            // After that line we will finish getting continuous calls to our callback function
            camera.Stop();

            // Unregister the delegate for future callbacks
            stream.BufferCallbackUnregister(del);

            // Close the camera connection
            camera.Close();

            // Close the grabber
            grabber.Close();

            Console.WriteLine("\nPress some key to exit script...");
            if (!args.unattended) {Console.ReadKey(); }
            
        }

        static object GetAndPrintParameter(KAYA.IParameters obj, string paramName) 
        {
            try
            {
                Object paramValue = obj.GetValue(paramName);
                Console.WriteLine("{0}'s type:\t{1} {2}", paramName, paramValue.GetType(), paramValue.GetType().IsEnum ? "- Enum with values:" : "");
                if (paramValue.GetType().IsEnum)
                {
                    foreach (string s in Enum.GetNames(paramValue.GetType()))
                        Console.WriteLine("\t\t{0}", s);
                    //Console.WriteLine("{0} value:\t{1} ({2})", paramName, paramValue, (long)paramValue);
                }
                else
                {
                    Console.WriteLine("{0} value:\t{1}", paramName, paramValue);
                }
                return paramValue;
            }
            catch (KAYA.KYFGLibException ex)
            {
                Console.WriteLine();
                Console.WriteLine("Cannot get parameter {0}, {1}", paramName, ex.Message);
                return null;
            }
        }
    }
}




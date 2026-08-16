using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Threading;

namespace Case_3360
{
    class Program
    {
        static public KAYA.Lib kyLib;
        static public KAYA.IDevice grabber;
        static public KAYA.ICamera camera;
        static public KAYA.IStream stream;
        static public int callbackCounter = 0;
       
        static IntPtr AlignedMalloc(int size, int alignment)
        {
            IntPtr ptr = Marshal.AllocHGlobal(size + alignment);

            IntPtr alignedPtr = new IntPtr((ptr.ToInt64() + (alignment - 1)) & ~(alignment - 1));

            return ptr;
        }

        static void FreeAlignedMalloc(IntPtr pBuffer)
        {
            Marshal.FreeHGlobal(pBuffer);
        }
        
        public static void Stream_callback_func(KAYA.IStreamBuffer stream_buffer, Object userContext)
        {
            
            if (stream_buffer == null)
            {
                return;
            }
            try
            {
                // ticket 4976 pay attention that the put this code under try-catch because BufferToQueue() potentialy can throw an exception 
                stream_buffer.BufferToQueue(KAYA.KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT);
            }
            catch
            {
            }
            
            callbackCounter++;

        }
            static int Main(string[] args)
        {
            int infosize = 0;
            int buffers = 16; // ticket 4976 here we define number of triggers to be sent

            List<IntPtr> pBuffers = new List<IntPtr>();

            Console.WriteLine("Starting Test Script\n");

            // Get the Root object to Library
            kyLib = new KAYA.Lib();

            // Initialize
            // Optional call
            KAYA.InitParameters initParams = new KAYA.InitParameters();
            initParams.version = 1;
            kyLib.Initialize(initParams);
            infosize = kyLib.Scan();
            Console.WriteLine("Number of scan results {0}", infosize);
            for (int i = 0; i < infosize; i++) 
            { 
                KAYA.DEVICE_INFO deviceInfo = kyLib.DeviceInfo(i);
                Console.WriteLine("[{0}] {1} on PCI slot [{2}:{3}:{4}]\n", i, deviceInfo.DeviceName,
                                   deviceInfo.Bus, deviceInfo.Slot, deviceInfo.Function);
            }
            Console.WriteLine("\n");
            // Choose the grabber
            Console.WriteLine("Which grabber you'd like to use: ");
            // read input from user
            int grabberIndex = Convert.ToInt32(Console.ReadLine());
            try
            {
                grabber = kyLib.Open(grabberIndex);
            }
            catch (KAYA.KYFGLibException ex)
            {
                Console.WriteLine();
                Console.WriteLine("Unable to open frame grabber, {0}", ex.Message);
                Console.WriteLine("Press any key to exit...");
                Console.ReadKey();
                return -1;
            }
            Console.WriteLine("Detection cameras");
            System.Collections.Generic.List<KAYA.ICamera> camHandles =  grabber.UpdateCameraList();
            if (camHandles.Count == 0)
            {
                Console.WriteLine("\nNo cameras detected. Press any key to exit...");
                Console.ReadKey();
                return -1;
            }
            for (int i = 0; i < camHandles.Count; i++)
            {
                KAYA.CAMERA_INFO camInfo = camHandles[i].CameraInfo();
                Console.WriteLine("[{0}] Camera: {1} ", i, camInfo.deviceModelName);
            }
            Console.WriteLine("Which camera you'd like to use: ");
            // read input from user
            int cameraIndex = Convert.ToInt32(Console.ReadLine());
            if (cameraIndex > camHandles.Count)
                Console.WriteLine("There is no camera with index {0} on this grabber", cameraIndex);
            else
                camera = camHandles[cameraIndex];
            try
            {
                camera.Open(null);
            }
            catch (KAYA.KYFGLibException ex)
            {
                Console.WriteLine();
                Console.WriteLine("Unable to open camera, {0}", ex.Message);
                Console.WriteLine("Press any key to exit...");
                Console.ReadKey();
                return -1;
            }
            KAYA.CAMERA_INFO current_camInfo = camHandles[cameraIndex].CameraInfo();
            Console.WriteLine("Camera: {0} opened for test ", current_camInfo.deviceModelName);
            // Set Resolution settings
            Console.WriteLine("Set camera resolution");
            Console.WriteLine("Please set Width parameter");
            int width = Convert.ToInt32(Console.ReadLine());
            camera.SetValue("Width", width);
            Console.WriteLine("Please set Height parameter");
            int heidth = Convert.ToInt32(Console.ReadLine());
            camera.SetValue("Height", heidth);
            Console.WriteLine("How many triggers would you like to sent?");
            int trigger_count = Convert.ToInt32(Console.ReadLine());
            Console.WriteLine("Set streamDuration?");
            int streamDuration = Convert.ToInt32(Console.ReadLine());

            // Set grbber value ticket 4976 here we are setting grabber parameters needed for our tests, please change it to what is needed for your scenario

            //  ticket 4976 in our tests, we are using triggers from grabber, since you are using other triggers you probably don't neded the following
            if (current_camInfo.deviceModelName.Contains("Chameleon"))
            {
                camera.SetValue("SimulationTriggerMode", 1);
                camera.SetValue("SimulationTriggerSource", "KY_CAM_TRIG");
            }
            else
            {
                camera.SetValue("TriggerMode", 1);
                camera.SetValue("TriggerSource", "LinkTrigger0");
            }
            grabber.SetValue("CameraSelector", cameraIndex);
            grabber.SetValue("CameraTriggerMode", 1);
            grabber.SetValue("CameraTriggerActivation", "AnyEdge");
            grabber.SetValue("CameraTriggerSource", "KY_TIMER_ACTIVE_1");

            grabber.SetValue("TimerSelector", "Timer1");
            grabber.SetValue("TimerDelay", 1e+6 / (trigger_count/streamDuration) / 2);
            grabber.SetValue("TimerDuration", 1e+6 / (trigger_count / streamDuration) / 2);
            grabber.SetValue("TimerActivation", "LevelHigh");
            grabber.SetValue("TimerTriggerSource", "KY_TIMER_ACTIVE_0");

            grabber.SetValue("TimerSelector", "Timer0");
            grabber.SetValue("TimerDelay", Convert.ToDouble(10));
            grabber.SetValue("TimerDuration", Convert.ToDouble(1e+6 *streamDuration));
            grabber.SetValue("TimerActivation", "RisingEdge");
            grabber.SetValue("TimerTriggerSource", "KY_SOFTWARE");



            
            stream = camera.StreamCreate();
            // Declare the delegate to the StreamBufferCallback function
            KAYA.StreamBufferCallback streamBufferCallbackDelegator = new KAYA.StreamBufferCallback(Stream_callback_func);
            // Register the delegate for future callbacks
            
            stream.BufferCallbackRegister(streamBufferCallbackDelegator, null);
            ulong frameDataSize = (ulong)stream.GetInfo(KAYA.KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE);
            ulong frameDataAlignment = (ulong)stream.GetInfo(KAYA.KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT);
            // Select type of allocating buffer
            bool userAllocatedBuffers = false;
            for (int i = 0; i < buffers; i++)
            {
                stream.BufferAllocAndAnnounce(frameDataSize);
            }
            // Put all buffers to input queue
            stream.BufferQueueAll(KAYA.KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED, KAYA.KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT);

            // Start the camera
            // After this line we will get continuous calls to our callback function (that we registered earlier) ...
            camera.Start(stream, 0);
            Console.WriteLine("Stream Started");
            grabber.ExecuteCommand("TimerTriggerSoftware");
            Thread.Sleep(Convert.ToInt32(streamDuration*1000));
            
                       

            // some additional waiting is needed after all triggers are sent before stopping the camera (explanation is in the ticket)
            Thread.Sleep(Convert.ToInt32(2000));

            camera.Stop();
            stream.BufferCallbackUnregister(streamBufferCallbackDelegator);
            // Checking test results
            int frameCounter = Convert.ToInt32(grabber.GetValue("RXFrameCounter"));
            int DropframeCounter = Convert.ToInt32(grabber.GetValue("DropFrameCounter"));
           // int dropFrameCounter = Convert.ToInt32(grabber.GetValue("DropFrameCounter"));
            Console.WriteLine("\nRXFrameCounter {0}", frameCounter);
           // Console.WriteLine("DropFrameCounter {0}", dropFrameCounter);
            Console.WriteLine("CallbackCounter {0}", callbackCounter);
            Console.WriteLine("DropframeCounter {0}", DropframeCounter);
            // Close the camera connection
            camera.Close();

            // Close the grabber
            grabber.Close();
            // Free user allocated buffers
            if (userAllocatedBuffers)
            {
                for (int i = 0; i < pBuffers.Count; i++)
                {
                    FreeAlignedMalloc(pBuffers[i]);
                }
            }

            pBuffers.Clear();
            int returnCode;
            if (frameCounter == callbackCounter && (frameCounter - trigger_count >= -1 && frameCounter - trigger_count <= 1))
            {
                Console.WriteLine("TEST PASSED");
                returnCode = 0;
            }
                

            else
            {
                Console.WriteLine("Test failed");
                returnCode = - 1;
            }
                


            Console.WriteLine("\nPress some key to exit script...");
            Console.Read();
            return returnCode;
        }
    }
}

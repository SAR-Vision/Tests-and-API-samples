# Common KAYA imports DO NOT EDIT!!!
import sys
import os
import argparse
os.environ["WithAdapter"] = "1"
sys.path.insert(0, os.environ['KAYA_VISION_POINT_PYTHON_PATH'])
from KYFGLib import *

# Common Case imports DO NOT EDIT!!!
from enum import IntEnum  # for CaseReturnCode

# additional imports required by particular case, ADD CASE SPECIFIC IMPORTS UNDER THIS LINE:
# For example:
# import numpy as np
# import cv2
# from numpngw import write_png
import time
import json
import pathlib
import math


def CaseArgumentParser():
    parser = argparse.ArgumentParser()
    # Common arguments for all cases DO NOT EDIT!!!
    parser.add_argument('--unattended', default=False, action='store_true', help='Do not interact with user')
    parser.add_argument('--no-unattended', dest='unattended', action='store_false')
    parser.add_argument('--deviceList', default=False, action='store_true',
                        help='Print list of available devices and exit')
    parser.add_argument('--deviceIndex', type=int, default=-1,
                        help='Index of PCI device to use, '
                             'run this script with "--deviceList" to see available devices and exit')
    # Other arguments needed for this specific case, PARSE CASE SPECIFIC ARGUMENTS UNDER THIS LINE:
    return parser


# Common KAYA fragment_03
# Grabber initialization for this specific test
def Reset_grabber(grabberHandle):
    try:
        (status, value) = KYFG_GetGrabberValueEnum(grabberHandle, 'CxpPoCxpStatus')
        # (status_str,) = KYFG_GetGrabberValueEnum_ByValueName(grabberHandle, 'CxpPoCxpStatus', status_value)
        print('CxpPoCxpStatus Before Reset', value)
        if value != '0':
            if KYFG_IsGrabberValueImplemented(grabberHandle, 'CxpPoCxpHostConnectionSelector'):
                KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, 'CxpPoCxpHostConnectionSelector', 'All')
                KYFG_GrabberExecuteCommand(grabberHandle, 'CxpPoCxpAuto')
                time.sleep(30)
                (status, value) = KYFG_GetGrabberValueEnum(grabberHandle, 'CxpPoCxpStatus')
                print('CxpPoCxpStatus After Reset', value)
        if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
            KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, 'TriggerMode', 'Off')
        # if KYFG_IsGrabberValueImplemented(grabberHandle, 'CameraTriggerMode'):
        #     KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, 'CameraTriggerMode', 'Off')
        if KYFG_IsGrabberValueImplemented(grabberHandle, 'PulseMessageMode'):
            KYFG_SetGrabberValueEnum(grabberHandle, 'PulseMessageMode', 0)
            # KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, 'PulseMessageMode', 'Basic')
    except:
        pass
    print('#################### Reset Grabber Completed ###################')


def Reset_camera(cameraHandle, grabberHandle):     # Camera initialization for this specific test
    # 1. open json file with camera descriptions
    # 2. find this particular camera description
    # 3. from camera description take its "reset_camera_sequence" and "reset_grabber_sequence"
    # 4. perform the "reset_camera_sequence" and "reset_grabber_sequence" defined for this camera

    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
    model_name = camInfo.deviceModelName
    vendor_name = camInfo.deviceVendorName

    # Gets the BIN folder location from environment variable
    kaya_path = os.environ.get("KAYA_VISION_POINT_CONF")  # Gets the value of the env variable
    if not kaya_path:
        raise EnvironmentError("None of Environment variables KAYA_VISION_POINT_CONF")

    json_path = pathlib.Path(kaya_path) / "KAYA_Known_cameras.json"
    print("kaya_path: ", kaya_path)

    if not os.path.exists(json_path):
        print(f"[ERROR] JSON file not found: {json_path}")
        return

    try:
        # Load JSON and strip both full-line and inline // comments
        with open(json_path, 'r', encoding='utf-8') as f:
            cleaned_json_lines = []
            for line in f:
                stripped = line.strip()
                if stripped.startswith("//"):  # whole line comment
                    continue
                # remove inline comment after valid JSON content
                if "//" in line:
                    line = line.split("//", 1)[0].rstrip()
                cleaned_json_lines.append(line)

            cleaned_json_text = '\n'.join(cleaned_json_lines)

        jsonCameras = json.loads(cleaned_json_text)

    except json.JSONDecodeError as e:
        print(f"[ERROR] Failed to parse JSON: {e}")
        return

    # Combine vendor name + camera name
    if "Chameleon" in model_name:
        model_name = "Chameleon"
    lookup_name = f"{vendor_name}#{model_name}"
    print(lookup_name)

    if lookup_name not in jsonCameras:
        print(f"[ERROR] No data for camera '{lookup_name}' in JSON.")
        return

    cam_entry = jsonCameras[lookup_name]

    # Select the _Default_ profile or first available one
    profile_name = "_Default_"
    if profile_name not in cam_entry:
        # If "_Default_" not found, pick the first key
        profile_name = next(iter(cam_entry.keys()))
        print(f"[INFO] Using profile '{profile_name}' for '{lookup_name}'")

    camData = cam_entry[profile_name]

    # Handle 'refer' field if exists (optional)
    referenced_data = camData.get("refer")
    if referenced_data:
        camData = jsonCameras.get(referenced_data, camData)

    # Extract reset sequences
    reset_camera_sequence = camData.get("reset_camera_sequence")
    reset_grabber_sequence = camData.get("reset_grabber_sequence")

    if not reset_camera_sequence:
        print(f"[INFO] No 'reset_camera_sequence' found for camera '{model_name}'.")
        return
    print()

    print("#################### Reset Camera Start ###################")
    print()

    print(f"Camera: {model_name}")
    for step in reset_camera_sequence:
        for key, value in step.items():
            print(f" - {key} = {value}")
            (status, paramValueType) = KYFG_GetCameraValueType(cameraHandle, key)
            if paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_INT:
                KYFG_SetCameraValueInt(cameraHandle, key, value)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_BOOL:
                KYFG_SetCameraValueBool(cameraHandle, key, value)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_STRING:
                KYFG_SetCameraValueString(cameraHandle, key, value)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_FLOAT:
                KYFG_SetCameraValueFloat(cameraHandle, key, value)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_ENUM:
                if isinstance(value, str):
                    KYFG_SetCameraValueEnum_ByValueName(cameraHandle, key, value)
                else:
                    KYFG_SetCameraValueEnum(cameraHandle, key, value)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_COMMAND:
                KYFG_CameraExecuteCommand(cameraHandle, key)

    for cam_injson in reset_grabber_sequence:
        for key1, value1 in cam_injson.items():
            print(f" - ## grabber ## {key1} = {value1}")
            (status, paramValueType) = KYFG_GetGrabberValueType(grabberHandle, key1)
            pass
            if paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_INT:
                KYFG_SetGrabberValueInt(grabberHandle, key1, value1)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_BOOL:
                KYFG_SetGrabberValueBool(grabberHandle, key1, value1)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_STRING:
                KYFG_SetGrabberValueString(grabberHandle, key1, value1)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_FLOAT:
                KYFG_SetGrabberValueFloat(grabberHandle, key1, value1)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_ENUM:
                if isinstance(value1, str):
                    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, key1, value1)
                else:
                    KYFG_SetGrabberValueEnum(grabberHandle, key1, value1)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_COMMAND:
                KYFG_GrabberExecuteCommand(grabberHandle, key1)

            elif paramValueType == KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_UNKNOWN:
                print(
                    f" - ## is not possible set grabber parameter ## {key1} to {value1}, the parameter type: "
                    f"PROPERTY_TYPE_UNKNOWN")

    print()
    print("#################### Reset Camera Stop ####################")
    print()
    return
# END OF Common KAYA fragment_03


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


###################### Callback Function ####################################
class StreamInfoStruct:
    def __init__(self):
        self.callbackCount = 0
        self.instantsFPS = []
        return


def Stream_callback_func(buffHandle, userContext):
    try:
        (status, instantfps, pInfoSize, pInfoType) = KYFG_BufferGetInfo(buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_INSTANTFPS)
        #print(f"frame: {userContext.callbackCount}, instantfps: {instantfps}")
        userContext.instantsFPS.append(instantfps)
    except:
        pass

    userContext.callbackCount += 1

    return


def is_approximately_equal(num1, num2, tolerance):
    diff = abs(num1 - num2)
    avg = (abs(num1) + abs(num2)) / 2.0
    return diff <= avg * tolerance


def WaitForSleep(sleepTime):
    threadSleepSeconds = sleepTime
    print(f"Thread sleep for {threadSleepSeconds} seconds: ")
    for remaining in range(threadSleepSeconds, 0, -1):
        sys.stdout.write("\r")
        sys.stdout.write("{:2d} seconds remaining".format(remaining))
        sys.stdout.flush()
        time.sleep(1)
    sys.stdout.write("\rComplete!            \n")
    return threadSleepSeconds


def CaseRun(args):
    print(f'\nEntering CaseRun({args}) (use -h or --help to print available parameters and exit)...')

    device_infos = {}

    # Start of common KAYA prolog for 'def CaseRun(args)'
    unattended = args["unattended"]
    device_index = args["deviceIndex"]

    class CaseReturnCode(IntEnum):
        SUCCESS = 0
        COULD_NOT_RUN = 1
        NO_HW_FOUND = 2
        NO_REQUIRED_PARAM = 3
        WRONG_PARAM_VALUE = 4

    # Find and print list of available devices
    (status, infosize_test) = KY_DeviceScan()
    for x in range(0, infosize_test):
        (status, device_infos[x]) = KY_DeviceInfo(x)
        dev_info = device_infos[x]
        print(f'Found device [{x}]: "{dev_info.szDeviceDisplayName}"')

    # If only print of available devices list was requested
    if args["deviceList"]:
        return CaseReturnCode.SUCCESS  # we are done

    # deviceIndex == -1 means we need to ask user
    if device_index < 0:
        # Ask user what device to use for this test
        # in unattended mode, use the first device detected in the system (index 0)
        if unattended:
            device_index = 0
            print(f'\n!!! deviceIndex {device_index} forcibly selected in unattended mode !!!')
        else:
            device_index = int(input(f'Select PCI device to use (0 ... {infosize_test - 1})'))
            print(f'\ndeviceIndex {device_index} selected')

    # Verify deviceIndex being in the allowed range
    if device_index >= infosize_test:
        print(f'\nDevice with the index {device_index} does not exist, exiting...')
        return CaseReturnCode.NO_HW_FOUND

    # End of common KAYA prolog for "def CaseRun(args)"

    # Other parameters used by this particular case
    ##########################################################################
    ### Check for stream compatibility on 3-rd generation grabbers with VPI core
    ##########################################################################

    (status, device_info) = KY_DeviceInfo(device_index)

    OS_ENV_VALUE = os.environ.get("WithAdapter", "")
    if not len(OS_ENV_VALUE):
        KAYA_VISION_POINT_2_USE_VP_1_API_ADAPTER = False
    else:
        KAYA_VISION_POINT_2_USE_VP_1_API_ADAPTER = OS_ENV_VALUE.strip() == "1"

    if device_info.DeviceGeneration == 3 and not KAYA_VISION_POINT_2_USE_VP_1_API_ADAPTER:
        print("\n-----------------------------------------------------------")
        print('Test COULD NOT RUN on 3rd generation grabber with VPI core, STREAMING IS NOT SUPPORTED')
        print("-----------------------------------------------------------\n")
        return CaseReturnCode.COULD_NOT_RUN

    ##########################################################################

    errorCount = 0

    (grabberHandle,) = KYFG_Open(device_index)

    ############################
    Reset_grabber(grabberHandle)
    ############################

    print("-----------------------------------------------------------")
    print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
    print("-----------------------------------------------------------\n")

    (status, camHandleArray) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0

    print(f'Found {len(camHandleArray)} cameras')
    if len(camHandleArray) == 0:
        print("-----------------------------------------------------------")
        print('There is no cameras on this grabber')
        print("-----------------------------------------------------------\n")
        return CaseReturnCode.NO_HW_FOUND

    camerasWithTriggerMode = False

    for cameraIndex, cameraHandle in enumerate(camHandleArray):
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        print(f"{'*' * 32}Camera {str(camInfo.deviceModelName).ljust(20).rjust(20)} is open{'*' * 32}")
        print(f"{'*' * 32}Camera Firmware {str(camInfo.deviceFirmwareVersion).ljust(20).rjust(20)} {'*' * 32}")
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

        KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
        #########################################
        Reset_camera(cameraHandle, grabberHandle)
        #########################################
        try:
            if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
                KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
        except:
            pass

        print("-----------------------------------------------------------")
        print(f"Selected camera: [{cameraIndex}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
        print("-----------------------------------------------------------\n")

        fpsTolerance = 0.01  # 1%

        frame_fps_max = 0
        frame_fps = 0

        try:
            if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
                (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerMode", "Off")

            if KYFG_IsCameraValueImplemented(cameraHandle, "AcquisitionFrameRateMax"):
                (status, frame_fps_max) = KYFG_GetCameraValueFloat(cameraHandle, "AcquisitionFrameRateMax")
                KYFG_SetCameraValueFloat(cameraHandle, "AcquisitionFrameRate", frame_fps_max)
                (status, frame_fps) = KYFG_GetCameraValueFloat(cameraHandle, "AcquisitionFrameRate")
            else:
                (status, frame_fps) = KYFG_GetCameraValueFloat(cameraHandle, "AcquisitionFrameRate")
                frame_fps_max = frame_fps

            if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
                (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerSource", "LinkTrigger0")
                (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerMode", "On")
            else:
                (status,) = KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 1)
                (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "SimulationTriggerSource", "KY_CAM_TRIG")

            camerasWithTriggerMode = True

        except:
            print(f'There is no "TriggerMode" on camera {camInfo.deviceModelName}"')
            (status,) = KYFG_CameraClose(cameraHandle)
            continue

        if frame_fps_max == 0:
            frame_fps = frame_fps
        else:
            frame_fps = frame_fps_max

        expectedFps = frame_fps_max * 0.9

        # In case the FPS = 0, the test case fail for "ZeroDivisionError".
        if frame_fps != 0:
            FRAME_PERIOD_USEC = 1e6 / expectedFps
            TIMER_PER_DELAY = FRAME_PERIOD_USEC / 2
        else:
            TIMER_PER_DELAY = 0
            print("fps is 0, setting timer_per_delay to 0")
        print(f'timer_per_delay = {TIMER_PER_DELAY}')

        print(f"CAMERA FPS MAX: {frame_fps_max:.3f}")
        print(f"CAMERA FPS: {frame_fps:.3f}")
        print(f"FRAME_PERIOD_USEC: {FRAME_PERIOD_USEC:.3f}")
        print(f"TIMER_PER_DELAY: {TIMER_PER_DELAY:.3f}")

        # SetUp timer and triggers
        if KYFG_IsCameraValueImplemented(cameraHandle, "ExposureMode"):
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "ExposureMode", "Timed")
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "ExposureAuto", "Off")
            expTime = math.ceil(FRAME_PERIOD_USEC * 0.9)
            print(f"ExposureTime: {expTime}")
            (status,) = KYFG_SetCameraValueFloat(cameraHandle, "ExposureTime", float(expTime))

        (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerSelector", "Timer0")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_DISABLED")
        (status,) = KYFG_SetGrabberValueFloat(grabberHandle, "TimerDelay", float(TIMER_PER_DELAY))
        (status,) = KYFG_SetGrabberValueFloat(grabberHandle, "TimerDuration", float(TIMER_PER_DELAY))
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerActivation", "RisingEdge")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerSource", "KY_TIMER_ACTIVE_0")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerActivation", "AnyEdge")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerMode", "On")

        # CreateStream
        (status, cameraStreamHandle) = KYFG_StreamCreateAndAlloc(cameraHandle, 16, 0)
        stream_info_struct = StreamInfoStruct()
        (KYFG_StreamBufferCallbackRegister_status,) = KYFG_StreamBufferCallbackRegister(cameraStreamHandle,
                                                                                        Stream_callback_func,
                                                                                        stream_info_struct)

        # START CAMERA
        (status,) = KYFG_CameraStart(cameraHandle, cameraStreamHandle, 0)
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_CONTINUOUS")
        duration = 5

        WaitForSleep(duration)

        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_DISABLED")
        (status,) = KYFG_CameraStop(cameraHandle)
        (status, dropped_frame_counter) = KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")

        # Checking results
        frameIndex = 0
        for i in stream_info_struct.instantsFPS[1:]:
            if not is_approximately_equal(i, expectedFps, fpsTolerance):
                print(
                    f"Acquired frame [{frameIndex}] triggered fps: {i:.2f} does not match the expected calculated fps: {expectedFps:.2f}")
                errorCount += 1
            frameIndex += 1

        print(f'Drop Frames:    {dropped_frame_counter}')
        print(f'Frame Counter:  {stream_info_struct.callbackCount}')

        if stream_info_struct.callbackCount == 0:
            print('Frame counter is zero')
            errorCount += 1

        frame_threshold = 5

        if dropped_frame_counter > frame_threshold:
            print('There are dropped frames')
            errorCount += 1

        KYFG_StreamBufferCallbackUnregister(cameraStreamHandle, Stream_callback_func)
        KYFG_StreamDelete(cameraStreamHandle)

        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerMode", "Off")

        if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerMode", "Off")
        else:
            if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
                KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)

        (status,) = KYFG_CameraClose(cameraHandle)
        camIndex += 1

    (status) = KYFG_Close(grabberHandle)

    assert errorCount == 0, 'On some cameras there are errors'
    assert camerasWithTriggerMode is True, 'There is no cameras with trigger mode on this grabber'

    return CaseReturnCode.SUCCESS


if __name__ == "__main__":
    try:
        print("case 2869 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)
    exit(return_code)

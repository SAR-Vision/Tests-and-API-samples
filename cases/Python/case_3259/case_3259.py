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
    parser.add_argument('--segmentsperbuffer', type=int, default=16, help='segmentsperbuffer')
    parser.add_argument('--cameraModel', default='Any', type=str, help='Camera model')
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


camHandleArray = {}


class StreamCallbackStructure:
    def __init__(self) -> None:
        self.duration = 0
        self.period = 0
        self.fpsTotal = 0
        self.callbackCount = 0
        self.first_timestamp = 0
        self.last_timestamp = 0


def Stream_Callback_func(buffHandle, userContext):
    if buffHandle == 0:
        return

    (KYFG_BufferGetInfo_status, pInfoTimestamp, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
        buffHandle,
        KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_TIMESTAMP
    )

    (KYFG_BufferGetInfo_status, pInfoInstantFps, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
        buffHandle,
        KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_INSTANTFPS
    )

    userContext.callbackCount += 1

    if not userContext.first_timestamp:
        userContext.first_timestamp = pInfoTimestamp

    userContext.last_timestamp = pInfoTimestamp
    userContext.fpsTotal += pInfoInstantFps

    #print(f"Buffer callback, handle: {str(buffHandle)}, timestamp: {pInfoTimestamp}, callbackCount: {userContext.callbackCount}")


def check_is_camera_available(deviceModelName, camHandles):
    cams_num = len(camHandles)
    for i in range(cams_num):
        cameraHandle = camHandles[i]
        (Status, camInfo) = KYFG_CameraInfo2(cameraHandle)

        if deviceModelName == camInfo.deviceModelName:
            return True
    return False


def start_stream(grabberHandle, cameraHandle, cameraSelector, segmentsperbuffer = 1):
    (status,) = KYFG_CameraOpen2(cameraHandle, None)

    # camIndex = 0
    # KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraSelector)
    #########################################
    Reset_camera(cameraHandle, grabberHandle)
    #########################################

    try:
        if KYFG_IsCameraValueImplemented(cameraHandle, 'TriggerMode'):
            KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)
    except:
        pass

    (status, originalHeight) = KYFG_GetCameraValueInt(cameraHandle, "Height")

    (status, minHeight) = KYFG_GetCameraValueInt(cameraHandle, "HeightMin")
    (status,) = KYFG_SetCameraValueInt(cameraHandle, "Height", minHeight)

    # (status, fps_max, fps_min) = KYFG_GetCameraValueFloatMaxMin(cameraHandle, "AcquisitionFrameRate")
    # Set "AcquisitionFrameRate" of the camera to 100
    #frame_rate = float(fps_max if fps_max <= 100 else 100)

    (status, fpsMax) = KYFG_GetCameraValueFloat(cameraHandle, "AcquisitionFrameRateMax")
    (status,) = KYFG_SetCameraValueFloat(cameraHandle, "AcquisitionFrameRate", fpsMax)

    # Set "SegmentsPerBuffer" on grabber side
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "SegmentsPerBuffer", segmentsperbuffer)

    # Create stream and get buffer size
    (_, streamHandle) = KYFG_StreamCreateAndAlloc(cameraHandle, 2, 0)
    (status, buffSize, frameDataSize, pInfoType) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

    # Set camera selector to current camera index
    print(f"Start stream: camera selector: {cameraSelector}, FGHANDLE: {str(grabberHandle)}, CAMHANDLE: {hex(cameraHandle)}, STREAMHANDLE: {str(streamHandle)}")

    streamCallbackStruct = StreamCallbackStructure()
    (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, Stream_Callback_func, streamCallbackStruct)

    (status, fpsReal) = KYFG_GetCameraValueFloat(cameraHandle, "AcquisitionFrameRate")
    print(f"Camera: {hex(cameraHandle)} AcquisitionFrameRate: {fpsReal:.1f}")

    print("-----------------------------------------------------------")
    print(f"Stream started...")

    KYFG_CameraStart(cameraHandle, streamHandle, 2)

    time.sleep(5)

    KYFG_CameraStop(cameraHandle)

    print(f"Stream finished\n")

    KYFG_StreamBufferCallbackUnregister(streamHandle, Stream_Callback_func)
    KYFG_StreamDelete(streamHandle)

    (status,) = KYFG_SetCameraValueInt(cameraHandle, "Height", originalHeight)

    (status,) = KYFG_CameraClose(cameraHandle)
    # camIndex += 1

    streamCallbackStruct.duration = (streamCallbackStruct.last_timestamp - streamCallbackStruct.first_timestamp)

    streamCallbackStruct.period = 0
    fpsAvg = 0

    if streamCallbackStruct.callbackCount:
        streamCallbackStruct.period = streamCallbackStruct.duration / streamCallbackStruct.callbackCount
        fpsAvg = streamCallbackStruct.fpsTotal / streamCallbackStruct.callbackCount

    print(f"Statistics:")
    print(f"SegmentsPerBuffer:              {segmentsperbuffer}")
    print(f"Payload size:                   {buffSize}")
    print(f"Callbacks:                      {streamCallbackStruct.callbackCount}")
    print(f"FPS (avg):                      {fpsAvg:.2f}")
    print(f"Duration:                       {streamCallbackStruct.duration}")
    print(f"Period (avg):                   {streamCallbackStruct.period:.2f}")
    if 1 < segmentsperbuffer:
        print(f"Period per segments (avg):      {streamCallbackStruct.period / segmentsperbuffer:.2f}")
    print(f"Timestamp (first):              {streamCallbackStruct.first_timestamp}")
    print(f"Timestamp (last):               {streamCallbackStruct.last_timestamp}")
    print("-----------------------------------------------------------\n")

    return streamCallbackStruct.period, buffSize, streamCallbackStruct.callbackCount


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
    segmentsperbuffer = args['segmentsperbuffer']
    cameraModel = args['cameraModel']

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

    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    print("-----------------------------------------------------------")
    print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
    print("-----------------------------------------------------------\n")

    (_, camHandleArray[device_index]) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0
    print("Detected cameras:", len(camHandleArray[device_index]))

    cameraHandle = 0

    if cameraModel == "Any":
        cameraHandle = camHandleArray[device_index][0]
    else:
        for cameraHandle in camHandleArray[device_index]:
            (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
            #if "Iron" in camInfo.deviceModelName:
            if camInfo.deviceModelName == cameraModel:
                cameraHandle = cameraHandle
        if cameraHandle == 0:
            print(f'There is no camera {cameraModel} on grabber')
            return CaseReturnCode.NO_HW_FOUND
    # if not check_is_camera_available(cameraModel, camHandleArray[device_index]):
    #     KYFG_Close(grabberHandle)
    #     print(f"\nRequired camera {cameraModel} not found")
    #     return CaseReturnCode.NO_HW_FOUND

    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
    cameraIndex = camHandleArray[device_index].index(cameraHandle)

    print("-----------------------------------------------------------")
    print(f"Selected camera: [{cameraIndex}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
    print("-----------------------------------------------------------\n")

    # check trigger mode
    try:
        if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
            KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)

        if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
            KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)

        if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
            KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
    except:
        pass

    # First period
    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
    print(f'Camera {camInfo.deviceModelName} is open')

    stream_period_1, buffSize_1, stream_callback_count_1 = start_stream(grabberHandle, cameraHandle, cameraIndex)

    # Second period
    stream_period_2, buffSize_2, stream_callback_count_2 = start_stream(grabberHandle, cameraHandle, cameraIndex, segmentsperbuffer)
    stream_period_2 = stream_period_2 / segmentsperbuffer

    # If either period is zero or no callbacks were received, it may indicate that streaming did not start correctly,
    # no buffers were processed, or the callback function was not triggered — possibly due to hardware issues,
    # misconfiguration, or camera connection failure.
    assert stream_period_1 > 0 and stream_period_2 > 0 and \
           stream_callback_count_1 > 0 and stream_callback_count_2 > 0, \
        ('Assertion failed: one of the stream periods is zero or no callbacks were received - this may indicate a '
         'streaming issue or no data received')

    period2_1percent = stream_period_2 / 100
    assert (stream_period_2 - period2_1percent) <= stream_period_1 <= (stream_period_2 + period2_1percent), 'Assertion failed: stream 2 period (avg) is not within +-1% of stream 1 period (avg)'
    assert buffSize_1 == buffSize_2 / segmentsperbuffer, f'Assertion failed: buffSize_1 ({buffSize_1}) is not equal to buffSize_2 ({buffSize_2}) / segmentsperbuffer ({segmentsperbuffer})'

    (status) = KYFG_Close(grabberHandle)

    print(f'\nExiting from CaseRun({args}) with code 0...')

    return CaseReturnCode.SUCCESS


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 3259 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

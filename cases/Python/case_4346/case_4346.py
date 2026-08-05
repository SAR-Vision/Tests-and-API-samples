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
import shutil
import time
import random
import pathlib
import json


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
    parser.add_argument("--number_of_frames", type=int, default=100, help='Number of allocated frames and images for generation')
    parser.add_argument("--width", type=int, default=320, help="Image Width")
    parser.add_argument("--height", type=int, default=284, help='Image height')
    parser.add_argument("--bitDepth", type=int, default=16, help="Image bit depth")
    parser.add_argument("--frameRate", type=int, default=1100, help="Image bit depth")
    parser.add_argument("--ConnectionConfig", type=str, default="x1_CXP_2", help="Connection Config value")
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


class StreamStruct:
    def __init__(self):
        self.callbackCounter = 0
        self.maxCallbackCounter = 0
        self.runStream = False


class ImageDescriptor:
    def __init__(self):
        self.width = 0
        self.height = 0
        self.folderPath = ''
        self.pixelFormat = ""


def streamCallbackFunc(bufferHandle, userContext):
    if bufferHandle == INVALID_STREAM_BUFFER_HANDLE:
        return
    userContext.callbackCounter += 1
    if userContext.callbackCounter >= userContext.maxCallbackCounter:
        userContext.runStream = False
    print(f"userContext.callbackCounter: {userContext.callbackCounter}", end='\r')
    try:
        (status,) = KYFG_BufferToQueue(bufferHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        pass


def generateImage(imageDescriptor: ImageDescriptor, number_of_images: int ):
    bitDepth = 8 if imageDescriptor.pixelFormat.endswith("8") else 16
    for i in range(number_of_images):
        pixels_array = []
        number_of_bytes = int(imageDescriptor.width*imageDescriptor.height)
        [pixels_array.append(random.randint(0, 1 << bitDepth-1).to_bytes(2, byteorder='big', signed=False)) for next_byte in range(number_of_bytes)]
        with open(pathlib.Path(imageDescriptor.folderPath).joinpath(f"image_{i}.raw"), 'wb') as f:
            for b in pixels_array:
                f.write(b)


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
    (status, device_info) = KY_DeviceInfo(device_index)
    if device_info.m_Protocol != KY_DEVICE_PROTOCOL.KY_DEVICE_PROTOCOL_CoaXPress:
        print('Test could not run on this grabber')
        return CaseReturnCode.COULD_NOT_RUN
    images_folder = pathlib.Path(__file__).parent.joinpath("images_folder")
    imageDescriptor = ImageDescriptor()
    imageDescriptor.width = args["width"]
    number_of_frames = args["number_of_frames"]
    imageDescriptor.height = args["height"]
    frameRate = args['frameRate']
    connectionConfig = args["ConnectionConfig"]
    bitDepth = args["bitDepth"]
    if bitDepth not in range(8, 17, 2):
        print("Wrong parameter valaue: bitDepth")
        return CaseReturnCode.WRONG_PARAM_VALUE
    imageDescriptor.pixelFormat = f"Mono{bitDepth}"
    imageDescriptor.folderPath = images_folder.absolute().as_posix()
    if images_folder.exists():
        shutil.rmtree(images_folder)
    images_folder.mkdir()
    generateImage(imageDescriptor, number_of_frames)
    (grabberHandle,) = KYFG_Open(device_index)
    (status, camList) = KYFG_UpdateCameraList(grabberHandle)
    cameraHandle = None
    for cam in camList:
        (status, camInfo) = KYFG_CameraInfo2(cam)
        if "Chameleon" in camInfo.deviceModelName:
            cameraHandle = cam
            break
    if cameraHandle is None:
        print("There is no Chameleon camera on this grabber")
        return CaseReturnCode.NO_HW_FOUND
    (status,) = KYFG_CameraOpen2(cameraHandle, None)

#############################################
    Reset_camera(cameraHandle, grabberHandle)
#############################################
               
    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
    print(camInfo.deviceModelName, "is open for test")
    try:
        if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
            KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)
        if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
            KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 1)
            KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "SimulationTriggerSource", "KY_CAM_TRIG")
    except:
        pass
    KYFG_SetCameraValueInt(cameraHandle, "Width", imageDescriptor.width)
    KYFG_SetCameraValueInt(cameraHandle, "Height", imageDescriptor.height)
    KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "PixelFormat", imageDescriptor.pixelFormat)
    KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "VideoSourceType", "Folder")
    KYFG_SetCameraValueString(cameraHandle, "SourceFolderPath", imageDescriptor.folderPath)
    KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "ConnectionConfig", connectionConfig)
    # timer control
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerSelector", "Timer0")
    KYFG_SetGrabberValueFloat(grabberHandle, "TimerDelay", float(1e6/frameRate/2))
    KYFG_SetGrabberValueFloat(grabberHandle, "TimerDuration", float(1e6/frameRate/2))
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerActivation", "RisingEdge")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_DISABLED")
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camList.index(cameraHandle))
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerMode", "On")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerActivation", "AnyEdge")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerSource", "KY_TIMER_ACTIVE_0")

    streamStruct = StreamStruct()
    streamStruct.maxCallbackCounter = number_of_frames+2

    (status, streamHandle) = KYFG_StreamCreate(cameraHandle,0)
    buffers = [0 for i in range(16)]
    (status, payloadSize, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
    for IFrame in buffers:
        (status, buffers[IFrame]) = KYFG_BufferAllocAndAnnounce(streamHandle, payloadSize, 0)
    (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, streamCallbackFunc, streamStruct)
    (status,) = KYFG_BufferQueueAll(streamHandle,KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
    streamStruct.runStream = True
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_CONTINUOUS")
    # time.sleep(2)
    # if streamStruct.callbackCounter == 0:
    #     print("Acquisition is NOT STARTED")

    while streamStruct.runStream:
        if not streamStruct.runStream:
            break
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_DISABLED")
    print("STOP")
    time.sleep(10)
    (status,) = KYFG_CameraStop(cameraHandle)
    KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
    (status, frameCounter) = KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")
    (status, dropFrameCounter) = KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")

    (status,) = KYFG_StreamBufferCallbackUnregister(streamHandle, streamCallbackFunc)
    (status,) = KYFG_StreamDelete(streamHandle)
    (status,) = KYFG_CameraClose(cameraHandle)
    (status,) = KYFG_Close(grabberHandle)
    shutil.rmtree(imageDescriptor.folderPath)
    print(f"\nTEST Statistic: \n\nFrame counter: {frameCounter}\nDrop frame counter: {dropFrameCounter}\n")
    assert frameCounter > number_of_frames and dropFrameCounter == 0, "Test not passed"
    print(f'\nExiting from CaseRun({args}) with code SUCCESS...')

    return CaseReturnCode.SUCCESS


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


# The flow starts here
if __name__ == "__main__":
    # try:
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    # except Exception as ex:
    #     print(f"Exception of type {type(ex)} occurred: {str(ex)}")
    #     exit(-200)
    #
    # exit(return_code)

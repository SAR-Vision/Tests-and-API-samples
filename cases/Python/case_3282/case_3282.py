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
import xml.etree.ElementTree as ET
import time
from zipfile import ZipFile
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
    parser.add_argument('--number_of_cycles', type=int, default=1000, help='Number of Cycles')
    parser.add_argument('--cameraModel', type=str, default='Any', help='Camera model')
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
cameras_info_before_streaming = {}


class StreamStructure:
    def __init__(self) -> None:
        self.is_assertion_exist = False
        self.callbackCount = 0
        self.cameraHandle = 0


def Stream_callback_func(buffHandle, userContext):
    if buffHandle == 0:
        return

    try:
        (KYFG_BufferToQueue_status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except KYException as err:
        print(f"KYException: {err}")
        return

    #(KYFG_BufferGetInfo_status, pInfoID, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
    #    buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_STREAM_HANDLE)  # UINT32
    #print(f"StreamHandle: {hex(pInfoID)}")

    userContext.callbackCount += 1


def check_is_camera_available(requestedCameraModel, camHandles):
    cams_num = len(camHandles)
    for i in range(cams_num):
        cameraHandle = camHandles[i]
        (Status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        print(camInfo.deviceModelName)
        if camInfo.deviceModelName == requestedCameraModel:
            return True
    return False


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
    number_of_cycles = args['number_of_cycles']
    requestedCameraModel = args['cameraModel']
    streamBufferHandle = {}
    buffers_count = 100
    global cameras_info_before_streaming

    (grabberHandle,) = KYFG_Open(device_index)
    (status, camHandleArray[device_index]) = KYFG_UpdateCameraList(grabberHandle)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    for i in range(device_index):
        try:
            if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
                KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)
        except:
            pass

    cameras_length = len(camHandleArray[device_index])
    print(f"Found {cameras_length} cameras: ")
    if cameras_length == 0:
        print("There are no cameras on this grabber")
        return CaseReturnCode.NO_HW_FOUND

    #    if not check_is_camera_available(cameraModel, camHandleArray[device_index]):
    #        KYFG_Close(grabberHandle)
    #        print(f"\nRequired camera {cameraModel} not found")
    #        return CaseReturnCode.NO_HW_FOUND

    stream_handle = {}
    Stream_callback_func.assertion_exist = False

    supported_cameras = []

    # Open all cameras
    for i in range(cameras_length):
        cameraHandle = camHandleArray[device_index][i]
        (Status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        print(camInfo.deviceModelName)
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

        KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", i)
        
        (_, pixel_format_int, pixel_format_name) = KYFG_GetCameraValue(cameraHandle, "PixelFormat")

        is_supported = False

        if requestedCameraModel != "Any":
            if camInfo.deviceModelName == requestedCameraModel:
                print(f"Requested camera model: {requestedCameraModel} found")
            else:
                (status,) = KYFG_CameraClose(cameraHandle)
                continue

        if pixel_format_name.startswith("Mono"):
            setting_pixel_format = "Mono8"
            is_supported = True
        elif pixel_format_name.startswith('BayerBG'):
            setting_pixel_format = "BayerBG8"
            is_supported = True
        elif pixel_format_name.startswith('BayerGR'):
            setting_pixel_format = "BayerGR8"
            is_supported = True
        elif pixel_format_name.startswith('BayerRG'):
            setting_pixel_format = "BayerRG8"
            is_supported = True
        elif pixel_format_name.startswith('BayerGB'):
            setting_pixel_format = "BayerGB8"
            is_supported = True
        elif pixel_format_name.startswith('RGBA'):
            setting_pixel_format = "RGBA8"
            is_supported = True
        elif pixel_format_name.startswith('RGB'):
            setting_pixel_format = "RGB8"
            is_supported = True
        else:
            print(f"\n Camera does not support 8 bit pixelFormat. Current pixelFormat is: {pixel_format_name}")

        if is_supported:
            supported_cameras.append(i)
        else:
            (status,) = KYFG_CameraClose(cameraHandle)
            continue

        #########################################
        Reset_camera(cameraHandle, grabberHandle)
        #########################################

        KYFG_SetCameraValueInt(cameraHandle, "Width", 1024)
        KYFG_SetCameraValueInt(cameraHandle, "Height", 512)

        (SetCameraValueEnum_ByValueName_status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "PixelFormat",
                                                                                       setting_pixel_format)
        frame_rate = 1000.00
        if KYFG_IsCameraValueImplemented(cameraHandle, "AcquisitionFrameRateMax"):
            (_, max_frame_rate) = KYFG_GetCameraValueFloat(cameraHandle, "AcquisitionFrameRateMax")
            frame_rate = 1000.00 if max_frame_rate >= 1000 else max_frame_rate

        (camval_status,) = KYFG_SetCameraValueFloat(cameraHandle, "AcquisitionFrameRate", frame_rate)

    if len(supported_cameras) == 0:
        print("There are no supported cameras on this grabber")
        return CaseReturnCode.NO_HW_FOUND

    # cameraHandle = camHandleArray[device_index][i]
    # (Status, camInfo) = KYFG_CameraInfo2(cameraHandle)
    # print(f"Supported cameras: {deviceModelName}\n")

    # Start acquisition
    for i in supported_cameras:
        cameraHandle = camHandleArray[device_index][i]
        cameraIndex = camHandleArray[device_index].index(cameraHandle)
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

        print("Supported camera:", camInfo.deviceModelName)

        KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)

        # create stream
        (status, stream_handle[i]) = KYFG_StreamCreate(cameraHandle, i)
        stream_structure = StreamStructure()
        stream_structure.cameraHandle = cameraHandle
        (KYFG_StreamBufferCallbackRegister_status,) = KYFG_StreamBufferCallbackRegister(stream_handle[i],
                                                                                        Stream_callback_func,
                                                                                        stream_structure)

        (_, device_vendor_name) = KYFG_GetCameraValueStringCopy(cameraHandle, 'DeviceVendorName')
        (_, device_model_name) = KYFG_GetCameraValueStringCopy(cameraHandle, 'DeviceModelName')
        cameras_info_before_streaming[int(stream_handle[i])] = {"device_vendor_name": device_vendor_name,
                                                                "device_model_name": device_model_name}

        (status, payload_size, frameDataSize, pInfoType) = KYFG_StreamGetInfo(stream_handle[i],
                                                                              KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

        streamBufferHandle[i] = {}
        for iFrame in range(0, buffers_count):
            (KYFG_BufferAllocAndAnnounce_status, streamBufferHandle[i][iFrame]) = KYFG_BufferAllocAndAnnounce(
                stream_handle[i], payload_size, 0)

        (KYFG_BufferQueueAll_status,) = KYFG_BufferQueueAll(
            stream_handle[i],
            KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
            KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT
        )

        (KYFG_CameraStart_status,) = KYFG_CameraStart(cameraHandle, stream_handle[i], 0)

        cam_before_streaming = cameras_info_before_streaming[int(stream_handle[i])]
        for cycle in range(0, number_of_cycles):
            (_, device_vendor_name) = KYFG_GetCameraValueStringCopy(cameraHandle, 'DeviceVendorName')
            (_, device_model_name) = KYFG_GetCameraValueStringCopy(cameraHandle, 'DeviceModelName')
            #print(f"cycle: {cycle}, callbacks: {stream_structure.callbackCount}, device_vendor_name: {device_vendor_name}, device_model_name:{device_model_name}")
            if cam_before_streaming['device_vendor_name'] != camInfo.deviceVendorName  or cam_before_streaming['device_model_name'] != camInfo.deviceModelName:
                stream_structure.is_assertion_exist = True
                print("DeviceVendorName or DeviceModelName does not match")
                break
            time.sleep(0.01)

        frame_threshold = 5
        dropped_frames = 0

        if stream_structure.is_assertion_exist:
            (CameraStop_status,) = KYFG_CameraStop(cameraHandle)
            KYFG_StreamBufferCallbackUnregister(stream_handle[i], Stream_callback_func)
            KYFG_StreamDelete(stream_handle[i])
            assert stream_structure.is_assertion_exist is False, 'DeviceVendorName or DeviceModelName does not match'
        else:
            (CameraStop_status,) = KYFG_CameraStop(cameraHandle)

            _crc_errors         = 0
            _dropped_packets    = 0
            _dropped_frames     = 0
            _rx_frames          = 0

            if KYFG_IsGrabberValueImplemented(grabberHandle, "CRCErrorCounter"):
                (status, _crc_errors) = KYFG_GetGrabberValue(cameraHandle, "CRCErrorCounter")
                print("CRCErrorCounter:", _crc_errors)
            if KYFG_IsGrabberValueImplemented(grabberHandle, "DropPacketCounter"):
                (status, _dropped_packets) = KYFG_GetGrabberValue(cameraHandle, "DropPacketCounter")
                print("DropPacketCounter:", _dropped_packets)
            if KYFG_IsGrabberValueImplemented(grabberHandle, "DropFrameCounter"):
                (status, _dropped_frames) = KYFG_GetGrabberValue(cameraHandle, "DropFrameCounter")
                print("DropFrameCounter:", _dropped_frames)
            if KYFG_IsGrabberValueImplemented(grabberHandle, "RXFrameCounter"):
                (status, _rx_frames) = KYFG_GetGrabberValue(cameraHandle, "RXFrameCounter")
                print("RXFrameCounter:", _rx_frames)
            
            KYFG_StreamBufferCallbackUnregister(stream_handle[i], Stream_callback_func)
            KYFG_StreamDelete(stream_handle[i])

            (status,) = KYFG_CameraClose(cameraHandle)

            assert _crc_errors == 0, f"CRCErrorCounter:{_crc_errors}"
            assert _dropped_frames < frame_threshold, f"DropFrameCounter:{_dropped_frames}"

    KYFG_Close(grabberHandle)
    print(f'\nExiting from CaseRun({args}) with code 0...')
    return CaseReturnCode.SUCCESS


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 3282 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

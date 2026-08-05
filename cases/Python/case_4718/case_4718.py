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
from datetime import datetime
import time
import threading
import subprocess
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
    parser.add_argument('--cameraIndex', type=int, default=0, help='Camera index for this instance')
    parser.add_argument('--number_of_sent_tests', type=int, default=3, help='Number of sent triggers')
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


class StreamInfoStruct:
    def __init__(self):
        self.callbackCount = 0
        self.instantsFps = []
        self.timestamps = []
        return


def Stream_callback_func(buffHandle, userContext):
    if buffHandle == 0:
        return
    userContext.callbackCount += 1
    (KYFG_BufferGetInfo_status, pInfoFPS, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
        buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_INSTANTFPS)
    (KYFG_BufferGetInfo_status, timestamp, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
        buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_TIMESTAMP)

    userContext.instantsFps.append(pInfoFPS)
    userContext.timestamps.append(timestamp)

    try:
        (status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        return
    return


def is_approximately_equal(num1, num2, tolerance_percent):
    tolerance = tolerance_percent / 100.0
    diff = abs(num1 - num2)
    avg = (abs(num1) + abs(num2)) / 2.0
    return diff <= avg * tolerance


def waitFortime(time_for_sleep):
    threadSleepSeconds = time_for_sleep
    print(f"Thread sleep for {threadSleepSeconds} seconds: ")
    for remaining in range(threadSleepSeconds, 0, -1):
        sys.stdout.write("\r")
        sys.stdout.write("{:2d} seconds remaining".format(remaining))
        sys.stdout.flush()
        time.sleep(1)
    sys.stdout.write("\rComplete!            \n")
    return threadSleepSeconds


errorCount = 0


def new_instance(device_index, camera_index, number_of_sent_tests):
    global errorCount
    instance_process = subprocess.Popen(
        ["python3", f"{__file__}", '--unattended', '--deviceIndex', f'{device_index}', "--cameraIndex",
         f"{camera_index}",
         "--number_of_sent_tests", f"{number_of_sent_tests}"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        close_fds=True)

    stdout_output, stderr_output = instance_process.communicate()
    print(stdout_output.decode())
    print(stderr_output)
    return_code = instance_process.returncode
    print(return_code)
    if return_code != 0:
        errorCount += 1

    return return_code


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

    camera_index = args['cameraIndex']
    number_of_sent_tests = args['number_of_sent_tests']
    # if camera_index == 0:

    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################
    
    print("-----------------------------------------------------------")
    print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
    print("-----------------------------------------------------------\n")

    # try:
    #     (status, isSharingSupported) = KYFG_GetGrabberValueInt(grabberHandle, 'SWCapable_InterProcessSharing_Imp')
    #     if not isSharingSupported:
    #         print('Grabber sharing is not supported on this device')
    #         (status,) = KYFG_Close(grabberHandle)
    #         return CaseReturnCode.COULD_NOT_RUN
    # except:
    #     print('Grabber sharing is not supported on this device')
    #     (status,) = KYFG_Close(grabberHandle)
    #     return CaseReturnCode.COULD_NOT_RUN

    (status, camera_list) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0

    for cam in camera_list:
        (status, camInfo) = KYFG_CameraInfo2(cam)
        if 'Chameleon' in camInfo.deviceModelName:
            camera_list.remove(cam)
            print(f'Camera {camInfo.deviceModelName} removed from camera list for test')
    if camera_index == 0:
        if len(camera_list) == 0:
            return CaseReturnCode.NO_HW_FOUND

    # Detect camera
    cameraHandle = camera_list[camera_index]
    if camera_index == 0:
        (status,) = KYFG_CameraOpen2(camera_list[0], None)
        Reset_camera(cameraHandle, grabberHandle)
    else:
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)

    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
    #########################################
    Reset_camera(cameraHandle, grabberHandle)
    #########################################

    print("-----------------------------------------------------------")
    print(f"Selected camera: [{camera_index}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
    print("-----------------------------------------------------------\n")

    try:
        if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
            KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)

        if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
            KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)

        if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
            KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
    except:
        pass

    camera_master_link = camInfo.master_link
    # Select the "CxpConnectionSelector" where camera is connected (i.e master link of the camera)
    (status, camera_width_type) = KYFG_GetCameraValueType(cameraHandle, "Width")
    (status, camera_pixelFormat_type) = KYFG_GetCameraValueType(cameraHandle, "PixelFormat")
    (status, grabber_deviceStatus_type) = KYFG_GetGrabberValueType(grabberHandle, "DeviceStatus")
    (status, grabber_coreTemperature_type) = KYFG_GetGrabberValueType(grabberHandle, "DeviceTemperature")
    (status, debayer_mode) = KYFG_GetGrabberValueEnum(grabberHandle, "DebayerMode")
    (status, grabberpfName) = KYFG_GetGrabberValueStringCopy(grabberHandle, "PixelFormat")
    (status, camerapfName) = KYFG_GetCameraValueStringCopy(cameraHandle, "PixelFormat")

    if KYFG_IsCameraValueImplemented(cameraHandle, "BinningSelector"):
        (status, camera_binningSelector_type) = KYFG_GetCameraValueType(cameraHandle, "BinningSelector")
        print(f'camera_binningSelector type {KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_ENUM == camera_binningSelector_type}')
        assert (
                KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_ENUM == camera_binningSelector_type), "Incorrect camera property type ENUM is returned"

    # CxpRemoteTransferMaxRetries
    (status, grabber_IMax, grabber_IMin) = KYFG_GetGrabberValueIntMaxMin(grabberHandle, "Width")
    (status, grabber_FMax, grabber_FMin) = KYFG_GetGrabberValueFloatMaxMin(grabberHandle, "AcquisitionFps")
    (status, camera_IMax, camera_IMin) = KYFG_GetCameraValueIntMaxMin(cameraHandle, "Width")
    (status, camera_FMax, camera_FMin) = KYFG_GetCameraValueFloatMaxMin(cameraHandle, "AcquisitionFrameRate")
    #(status, camera_PropertyValue) = KY_GetCameraPropertyParameterValue(cameraHandle, "WidthMin", "ToolTip")
    #(status, grabber_PropertyValue) = KY_GetGrabberPropertyParameterValue(grabberHandle, "Width", "ToolTip")

    print(f"debayer_mode {debayer_mode}")
    assert (debayer_mode == 0) or (debayer_mode == 1), "Incorrect DebayerMode is returned"
    print(f"grabberpfName {grabberpfName}")
    assert (grabberpfName == "Normal") or ("Mono" in grabberpfName) or ("RGB" in grabberpfName) or (
                "Bayer" in grabberpfName), "Incorrect grabber PixelFormat is returned"
    print(f"camerapfName {camerapfName}")
    assert ("Mono" in camerapfName) or ("RGB" in camerapfName) or (
                "Bayer" in camerapfName), "Incorrect camera PixelFormat is returned"

    print(f'camera_width type {KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_INT == camera_width_type}')
    assert (
                KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_INT == camera_width_type), "Incorrect camera property type INT is returned"
    print(f'camera_pixelFormat type {KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_ENUM == camera_pixelFormat_type}')
    assert (
                KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_ENUM == camera_pixelFormat_type), "Incorrect camera property type ENUM is returned"

    print(f'grabber_deviceStatus_type {KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_STRING == grabber_deviceStatus_type}')
    assert (
            KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_STRING == grabber_deviceStatus_type), "Incorrect grabber property type STRING is returned"
    print(f'grabber_coreTemperature_type {KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_INT == grabber_coreTemperature_type}')
    assert (
            KY_CAM_PROPERTY_TYPE.PROPERTY_TYPE_INT == grabber_coreTemperature_type), "Incorrect grabber property type INT is returned"

    print(f'grabber_IMaxMin {grabber_IMax} {grabber_IMin}')
    assert (grabber_IMax > grabber_IMin), "Incorrect grabber IntMaxMin value is returned"
    print(f'grabber_FMaxMin {grabber_FMax} {grabber_FMin}')
    assert (grabber_FMax > grabber_FMin), "Incorrect grabber FloatMaxMin value is returned"
    print(f'camera_IMaxMin {camera_IMax} {camera_IMin}')
    assert (camera_IMax > camera_IMin), "Incorrect camera IntMaxMin value is returned"
    print(f'camera_FMaxMin {camera_FMax} {camera_FMin}')
    assert (camera_FMax > camera_FMin), "Incorrect camera FloatMaxMin value is returned"
    #print(f'camera_PropertyValue - {camera_PropertyValue}')
    #print(f'grabber_PropertyValue - {grabber_PropertyValue}')

    (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
    streamStruct = StreamInfoStruct()
    (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, Stream_callback_func, streamStruct)
    # Retrieve information about required frame buffer size and alignment
    number_of_buffers = 16
    streamAlignedBuffer = {}
    streamBufferHandle = {}
    (KYFG_StreamGetInfo_status, payload_size, frameDataSize, pInfoType) = \
        KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

    (KYFG_StreamGetInfo_status, buf_alignment, frameDataAlignment, pInfoType) = \
        KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)

    for iFrame in range(number_of_buffers):
        streamAlignedBuffer[iFrame] = aligned_array(buf_alignment, c_ubyte, payload_size)
        # print("Address of alligned array: " + hex(addressof(streamAllignedBuffer[iFrame])))
        (status, streamBufferHandle[iFrame]) = KYFG_BufferAnnounce(streamHandle,
                                                                   streamAlignedBuffer[iFrame], None)
    (status,) = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                    KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
    time.sleep(10)
    (status, frame_index) = KYFG_StreamGetFrameIndex(streamHandle)
    (status,) = KYFG_CameraStop(cameraHandle)

    print(f"Frame index: {frame_index}")
    assert (frame_index > 0), "Incorrect index of the last acquired frame"

    (status,) = KYFG_CameraClose(cameraHandle)
    camIndex += 1

    (status,) = KYFG_Close(grabberHandle)

    print(f'\nExiting from CaseRun({args}) with code 0...')
    return CaseReturnCode.SUCCESS


if __name__ == "__main__":
    try:
        print("case 4718 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

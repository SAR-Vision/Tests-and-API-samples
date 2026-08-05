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
    parser.add_argument('--duration', type=int, default=10, help='Stream duration')
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


##########
# Classes
##########
class CameraCallbacksCounter:
    def __init__(self):
        self.streamCallbackCounter = 0
        self.cameraCallbackCounter = 0

class GrabberCallbacksCounter:
    def __init__(self):
        self.auxCallbackCounter = 0
        self.eventCallbackCounter = 0


##########
# Functions
#########
def cameraCallbackFunction(userContext, streamHandle):
    streamInfo = cast(userContext, py_object).value
    streamInfo.cameraCallbackCounter += 1

def device_event_callback_func(userContext, event):
    # print(type(userContext))
    streamInfo = cast(userContext, py_object).value
    streamInfo.eventCallbackCounter += 1

def auxCallbackFunc(bufferHandle, userContext):
    streamInfo = cast(userContext, py_object).value
    streamInfo.auxCallbackCounter += 1

def streamCallbackFunction(buffHandle, userContext):
    if buffHandle == 0:
        return
    userContext.streamCallbackCounter += 1
    try:
        (status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        return
    return


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
    duration = args['duration']
    grabbers = [0 for i in range(infosize_test)]
    grabbers_info = [KY_DEVICE_INFO() for i in range(infosize_test)]
    grabber_callbacks = [0 for i in range(infosize_test)]
    camera_callbacks = [[0 for i in range(4)] for i in range(infosize_test)]
    camera_infos_array = [[KYFGCAMERA_INFO2() for i in range(4)] for i in range(infosize_test)]
    camerasArray = [[0 for i in range (4)] for i in range(infosize_test)]
    streamHandleArray = [[0 for i in range (4)] for i in range(infosize_test)]
    error_count = 0

    for x in range(0, infosize_test):
        (status, device_info) = KY_DeviceInfo(x)
        grabbers_info[x] = device_info
        if device_info.m_Flags == KY_DEVICE_INFO_FLAGS.GRABBER and not device_info.isVirtual:
            try:
                (grabberHandle,) = KYFG_Open(x)
                grabbers[x] = grabberHandle
                (status, camerasArray[x]) = KYFG_UpdateCameraList(grabberHandle)
            except:
                grabbers[x] = 0
                continue
        else:
            grabbers[x] = 0

    # Grabber preparation
    for i in range(len(grabbers)):
        if grabbers[i] == 0:
            continue
        grabberHandle = grabbers[i]
        print(f"\nGrabber: {hex(int(grabberHandle))}")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerSelector", "Timer0")
        (status,) = KYFG_SetGrabberValueFloat(grabberHandle, "TimerDelay", 50000.)
        (status,) = KYFG_SetGrabberValueFloat(grabberHandle, "TimerDuration", 50000.)
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerActivation", "RisingEdge")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_DISABLED")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerEventMode", "RisingEdge")

        # callbacks registration
        grabber_callbacks_counter = GrabberCallbacksCounter()
        (status,) = KYFG_AuxDataCallbackRegister(grabberHandle, auxCallbackFunc, py_object(grabber_callbacks_counter))
        print("AUX Callback registered")

        (status,) = KYDeviceEventCallBackRegister(grabberHandle, device_event_callback_func, py_object(grabber_callbacks_counter))
        print("Event Callback registered")
        grabber_callbacks[i] = grabber_callbacks_counter
        # camera preparation
        for camera_index in range(len(camerasArray[i])):
            cameraHandle = camerasArray[i][camera_index]
            (status, camera_infos_array[i][camera_index]) = KYFG_CameraInfo2(cameraHandle)
            if "Chameleon" in camera_infos_array[i][camera_index].deviceModelName:
                continue
            print(f'Camera: {hex(cameraHandle)} : {camera_infos_array[i][camera_index].deviceModelName}')
            (status,) = KYFG_CameraOpen2(cameraHandle, None)

            #########################################
            Reset_camera(cameraHandle, grabberHandle)
            #########################################
               
            (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camera_index)
            (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerMode", "On")
            (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerActivation", "AnyEdge")
            (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerSource", "KY_TIMER_ACTIVE_0")
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerMode", "On")
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerSource", "LinkTrigger0")

            # callbacks registration and stream start
            camera_callbacks_counter = CameraCallbacksCounter()
            (status,) = KYFG_CameraCallbackRegister(cameraHandle, cameraCallbackFunction, py_object(camera_callbacks_counter))
            print("Camera Callback registered")
            (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
            streamHandleArray[i][camera_index] = streamHandle
            (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, streamCallbackFunction, camera_callbacks_counter)
            print("Stream Callback registered")

            camera_callbacks[i][camera_index] = camera_callbacks_counter
            (status,payload_size, _,_) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
            number_of_buffers = [0 for i in range(16)]

            for iFrame in range(len(number_of_buffers)):
                (status, number_of_buffers[iFrame]) = KYFG_BufferAllocAndAnnounce(streamHandle, payload_size, 0)
            (status,) = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                            KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
            (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)

    # Start trigger generation
    for i in range(len(grabbers)):
        if grabbers[i] == 0:
            continue
        KYFG_SetGrabberValueEnum_ByValueName(grabbers[i], "TimerTriggerSource", "KY_CONTINUOUS")
    time.sleep(duration)

    for i in range(len(grabbers)):
        if grabbers[i] == 0:
            continue
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabbers[i], "TimerTriggerSource", "KY_DISABLED")

    for i in range(len(grabbers)):
        if grabbers[i] == 0:
            continue
        grabberHandle = grabbers[i]
        print(f"\nClosing grabber {hex(grabberHandle.val)}")
        for camera_index in range(len(camerasArray[i])):
            cameraHandle = camerasArray[i][camera_index]
            (status,) = KYFG_CameraStop(cameraHandle)
            print(f'Camera {camera_infos_array[i][camera_index].deviceModelName} closed')
            (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camera_index)
            (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerMode", "Off")
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerMode", "Off")
            (status) = KYFG_CameraCallbackUnregister(cameraHandle, cameraCallbackFunction)
            (status) = KYFG_StreamBufferCallbackUnregister(streamHandleArray[i][camera_index],streamCallbackFunction)
            (status,) = KYFG_StreamDelete(streamHandleArray[i][camera_index])
            (status,) = KYFG_CameraClose(cameraHandle)
        (status,) = KYFG_AuxDataCallbackUnregister(grabberHandle, auxCallbackFunc)
        (status,) = KYDeviceEventCallBackUnregister(grabberHandle, device_event_callback_func)
        (status,) = KYFG_Close(grabberHandle)
        for i in range(len(grabbers)):
            if grabbers[i] == 0:
                continue

            print(f'\nGrabber: {grabbers_info[i].szDeviceDisplayName}: ')
            print("auxCallbackCounter", grabber_callbacks[i].auxCallbackCounter)
            print("eventCallbackCounter", grabber_callbacks[i].eventCallbackCounter)
            for camera_index in range(len(camerasArray[i])):
                if camerasArray[i][camera_index] == 0:
                    continue
                print(f'Camera: {camera_infos_array[i][camera_index].deviceModelName}')
                print("streamCallbackCounter", camera_callbacks[i][camera_index].streamCallbackCounter)
                print("cameraCallbackCounter", camera_callbacks[i][camera_index].cameraCallbackCounter)
                if camera_callbacks[i][camera_index].streamCallbackCounter == 0 \
                        or camera_callbacks[i][camera_index].cameraCallbackCounter == 0:
                    error_count += 1
            if grabber_callbacks[i].auxCallbackCounter == 0 or grabber_callbacks[i].eventCallbackCounter == 0:
                error_count += 1

    assert error_count == 0, 'Test not passed'
    print(f'\nExiting from CaseRun({args}) with code SUCCESS...')
    return CaseReturnCode.SUCCESS


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 3398 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

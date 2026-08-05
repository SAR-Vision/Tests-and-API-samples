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
    parser.add_argument('--slaveDeviceIndex', default=2, type=int, help='Slave grabber ind')
    parser.add_argument('--streamDuration', default=5, type=int, help='Slave grabber ind')
    parser.add_argument('--expectedFPS', default=10, type=int, help='Slave grabber ind')
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
class StreamCallbackStruct():
    def __init__(self):
        self.callbackCounter = 0
        self.timestamps = []


##########
# Functions
##########
def streamCallbackFunc(buffHandle, userContext):
    if buffHandle == 0:
        return
    streamInfo = cast(userContext, py_object).value
    # print('Good callback streams buffer handle: ' + str(format(int(buffHandle), '02x')), end='\r')
    # userContext.callbackCounter += 1
    streamInfo.callbackCounter += 1
    (KYFG_BufferGetInfo_status, pInfoTimestamp, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
        buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_TIMESTAMP)  # UINT64
    streamInfo.timestamps.append(pInfoTimestamp)
    # sys.stdout.flush()
    try:
        (KYFG_BufferToQueue_status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        return
    return


def camera_param_setter(cameraHandle):
    try:
        KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 1)
        KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerSource", 'LinkTrigger0')
    except:
        KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
        KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerSource", 13)


def grabberCameraSetter(grabberHandle,cameraIndex):
    (status,) = KYFG_SetGrabberValueInt(int(grabberHandle), 'CameraSelector', cameraIndex)
    (status,) = KYFG_SetGrabberValueEnum(grabberHandle, "CameraTriggerMode", 1)
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, 'CameraTriggerActivation', "AnyEdge")
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, 'CameraTriggerSource', "KY_TTL_0")


def masterGrabberSetter(masterGrabber, ExpectedFPS):
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'LineSelector', "KY_TTL_0")
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'LineMode', "Output")
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'LineSource', "KY_TIMER_ACTIVE_0")
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'TimerSelector', "Timer0")
    FrameTime = 1e+6 / ExpectedFPS
    (status,) = KYFG_SetGrabberValueFloat(int(masterGrabber), 'TimerDelay', FrameTime/2)
    (status,) = KYFG_SetGrabberValueFloat(int(masterGrabber), 'TimerDuration', FrameTime/2)
    (status,) = KYFG_SetGrabberValueEnum(masterGrabber, "TimerTriggerSource", 0)


def slaveGrabberSetter(masterGrabber):
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'LineSelector', "KY_TTL_0")
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'LineMode', "Input")
    # (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'LineSource', "Disabled")
    (status,) = KYFG_SetGrabberValueEnum(masterGrabber, "LineSource", 0)


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
    slaveDeviceIndex = args['slaveDeviceIndex']
    streamDuration = args['streamDuration']
    expectedFPS = args['expectedFPS']
    (masterGrabber,) = KYFG_Open(device_index)
    (slaveGrabber,) = KYFG_Open(slaveDeviceIndex)
    (status, masterCameraList) = KYFG_UpdateCameraList(masterGrabber)
    (status, slaveCameraList) = KYFG_UpdateCameraList(slaveGrabber)
    assert len(masterCameraList) > 0 and len(slaveCameraList) > 0, 'No cameras'

    # MasterGrabber setting
    masterGrabberSetter(masterGrabber, expectedFPS)
    for cameraHandle in masterCameraList:
        (status,) = KYFG_CameraOpen2(cameraHandle, None)
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)

##############################################
        Reset_camera(cameraHandle)
##############################################

        print(f'Camera {camInfo.deviceModelName} opened on master grabber')
        camera_param_setter(cameraHandle)
        grabberCameraSetter(masterGrabber, masterCameraList.index(cameraHandle))

    # SlaveGrabber parameters setter
    slaveGrabberSetter(slaveGrabber)
    for cameraHandle in slaveCameraList:
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

##############################################
        Reset_camera(cameraHandle)
##############################################

        print(f'Camera {camInfo.deviceModelName} opened on slave grabber')
        camera_param_setter(cameraHandle)
        grabberCameraSetter(slaveGrabber, slaveCameraList.index(cameraHandle))
    masterCallbackStructList = []
    slaveCallbackStructList = []
    masterStreamHandleArray = []
    slaveStreamHandleArray = []
    error_count = 0
    # Master grabber stream prep
    for cameraHandle in masterCameraList:
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
        masterStreamHandleArray.append(streamHandle)
        streamCallbackStruct = StreamCallbackStruct()
        masterCallbackStructList.append(streamCallbackStruct)
        (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, streamCallbackFunc, py_object(streamCallbackStruct))
        (_, payload_size, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
        (_, buf_allignment, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)
        streamBufferHandle = [0 for i in range(16)]
        streamAllignedBuffer = [0 for i in range(16)]
        for iFrame in range(len(streamBufferHandle)):
            streamAllignedBuffer[iFrame] = aligned_array(buf_allignment, c_ubyte, payload_size)
            (status, streamBufferHandle[iFrame]) = KYFG_BufferAnnounce(streamHandle, streamAllignedBuffer[iFrame], None)
        print(f'GrabberHandle: {masterGrabber}; CameraHandle: {hex(cameraHandle)}; streamHandle: {streamHandle}')
        print('Stram preparation completed')
        (status,) = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                        KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
        (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
        print(f"Camera {camInfo.deviceModelName} stream started")
    # Slave grabber stream prep
    for cameraHandle in slaveCameraList:
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
        slaveStreamHandleArray.append(streamHandle)
        streamCallbackStruct = StreamCallbackStruct()
        slaveCallbackStructList.append(streamCallbackStruct)
        (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, streamCallbackFunc, py_object(streamCallbackStruct))
        (_, payload_size, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
        (_, buf_allignment, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)
        streamBufferHandle = [0 for i in range(16)]
        streamAllignedBuffer = [0 for i in range(16)]
        for iFrame in range(len(streamBufferHandle)):
            streamAllignedBuffer[iFrame] = aligned_array(buf_allignment, c_ubyte, payload_size)
            (status, streamBufferHandle[iFrame]) = KYFG_BufferAnnounce(streamHandle, streamAllignedBuffer[iFrame], None)
        (status,) = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                        KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
        print(f'GrabberHandle: {slaveGrabber}; CameraHandle: {hex(cameraHandle)}; streamHandle: {streamHandle}')
        print('Stram preparation completed')
        (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
        print(f"Camera {camInfo.deviceModelName} stream started")
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(masterGrabber, 'TimerSelector', "Timer0")
    (status,) = KYFG_SetGrabberValueEnum(masterGrabber, "TimerTriggerSource", 42)  # Continuous
    time.sleep(streamDuration)
    (status,) = KYFG_SetGrabberValueEnum(masterGrabber, "TimerTriggerSource", 0)
    print('masterCallbackStructList', masterCallbackStructList)
    print('slaveCallbackStructList', slaveCallbackStructList)
    print('masterStreamHandleArray', masterStreamHandleArray)
    print('slaveStreamHandleArray,', slaveStreamHandleArray)
    for cameraHandle in masterCameraList:
        print('cameraHandle', cameraHandle)
        streamHandle = masterStreamHandleArray[masterCameraList.index(cameraHandle)]
        streamCallbackStruct = masterCallbackStructList[masterCameraList.index(cameraHandle)]
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        (status,) = KYFG_CameraStop(cameraHandle)
        (status, frame_counter) = KYFG_GetGrabberValueInt(masterGrabber, "RXFrameCounter")
        (status, drop_frame_counter) = KYFG_GetGrabberValueInt(masterGrabber, "DropPacketCounter")
        (status,) = KYFG_StreamBufferCallbackUnregister(streamHandle, streamCallbackFunc)
        (status,) = KYFG_StreamDelete(streamHandle)
        KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)
        (status,) = KYFG_CameraClose(cameraHandle)
        print(f"Camera {camInfo.deviceModelName} Closed")
        print("frame_counter: ", frame_counter)
        print("drop_frame_counter: ", drop_frame_counter)
        print("callbackCounter: ", streamCallbackStruct.callbackCounter)
        print("Timestamps: ", streamCallbackStruct.timestamps)
        if drop_frame_counter > 0 or frame_counter == 0 or streamCallbackStruct.callbackCounter == 0:
            print("Camera test not Passed")
            error_count += 1

        KYFG_SetGrabberValueEnum(masterGrabber, "CameraTriggerMode", 0)
    for cameraHandle in slaveCameraList:
        print('cameraHandle', cameraHandle)
        streamHandle = slaveStreamHandleArray[slaveCameraList.index(cameraHandle)]
        streamCallbackStruct = slaveCallbackStructList[slaveCameraList.index(cameraHandle)]
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        (status,) = KYFG_CameraStop(cameraHandle)
        (status, frame_counter) = KYFG_GetGrabberValueInt(slaveGrabber, "RXFrameCounter")
        (status, drop_frame_counter) = KYFG_GetGrabberValueInt(slaveGrabber, "DropPacketCounter")
        (status,) = KYFG_StreamBufferCallbackUnregister(streamHandle, streamCallbackFunc)
        (status,) = KYFG_StreamDelete(streamHandle)
        KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)
        (status,) = KYFG_CameraClose(cameraHandle)
        print(f"Camera {camInfo.deviceModelName} Closed")
        print("frame_counter: ", frame_counter)
        print("drop_frame_counter: ", drop_frame_counter)
        print("callbackCounter: ", streamCallbackStruct.callbackCounter)
        print("Timestamps: ", streamCallbackStruct.timestamps)
        if drop_frame_counter > 0 or abs(frame_counter - streamCallbackStruct.callbackCounter) > 1 \
                or abs(frame_counter - (expectedFPS*streamDuration) > 1):
            print("Camera test not Passed")
            error_count += 1

        KYFG_SetGrabberValueEnum(slaveGrabber, "CameraTriggerMode", 0)
    (status,) = KYFG_Close(int(masterGrabber))
    (status,) = KYFG_Close(int(slaveGrabber))
    assert error_count == 0
    print(f'\nExiting from CaseRun({args}) with code SUCCESS...')
    return CaseReturnCode.SUCCESS


# The flow starts here
if __name__ == "__main__":
    try:
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

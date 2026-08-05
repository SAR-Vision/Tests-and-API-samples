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
    parser.add_argument("--camera_model", type=str, default="Any", help="Camera model")
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
        self.width = 0
        self.height = 0
        self.callbackCount = 0
        return


def Stream_callback_func(buffHandle, userContext):
    try:
        (KYFG_BufferToQueue_status,) = KYFG_BufferToQueue(buffHandle ,KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except KYException:
        return
    Stream_callback_func.copyingDataFlag = 0
    return

################################################


def FindResolutionStep(camera_handle, min_value, param):
    step_list = [1, 2, 4, 8, 16, 32]
    original_value = min_value

    for step in step_list:
        try:
            KYFG_SetCameraValueInt(camera_handle, param, original_value + step)
            KYFG_SetCameraValueInt(camera_handle, param, original_value)
            return step
        except KYException:
            pass

    raise RuntimeError(f"Could not determine {param} step")


Stream_callback_func.data = 0
Stream_callback_func.copyingDataFlag = 0


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
    camera_model = args['camera_model']
    streamInfoStruct = StreamInfoStruct()
    streamBufferHandle = [0 for i in range(16)]
    streamAllignedBuffer = [0 for i in range(16)]
    # OPEN device
    (grabberHandle,) = KYFG_Open(device_index)

    ############################
    Reset_grabber(grabberHandle)
    ############################

    device_info = device_infos[device_index]
    print(
        f'Opened device [{device_index}]: (PCI {device_info.nBus}:{device_info.nSlot}:{device_info.nFunction})"{device_info.szDeviceDisplayName}"')
    # scan and open camera
    (status, camHandleArray_col) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0

    print(f'Camera scan result:\nStatus: {status}\nCamHandleArray: {camHandleArray_col}')
    if len(camHandleArray_col) == 0:
        print('There is no cameras on this device')
        return CaseReturnCode.NO_HW_FOUND
    error_count = 0

    camHandle = None

    for cameraHandle in camHandleArray_col:
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)

        if camera_model == "Any":
            print(f'Camera {camInfo.deviceModelName} Found on grabber')
            camHandle = cameraHandle
            break

        if camInfo.deviceModelName == camera_model:
            print(f'Camera {camInfo.deviceModelName} Found on grabber')
            camHandle = cameraHandle
            break

    if camHandle is None:
        print(f"Camera {camera_model} is not found on this grabber")
        return CaseReturnCode.NO_HW_FOUND

    (status,) = KYFG_CameraOpen2(camHandle, None)
    (status, camInfo) = KYFG_CameraInfo2(camHandle)

    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
##########################################
    Reset_camera(camHandle, grabberHandle)
##########################################

    # Set camera selector to current camera index
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camHandleArray_col.index(camHandle))

    # check trigger mode
    try:
        if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
            KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)
        if KYFG_IsCameraValueImplemented(camHandle, "TriggerMode"):
            KYFG_SetCameraValueEnum(camHandle, "TriggerMode", 0)
        if KYFG_IsCameraValueImplemented(camHandle, "SimulationTriggerMode"):
            KYFG_SetCameraValueEnum(camHandle, "SimulationTriggerMode", 0)
    except:
        pass

    KYFG_SetCameraValueInt(camHandle, "Width", 1024)
    KYFG_SetCameraValueInt(camHandle, "Height", 960)
    (status, max_width) = KYFG_GetCameraValueInt(camHandle, "WidthMax")
    (status, max_height) = KYFG_GetCameraValueInt(camHandle, "HeightMax")
    (status, min_width) = KYFG_GetCameraValueInt(camHandle, 'WidthMin')
    (status, min_height) = KYFG_GetCameraValueInt(camHandle, 'HeightMin')
    (status, max_width, min_width) = KYFG_GetCameraValueIntMaxMin(camHandle, "Width")
    (status, max_height, min_height) = KYFG_GetCameraValueIntMaxMin(camHandle, "Height")

    width_step = FindResolutionStep(camHandle, min_width, "Width")
    height_step = FindResolutionStep(camHandle, min_height, "Height")
    for i in range(0, 5):
        width = int((int((max_width - min_width) / 4) * i) / width_step) * width_step + min_width
        height = int((int((max_height - min_height) / 4) * i) / height_step) * height_step + min_height
        streamInfoStruct.width = width
        streamInfoStruct.height = height
        try:
            (status,) = KYFG_SetCameraValueInt(camHandle, "Width", width)
            (status,) = KYFG_SetCameraValueInt(camHandle, "Height", height)
        except:
            print(f"Camera resolution {width}x{height} is invalid")
            continue
        print()
        print(f"Camera resolution is {width}x{height}")
        # stream register
        (KYFG_StreamCreate_status, cameraStreamHandle) = KYFG_StreamCreate(camHandle, 0)
        (KYFG_StreamBufferCallbackRegister_status,) = KYFG_StreamBufferCallbackRegister(cameraStreamHandle,
                                                                                        Stream_callback_func,
                                                                                        py_object(streamInfoStruct))
        # stream info
        (KYFG_StreamGetInfo_status, payload_size, frameDataSize, pInfoType) = \
            KYFG_StreamGetInfo(cameraStreamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

        (KYFG_StreamGetInfo_status, buf_allignment, frameDataAligment, pInfoType) = \
            KYFG_StreamGetInfo(cameraStreamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)
        for iFrame in range(len(streamBufferHandle)):
            # streamAllignedBuffer[iFrame] = aligned_array(buf_allignment, c_ubyte, payload_size)
            (status, streamBufferHandle[iFrame]) = KYFG_BufferAllocAndAnnounce(cameraStreamHandle, payload_size, None)
        for iFrame in range(len(streamBufferHandle)):
            # (status, FPS) = KYFG_GetCameraValue(camera_handle, "AcquisitionFrameRate")
            # The low frame rate select for DropFrames check
            FPS = 4.0
            (status,) = KYFG_SetCameraValueFloat(camHandle, "AcquisitionFrameRate", FPS)

            (KYFG_BufferQueueAll_status,) = KYFG_BufferQueueAll(cameraStreamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                                                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
            (KYFG_CameraStart_status,) = KYFG_CameraStart(camHandle, cameraStreamHandle, 0)
            time_s = int(iFrame / FPS + 1)
            time.sleep(time_s + 1)
            (CameraStop_status,) = KYFG_CameraStop(camHandle)

            # Ensure acquiring started and frames were acquired
            (status_rxf, fg_stat_rxf) = KYFG_GetGrabberValue(grabberHandle, "RXFrameCounter")
            (status_rxp, fg_stat_rxp) = KYFG_GetGrabberValue(grabberHandle, "RXPacketCounter")
            if fg_stat_rxp <= 0 or fg_stat_rxf <= 0:
                error_count += 1
            if fg_stat_rxp < fg_stat_rxf:
                print("RXFrameCounter < RXPacketCounter")
                error_count += 1
            (status, crc_errors) = KYFG_GetGrabberValue(camHandle, "CRCErrorCounter")
            if KYFG_IsGrabberValueImplemented(camHandle, 'DropPacketCounter'):
                (status, dropped_packets) = KYFG_GetGrabberValue(camHandle, "DropPacketCounter")
                print("dropped packets: ", dropped_packets)
            (status, dropped_frames) = KYFG_GetGrabberValue(camHandle, "DropFrameCounter")

            print(f'CRCErrorCounter: {crc_errors}', f'DropFrameCounter: {dropped_frames}')
            print(f'RXFrameCounter: {fg_stat_rxf}', f'RXPacketCounter: {fg_stat_rxp}')
            print("Received frames: " + str(fg_stat_rxf))
            # Currently we have some DropFrames that we allow
            if 0 != dropped_frames or crc_errors != 0:
                print('Not all "CRCErrorCounter", "DropFrameCounter" = 0')
                error_count += 1
            if fg_stat_rxf > fg_stat_rxp:
                print('fg_stat_rxf>fg_stat_rxp')
                error_count += 1
        (CallbackRegister_status,) = KYFG_StreamBufferCallbackUnregister(cameraStreamHandle, Stream_callback_func)
        (status,) = KYFG_StreamDelete(cameraStreamHandle)
    if camHandle > 0:
        (KYFG_CameraClose_status,) = KYFG_CameraClose(camHandle)
        camIndex += 1

    if grabberHandle != 0:
        (KYFG_Close_status,) = KYFG_Close(grabberHandle)
    assert error_count == 0, 'There are errors while test'
    return CaseReturnCode.SUCCESS


if __name__ == "__main__":
    # try:
        print("case 2882 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    # except Exception as ex:
    #     print(f"Exception of type {type(ex)} occurred: {str(ex)}")
    #     exit(-200)
    #
    # exit(return_code)

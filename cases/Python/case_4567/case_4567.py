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


class StreamCallbackStruct:
    def __init__(self):
        self.callbackCounter = 0
        self.invalidBuffers = 0
        self.nullBuffers = 0


def streamCallbackFunction(buffHandle, userContext):
    if buffHandle == 0:
        print(f"buffHandle == 0")
        return

    if buffHandle == INVALID_STREAM_BUFFER_HANDLE:
        userContext.invalidBuffers += 1
        print(f"INVALID_STREAM_BUFFER_HANDLE received in callback: {userContext.callbackCounter}")

    userContext.callbackCounter += 1

    try:
        (status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        pass

BAYER_PIXEL_FORMATS = {
    # ---- BayerGR ----
    0x0311: {"bitness": 8,  "name": "BayerGR8"},
    0x01080008: {"bitness": 8,  "name": "BayerGR8"},

    0x0312: {"bitness": 10, "name": "BayerGR10"},
    0x0110000C: {"bitness": 10, "name": "BayerGR10"},
    0x010A0056: {"bitness": 10, "name": "BayerGR10p"},
    0x010C0026: {"bitness": 10, "name": "BayerGR10Packed"},

    0x0313: {"bitness": 12, "name": "BayerGR12"},
    0x01100010: {"bitness": 12, "name": "BayerGR12"},
    0x010C0057: {"bitness": 12, "name": "BayerGR12p"},
    0x010C002A: {"bitness": 12, "name": "BayerGR12Packed"},

    0x0314: {"bitness": 14, "name": "BayerGR14"},
    0x01100109: {"bitness": 14, "name": "BayerGR14"},
    0x010E0105: {"bitness": 14, "name": "BayerGR14p"},

    0x0315: {"bitness": 16, "name": "BayerGR16"},
    0x0110002E: {"bitness": 16, "name": "BayerGR16"},

    # ---- BayerRG ----
    0x0321: {"bitness": 8,  "name": "BayerRG8"},
    0x01080009: {"bitness": 8,  "name": "BayerRG8"},

    0x0322: {"bitness": 10, "name": "BayerRG10"},
    0x0110000D: {"bitness": 10, "name": "BayerRG10"},
    0x010A0058: {"bitness": 10, "name": "BayerRG10p"},
    0x010C0027: {"bitness": 10, "name": "BayerRG10Packed"},

    0x0323: {"bitness": 12, "name": "BayerRG12"},
    0x01100011: {"bitness": 12, "name": "BayerRG12"},
    0x010C0059: {"bitness": 12, "name": "BayerRG12p"},
    0x010C002B: {"bitness": 12, "name": "BayerRG12Packed"},

    0x0324: {"bitness": 14, "name": "BayerRG14"},
    0x0110010A: {"bitness": 14, "name": "BayerRG14"},
    0x010E0106: {"bitness": 14, "name": "BayerRG14p"},

    0x0325: {"bitness": 16, "name": "BayerRG16"},
    0x0110002F: {"bitness": 16, "name": "BayerRG16"},

    # ---- BayerGB ----
    0x0331: {"bitness": 8,  "name": "BayerGB8"},
    0x0108000A: {"bitness": 8,  "name": "BayerGB8"},

    0x0332: {"bitness": 10, "name": "BayerGB10"},
    0x0110000E: {"bitness": 10, "name": "BayerGB10"},
    0x010A0054: {"bitness": 10, "name": "BayerGB10p"},
    0x010C0028: {"bitness": 10, "name": "BayerGB10Packed"},

    0x0333: {"bitness": 12, "name": "BayerGB12"},
    0x01100012: {"bitness": 12, "name": "BayerGB12"},
    0x010C0055: {"bitness": 12, "name": "BayerGB12p"},
    0x010C002C: {"bitness": 12, "name": "BayerGB12Packed"},

    0x0334: {"bitness": 14, "name": "BayerGB14"},
    0x0110010B: {"bitness": 14, "name": "BayerGB14"},
    0x010E0107: {"bitness": 14, "name": "BayerGB14p"},

    0x0335: {"bitness": 16, "name": "BayerGB16"},
    0x01100030: {"bitness": 16, "name": "BayerGB16"},

    # ---- BayerBG ----
    0x0341: {"bitness": 8,  "name": "BayerBG8"},
    0x0108000B: {"bitness": 8,  "name": "BayerBG8"},

    0x0342: {"bitness": 10, "name": "BayerBG10"},
    0x0110000F: {"bitness": 10, "name": "BayerBG10"},
    0x010A0052: {"bitness": 10, "name": "BayerBG10p"},
    0x010C0029: {"bitness": 10, "name": "BayerBG10Packed"},

    0x0343: {"bitness": 12, "name": "BayerBG12"},
    0x01100013: {"bitness": 12, "name": "BayerBG12"},
    0x010C0053: {"bitness": 12, "name": "BayerBG12p"},
    0x010C002D: {"bitness": 12, "name": "BayerBG12Packed"},

    0x0344: {"bitness": 14, "name": "BayerBG14"},
    0x0110010C: {"bitness": 14, "name": "BayerBG14"},
    0x010E0108: {"bitness": 14, "name": "BayerBG14p"},

    0x0345: {"bitness": 16, "name": "BayerBG16"},
    0x01100031: {"bitness": 16, "name": "BayerBG16"},
}

def GetBayerInfo(pixelFormat: int) -> dict | None:
    pf = int(pixelFormat) & 0xFFFFFFFF
    print(f"PixelFormat: {hex(pf)}")
    return BAYER_PIXEL_FORMATS.get(pf)

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

    streamTimeout = 5

    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    print("-----------------------------------------------------------")
    print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
    print("-----------------------------------------------------------\n")

    (status, timestamp) = KYFG_GetGrabberValueInt(grabberHandle, "Timestamp")
    KYFG_SetGrabberValueString(grabberHandle, "DeviceUserID", "20")
    print(f"Grabber Timestamp {timestamp}; status: {hex(status)}")
    
    (status, cameraList) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0

    print(f'Found {len(cameraList)} cameras')
    if len(cameraList) == 0:
        print("-----------------------------------------------------------")
        print('There is no cameras on this grabber')
        print("-----------------------------------------------------------\n")
        return CaseReturnCode.NO_HW_FOUND

    cameraIndex = 0
    cameraHandle = cameraList[cameraIndex]

    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
    (status,) = KYFG_CameraOpen2(cameraHandle, None)

    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
    #########################################
    Reset_camera(cameraHandle, grabberHandle)
    #########################################

    print("-----------------------------------------------------------")
    print(f"Selected camera: [{cameraIndex}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
    print("-----------------------------------------------------------\n")

    print(f"Camera {camInfo.deviceModelName} is open")

    stream_callback_struct = StreamCallbackStruct()

    (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
    (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, streamCallbackFunction, stream_callback_struct)

    (status, pixelFormat) = KYFG_GetCameraValueEnum(cameraHandle, "PixelFormat")
    print(f"Camera pixel format: {hex(pixelFormat)}")

    ##################################################################################
    # Enabled hardware debayering testing when the camera pixel format matches the supported criteria
    ##################################################################################

    info = GetBayerInfo(pixelFormat)
    if info:
        sourcePixelFormatBitness = info["bitness"]
        sourcePixelFormatName = info["name"]
        bitness = info["bitness"]
        transformationPixelFormatStr = f"RGB{bitness}"
        KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "PixelFormat", transformationPixelFormatStr)
        print(f'#########################################################')
        print(f'Hardware debayering testing enabled')
        print(f'Source pixel format: {hex(pixelFormat)}, "{sourcePixelFormatName}"')
        print(f'Transformation pixel format: "{transformationPixelFormatStr}"')
        print(f'#########################################################')

    ##################################################################################

    number_of_buffer = 16
    buffers = [0 for i in range(number_of_buffer)]
    payloadSize = 0
    (status, payloadSize, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
    print(f"Stream payload size: {payloadSize} (bytes)\n")

    for IFrame in range(number_of_buffer):
        (status, buffers[IFrame]) = KYFG_BufferAllocAndAnnounce(streamHandle, payloadSize, None)
        print(f"Allocated buffer: {IFrame}, payload size: {payloadSize} (bytes), BUFFERHANDLE: {str(buffers[IFrame])}")

    (status,) = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    print(f"\nStream started...")
    (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
    time.sleep(streamTimeout)
    (status,) = KYFG_CameraStop(cameraHandle)
    print(f"Stream finished")

    (status,) = KYFG_StreamBufferCallbackUnregister(streamHandle, streamCallbackFunction)
    (status,) = KYFG_StreamDelete(streamHandle)

    (status,) = KYFG_CameraClose(cameraHandle)
    camIndex += 1
    (status,) = KYFG_Close(grabberHandle)

    print("\nStream test result:")
    print(f'Callbacks: {stream_callback_struct.callbackCounter}')

    assert stream_callback_struct.callbackCounter > 0, (f'Assertion failed: no callbacks were received - this may '
                                                        f'indicate a streaming issue or no data received')
    assert stream_callback_struct.invalidBuffers == 0, (f'Assertion failed: Detected non-zero count of invalid buffers '
                                                        f'during streaming: {stream_callback_struct.invalidBuffers}')

    print(f'\nExiting from CaseRun({args}) with code SUCCESS...')
    return CaseReturnCode.SUCCESS


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 4567 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

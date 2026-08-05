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
import ctypes
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
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
    parser.add_argument('--number_of_sent_tests', type=int, default=0, help='Number of sent triggers')
    return parser


g_isFailState = False


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


def findPoCXPLik(grabberHandle):
    for i in range(8):
        (status, result) = KYFG_GetGrabberValueEnum(grabberHandle, f"PoCXP{i}")
        if result:
            return i


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


def callbackFunc(buffHandle, userContext):
    global g_isFailState
    if buffHandle == NULL_STREAM_BUFFER_HANDLE or buffHandle == INVALID_STREAM_BUFFER_HANDLE:
        return
    (status, base, _, _) = KYFG_BufferGetInfo(buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_BASE)
    (status, size, _, _) = KYFG_BufferGetInfo(buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_SIZE)
    buf_type = ctypes.c_uint8 * size
    buf = buf_type.from_address(base)

    arr = np.frombuffer(buf, dtype=np.uint16)
    # print("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
    arr = arr.reshape((userContext.height, userContext.width))
    if not np.array_equal(arr, userContext.testPatternBuffer):

        userContext.testResult = False
        if not g_isFailState:

            Image.fromarray(arr).save(f"failedImage.png")
            with open(f"failedImage.raw", 'wb')as f:
                f.write(arr.tobytes())
        g_isFailState = True
    else:
        print("Success TO COMPARE IMAGE")
        pass
    (status, frameCount) = KYFG_GetGrabberValueInt(userContext.grabberHandle, "RXFrameCounter")
    (status, dropFrameCounter) = KYFG_GetGrabberValueInt(userContext.grabberHandle, "DropFrameCounter")
    print(f"frameCount = {frameCount} dropFrameCounter = {dropFrameCounter}", end='\r')
    userContext.frameCounter += 1
    try:
        (KYFG_BufferToQueue_status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        return
    return


class CallbackStruct:
    testPatternBuffer = []
    testResult = True
    width = 0
    height = 0
    grabberHandle = 0x0
    frameCounter = 0

    def __init__(self, buffer: np.array, width, height, grabberHandle):
        self.testPatternBuffer = np.array(buffer)
        self.width = width
        self.height = height
        self.grabberHandle = grabberHandle


class TestPatternDescriptor():
    m_width = 0
    m_height = 0
    m_min = 0
    m_max = 0
    m_step = 0
    m_buffer = []

    def __init__(self, width, height, min, max, step):
        self.m_width = width
        self.m_height = height
        self.m_min = min
        self.m_max = max
        self.m_step = step
        if self.m_width < self.m_max:
            self.m_max = self.m_width

    def generate_horizontal_pattern(self, dtype=np.uint16):

        values = np.arange(self.m_min, self.m_max + 1, self.m_step, dtype=dtype)

        if len(values) == 0:
            raise ValueError("Invalid range/step")

        repeats = int(np.ceil(self.m_width / len(values)))
        row = np.tile(values, repeats)[:self.m_width]

        self.m_buffer = np.tile(row, (self.m_height, 1))

        return self.m_buffer

    def show_pattern(self):
        if self.m_buffer is None:
            self.generate_horizontal_pattern()

        plt.figure()
        plt.imshow(self.m_buffer, cmap='gray',)
        plt.colorbar()
        plt.title("Horizontal Test Pattern")
        plt.xlabel("Width")
        plt.ylabel("Height")
        plt.show()


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

    ##########################################################################

    number_of_sent_tests = args['number_of_sent_tests']
    # if camera_index == 0:

    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    (status, camera_list) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0
    (status,) = KYFG_CameraOpen2(camera_list[camIndex], None)
    cameraHandle = camera_list[camIndex]
    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)

    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
    #########################################
    Reset_camera(cameraHandle, grabberHandle)
    #########################################

    print("-----------------------------------------------------------")
    print(f"Selected camera: [{camIndex}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
    print("-----------------------------------------------------------\n")

    PoXCPLink = findPoCXPLik(grabberHandle)

    (status, width) = KYFG_GetCameraValueInt(cameraHandle, "Width")
    (status, height) = KYFG_GetCameraValueInt(cameraHandle, "Height")
    (status, minValue) = KYFG_GetCameraValueInt(cameraHandle, "TestPatternValueMin")
    (status, maxValue) = KYFG_GetCameraValueInt(cameraHandle, "TestPatternValueMax")
    (status, step) = KYFG_GetCameraValueInt(cameraHandle, "TestPatternStep")

    testPattern = TestPatternDescriptor(width, height, minValue, maxValue, step)
    testPattern.generate_horizontal_pattern()
    #testPattern.show_pattern()
    KYFG_CameraClose(cameraHandle)

    KYFG_SetGrabberValueBool(grabberHandle, "PoCXPAutoActive", False)
    KYFG_SetGrabberValueEnum(grabberHandle, f"PoCXP{PoXCPLink}", 0)
    KYFG_Close(grabberHandle)

    for i in range(number_of_sent_tests):
        print(f"START TEST NUMBER {i+1}")
        time.sleep(2)
        KYFG_Open(device_index)
        KYFG_SetGrabberValueEnum(grabberHandle, f"PoCXP{PoXCPLink}", 1)
        time.sleep(5)

        (status, camera_list) = KYFG_UpdateCameraList(grabberHandle)
        (status,) = KYFG_CameraOpen2(camera_list[camIndex], None)
        KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TestPattern", "GrayHorizontalRamp")
        (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
        streamCallbackStruct = CallbackStruct(testPattern.m_buffer, width, height, grabberHandle)

        (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, callbackFunc, streamCallbackStruct)

        (_, payload_size, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
        (_, buf_alignment, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)
        streamBufferHandle = [0 for i in range(16)]

        for iFrame in range(len(streamBufferHandle)):
            (status, streamBufferHandle[iFrame]) = KYFG_BufferAllocAndAnnounce(streamHandle, payload_size, 0)
        KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)

        KYFG_CameraStart(cameraHandle, streamHandle, 0)
        time.sleep(3)
        KYFG_CameraStop(cameraHandle)

        (status, frameCount) = KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")
        (status, dropFrameCounter) = KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")


        KYFG_StreamDelete(streamHandle)
        KYFG_CameraClose(cameraHandle)
        print(f"Test statistic: frame counter {frameCount} Drop frames: {dropFrameCounter} callbacks {streamCallbackStruct.frameCounter}")
        assert (frameCount != 0 and dropFrameCounter == 0), f"TEST NUMBER {i+1} IS FAILED"
        assert streamCallbackStruct.testResult, f"TEST NUMBER {i + 1} IS FAILED"
        print(f"TEST NUMBER {i+1} IS PASSED")

        KYFG_SetGrabberValueEnum(grabberHandle, f"PoCXP{PoXCPLink}", 0)
        KYFG_Close(grabberHandle)

    KYFG_Open(device_index)
    KYFG_SetGrabberValueBool(grabberHandle, "PoCXPAutoActive", True)
    KYFG_Close(grabberHandle)
    print(f'\nExiting from CaseRun({args}) with code 0...')
    return CaseReturnCode.SUCCESS


if __name__ == "__main__":
    try:
        print("case 5695 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

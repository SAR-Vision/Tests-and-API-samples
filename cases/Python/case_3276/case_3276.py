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
import numpy as np
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
    parser.add_argument('--camera', default='S-65A35-Kc/CXP-6')
    parser.add_argument('--width', type=int, default=5120)
    parser.add_argument('--height', type=int, default=5120)
    parser.add_argument('--cameraPixelFormat', type=str, default='BayerRG10')
    parser.add_argument('--testImageSelector', type=str, default='AdimecTestPattern')
    parser.add_argument('--normal_expected_raw', type=str, default='Adimec_before_debayering')
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


def get_numpy_array_expected(normal_expected_raw, width, height, data_type):
    global numpy_array_expected
    file_path = normal_expected_raw + ".raw"

    # Read the raw file as binary data
    with open(file_path, 'rb') as f:
        binary_data = f.read()

    shape = (width, height)  # Adjust this to match the desired shape
    numpy_array_expected = np.frombuffer(binary_data, dtype=data_type).reshape(width, height)

    return numpy_array_expected


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


def numpy_from_data(buffData, buffSize, datatype):
    data_pointer = ctypes.cast(buffData, ctypes.c_char_p)
    buffer_from_memory = ctypes.pythonapi.PyMemoryView_FromMemory
    buffer_from_memory.restype = ctypes.py_object
    # buffer_from_memory.restype=ctypes.c_uint16
    buffer = buffer_from_memory(data_pointer, buffSize)
    return np.frombuffer(buffer, datatype)


class StreamInfoStruct:
    def __init__(self):
        self.width = 0
        self.height = 0
        self.datatype = 0
        self.callbackCount = 0
        self.numpy_array_expected=[]
        self.successful_comparison_counter=0
        self.acquired_images=[]
        return


def callbackFunc(buffHandle,userContext):
    try:
        (KYFG_BufferGetInfo_status, pInfoPTR, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
            buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_BASE)  # PTR
        (KYFG_BufferGetInfo_status, bufSize, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
            buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_SIZE)  # SIZET
        userContext.callbackCount+=1
        acquired_image = numpy_from_data(pInfoPTR, bufSize, userContext.datatype).reshape(
                userContext.height, userContext.width)
        userContext.acquired_images.append(acquired_image)
        if np.array_equal(numpy_array_expected, acquired_image):
            print('IMAGES ARE EQUALS')
            userContext.successful_comparison_counter+=1
        (KYFG_BufferToQueue_status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except: return
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
    camera = args["camera"]
    width = args["width"]
    height = args["height"]
    cameraPixelFormat = args["cameraPixelFormat"]
    testImageSelector = args["testImageSelector"]
    normal_expected_raw = str(pathlib.Path(__file__).parent.joinpath(args["normal_expected_raw"]))
    # Open selected PCI device (grabber)

    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    (status, cameraList) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0
    if len(cameraList) == 0:
        print('There are no cameras on this grabber')
        return CaseReturnCode.NO_HW_FOUND

    cameraHandle = 0
    for cam_handle in cameraList:
        (status, camInfo) = KYFG_CameraInfo2(cam_handle)
        if camera == camInfo.deviceModelName:
            cameraHandle = cam_handle

    if cameraHandle == 0:
        print(f'There is no camera {camera} on grabber')
        return CaseReturnCode.NO_HW_FOUND

    # Camera open and start
    (status,) = KYFG_CameraOpen2(cameraHandle, None)

    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
    #########################################
    Reset_camera(cameraHandle, grabberHandle)
    #########################################

    # Set all required camera parameters (received from the parameters)
    (status,) = KYFG_SetCameraValueInt(cameraHandle, "Width", width)
    (status,) = KYFG_SetCameraValueInt(cameraHandle, "Height", height)
    (status, width) = KYFG_GetCameraValueInt(cameraHandle, "Width")
    (status, height) = KYFG_GetCameraValueInt(cameraHandle, "Height")
    # check trigger mode
    try:
        if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
            KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)
        if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
            KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)
    except:
        pass
    if KYFG_IsCameraValueImplemented(cameraHandle, 'TestImageSelector'):
        KYFG_SetCameraValueEnum(cameraHandle, "TestImageSelector", 0)
    else:
        print("TestImageSelector is not supported on this camera")
        return CaseReturnCode.NO_HW_FOUND
    (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "PixelFormat", cameraPixelFormat)
    (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TestImageSelector", testImageSelector)
    # Create the stream
    (status,streamHandle) = KYFG_StreamCreateAndAlloc(cameraHandle,1,0)
    # Register the callback function
    streamInfoStruct = StreamInfoStruct()
    streamInfoStruct.width=width
    streamInfoStruct.height=height

    if cameraPixelFormat[-1] == "8":
        streamInfoStruct.datatype = np.uint8
    else:
        streamInfoStruct.datatype = np.uint16

    numpy_array_expected = get_numpy_array_expected(normal_expected_raw, width, height, streamInfoStruct.datatype)
    streamInfoStruct.numpy_array_expected = numpy_array_expected
    number_of_buffers = 1
    streamAlignedBuffer = {}
    streamBufferHandle = {}

    (status,) = KYFG_StreamBufferCallbackRegister(streamHandle,callbackFunc,streamInfoStruct)
    # Retrieve information about required frame buffer size and alignment
    (KYFG_StreamGetInfo_status, payload_size, frameDataSize, pInfoType) = \
        KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

    (KYFG_StreamGetInfo_status, buf_allignment, frameDataAlignment, pInfoType) = \
        KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)
    for iFrame in range(number_of_buffers):
        streamAlignedBuffer[iFrame] = aligned_array(buf_allignment, c_ubyte, payload_size)
        # print("Address of alligned array: " + hex(addressof(streamAllignedBuffer[iFrame])))
        (status, streamBufferHandle[iFrame]) = KYFG_BufferAnnounce(streamHandle,
                                                                   streamAlignedBuffer[iFrame], None)
    # Start the camera and check that you received the callback
    (KYFG_BufferQueueAll_status,) = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                                        KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    (status,) = KYFG_CameraStart(cameraHandle,streamHandle,number_of_buffers)
    time.sleep(2)
    # Stop the camera
    (status,) = KYFG_CameraStop(cameraHandle)

    (status,) = KYFG_StreamBufferCallbackUnregister(streamHandle,callbackFunc)
    (status,) = KYFG_StreamDelete(streamHandle)
    (status,) = KYFG_CameraClose(cameraHandle)
    camIndex += 1

    (status,) = KYFG_Close(grabberHandle)
    assert streamInfoStruct.successful_comparison_counter>0, 'Images are not equals'
    return CaseReturnCode.SUCCESS


# The flow starts here
if __name__ == "__main__":
    # try:
        print("case 3276 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    # except Exception as ex:
    #     print(f"Exception of type {type(ex)} occurred: {str(ex)}")
    #     exit(-200)
    #
    # exit(return_code)

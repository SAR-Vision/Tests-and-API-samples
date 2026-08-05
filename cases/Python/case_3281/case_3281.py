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
from zipfile import ZipFile
import time
import numpy as np
import xml.etree.ElementTree as ET
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
    parser.add_argument('--decimationVertical', type=int, default=2, help='Decimation Vertical (INT)')
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

# Callback functions


class StreamInfoStruct:
    def __init__(self):
        self.width = 0
        self.height = 0
        self.datatype = 0
        self.callbackCount = 0
        self.decimationVertical = 0
        self.stream_buffer_data = []
        return


stream_buffer_data = []


def stream_callback_func(buffHandle, userContext):
    global stream_buffer_data
    if buffHandle == 0:
        return
    print('Good callback streams buffer handle: ' + str(buffHandle))
    userContext.callbackCount += 1
    (_, buffData, _, _) = KYFG_BufferGetInfo(buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_BASE)
    (_, buffSize, _, _) = KYFG_BufferGetInfo(buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_SIZE)
    try:
        np_array = numpy_from_data(buffData, buffSize, userContext.datatype).reshape(
            userContext.height, userContext.width)
    except:
        print(f'cannot reshape array of size {buffSize} into shape ({userContext.height},{userContext.width})')
        np_array = numpy_from_data(buffData, buffSize, userContext.datatype).reshape(
            int(userContext.height / userContext.decimationVertical), userContext.width)
    stream_buffer_data.append(np_array.copy())
    (KYFG_BufferToQueue_status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)

    return


# Other functions
def numpy_from_data(buffData, buffSize, datatype):
    data_pointer = ctypes.cast(buffData, ctypes.c_char_p)
    buffer_from_memory = ctypes.pythonapi.PyMemoryView_FromMemory
    buffer_from_memory.restype = ctypes.py_object
    # buffer_from_memory.restype=ctypes.c_uint16
    buffer = buffer_from_memory(data_pointer, buffSize)
    return np.frombuffer(buffer, datatype)


def get_enums_from_xml(camHandle, paramName):
    (KYFG_CameraGetXML_status, isZipped, buffer) = KYFG_CameraGetXML(camHandle)
    if isZipped == False:
        xmlContent = ''.join(buffer)
    else:
        newFile = open(f"{os.path.dirname(__file__)}/camera_xml.zip", "wb")
        newFile.write(bytes(buffer))
        newFile.close()
        with ZipFile(f"{os.path.dirname(__file__)}/camera_xml.zip", "r") as zipxml:
            for name in zipxml.namelist():
                _, ext = os.path.splitext(name)
                if ext == '.xml':
                    with zipxml.open(name) as camera_xml:
                        xmlContent = camera_xml.read()
    root = ET.fromstring(xmlContent)
    ns = '{http://www.genicam.org/GenApi/Version_1_1}'

    test_patterns = []
    try:
        for group in root.findall(f".//*[@Name='{paramName}']/{ns}EnumEntry"):
            pattern_value = group.find(f"{ns}Value").text
            pattern_display_name = str(group.find(f"{ns}DisplayName").text).replace(chr(32), '')
            pattern_name = group.attrib.get("Name")
            test_patterns.append({"name": pattern_name, "value": pattern_value, "displayName": pattern_display_name})
    except:
        for group in root.findall(f".//*[@Name='{paramName}']/{ns}EnumEntry"):
            pattern_value = group.find(f"{ns}Value").text
            pattern_name = group.attrib.get("Name")
            test_patterns.append({"name": pattern_name, "value": pattern_value})
    if os.path.exists(f"{os.path.dirname(__file__)}/camera_xml.zip"):
        os.remove(f"{os.path.dirname(__file__)}/camera_xml.zip")
    return test_patterns


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
    global stream_buffer_data
    (status, device_info) = KY_DeviceInfo(device_index)
    if device_info.m_Protocol != KY_DEVICE_PROTOCOL.KY_DEVICE_PROTOCOL_CoaXPress:
        print('Test could not run on this grabber')
        return CaseReturnCode.COULD_NOT_RUN
    decimation_vertical = args['decimationVertical']
    error_count = 0
    success_count = 0
    (grabberHandle,) = KYFG_Open(device_index)
    (status, cameraList) = KYFG_UpdateCameraList(grabberHandle)
    if len(cameraList) == 0:
        print('There is no cameras on this grabber')
        return CaseReturnCode.NO_HW_FOUND
    for cameraHandle in cameraList:
        number_of_frames = 2
        stream_buffer_data = []
        (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

##############################################
        Reset_camera(cameraHandle)
##############################################
               
        try:
            if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
                KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)
            if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
                KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)
            if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
                KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
        except:
            pass
        print(f'\n\nCamera {camInfo.deviceModelName} is open')
        if 'Chameleon' not in camInfo.deviceModelName:
            available_test_patterns = get_enums_from_xml(cameraHandle, 'TestPattern')
        else:
            available_test_patterns = get_enums_from_xml(cameraHandle, 'VideoSourcePatternType')
        if len(available_test_patterns) == 0:
            print('There are no test patterns on camera', camInfo.deviceModelName)
            continue
        try:
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "PixelFormat", 'Mono8')
        except:
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "PixelFormat", 'BayerBG8')
        (status, pFormat) = KYFG_GetCameraValueEnum(cameraHandle, "PixelFormat")
        print(f'Pixel format {pFormat} set')
        # Looking for available test patterns
        test_pattern = None
        for pattern in available_test_patterns:
            if 'vertical' in pattern['name'].lower() or 'horizontal' in pattern['name'].lower():
                test_pattern = pattern['name']
                break
        if test_pattern is None:
            for pattern in available_test_patterns:
                if 'vertical' in pattern['displayName'].lower() or 'horizontal' in pattern['displayName'].lower():
                    if 'Chameleon' in camInfo.deviceModelName:
                        test_pattern = int(pattern['value'])
                    else:
                        test_pattern = pattern['name']
                    break
        if test_pattern is None:
            print("any vertical or horizontal pattern doesn't exists on camera", camInfo.deviceModelName)
            continue

        print("test_pattern", test_pattern)
        if 'Chameleon' in camInfo.deviceModelName:
            KYFG_SetCameraValueEnum(cameraHandle, "VideoSourcePatternType", test_pattern)
        else:
            (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, 'TestPattern', test_pattern)
        print("test_pattern", test_pattern, 'set')
        # create stream struct object
        streamInfoStruct = StreamInfoStruct()
        streamInfoStruct.decimationVertical = decimation_vertical
        (KYFG_GetValue_status, width) = KYFG_GetCameraValue(cameraHandle, "Width")
        (KYFG_GetValue_status, height) = KYFG_GetCameraValue(cameraHandle, "Height")
        streamInfoStruct.width = width
        streamInfoStruct.height = height
        (status, camera_pixel_format_int, camera_pixel_format) = KYFG_GetCameraValue(cameraHandle, "PixelFormat")
        if camera_pixel_format[-1] == "8":
            streamInfoStruct.datatype = np.uint8
        else:
            streamInfoStruct.datatype = np.uint16
        # Stream prepare
        number_of_buffers = 16

        for i in range(number_of_frames):
            (status, currentDec) = KYFG_GetGrabberValueInt(grabberHandle, "DecimationVertical")
            print(f'DecimationVertical in the {i} time {currentDec}')
            streamAlignedBuffer = {}
            streamBufferHandle = {}
            (KYFG_StreamCreate_status, cameraStreamHandle) = KYFG_StreamCreate(cameraHandle, 0)
            (KYFG_StreamBufferCallbackRegister_status,) = KYFG_StreamBufferCallbackRegister(cameraStreamHandle,
                                                                                            stream_callback_func,
                                                                                            streamInfoStruct)
            (KYFG_StreamGetInfo_status, payload_size, frameDataSize, pInfoType) = \
                KYFG_StreamGetInfo(cameraStreamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

            (KYFG_StreamGetInfo_status, buf_allignment, frameDataAlignment, pInfoType) = \
                KYFG_StreamGetInfo(cameraStreamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)

            # allocate memory for desired number of frame buffers
            for iFrame in range(number_of_buffers):
                streamAlignedBuffer[iFrame] = aligned_array(buf_allignment, c_ubyte, payload_size)
                (status, streamBufferHandle[iFrame]) = KYFG_BufferAnnounce(cameraStreamHandle,
                                                                           streamAlignedBuffer[iFrame], None)
            # start first streaming
            (KYFG_BufferQueueAll_status,) = KYFG_BufferQueueAll(cameraStreamHandle,
                                                                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                                                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
            (KYFG_CameraStart_status,) = KYFG_CameraStart(cameraHandle, cameraStreamHandle, 1)
            time.sleep(1)
            (status,) = KYFG_CameraStop(cameraHandle)
            try:
                matrixData = stream_buffer_data[i]
            except:
                print(f'Acquisition is not started on camera {camInfo.deviceModelName}')
                error_count += 1
            (status,) = KYFG_StreamBufferCallbackUnregister(cameraStreamHandle, stream_callback_func)

            (status,) = KYFG_StreamDelete(cameraStreamHandle)
            if len(stream_buffer_data) == 1:
                (status,) = KYFG_SetGrabberValueInt(grabberHandle, "DecimationVertical", decimation_vertical)
            elif len(stream_buffer_data) == 2:
                (status,) = KYFG_SetGrabberValueInt(grabberHandle, "DecimationVertical", 1)
                data_after_decimal = stream_buffer_data[0][::decimation_vertical]
                if len(data_after_decimal) != len(stream_buffer_data[1]):
                    data_after_decimal = data_after_decimal[:-1]
                print(data_after_decimal.shape)
                print(stream_buffer_data[1].shape)
                if np.array_equal(data_after_decimal, stream_buffer_data[1]):
                    print('STREAMS DATA ARE EQUAL')
                else:
                    print('Buffer data not same')
                    error_count += 1
    (status,) = KYFG_Close(grabberHandle)
    assert error_count == 0, 'There are errors while test'
    print(f'\nExiting from CaseRun({args}) with code 0...')
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

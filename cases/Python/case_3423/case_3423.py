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
from ctypes import py_object


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
    parser.add_argument('--encoderInverterValue', default=1, type=int, help='Inverter value for encoder')
    parser.add_argument('--encoderPositionTrigger', default=8, type=int, help='Encoder Position Trigger')
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


############
# Classes
############
class AXUCallbackStruct:
    def __init__(self):
        self.callbackCount = 0


class StreamCallbackStruct:
    def __init__(self):
        self.callbackCount = 0


############
# Functions
############

def AUXCallbackFunc(streamHandle, userContext):
    callbackStruct = cast(userContext, py_object).value
    callbackStruct.callbackCount += 1
    # print('TRIGGER', callbackStruct.callbackCount)


def streamCallbackFunc(buffHandle, userContext):
    if buffHandle == 0:
        return
    userContext.callbackCount += 1
    # print("Frame", userContext.callbackCount)
    try:
        (status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        return
    return


def SourcesControl(grabberHandle, source: str, command: int):
    # source - Source of the encoder
    # command = Rising or falling (1, 0)
    assert command == 0 or command == 1, 'Wrong value of command'
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "UserOutputSelector",
                                         f"UserOutput{'0' if source == 'A' else '1'}")
    KYFG_SetGrabberValueBool(grabberHandle, "UserOutputValue", bool(command))


def fourStepsForward(grabberHandle):
    print("\nSteps forward")
    print("Current position: ", KYFG_GetGrabberValueInt(grabberHandle, "EncoderValue")[1])
    SourcesControl(grabberHandle, "A", 1)
    time.sleep(1)
    SourcesControl(grabberHandle, "B", 1)
    time.sleep(1)
    SourcesControl(grabberHandle, "A", 0)
    time.sleep(1)
    SourcesControl(grabberHandle, "B", 0)
    time.sleep(1)
    print("New position: ", KYFG_GetGrabberValueInt(grabberHandle, "EncoderValue")[1])


def fourStepsBackward(grabberHandle):
    print("\nSteps backward")
    print("Current position: ", KYFG_GetGrabberValueInt(grabberHandle, "EncoderValue")[1])
    SourcesControl(grabberHandle, "B", 1)
    time.sleep(1)
    SourcesControl(grabberHandle, "A", 1)
    time.sleep(1)
    SourcesControl(grabberHandle, "B", 0)
    time.sleep(1)
    SourcesControl(grabberHandle, "A", 0)
    time.sleep(1)
    print("New position: ", KYFG_GetGrabberValueInt(grabberHandle, "EncoderValue")[1], '\n')


def encoderSetting(grabberHandle, encoderInverterValue, EncoderPositionTrigger):
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderSelector", "Encoder0")
    KYFG_SetGrabberValueBool(grabberHandle, "EncoderInverter", encoderInverterValue)  # change after bug fixing
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderASource", "KY_TTL_0")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderBSource", "KY_TTL_1")
    KYFG_SetGrabberValueInt(grabberHandle, "EncoderValue", 6)
    KYFG_SetGrabberValueInt(grabberHandle, "EncoderPositionTrigger", EncoderPositionTrigger)
    KYFG_SetGrabberValueFloat(grabberHandle, "EncoderFilter", 1.)
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderEventEnable", "Disable")  # change if it is needed


def sourceASetting(grabberHandle):
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "LineSelector", "KY_TTL_0")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "LineMode", "Output")
    KYFG_SetGrabberValueBool(grabberHandle, "LineInverter", False)
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "LineSource", "KY_USER_OUT_0")


def sourceBSetting(grabberHandle):
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "LineSelector", "KY_TTL_1")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "LineMode", "Output")
    KYFG_SetGrabberValueBool(grabberHandle, "LineInverter", False)
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "LineSource", "KY_USER_OUT_1")


def grabberCameraSetting(grabberHandle, cameraIndex: int):
    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)
    # KYFG_SetGrabberValueInt(grabberHandle, "CxpConnectionSelector", cameraIndex)
    KYFG_SetGrabberValueEnum(grabberHandle, "CameraTriggerMode", 1)
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerActivation", "AnyEdge")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerSource", "KY_ENCODER_0")
    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerEventEnable", "Enable")
    if KYFG_IsGrabberValueImplemented(grabberHandle, "TriggerSentCount"):
        KYFG_SetGrabberValueInt(grabberHandle, "TriggerSentCount", 0)


def cameraSetting(cameraHandle, triggerMode: int):
    KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", triggerMode)
    KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerSource", "LinkTrigger0")


def chameleonSetting(cameraHandle, triggerMode: int):
    KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", triggerMode)
    KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "SimulationTriggerSource", "KY_CAM_TRIG")


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

    # if device_info.m_Protocol != KY_DEVICE_PROTOCOL.KY_DEVICE_PROTOCOL_CoaXPress:
    #     print('Test could not run on this grabber')
    #     return CaseReturnCode.COULD_NOT_RUN

    encoderInverterValue = bool(args['encoderInverterValue'])
    EncoderPositionTrigger = args['encoderPositionTrigger']
    error_count = 0

    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    (status, camList) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0

    if len(camList) == 0:
        print('There is no cameras on this grabber')
        error_count += 1
        return CaseReturnCode.NO_HW_FOUND

    aux_callback_struct = AXUCallbackStruct()
    (status,) = KYFG_AuxDataCallbackRegister(grabberHandle, AUXCallbackFunc, py_object(aux_callback_struct))
    encoderSetting(grabberHandle, encoderInverterValue, EncoderPositionTrigger)
    time.sleep(1)
    sourceASetting(grabberHandle)
    sourceBSetting(grabberHandle)
    HWFound = True
    for cameraIndex in range(len(camList)):
        cameraHandle = camList[cameraIndex]
        (status, cameraInfo) = KYFG_CameraInfo2(cameraHandle)
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

        KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
        #########################################
        Reset_camera(cameraHandle, grabberHandle)
        #########################################

        print(f'\nCamera {cameraInfo.deviceModelName} open for test')
        try:
            if "Chameleon" in cameraInfo.deviceModelName:
                chameleonSetting(cameraHandle, 1)
            else:
                cameraSetting(cameraHandle, 1)
        except:
            print("There is no triggerMode on this camera")
            HWFound = False
            continue

        grabberCameraSetting(grabberHandle, cameraIndex)
        (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
        stream_struct = StreamCallbackStruct()
        (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, streamCallbackFunc, stream_struct)
        buffers_array = [0 for i in range(16)]
        (status, payload_size, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

        for iFrame in range(len(buffers_array)):
            (status, buffers_array[iFrame]) = KYFG_BufferAllocAndAnnounce(streamHandle, payload_size, 0)
        (status,) = KYFG_BufferQueueAll(streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                                        KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
        (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
        time.sleep(2)
        print("\nPOSITION MODE")

        KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderOutputMode", "Position")

        # RXFrameCounter and DropFrameCounter are readable counters but may be read-only.
        # Do not reset them by writing 0. Instead, measure before/after deltas.
        rx_before = KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")[1]
        drop_before = KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")[1]

        aux_callback_struct.callbackCount = 0
        stream_struct.callbackCount = 0

        if KYFG_IsGrabberValueImplemented(grabberHandle, "TriggerSentCount"):
            KYFG_SetGrabberValueInt(grabberHandle, "TriggerSentCount", 0)

        fourStepsForward(grabberHandle)
        time.sleep(5)
        fourStepsBackward(grabberHandle)
        time.sleep(5)

        rx_after = KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")[1]
        drop_after = KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")[1]

        rx_delta = rx_after - rx_before
        drop_delta = drop_after - drop_before

        trigger_sent = None
        if KYFG_IsGrabberValueImplemented(grabberHandle, "TriggerSentCount"):
            trigger_sent = KYFG_GetGrabberValueInt(grabberHandle, "TriggerSentCount")[1]
            print("Trigger sent:", trigger_sent)

        print("RXFrameCounter before:", rx_before)
        print("RXFrameCounter after:", rx_after)
        print("RXFrameCounter delta:", rx_delta)
        print("DropFrameCounter before:", drop_before)
        print("DropFrameCounter after:", drop_after)
        print("DropFrameCounter delta:", drop_delta)
        print("AUX callback count:", aux_callback_struct.callbackCount)
        print("Stream callback count:", stream_struct.callbackCount)
        # print("\nSTEPFORWARD MODE")
        # KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderOutputMode", "Stepforward")
        # fourStepsForward(grabberHandle)
        # fourStepsBackward(grabberHandle)
        # print("trigger sent:", KYFG_GetGrabberValueInt(grabberHandle, "TriggerSentCount")[1])
        # print('FrameCount', KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")[1])
        # print('DropFrameCount', KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")[1])
        # print("AUX callback count:", aux_callback_struct.callbackCount, "stream callback count:",
        #       stream_struct.callbackCount)
        # print("\nSTEPBACKWARD MODE")
        # KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderOutputMode", "Stepbackward")
        # fourStepsForward(grabberHandle)
        # fourStepsBackward(grabberHandle)
        # print("trigger sent:", KYFG_GetGrabberValueInt(grabberHandle, "TriggerSentCount")[1])
        # print('FrameCount', KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")[1])
        # print('DropFrameCount', KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")[1])
        # print("AUX callback count:", aux_callback_struct.callbackCount, "stream callback count:",
        #       stream_struct.callbackCount)
        # print("\nANYSTEP MODE")
        # KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "EncoderOutputMode", "Anystep")
        # fourStepsForward(grabberHandle)
        # fourStepsBackward(grabberHandle)
        # print("trigger sent:", KYFG_GetGrabberValueInt(grabberHandle, "TriggerSentCount")[1])
        # print('FrameCount', KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")[1])
        # print('DropFrameCount', KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")[1])
        # print("AUX callback count:", aux_callback_struct.callbackCount, "stream callback count:",
        #       stream_struct.callbackCount)
        (status,) = KYFG_CameraStop(cameraHandle)

#############################################################################################
        if KYFG_IsGrabberValueImplemented(grabberHandle, "TriggerSentCount"):
            expected_frame_count = trigger_sent // 2

            if rx_delta != expected_frame_count:
                print(f"ERROR: RXFrameCounter delta expected {expected_frame_count}, actual {rx_delta}")
                error_count += 1

            if stream_struct.callbackCount != expected_frame_count:
                print(
                    f"ERROR: Stream callback count expected {expected_frame_count}, "
                    f"actual {stream_struct.callbackCount}"
                )
                error_count += 1

            if drop_delta != 0:
                print(f"ERROR: DropFrameCounter delta expected 0, actual {drop_delta}")
                error_count += 1

            # KYFG_SetGrabberValueInt(grabberHandle, "CxpConnectionSelector", cameraIndex)
            KYFG_SetGrabberValueInt(grabberHandle, "TriggerSentCount", 0)

            if "Chameleon" in cameraInfo.deviceModelName:
                chameleonSetting(cameraHandle, 0)
            else:
                cameraSetting(cameraHandle, 0)
            (status,) = KYFG_StreamBufferCallbackUnregister(streamHandle, streamCallbackFunc)
            (status,) = KYFG_StreamDelete(streamHandle)
            (status,) = KYFG_CameraClose(cameraHandle)
            camIndex += 1

        else:
            expected_frame_count = 2

            if rx_delta != expected_frame_count:
                print(f"ERROR: RXFrameCounter delta expected {expected_frame_count}, actual {rx_delta}")
                error_count += 1

            if stream_struct.callbackCount != expected_frame_count:
                print(
                    f"ERROR: Stream callback count expected {expected_frame_count}, "
                    f"actual {stream_struct.callbackCount}"
                )
                error_count += 1

            if drop_delta != 0:
                print(f"ERROR: DropFrameCounter delta expected 0, actual {drop_delta}")
                error_count += 1

        KYFG_GrabberExecuteCommand(grabberHandle, "EncoderReset")
        # KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)

    if KYFG_GetGrabberValueInt(grabberHandle, "EncoderValue")[1] != 0 or \
            KYFG_GetGrabberValueInt(grabberHandle, "EncoderValueAtReset")[1] != 6:
        print('Reset function is not working correctly')
        error_count += 1

    # AUX callbacks are asynchronous grabber-side events and may include extra events
    # from setup or UserOutput transitions. They are printed for debug only and are not
    # used as pass/fail criteria.

    print("EncoderValue:", KYFG_GetGrabberValueInt(grabberHandle, "EncoderValue")[1])
    print("EncoderValueAtReset:", KYFG_GetGrabberValueInt(grabberHandle, "EncoderValueAtReset")[1])
    # KYFG_SetGrabberValueEnum(grabberHandle, "CameraTriggerMode", 0)
    (status,) = KYFG_Close(grabberHandle)

    assert error_count == 0, 'Test not passed'
    if not HWFound:
        return CaseReturnCode.NO_HW_FOUND
    print(f'\nExiting from CaseRun({args}) with code SUCCESS...')
    return CaseReturnCode.SUCCESS


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 3423 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

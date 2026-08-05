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


class Stream_struct:
    def __init__(self):
        self.frames_acquired = 0
        self.frames_dropped = 0
        return


def streamCallbackFunction(buffHandle, userContext):
    if buffHandle == NULL_STREAM_BUFFER_HANDLE or buffHandle == INVALID_STREAM_BUFFER_HANDLE:
        return
    try:
        (status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except:
        return

    userContext.frames_acquired += 1


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

    if device_info.m_Protocol != KY_DEVICE_PROTOCOL.KY_DEVICE_PROTOCOL_CoaXPress:
        print("\n-----------------------------------------------------------")
        print('Test COULD NOT RUN on this grabber, supported only CXP grabbers')
        print("-----------------------------------------------------------\n")
        return CaseReturnCode.COULD_NOT_RUN

    (grabberHandle,) = KYFG_Open(device_index)
    (status, first_cameraList) = KYFG_UpdateCameraList(grabberHandle)
    print(f"First detection cameras found: {len(first_cameraList)}")

    if len(first_cameraList) < 2:
        print("-----------------------------------------------------------")
        print(f"There is not enough cameras for this test, needed at least 2 cameras, detected: {len(first_cameraList)}")
        print("-----------------------------------------------------------\n")
        return CaseReturnCode.NO_HW_FOUND

    print("-----------------------------------------------------------")
    print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
    print("-----------------------------------------------------------\n")

    waitingTime = 60
    assertionFailed = False

    (status, camInfo) = KYFG_CameraInfo2(first_cameraList[0])

    print("Disable PoCXP for all grabber links")

    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CxpPoCxpHostConnectionSelector", "All")
    (status,) = KYFG_GrabberExecuteCommand(grabberHandle, "CxpPoCxpTurnOff")

    time.sleep(10)

    # Enable PoCXP for links of first camera in list
    link_mask = camInfo.link_mask

    for bit_index in range(32):  # assuming max 32 links
        if (link_mask >> bit_index) & 1:  # check if bit is set
            try:
                (status,) = KYFG_SetGrabberValueEnum(grabberHandle, "CxpPoCxpHostConnectionSelector", bit_index)
                (status,) = KYFG_GrabberExecuteCommand(grabberHandle, "CxpPoCxpAuto")
            except:
                assertionFailed = True
                print(f"Failed to activate PoCxp for link: {bit_index}")

        if assertionFailed:
            print(f"Enabled PoCXP for link: {bit_index}")
            break

    if not assertionFailed:
        print(f"Waiting ({waitingTime} sec) activation PoCXP on requested links...")
        time.sleep(waitingTime)

        #Second detection: only the first camera should remain visible
        (status, second_cameraList) = KYFG_UpdateCameraList(grabberHandle)
        print(f"Second detection cameras found: {len(second_cameraList)}")

        assert len(second_cameraList) == 1, \
            (f"Assertion failed: second camera list size: {len(second_cameraList)} does not match expected "
             f"conditions, the size should be equal to 1")

        for cameraIndex, cameraHandle in enumerate(second_cameraList):

            (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
            (status,) = KYFG_CameraOpen2(cameraHandle, None)

            #################################################
            Reset_camera(cameraHandle, grabberHandle)
            #################################################

            print("-----------------------------------------------------------")
            print(f"Selected camera: [{cameraIndex}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
            print("-----------------------------------------------------------\n")

            (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)

            try:
                if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
                    KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)
                if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
                    KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)
                if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
                    KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
            except:
                pass

            stream_struct = Stream_struct()

            (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
            (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, streamCallbackFunction, stream_struct)

            # Retrieve information about required frame buffer size and alignment
            (status, payload_size, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
            (status, buf_allignment, _, _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)

            # Allocate memory for desired number of frame buffers
            streamBufferHandle = [0 for _ in range(16)]
            streamAllignedBuffer = [0 for _ in range(16)]
            for iFrame in range(len(streamBufferHandle)):
                streamAllignedBuffer[iFrame] = aligned_array(buf_allignment, c_ubyte, payload_size)
                (status, streamBufferHandle[iFrame]) = KYFG_BufferAnnounce(
                    streamHandle,
                    streamAllignedBuffer[iFrame],
                    None
                )

            (status,) = KYFG_BufferQueueAll(
                streamHandle,
                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT
            )
            (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
            time.sleep(5)
            (status,) = KYFG_CameraStop(cameraHandle)

            (status, drop_frame_counter) = KYFG_GetGrabberValueInt(grabberHandle, "DropFrameCounter")
            stream_struct.frames_dropped = drop_frame_counter

            (status,) = KYFG_StreamBufferCallbackUnregister(streamHandle, streamCallbackFunction)
            (status,) = KYFG_StreamDelete(streamHandle)
            (status,) = KYFG_CameraClose(cameraHandle)

            try:
                assert stream_struct.frames_acquired > 0, \
                    'No callbacks were received - this may indicate a streaming issue or that no data was received'
                assert stream_struct.frames_dropped == 0, \
                    f"Detected dropped frames during stream runs: {stream_struct.frames_dropped}"
            except AssertionError as e:
                print(f"Assertion failed: {e}")
                assertionFailed = True

            if assertionFailed:
                break

        # Restore PoCXP on all links before the third detection
        KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CxpPoCxpHostConnectionSelector", "All")
        (status,) = KYFG_GrabberExecuteCommand(grabberHandle, "CxpPoCxpAuto")
        print(f"Waiting ({waitingTime} sec) activation PoCXP on all links...")
        time.sleep(waitingTime)

        # Third detection: all initially detected cameras should be visible again
        (status, third_cameraList) = KYFG_UpdateCameraList(grabberHandle)
        print(f"Third detection cameras found: {len(third_cameraList)}")

        assert len(third_cameraList) == len(first_cameraList), \
            (f"Assertion failed: camera list sizes do not match expected conditions, "
             f"first list size: {len(first_cameraList)}, third list size: {len(third_cameraList)}, "
             f"the sizes should be equal")

    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CxpPoCxpHostConnectionSelector", "All")
    KYFG_GrabberExecuteCommand(grabberHandle, 'CxpPoCxpAuto')
    time.sleep(10)

    (status,) = KYFG_Close(grabberHandle)

    if assertionFailed:
        assert 0

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

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
import asyncio
from datetime import datetime
import time
import threading
import subprocess
import queue
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
    parser.add_argument('--number_of_sent_triggers', type=int, default=10, help='Number of sent triggers')
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


error_count = 0

result_queue = queue.Queue()


def run_async_function(device_index, camera_index, number_of_sent_triggers):
    result = asyncio.run(new_instance(device_index, camera_index, number_of_sent_triggers))
    result_queue.put(result)


async def new_instance(device_index, camera_index, number_of_sent_triggers):
    instance_process = subprocess.Popen(
        ["python", f"{__file__}", '--unattended', '--deviceIndex', f'{device_index}', "--cameraIndex",
         f"{camera_index}",
         "--number_of_sent_triggers", f"{number_of_sent_triggers}"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        close_fds=True)
    stdout_output, stderr_output = instance_process.communicate()
    print(stdout_output.decode())
    print(stderr_output)
    return_code = instance_process.returncode
    print(return_code)

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
    (status, device_info) = KY_DeviceInfo(device_index)
    if device_info.m_Protocol != KY_DEVICE_PROTOCOL.KY_DEVICE_PROTOCOL_CoaXPress:
        print('Test could not run on this grabber')
        return CaseReturnCode.COULD_NOT_RUN
    camera_index = args['cameraIndex']
    number_of_sent_triggers = args['number_of_sent_triggers']

    if camera_index == 0:
        global error_count
    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    time.sleep(7)
    (status, camera_list,) = KYFG_UpdateCameraList(grabberHandle)
    camIndex = 0

    if len(camera_list) == 0:
        return CaseReturnCode.NO_HW_FOUND
    if camera_index == 0:
        threads = []
        for i in range(1, len(camera_list)):
            thread = threading.Thread(target=run_async_function,
                                      args=(device_index, camera_index + i, number_of_sent_triggers))
            threads.append(thread)
            thread.start()

    # Detect camera
    cameraHandle = camera_list[camera_index]
    if camera_index == 0:
        (status,) = KYFG_CameraOpen2(camera_list[0], None)
    else:
        (status,) = KYFG_CameraOpen2(cameraHandle, None)

    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)

    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camIndex)
    ################################################
    Reset_camera(cameraHandle, grabberHandle)
    ################################################

    camera_master_link = camInfo.master_link
    # Select the "CxpConnectionSelector" where camera is connected (i.e. master link of the camera)
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CxpConnectionSelector", camera_master_link)
    # Clear the statistics of the following parameters by writing 0 to them:
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "TriggerMissedCount", 0)
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "TriggerSentCount", 0)
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "TriggerAcknowledgeCount", 0)
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "TriggerChangeCount", 0)
    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", camera_master_link)

    # Set timer for triggering
    (status,) = KYFG_SetGrabberValueEnum(grabberHandle, "TimerSelector", camera_master_link)
    (status,) = KYFG_SetGrabberValueFloat(grabberHandle, "TimerDelay", 10.0)
    (status,) = KYFG_SetGrabberValueFloat(grabberHandle, "TimerDuration", 10.0)
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerActivation", "RisingEdge")
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TimerTriggerSource", "KY_SOFTWARE")

    # Set camera trigger
    try:
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerMode", 'On')
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerActivation", "RisingEdge")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TriggerMode", "On")
        (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TriggerActivation", "RisingEdge")
        (status,) = KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 1)
        (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TriggerActivation", "AnyEdge")
    except:
        print('Trigger Mode is not supported on this camera')
        return CaseReturnCode.NO_HW_FOUND
    print(f'KY_TIMER_ACTIVE_{camera_master_link}')
    (status,) = KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "CameraTriggerSource",
                                                     f'KY_TIMER_ACTIVE_{camera_master_link}')

    for i in range(number_of_sent_triggers):
        print('Stop Loop', datetime.now().time())
        print(f'Sent trigger number {i + 1}')
        (status,) = KYFG_GrabberExecuteCommand(grabberHandle, "TimerTriggerSoftware")
        time.sleep(2)
    (status, sent_count) = KYFG_GetGrabberValueInt(grabberHandle, "TriggerSentCount")
    (status, change_count) = KYFG_GetGrabberValueInt(grabberHandle, "TriggerChangeCount")
    print("TriggerSentCount:", sent_count, "TriggerChangeCount:", change_count)

    (status,) = KYFG_CameraClose(cameraHandle)
    camIndex += 1
    (status,) = KYFG_Close(grabberHandle)

    print("number of sent triggers:", number_of_sent_triggers)

    if camera_index == 0:
        for thread in threads:
            thread.join()
        results = []
        while not result_queue.empty():
            result = result_queue.get()
            results.append(result)
        print('results =', results)

    assert sent_count == number_of_sent_triggers, f'TriggerSentCount: {sent_count} is not match to ' \
                                                  f'number_of_sent_triggers: {number_of_sent_triggers}'
    assert change_count == number_of_sent_triggers, f'TriggerChangeCount {change_count} is not match to ' \
                                                    f'number_of_sent_triggers {number_of_sent_triggers}'
    if camera_index == 0:
        for res in results:
            assert res == 0, 'Errors while test'
    print(f'\nExiting from CaseRun({args}) with code 0...')
    return CaseReturnCode.SUCCESS


# The flow starts here
if __name__ == "__main__":
    print("case 3049 Process ID:", os.getpid())
    try:
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

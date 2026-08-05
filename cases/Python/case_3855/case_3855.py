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
import zipfile
import io
import xml.etree.ElementTree as ET
import pathlib
import time
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
    parser.add_argument('--camera', type=str, default='Iron250M', help='Camera model name for test')
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


def extract_from_zip(buffer:list):
    # bytes_buffer = [bytes([i]) for i in buffer]
    bytes_buffer = bytes(buffer)
    buffer_io = io.BytesIO(bytes_buffer)
    extracted_files = {}
    with zipfile.ZipFile(buffer_io, 'r') as zip_ref:
        file_names = zip_ref.namelist()
        for file_name in file_names:
            with zip_ref.open(file_name) as file:
                extracted_files[file_name] = file.read().decode()
    return extracted_files


def remove_elements(element, tags_to_delete, text_values_to_delete):
    for child in list(element):
        remove_elements(child, tags_to_delete, text_values_to_delete)
        if child.text and child.text.strip() in text_values_to_delete:
            element.remove(child)
        elif "Name" in child.attrib.keys():
            if child.attrib["Name"] in text_values_to_delete:
                element.remove(child)


def change_xml_file(xml_data):
    root = ET.fromstring(xml_data)

    commands_to_delete = ["AcquisitionStart"]
    tags_to_delete = ["Command", "pFeature", "pValue", "IntReg"]
    remove_elements(root, tags_to_delete, commands_to_delete)

    new_xml_data = ET.tostring(root).decode()
    return new_xml_data


def save_xml_file(xml_path:str, data):
    with open(xml_path, 'w') as xml:
        xml.write(data)


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

    (grabberHandle,) = KYFG_Open(device_index)
    ############################
    Reset_grabber(grabberHandle)
    ############################

    print("-----------------------------------------------------------")
    print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
    print("-----------------------------------------------------------\n")

    (status, cameraList) = KYFG_UpdateCameraList(grabberHandle)

    if len(cameraList) == 0:
        print("-----------------------------------------------------------")
        print('There is no cameras on this grabber')
        print("-----------------------------------------------------------\n")
        return CaseReturnCode.NO_HW_FOUND

    camera_model = args['camera']

    cameraHandle = None
    cameraIndex = 0
    for cam in cameraList:
        (status, camInfo) = KYFG_CameraInfo2(cam)
        if str(camera_model).lower() in str(camInfo.deviceModelName).lower():
            cameraHandle = cam
            cameraIndex = cameraList.index(cam)
            break

    if cameraHandle is None:
        print(f"Camera {camera_model} not found")
        return CaseReturnCode.NO_HW_FOUND
    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
    (status,) = KYFG_CameraOpen2(cameraHandle, None)

    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)
    #########################################
    Reset_camera(cameraHandle, grabberHandle)
    #########################################

    print("-----------------------------------------------------------")
    print(f"Selected camera: [{cameraIndex}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
    print("-----------------------------------------------------------\n")

    print(camInfo.deviceModelName, 'is opened for test')
    (status, is_zipped_file, buffer) = KYFG_CameraGetXML(cameraHandle)
    (status,) = KYFG_CameraClose(cameraHandle)

    if is_zipped_file:
        files_dict = extract_from_zip(buffer)
    else:
        files_dict = {f"{camInfo.deviceModelName}.xml": "".join(buffer)}
    file_path = None
    for file_name, file_data in files_dict.items():
        new_file_data = change_xml_file(file_data)
        file_path = pathlib.Path(__file__).parent.joinpath(file_name).absolute().as_posix()
        save_xml_file(file_path, new_file_data)

    (status,) = KYFG_CameraOpen2(cameraHandle, file_path)

    (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)

    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TransferControlMode", "UserControlled")

    (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)

    (status, payloadSize, _ , _) = KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)
    print(f"Stream payload size: {payloadSize} (bytes)")

    buffers = [0 for i in range(5)]
    for IFrame in buffers:
        (status, buffers[buffers.index(IFrame)]) = KYFG_BufferAllocAndAnnounce(streamHandle, payloadSize, 0)
        print(f"Allocated buffer: {IFrame}, payload size: {payloadSize} (bytes), BUFFERHANDLE: {str(buffers[IFrame])}")

    is_test_passed = False

    transferMode = KYFG_GetGrabberValueEnum(grabberHandle, "TransferControlMode")
    print(f"transferMode:{transferMode}")

    (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)
    try:
        print(f"\nAttempted to execute command 'AcquisitionStart'. Expected result: exception raised, as the parameter does not exist in the camera XML")
        KYFG_CameraExecuteCommand(cameraHandle, "AcquisitionStart")
    except KYException as e:
        print("\n Exception:\n", type(e), str(e))
        print("\n-----------------------------------------------------------")
        print("Successfully caught the expected exception - test passed")
        print("-----------------------------------------------------------\n")
        is_test_passed = True

    (status,) = KYFG_CameraStop(cameraHandle)
    (status,) = KYFG_StreamDelete(streamHandle)
    (status,) = KYFG_CameraClose(cameraHandle)
    cameraIndex += 1

    KYFG_SetGrabberValueEnum_ByValueName(grabberHandle, "TransferControlMode", "Automatic")

    (status,) = KYFG_Close(grabberHandle)

    assert is_test_passed, "Test failed"

    pathlib.Path(file_path).unlink()

    print(f'\nExiting from CaseRun({args}) with code SUCCESS...')
    return CaseReturnCode.SUCCESS


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 3855 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

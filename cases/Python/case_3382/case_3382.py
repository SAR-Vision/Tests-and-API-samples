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
import pathlib
import platform
import subprocess
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
# Definition
##########
build_Report = {}


##########
# Functions
##########
def findMSBuild():
    # Find VsDevCmd.bat file
    commands = ["cd/", "dir VsDevCmd.bat /s", ]
    find_cmd_line = subprocess.run('&&'.join(commands), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True,
                                   text=True)
    stdout_output = find_cmd_line.stdout.strip().split('\n')
    stderr_output = find_cmd_line.stderr.strip()
    stdout_output = stdout_output
    paths_to_cmd = []
    path_to_VsDevCmd = str
    for i in stdout_output:
        if 'Directory of ' in i:
            new_path = i.replace('Directory of ', '')
            paths_to_cmd.append(new_path.strip())
    for i in paths_to_cmd:
        if 'Microsoft Visual Studio' in i and '2017' in i:
            path_to_VsDevCmd = i + r'\VsDevCmd.bat'
    # Find MSBuild
    command_for_find_MSBuild = f'"{path_to_VsDevCmd}" && where MSBuild'
    start_searching_toMSBuild = subprocess.run(command_for_find_MSBuild, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                               shell=True, text=True)
    searching_output = start_searching_toMSBuild.stdout.strip().split('\n')
    path_to_MSBuild = None
    for i in searching_output:
        if 'Microsoft Visual Studio' in i and '2017' in i:
            path_to_MSBuild = i
    print('Path to MSBuild: ', path_to_MSBuild)
    return path_to_MSBuild


def build_for_windows(msbuild_file, file_path, platform):
    global build_Report
    print()
    print('*' * 30, file_path.name, '*' * 30)
    print()
    print('*' * 30, 'DEBUG BUILDING', '*' * 30)
    print()
    DebugLogfile = fr"{pathlib.Path(file_path).parent}\DebugBuildLogFile.log"
    ReleaseLogfile = fr"{pathlib.Path(file_path).parent}\ReleaseBuildLogFile.log"
    command = f'"{msbuild_file}" "{file_path}" /p:configuration=Debug /p:platform={platform} /flp:logfile="{DebugLogfile}"'
    print(command)
    p = subprocess.Popen(
        command,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        close_fds=True
    )
    output, errors = p.communicate()
    outputLines = output.decode().strip().split('\n')
    print(output.decode())
    print('*' * 30, 'Release BUILDING', '*' * 30)
    logfile = fr"{pathlib.Path(file_path).parent}\BuildLogFile.log"
    command = f'"{msbuild_file}" "{file_path}" /p:configuration=Release /p:platform={platform} /flp:logfile="{ReleaseLogfile}"'
    print(command)
    p = subprocess.Popen(
        command,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        close_fds=True
    )
    output, errors = p.communicate()
    releaseOutputLines = output.decode().strip().split('\n')
    print(output.decode())
    success_build = True
    for nextLine in outputLines:
        if 'Error(s)' in nextLine:
            if ' 0 Error(s)' not in nextLine:
                print('Errors and warnings that occurred during the build DEBUG process\nLook at the output')
                success_build = False
    for nextLine in releaseOutputLines:
        if 'Error(s)' in nextLine:
            if ' 0 Error(s)' not in nextLine:
                print('Errors and warnings that occurred during the build RELEASE process\nLook at the output')
                success_build = False
    build_Report[file_path.name] = success_build
    return success_build


def build_for_linux(file_path):
    global build_Report
    print()
    print('*' * 30, file_path.name, '*' * 30)
    print()
    print('*' * 30, 'LINUX BUILDING', '*' * 30)
    print()
    cmd = 'make'
    p = subprocess.run(
        cmd,
        cwd=file_path.parent,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        stdin=subprocess.PIPE,
        close_fds=True
    )
    print(p.stdout, p.stderr, sep='\n')
    success_build = False
    if file_path.parent.joinpath(file_path.parent.name).exists():
        success_build = True
    else:
        print(f"{file_path.parent.joinpath(file_path.parent.name)} Doesn't exist")
    build_Report[file_path.name] = success_build
    return success_build


def iter_folder(folder: pathlib.Path, msbuild, vs_platform):
    system_platform = platform.system().lower()
    global build_Report
    for file in pathlib.Path.iterdir(folder):
        if pathlib.Path(file).is_dir():
            iter_folder(file, msbuild, vs_platform)
        else:
            if (file.name.endswith('.vcxproj') or file.name.endswith('.csproj')) and system_platform == 'windows':
                try:
                    build_for_windows(msbuild, file, vs_platform)
                except:
                    build_Report[file.name] = False
            elif file.name.startswith('Makefile') and system_platform == 'linux':
                try:
                    build_for_linux(file)
                except:
                    build_Report[file.name] = False


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
    global build_Report
    parent_path = pathlib.Path.cwd().joinpath('_Tests')
    # parent_path = pathlib.Path.cwd().parent.parent.joinpath('_Tests')  # for current folder running
    system_platform = platform.system().lower()
    if system_platform == 'windows':
        msbuild = findMSBuild()
    else:
        msbuild = None
    vs_platform = "x64" if is_python_64bit() else "win32"

    for file in pathlib.Path.iterdir(parent_path):
        if file.is_dir():
            iter_folder(file, msbuild, vs_platform)
    max_name_length = max(len(name) for name in build_Report.keys())  # Find max len for alignment

    for name, val in build_Report.items():
        formatted_name = name.ljust(max_name_length)
        print(f"{formatted_name} {val}")
    for val in build_Report.values():
        assert val is True, 'Not all of files built successful, check report'

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

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
import subprocess
import platform
import shutil
import json
import pathlib
import time


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
    parser.add_argument('--DevicePID', type=str, default='', help='use the DevicePID detected from the selected grabber')
    parser.add_argument('--emb_json_incorrect', type=str, default='', help='Optional incorrect JSON filename override')
    parser.add_argument('--emb_json_correct', type=str, default='', help='Optional correct JSON filename override')
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


def Reset_camera(cameraHandle, grabberHandle):  # Camera initialization for this specific test
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

camHandleArray = {}


def get_log_offsets(path):
    offsets = {}

    for file_name in os.listdir(path):
        full_path = os.path.join(path, file_name)

        if (
            os.path.isfile(full_path)
            and file_name.startswith("KAYA_")
            and file_name.endswith(".log")
        ):
            try:
                offsets[full_path] = os.path.getsize(full_path)
            except OSError:
                continue

    return offsets


def check_log(path, string_to_check, since_offsets=None):
    since_offsets = since_offsets or {}

    for file_name in os.listdir(path):
        full_path = os.path.join(path, file_name)

        if (
            os.path.isfile(full_path)
                and file_name.lower().endswith(".log")
        ):
            start_offset = since_offsets.get(full_path, 0)

            try:
                with open(
                    full_path,
                    "r",
                    encoding="utf-8",
                    errors="ignore"
                ) as log:
                    log.seek(start_offset)
                    data = log.read()

            except OSError as ex:
                print(f"[WARN] Could not read log file {full_path}: {ex}")
                continue

            if data:
                for line in data.splitlines():
                    if "json" in line.lower() or "invalid" in line.lower():
                        print(f"[LOG] {file_name}: {line}")

            if string_to_check.lower() in data.lower():
                print(f"[INFO] Found '{string_to_check}' in: {file_name}")
                return True

    return False


def normalize_device_pid(value):
    if value is None:
        return None

    if isinstance(value, int):
        return value

    value = str(value).strip()

    if not value:
        return None

    return int(value, 0)


def find_device_pid_folder(test_dir, device_pid):
    print(f"[INFO] Searching DevicePID folder in: {test_dir}")

    for item in sorted(test_dir.iterdir(), key=lambda p: p.name.lower()):
        if not item.is_dir():
            continue

        if not item.name.lower().startswith("0x"):
            continue

        try:
            folder_pid = int(item.name, 16)
        except ValueError:
            continue

        print(f"[INFO] Found DevicePID folder: {item.name}")

        if folder_pid == device_pid:
            return item

    return None


def get_json_files(pid_folder, args):
    pid_name = pid_folder.name

    incorrect_name = args.get("emb_json_incorrect")
    correct_name = args.get("emb_json_correct")

    if not incorrect_name:
        incorrect_name = f"incorrect_KYHWLib_{pid_name}_emb.json"

    if not correct_name:
        correct_name = f"KYHWLib_{pid_name}_emb.json"

    incorrect_json = pid_folder / incorrect_name
    correct_json = pid_folder / correct_name

    if not incorrect_json.is_file():
        raise FileNotFoundError(f"Incorrect JSON file not found: {incorrect_json}")

    if not correct_json.is_file():
        raise FileNotFoundError(f"Correct JSON file not found: {correct_json}")

    return incorrect_json, correct_json


def get_kaya_conf_folder():
    conf_path = os.environ.get("KAYA_VISION_POINT_CONF")

    if not conf_path:
        raise EnvironmentError("KAYA_VISION_POINT_CONF is not defined")

    conf_folder = pathlib.Path(conf_path)

    if not conf_folder.is_dir():
        raise FileNotFoundError(
            f"KAYA Instruments Conf folder not found: {conf_folder}"
        )

    return conf_folder


def get_vp_log_folder():
    system_platform = platform.system().lower()

    if system_platform == "windows":
        log_folder = pathlib.Path(r"C:\ProgramData\KAYA Instruments\Logs")
    elif system_platform == "linux":
        log_folder = pathlib.Path("/var/log/KAYA_Instruments")
    else:
        raise RuntimeError(f"Unsupported platform: {platform.system()}")

    if not log_folder.is_dir():
        raise FileNotFoundError(f"Vision Point log folder not found: {log_folder}")

    return log_folder


def scan_open_close(device_index, allow_open_failure=False):
    """
    Perform DeviceScan and open/close the selected grabber.

    With an incorrect JSON, KYFG_Open may fail.  That is acceptable for
    phase 1, but HW_INVALID_JSON_FILE must still be present in the log.
    """

    (status, device_count) = KY_DeviceScan()

    print(f"[INFO] Device scan found {device_count} device(s)")

    if device_index < 0 or device_index >= device_count:
        raise RuntimeError(
            f"Device index {device_index} is not available after DeviceScan")

    grabber_handle = None

    try:
        print(f"[INFO] Opening frame grabber [{device_index}]...")
        (grabber_handle,) = KYFG_Open(device_index)

        print(f"[INFO] Frame grabber opened. Handle: {grabber_handle}")

    except Exception as ex:
        print(f"[INFO] KYFG_Open raised: {ex}")

        if not allow_open_failure:
            raise

    finally:
        if grabber_handle:
            try:
                KYFG_Close(grabber_handle)
                print("[INFO] Frame grabber closed")
            except Exception as ex:
                print(f"[WARN] KYFG_Close failed: {ex}")


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
    selected_device_info = device_infos[device_index]
    detected_device_pid = int(selected_device_info.DevicePID)
    detected_pid_hex = f"0x{detected_device_pid:x}"

    print(
        f"[INFO] Selected grabber DevicePID: "
        f"{detected_device_pid} ({detected_pid_hex})"
    )

    # Optional --DevicePID parameter is only a validation.
    # The JSON folder is always selected according to the ACTUAL detected grabber.
    requested_device_pid = normalize_device_pid(
        args.get("DevicePID")
    )

    if (
        requested_device_pid is not None
        and requested_device_pid != detected_device_pid
    ):
        print(
            f"[ERROR] Requested DevicePID "
            f"{args['DevicePID']} does not match detected "
            f"DevicePID {detected_pid_hex}"
        )

        return CaseReturnCode.WRONG_PARAM_VALUE


    # ------------------------------------------------------------
    # Locate DevicePID folder
    # ------------------------------------------------------------

    test_dir = pathlib.Path(__file__).resolve().parent

    pid_folder = find_device_pid_folder(
        test_dir,
        detected_device_pid
    )

    if pid_folder is None:
        print(
            f"[ERROR] Folder for DevicePID "
            f"{detected_pid_hex} was not found in:"
        )

        print(test_dir)

        return CaseReturnCode.NO_REQUIRED_PARAM


    print(
        f"[INFO] DevicePID folder: {pid_folder}"
    )

    # ------------------------------------------------------------
    # Locate JSON files
    # ------------------------------------------------------------

    try:

        incorrect_json_src, correct_json_src = \
            get_json_files(pid_folder, args)

        json_conf_folder = get_kaya_conf_folder()

        log_folder = get_vp_log_folder()

    except Exception as ex:

        print(
            f"[ERROR] {ex}"
        )

        return CaseReturnCode.NO_REQUIRED_PARAM


    # Both files must be copied to the SAME installed filename.
    #
    # Example:
    #
    # source:
    #   0x610\incorrect_KYHWLib_0x610_emb.json
    #
    # destination:
    #   Common\bin\KYHWLib_0x610_emb.json
    #
    # Correct file:
    #   0x610\KYHWLib_0x610_emb.json
    #
    # destination:
    #   Common\bin\KYHWLib_0x610_emb.json

    target_json = (
        json_conf_folder /
        correct_json_src.name
    )


    print(
        f"[INFO] Incorrect JSON: {incorrect_json_src}"
    )

    print(
        f"[INFO] Correct JSON:   {correct_json_src}"
    )

    print(
        f"[INFO] Target JSON:    {target_json}"
    )

    print(
        f"[INFO] Log directory:  {log_folder}"
    )

    LOG_POLL_INTERVAL_SEC = 0.5
    LOG_POLL_TIMEOUT_SEC = 30


    def wait_for_log(
        string_to_check,
        since_offsets,
        timeout=LOG_POLL_TIMEOUT_SEC
    ):

        deadline = time.time() + timeout

        while True:

            if check_log(
                str(log_folder),
                string_to_check,
                since_offsets
            ):
                return True

            if time.time() >= deadline:
                return False

            time.sleep(
                LOG_POLL_INTERVAL_SEC
            )


    try:

        # ============================================================
        # PHASE 1
        # INCORRECT JSON
        # HW_INVALID_JSON_FILE MUST appear
        # ============================================================

        print(
            "\n========== PHASE 1: INCORRECT JSON =========="
        )

        print(
            f"[INFO] Copying:\n"
            f"       {incorrect_json_src}\n"
            f"    -> {target_json}"
        )

        # Take the log snapshot BEFORE changing the JSON.
        offsets_before_incorrect = get_log_offsets(
            str(log_folder)
        )

        shutil.copy2(
            incorrect_json_src,
            target_json
        )

        # Trigger JSON loading.

        scan_open_close(
            device_index,
            allow_open_failure=True
        )

        invalid_json_error_found = wait_for_log(
            "HW_INVALID_JSON_FILE",
            offsets_before_incorrect
        )

        assert invalid_json_error_found, (
            "HW_INVALID_JSON_FILE was not found in "
            "Vision Point logs with incorrect JSON"
        )

        print(
            "[PASS] HW_INVALID_JSON_FILE was found "
            "with incorrect JSON"
        )

        # ============================================================
        # PHASE 2
        # CORRECT JSON
        # HW_INVALID_JSON_FILE MUST NOT appear
        # ============================================================

        print(
            "\n========== PHASE 2: CORRECT JSON =========="
        )

        print(
            f"[INFO] Copying:\n"
            f"       {correct_json_src}\n"
            f"    -> {target_json}"
        )

        shutil.copy2(
            correct_json_src,
            target_json
        )


        offsets_before_correct = get_log_offsets(
            str(log_folder)
        )
        shutil.copy2(
            correct_json_src,
            target_json
        )

        # With valid JSON, opening the grabber itself must succeed.

        scan_open_close(
            device_index,
            allow_open_failure=False
        )

        invalid_json_error_found = wait_for_log(
            "HW_INVALID_JSON_FILE",
            offsets_before_correct
        )

        assert not invalid_json_error_found, (
            "HW_INVALID_JSON_FILE was found in "
            "Vision Point logs with correct JSON"
        )

        print(
            "[PASS] HW_INVALID_JSON_FILE was NOT found "
            "with correct JSON"
        )

    finally:

        # Always leave the machine with the valid JSON installed,
        # even if Phase 1 or Phase 2 fails.

        try:

            print(
                f"[INFO] Restoring correct JSON:\n"
                f"       {correct_json_src}\n"
                f"    -> {target_json}"
            )

            shutil.copy2(
                correct_json_src,
                target_json
            )

        except Exception as ex:

            print(
                f"[WARN] Could not restore correct JSON: {ex}"
            )

    print(
        f'\nExiting from CaseRun({args}) with code SUCCESS...'
    )

    return CaseReturnCode.SUCCESS


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 2990 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)
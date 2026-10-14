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
    parser.add_argument('--number_of_sent_tests', type=int, default=3, help='Number of sent tests')
    parser.add_argument('--PixelFormat', type=str, default='',
                        help='Camera Pixel Format. If omitted, use the PixelFormat from Default User Set')
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
    return None


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


def callbackFunc(buffHandle, userContext):
    if buffHandle == NULL_STREAM_BUFFER_HANDLE or buffHandle == INVALID_STREAM_BUFFER_HANDLE:
        return

    try:
        (status, base, _, _) = KYFG_BufferGetInfo(
            buffHandle,
            KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_BASE
        )
        (status, size, _, _) = KYFG_BufferGetInfo(
            buffHandle,
            KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_SIZE
        )

        expected_pixels = userContext.width * userContext.height
        expected_bytes = expected_pixels * np.dtype(userContext.dtype).itemsize

        if size < expected_bytes:
            userContext.callbackError = (
                f"Buffer size {size} is smaller than expected {expected_bytes} "
                f"for PixelFormat {userContext.pixelFormat}"
            )
            return

        buf_type = ctypes.c_uint8 * size
        buf = buf_type.from_address(base)

        arr = np.frombuffer(
            buf,
            dtype=userContext.dtype,
            count=expected_pixels
        ).reshape((userContext.height, userContext.width))

        userContext.frameCounter += 1

        if userContext.frameCounter == userContext.framesToReceive:
            userContext.lastFrame = arr.copy()

        (status, frameCount) = KYFG_GetGrabberValueInt(
            userContext.grabberHandle, "RXFrameCounter"
        )
        (status, dropFrameCounter) = KYFG_GetGrabberValueInt(
            userContext.grabberHandle, "DropFrameCounter"
        )

        print(
            f"Received frame {userContext.frameCounter}/{userContext.framesToReceive}; "
            f"RXFrameCounter = {frameCount}; DropFrameCounter = {dropFrameCounter}",
            end='\\r'
        )

    except Exception as ex:
        userContext.callbackError = str(ex)

    finally:
        try:
            (status,) = KYFG_BufferToQueue(
                buffHandle,
                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT
            )
        except:
            pass

    return


class CallbackStruct:
    def __init__(
            self,
            width,
            height,
            grabberHandle,
            dtype,
            pixelFormat,
            framesToReceive=10):
        self.width = width
        self.height = height
        self.grabberHandle = grabberHandle
        self.dtype = dtype
        self.pixelFormat = pixelFormat
        self.framesToReceive = framesToReceive
        self.frameCounter = 0
        self.lastFrame = None
        self.callbackError = None


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


def get_pixel_format_dtype(pixel_format):
    pixel_format_upper = pixel_format.upper()

    if "PACKED" in pixel_format_upper or pixel_format_upper.endswith("P"):
        raise ValueError(
            f"Packed PixelFormat '{pixel_format}' is not supported by case_5695"
        )

    if not (
        pixel_format_upper.startswith("MONO")
        or pixel_format_upper.startswith("BAYER")
    ):
        raise ValueError(
            f"PixelFormat '{pixel_format}' is not a supported Mono/Bayer format"
        )

    if pixel_format_upper.endswith("8"):
        return np.uint8

    return np.uint16


def get_camera_pixel_format(cameraHandle):
    (status, pixel_format) = KYFG_GetCameraValueStringCopy(
        cameraHandle,
        "PixelFormat"
    )
    return pixel_format


def load_default_user_set(cameraHandle):
    (status,) = KYFG_SetCameraValueEnum_ByValueName(
        cameraHandle,
        "UserSetSelector",
        "Default"
    )
    (status,) = KYFG_CameraExecuteCommand(
        cameraHandle,
        "UserSetLoad"
    )

    (status, user_set_value) = KYFG_GetCameraValueEnum(
        cameraHandle,
        "UserSetSelector"
    )

    assert user_set_value == 0, (
        f"UserSetSelector value is {user_set_value}, expected Default (0)"
    )

    print("UserSetSelector = Default")
    print("UserSetLoad executed")


def verify_default_user_set(cameraHandle):
    (status, user_set_value) = KYFG_GetCameraValueEnum(
        cameraHandle,
        "UserSetSelector"
    )

    print(
        f"UserSetSelector after Power ON = "
        f"{'Default' if user_set_value == 0 else user_set_value}"
    )

    assert user_set_value == 0, (
        f"UserSetSelector after Power ON is {user_set_value}, "
        f"expected Default (0)"
    )


def get_golden_parameters(cameraHandle, test_pattern_name):
    (status, test_pattern_value) = KYFG_GetCameraValueEnum(
        cameraHandle,
        "TestPattern"
    )
    (status, width) = KYFG_GetCameraValueInt(cameraHandle, "Width")
    (status, height) = KYFG_GetCameraValueInt(cameraHandle, "Height")
    (status, min_value) = KYFG_GetCameraValueInt(
        cameraHandle,
        "TestPatternValueMin"
    )
    (status, max_value) = KYFG_GetCameraValueInt(
        cameraHandle,
        "TestPatternValueMax"
    )
    (status, step) = KYFG_GetCameraValueInt(
        cameraHandle,
        "TestPatternStep"
    )

    return {
        "UserSetSelector": "Default",
        "TestPattern": test_pattern_name,
        "TestPatternValue": int(test_pattern_value),
        "TestPatternValueMin": int(min_value),
        "TestPatternValueMax": int(max_value),
        "TestPatternStep": int(step),
        "PixelFormat": get_camera_pixel_format(cameraHandle),
        "Width": int(width),
        "Height": int(height)
    }


def print_parameters(title, parameters):
    print()
    print(title)
    for name, value in parameters.items():
        print(f" - {name}: {value}")
    print()


def verify_cycle_parameters(cameraHandle, golden_parameters):
    (status, current_test_pattern) = KYFG_GetCameraValueEnum(
        cameraHandle,
        "TestPattern"
    )
    (status, current_min) = KYFG_GetCameraValueInt(
        cameraHandle,
        "TestPatternValueMin"
    )
    (status, current_max) = KYFG_GetCameraValueInt(
        cameraHandle,
        "TestPatternValueMax"
    )
    (status, current_step) = KYFG_GetCameraValueInt(
        cameraHandle,
        "TestPatternStep"
    )
    (status, current_width) = KYFG_GetCameraValueInt(
        cameraHandle,
        "Width"
    )
    (status, current_height) = KYFG_GetCameraValueInt(
        cameraHandle,
        "Height"
    )
    current_pixel_format = get_camera_pixel_format(cameraHandle)

    checks = {
        "TestPatternValue": int(current_test_pattern),
        "TestPatternValueMin": int(current_min),
        "TestPatternValueMax": int(current_max),
        "TestPatternStep": int(current_step),
        "PixelFormat": current_pixel_format,
        "Width": int(current_width),
        "Height": int(current_height)
    }

    for name, current_value in checks.items():
        golden_value = golden_parameters[name]

        assert current_value == golden_value, (
            f"{name} changed after Power ON: "
            f"current={current_value}, golden={golden_value}"
        )


def save_frame(frame, png_path, raw_path):
    Image.fromarray(frame).save(str(png_path))

    with open(raw_path, "wb") as raw_file:
        raw_file.write(frame.tobytes())

    print(f"[INFO] Saved image: {png_path}")
    print(f"[INFO] Saved RAW:   {raw_path}")


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

    number_of_sent_tests = args['number_of_sent_tests']
    requested_pixel_format = args['PixelFormat']

    case_folder = pathlib.Path(__file__).resolve().parent
    print(f"[INFO] Images will be saved in: {case_folder}")

    NUMBER_OF_FRAMES = 10
    ACQUISITION_TIMEOUT_SEC = 30
    TEST_PATTERN_NAME = "GrayHorizontalRamp"

    # ============================================================
    # INITIAL SETUP AND GOLDEN IMAGE
    # ============================================================

    (grabberHandle,) = KYFG_Open(device_index)

    ############################
    Reset_grabber(grabberHandle)
    ############################

    (status, camera_list) = KYFG_UpdateCameraList(grabberHandle)

    if len(camera_list) == 0:
        KYFG_Close(grabberHandle)
        return CaseReturnCode.NO_HW_FOUND

    camIndex = 0
    cameraHandle = camera_list[camIndex]

    (status,) = KYFG_CameraOpen2(cameraHandle, None)
    (status, camInfo) = KYFG_CameraInfo2(cameraHandle)

    (status,) = KYFG_SetGrabberValueInt(
        grabberHandle,
        "CameraSelector",
        camIndex
    )

    print("-----------------------------------------------------------")
    print(
        f"Selected camera: [{camIndex}] "
        f"{camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}"
    )
    print("-----------------------------------------------------------")

    # Set User Set = Default and execute UserSetLoad.
    load_default_user_set(cameraHandle)

    # Set requested PixelFormat. If omitted, keep Default User Set PixelFormat.
    if requested_pixel_format:
        (status,) = KYFG_SetCameraValueEnum_ByValueName(
            cameraHandle,
            "PixelFormat",
            requested_pixel_format
        )

    pixel_format = get_camera_pixel_format(cameraHandle)

    if requested_pixel_format:
        assert pixel_format == requested_pixel_format, (
            f"PixelFormat was requested as '{requested_pixel_format}', "
            f"but camera reports '{pixel_format}'"
        )

    pixel_dtype = get_pixel_format_dtype(pixel_format)

    # Set test pattern, then save all parameters used for Golden Image.
    (status,) = KYFG_SetCameraValueEnum_ByValueName(
        cameraHandle,
        "TestPattern",
        TEST_PATTERN_NAME
    )

    golden_parameters = get_golden_parameters(
        cameraHandle,
        TEST_PATTERN_NAME
    )

    print_parameters(
        "Golden Image parameters:",
        golden_parameters
    )

    width = golden_parameters["Width"]
    height = golden_parameters["Height"]
    minValue = golden_parameters["TestPatternValueMin"]
    maxValue = golden_parameters["TestPatternValueMax"]
    step = golden_parameters["TestPatternStep"]

    testPattern = TestPatternDescriptor(
        width,
        height,
        minValue,
        maxValue,
        step
    )

    golden_image = testPattern.generate_horizontal_pattern(
        dtype=pixel_dtype
    )

    safe_pixel_format = "".join(
        c if c.isalnum() or c in ("-", "_") else "_"
        for c in pixel_format
    )

    golden_png = case_folder / f"golden_{safe_pixel_format}.png"
    golden_raw = case_folder / f"golden_{safe_pixel_format}.raw"
    golden_json = case_folder / f"golden_{safe_pixel_format}_parameters.json"

    save_frame(
        golden_image,
        golden_png,
        golden_raw
    )

    with open(golden_json, "w", encoding="utf-8") as json_file:
        json.dump(golden_parameters, json_file, indent=4)

    print(f"[INFO] Saved parameters: {golden_json}")

    PoXCPLink = findPoCXPLik(grabberHandle)

    assert PoXCPLink is not None, "Could not find active PoCXP link"

    (status,) = KYFG_CameraClose(cameraHandle)

    # Power OFF before the first test cycle.
    (status,) = KYFG_SetGrabberValueBool(
        grabberHandle,
        "PoCXPAutoActive",
        False
    )
    (status,) = KYFG_SetGrabberValueEnum(
        grabberHandle,
        f"PoCXP{PoXCPLink}",
        0
    )
    (status,) = KYFG_Close(grabberHandle)

    # ============================================================
    # POWER CYCLES
    # ============================================================

    for test_index in range(number_of_sent_tests):

        cycle_number = test_index + 1

        print()
        print("===========================================================")
        print(f"START TEST NUMBER {cycle_number}")
        print("===========================================================")

        # Wait 1000 msec while camera is powered OFF.
        time.sleep(1)

        grabberHandle = None
        cameraHandle = None
        streamHandle = None

        try:
            # Power ON.
            (grabberHandle,) = KYFG_Open(device_index)

            (status,) = KYFG_SetGrabberValueBool(
                grabberHandle,
                "PoCXPAutoActive",
                False
            )
            (status,) = KYFG_SetGrabberValueEnum(
                grabberHandle,
                f"PoCXP{PoXCPLink}",
                1
            )

            # Keep the camera boot delay used by the original test.
            time.sleep(5)

            (status, camera_list) = KYFG_UpdateCameraList(
                grabberHandle
            )

            assert len(camera_list) > camIndex, (
                f"Camera was not discovered after Power ON "
                f"in test {cycle_number}"
            )

            cameraHandle = camera_list[camIndex]

            (status,) = KYFG_CameraOpen2(
                cameraHandle,
                None
            )

            (status, current_cam_info) = KYFG_CameraInfo2(
                cameraHandle
            )

            assert current_cam_info.deviceModelName == camInfo.deviceModelName, (
                f"Different camera detected after Power ON: "
                f"{current_cam_info.deviceModelName}, "
                f"expected {camInfo.deviceModelName}"
            )

            (status,) = KYFG_SetGrabberValueInt(
                grabberHandle,
                "CameraSelector",
                camIndex
            )

            # Verify that UserSetSelector is Default after Power ON.
            verify_default_user_set(cameraHandle)

            # Restore the same PixelFormat and TestPattern used for Golden Image.
            (status,) = KYFG_SetCameraValueEnum_ByValueName(
                cameraHandle,
                "PixelFormat",
                golden_parameters["PixelFormat"]
            )
            (status,) = KYFG_SetCameraValueEnum_ByValueName(
                cameraHandle,
                "TestPattern",
                golden_parameters["TestPattern"]
            )

            # Verify all relevant parameters against Golden Image parameters.
            verify_cycle_parameters(
                cameraHandle,
                golden_parameters
            )

            # Reset acquisition statistics when supported.
            try:
                (status,) = KYFG_SetGrabberValueInt(
                    grabberHandle,
                    "RXFrameCounter",
                    0
                )
            except:
                pass

            try:
                (status,) = KYFG_SetGrabberValueInt(
                    grabberHandle,
                    "DropFrameCounter",
                    0
                )
            except:
                pass

            # Receive exactly 10 images.
            (status, streamHandle) = KYFG_StreamCreate(
                cameraHandle,
                0
            )

            streamCallbackStruct = CallbackStruct(
                width=width,
                height=height,
                grabberHandle=grabberHandle,
                dtype=pixel_dtype,
                pixelFormat=pixel_format,
                framesToReceive=NUMBER_OF_FRAMES
            )

            (status,) = KYFG_StreamBufferCallbackRegister(
                streamHandle,
                callbackFunc,
                streamCallbackStruct
            )

            (status, payload_size, _, _) = KYFG_StreamGetInfo(
                streamHandle,
                KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE
            )

            streamBufferHandle = [0 for _ in range(16)]

            for iFrame in range(len(streamBufferHandle)):
                (status, streamBufferHandle[iFrame]) = KYFG_BufferAllocAndAnnounce(
                    streamHandle,
                    payload_size,
                    0
                )

            (status,) = KYFG_BufferQueueAll(
                streamHandle,
                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
                KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT
            )

            (status,) = KYFG_CameraStart(
                cameraHandle,
                streamHandle,
                NUMBER_OF_FRAMES
            )

            start_time = time.time()

            while streamCallbackStruct.frameCounter < NUMBER_OF_FRAMES:

                if streamCallbackStruct.callbackError is not None:
                    break

                if time.time() - start_time > ACQUISITION_TIMEOUT_SEC:
                    break

                time.sleep(0.05)

            try:
                (status,) = KYFG_CameraStop(cameraHandle)
            except:
                pass

            (status, frameCount) = KYFG_GetGrabberValueInt(
                grabberHandle,
                "RXFrameCounter"
            )
            (status, dropFrameCounter) = KYFG_GetGrabberValueInt(
                grabberHandle,
                "DropFrameCounter"
            )

            print(
                f"\\nTest statistic: frame counter {frameCount} "
                f"Drop frames {dropFrameCounter} "
                f"callbacks {streamCallbackStruct.frameCounter}"
            )

            assert streamCallbackStruct.callbackError is None, (
                f"Stream callback error: {streamCallbackStruct.callbackError}"
            )

            assert streamCallbackStruct.frameCounter == NUMBER_OF_FRAMES, (
                f"Received {streamCallbackStruct.frameCounter} images, "
                f"expected {NUMBER_OF_FRAMES}"
            )

            assert streamCallbackStruct.lastFrame is not None, (
                "The 10th image was not received"
            )

            last_frame = streamCallbackStruct.lastFrame

            # Save only the last (10th) image.
            cycle_png = (
                case_folder /
                f"cycle_{cycle_number:03d}_frame_010_{safe_pixel_format}.png"
            )
            cycle_raw = (
                case_folder /
                f"cycle_{cycle_number:03d}_frame_010_{safe_pixel_format}.raw"
            )

            save_frame(
                last_frame,
                cycle_png,
                cycle_raw
            )

            # Compare the last image with Golden Image.
            images_equal = np.array_equal(
                last_frame,
                golden_image
            )

            if not images_equal:
                mismatch_mask = last_frame != golden_image
                mismatch_count = int(np.count_nonzero(mismatch_mask))

                print(
                    f"[FAIL] Golden Image mismatch: "
                    f"{mismatch_count} different pixels"
                )

                mismatch_indexes = np.argwhere(mismatch_mask)

                for row, col in mismatch_indexes[:10]:
                    print(
                        f"  [{row}, {col}] "
                        f"actual={last_frame[row, col]}, "
                        f"golden={golden_image[row, col]}"
                    )

            assert images_equal, (
                f"TEST NUMBER {cycle_number} IS FAILED: "
                f"10th image is different from Golden Image"
            )

            assert dropFrameCounter == 0, (
                f"TEST NUMBER {cycle_number} IS FAILED: "
                f"DropFrameCounter = {dropFrameCounter}"
            )

            print(f"TEST NUMBER {cycle_number} IS PASSED")

        finally:
            if streamHandle:
                try:
                    KYFG_StreamDelete(streamHandle)
                except:
                    pass

            if cameraHandle:
                try:
                    KYFG_CameraClose(cameraHandle)
                except:
                    pass

            if grabberHandle:
                try:
                    KYFG_SetGrabberValueEnum(
                        grabberHandle,
                        f"PoCXP{PoXCPLink}",
                        0
                    )
                except:
                    pass

                try:
                    KYFG_Close(grabberHandle)
                except:
                    pass

    # Restore automatic PoCXP handling.
    (grabberHandle,) = KYFG_Open(device_index)

    try:
        (status,) = KYFG_SetGrabberValueBool(grabberHandle, "PoCXPAutoActive", True)
        (status,) = KYFG_GrabberExecuteCommand(grabberHandle, "CxpPoCxpAuto")
        print("PoCXPAutoActive is True")
    finally:
        KYFG_Close(grabberHandle)

    print(f'\\nExiting from CaseRun({args}) with code 0...')
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

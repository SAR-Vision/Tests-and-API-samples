# Common KAYA imports DO NOT EDIT!!!
import ctypes
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
import queue
import matplotlib.pyplot as plt
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
    parser.add_argument('--camera', type=str, default='Any', help='Camera model')
    parser.add_argument('--width', type=int, default=22, help='Width')
    parser.add_argument('--height', type=int, default=22, help='Height')
    parser.add_argument('--cameraPixelFormat', type=str, default="BayerBG2", help='cameraPixelFormat')
    parser.add_argument('--grabberPixelFormat', type=str, default="RGB2", help='grabberPixelFormat')
    parser.add_argument('normal_expected_raw', type=str, help='Expected RAW image before debayering')
    parser.add_argument('debayered_expected_raw', type=str, help='Expected RAW image after debayering')
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


class StreamStruct():
    def __init__(self):
        self.frame = []
        self.queue = queue.Queue()
        self.datatype = 0
        return


def Stream_callback_func(buffHandle, userContext):
    if buffHandle == 0:
        return
    (KYFG_BufferGetInfo_status, pInfoBase, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
        buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_BASE)
    (KYFG_BufferGetInfo_status, pSize, pInfoSize, pInfoType) = KYFG_BufferGetInfo(
        buffHandle, KY_STREAM_BUFFER_INFO_CMD.KY_STREAM_BUFFER_INFO_SIZE)
    userContext.frame = numpy_from_data(pInfoBase, pSize, userContext.datatype).copy()
    userContext.queue.put(True)
    (status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    return


def numpy_from_data(buffData, buffSize, datatype):
    data_pointer = ctypes.cast(buffData, ctypes.c_char_p)
    buffer_from_memory = ctypes.pythonapi.PyMemoryView_FromMemory
    buffer_from_memory.restype = ctypes.py_object
    buffer = buffer_from_memory(data_pointer, buffSize)
    return np.frombuffer(buffer, datatype)


def acquire_one_frame(cameraHandle, datatype, timeout=15):
    """Create a stream, acquire exactly one frame, delete the stream, and return a copied NumPy array."""
    streamStruct = StreamStruct()
    streamStruct.datatype = datatype
    streamHandle = 0

    try:
        (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
        (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, Stream_callback_func, streamStruct)

        (status, payload_size, frameDataSize, pInfoType) = KYFG_StreamGetInfo(
            streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

        buffersArray = [0 for _ in range(16)]
        for i in range(len(buffersArray)):
            buffersArray[i] = KYFG_BufferAllocAndAnnounce(streamHandle, payload_size, 0)

        (status,) = KYFG_BufferQueueAll(
            streamHandle,
            KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED,
            KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)

        (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 1)
        streamStruct.queue.get(timeout=timeout)
        (status,) = KYFG_CameraStop(cameraHandle)

        return streamStruct.frame.copy()

    finally:
        if streamHandle:
            try:
                KYFG_CameraStop(cameraHandle)
            except:
                pass
            try:
                KYFG_StreamBufferCallbackUnregister(streamHandle, Stream_callback_func)
            except:
                pass
            try:
                KYFG_StreamDelete(streamHandle)
            except:
                pass


def pixel_format_datatype(pixel_format):
    """Return the NumPy storage type used by VP buffers for the requested pixel format."""
    digits = ''.join(ch for ch in pixel_format if ch.isdigit())
    bits_per_component = int(digits) if digits else 8
    return np.uint8 if bits_per_component <= 8 else np.uint16


def camera_name_for_file(camera_model):
    """Convert a camera model to a filesystem-friendly name."""
    safe = ''.join(ch if ch.isalnum() else '_' for ch in camera_model)
    while '__' in safe:
        safe = safe.replace('__', '_')
    return safe.strip('_')


def camera_matches_request(camera_model, requested_camera, normal_raw_name, debayered_raw_name):
    """Return True when the detected camera belongs to this parametrized test run."""
    if requested_camera != 'Any':
        if requested_camera == 'Chameleon':
            return 'Chameleon' in camera_model
        if requested_camera.endswith('*'):
            return camera_model.startswith(requested_camera[:-1])
        if requested_camera == 'Iron':
            return camera_model.startswith('Iron')
        return camera_model == requested_camera

    # The current Redmine Iron parametrizations intentionally omit --camera.
    # Limit those runs to the Iron family based on their golden RAW names,
    # instead of applying them to every camera attached to the grabber.
    raw_names = (normal_raw_name + ' ' + debayered_raw_name).lower()
    if 'iron' in raw_names:
        return camera_model.lower().startswith('iron')

    return True


def _numbered_pair_candidates(before_path, after_path, max_index=9):
    """Yield configured golden pair followed by before1/after1, before2/after2, ... variants."""
    yield before_path, after_path

    for index in range(1, max_index + 1):
        before_numbered = before_path.with_name(before_path.stem + str(index) + before_path.suffix)
        after_numbered = after_path.with_name(after_path.stem + str(index) + after_path.suffix)
        yield before_numbered, after_numbered


def resolve_expected_raw_pair(test_dir, configured_before, configured_after,
                              camera_model, width, height):
    """
    Resolve a golden RAW pair for a detected camera.

    Priority:
      1. Exact names supplied by the parametrization.
      2. Numbered variants of those names (before1/after1, ...).
      3. Camera-specific names: <camera>_<width>x<height>_before/after.raw.
      4. Numbered variants of the camera-specific names.

    The files are resolved per detected camera so a test folder may contain
    golden data for several camera models/configurations.
    """
    fmt = {
        'camera': camera_model,
        'camera_safe': camera_name_for_file(camera_model),
        'width': width,
        'height': height,
    }

    try:
        configured_before = configured_before.format(**fmt)
        configured_after = configured_after.format(**fmt)
    except (KeyError, ValueError):
        # Preserve literal filenames if they contain unrelated braces.
        pass

    configured_before_path = test_dir / configured_before
    configured_after_path = test_dir / configured_after

    camera_safe = camera_name_for_file(camera_model)
    camera_before_path = test_dir / f'{camera_safe}_{width}x{height}_before.raw'
    camera_after_path = test_dir / f'{camera_safe}_{width}x{height}_after.raw'

    candidates = []
    candidates.extend(_numbered_pair_candidates(configured_before_path, configured_after_path))
    candidates.extend(_numbered_pair_candidates(camera_before_path, camera_after_path))

    checked = []
    for before_path, after_path in candidates:
        checked.append((before_path, after_path))
        if before_path.is_file() and after_path.is_file():
            return before_path.absolute(), after_path.absolute()

    checked_text = '\n'.join(
        f'  before: {before_path.name} ; after: {after_path.name}'
        for before_path, after_path in checked)
    raise FileNotFoundError(
        f'No complete golden RAW pair found for camera {camera_model}. Checked:\n{checked_text}')


def load_expected_raw(path, datatype):
    with open(path, 'rb') as raw_file:
        return np.frombuffer(raw_file.read(), dtype=datatype)


def validate_expected_raw_sizes(expected_before, expected_after, width, height,
                                grabber_pixel_format, before_path, after_path):
    expected_before_values = width * height
    after_channels = 3 if grabber_pixel_format.upper().startswith('RGB') else 1
    expected_after_values = width * height * after_channels

    if len(expected_before) != expected_before_values:
        raise ValueError(
            f'Golden RAW size mismatch: {before_path.name} contains {len(expected_before)} values, '
            f'but {width}x{height} Normal input expects {expected_before_values}.')

    if len(expected_after) != expected_after_values:
        raise ValueError(
            f'Golden RAW size mismatch: {after_path.name} contains {len(expected_after)} values, '
            f'but {width}x{height} {grabber_pixel_format} output expects {expected_after_values}.')


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

    if args["deviceList"]:
        return CaseReturnCode.SUCCESS

    if device_index < 0:
        if unattended:
            device_index = 0
            print(f'\n!!! deviceIndex {device_index} forcibly selected in unattended mode !!!')
        else:
            device_index = int(input(f'Select PCI device to use (0 ... {infosize_test - 1})'))
            print(f'\ndeviceIndex {device_index} selected')

    if device_index >= infosize_test:
        print(f'\nDevice with the index {device_index} does not exist, exiting...')
        return CaseReturnCode.NO_HW_FOUND

    width = args["width"]
    height = args["height"]
    cameraPixelFormat = args["cameraPixelFormat"]
    grabberPixelFormat = args["grabberPixelFormat"]
    requested_camera = args['camera']
    configured_before = args["normal_expected_raw"]
    configured_after = args["debayered_expected_raw"]
    test_dir = pathlib.Path(__file__).parent

    # PASS 1 and PASS 2 can have different storage widths.
    # Example: BayerRG16 -> RGB8 must use uint16 before and uint8 after.
    normal_datatype = pixel_format_datatype(cameraPixelFormat)
    debayered_datatype = pixel_format_datatype(grabberPixelFormat)

    (status, device_info) = KY_DeviceInfo(device_index)
    if device_info.m_Protocol != KY_DEVICE_PROTOCOL.KY_DEVICE_PROTOCOL_CoaXPress:
        return CaseReturnCode.COULD_NOT_RUN

    grabberHandle = 0
    matched_camera_found = False

    try:
        (grabberHandle,) = KYFG_Open(device_index)
        Reset_grabber(grabberHandle)

        print("-----------------------------------------------------------")
        print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
        print("-----------------------------------------------------------\n")

        (status, camera_list) = KYFG_UpdateCameraList(grabberHandle)
        if len(camera_list) == 0:
            print("-----------------------------------------------------------")
            print('There are no cameras on this grabber')
            print("-----------------------------------------------------------\n")
            return CaseReturnCode.NO_HW_FOUND

        for cameraIndex, cameraHandle in enumerate(camera_list):
            (status, camera_info) = KYFG_CameraInfo2(cameraHandle)

            if not camera_matches_request(
                    camera_info.deviceModelName,
                    requested_camera,
                    configured_before,
                    configured_after):
                continue

            matched_camera_found = True
            camera_opened = False

            try:
                expectedBeforeFile, expectedAfterFile = resolve_expected_raw_pair(
                    test_dir,
                    configured_before,
                    configured_after,
                    camera_info.deviceModelName,
                    width,
                    height)

                expectedBeforeFile_data = load_expected_raw(expectedBeforeFile, normal_datatype)
                expectedAfterFile_data = load_expected_raw(expectedAfterFile, debayered_datatype)

                validate_expected_raw_sizes(
                    expectedBeforeFile_data,
                    expectedAfterFile_data,
                    width,
                    height,
                    grabberPixelFormat,
                    expectedBeforeFile,
                    expectedAfterFile)

                print("-----------------------------------------------------------")
                print(f"Matched camera: [{cameraIndex}] {camera_info.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
                print(f"Golden before RAW: {expectedBeforeFile.name}")
                print(f"Golden after RAW : {expectedAfterFile.name}")
                print("-----------------------------------------------------------")

                (status,) = KYFG_CameraOpen2(cameraHandle, None)
                camera_opened = True

                (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)
                Reset_camera(cameraHandle, grabberHandle)

                (status,) = KYFG_SetCameraValueInt(cameraHandle, "Width", width)
                (status,) = KYFG_SetCameraValueInt(cameraHandle, "Height", height)

                # Configure deterministic image source for cameras supported by the original test.
                if "Adimec" in camera_info.deviceVendorName:
                    (status,) = KYFG_SetCameraValueEnum_ByValueName(
                        cameraHandle, "TestImageSelector", "AdimecTestPattern")
                elif "Chameleon" in camera_info.deviceModelName:
                    (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "VideoSourceType", "File")
                    (status,) = KYFG_SetCameraValueString(cameraHandle, "SourceFilePath", expectedBeforeFile.as_posix())

                try:
                    (status,) = KYFG_SetCameraValueEnum_ByValueName(
                        cameraHandle, "PixelFormat", cameraPixelFormat)
                except:
                    print(f'Incorrect camera Pixel Format {cameraPixelFormat}')
                    return CaseReturnCode.WRONG_PARAM_VALUE

                (status,) = KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)
                (status,) = KYFG_SetCameraValueEnum_ByValueName(cameraHandle, "TestPattern", "GrayHorizontalRamp")
                (status, pattern) = KYFG_GetCameraValueEnum(cameraHandle, "TestPattern")
                print("Test Pattern:", pattern)

                # -------------------------------------------------------------
                # PASS 1: debayering disabled (grabber PixelFormat = Normal).
                # -------------------------------------------------------------
                try:
                    (status,) = KYFG_SetGrabberValueEnum_ByValueName(
                        grabberHandle, "PixelFormat", "Normal")
                except:
                    print('Could not set grabber PixelFormat to Normal')
                    return CaseReturnCode.WRONG_PARAM_VALUE

                print("\nPASS 1: PixelFormat = Normal")
                print("Camera PixelFormat:", KYFG_GetCameraValueStringCopy(cameraHandle, "PixelFormat"))
                print("Grabber PixelFormat:", KYFG_GetGrabberValueStringCopy(grabberHandle, "PixelFormat"))

                normal_frame = acquire_one_frame(cameraHandle, normal_datatype)

                print("Expected before file length:", len(expectedBeforeFile_data),
                      ", acquired normal frame length:", len(normal_frame))

                if len(normal_frame) != len(expectedBeforeFile_data):
                    raise AssertionError(
                        f'Image before debayering has wrong size: got {len(normal_frame)}, '
                        f'expected {len(expectedBeforeFile_data)}')

                normal_different = np.count_nonzero(normal_frame != expectedBeforeFile_data)
                print("Different values before debayering:", normal_different)

                assert np.array_equal(normal_frame, expectedBeforeFile_data), \
                    "Image before debayering is not equal to expected image"

                print("PASS 1 succeeded: image before debayering matches expected image")

                # -------------------------------------------------------------
                # PASS 2: hardware debayering enabled by grabber PixelFormat.
                # A new stream is created because the payload size can change.
                # -------------------------------------------------------------
                try:
                    (status,) = KYFG_SetGrabberValueEnum_ByValueName(
                        grabberHandle, "PixelFormat", grabberPixelFormat)
                except:
                    print(f'Incorrect grabber Pixel Format {grabberPixelFormat} '
                          f'for camera PixelFormat {cameraPixelFormat}')
                    return CaseReturnCode.WRONG_PARAM_VALUE

                print(f"\nPASS 2: PixelFormat = {grabberPixelFormat}")
                print("Camera PixelFormat:", KYFG_GetCameraValueStringCopy(cameraHandle, "PixelFormat"))
                print("Grabber PixelFormat:", KYFG_GetGrabberValueStringCopy(grabberHandle, "PixelFormat"))

                debayered_frame = acquire_one_frame(cameraHandle, debayered_datatype)

                print("Expected after file length:", len(expectedAfterFile_data),
                      ", acquired debayered frame length:", len(debayered_frame))

                if len(debayered_frame) != len(expectedAfterFile_data):
                    raise AssertionError(
                        f'Image after debayering has wrong size: got {len(debayered_frame)}, '
                        f'expected {len(expectedAfterFile_data)}')

                # Preserve the original case behavior: for hardware debayering,
                # ignore the first and last output rows.
                if grabberPixelFormat.upper().startswith('RGB'):
                    row_size = width * 3
                    real_frame = debayered_frame[row_size:-row_size]
                    expected_frame = expectedAfterFile_data[row_size:-row_size]
                else:
                    real_frame = debayered_frame
                    expected_frame = expectedAfterFile_data

                different = np.count_nonzero(real_frame != expected_frame)
                print("Different values after debayering:", different)

                mismatch_idx = np.flatnonzero(real_frame != expected_frame)
                for idx in mismatch_idx[:20]:
                    print(
                        f"Mismatch at index {idx}: "
                        f"actual={real_frame[idx]}, expected={expected_frame[idx]}"
                    )

                assert np.array_equal(real_frame, expected_frame), \
                    "Image after debayering is not equal to expected image"

                print("PASS 2 succeeded: image after debayering matches expected image")

            finally:
                if camera_opened:
                    try:
                        KYFG_CameraClose(cameraHandle)
                    except:
                        pass

        if not matched_camera_found:
            if requested_camera == 'Any' and 'iron' in (configured_before + ' ' + configured_after).lower():
                print('No Iron-family camera was found for this parametrization')
            else:
                print(f"Requested camera model '{requested_camera}' was not found")
            return CaseReturnCode.NO_HW_FOUND

    finally:
        if grabberHandle:
            try:
                KYFG_Close(grabberHandle)
            except:
                pass

    print(f'\nExiting from CaseRun({args}) with code SUCCESS...')
    print('ALL MATCHED CAMERAS PASSED HARDWARE DEBAYERING TEST')
    return CaseReturnCode.SUCCESS


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 2856 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)

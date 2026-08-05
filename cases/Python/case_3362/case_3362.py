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
import psutil
import subprocess
import platform
import json
import pathlib
import gc
from typing import Dict, Any, List, Optional


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
    parser.add_argument('--numberOfTests', type=int, default=10, help='Number of cycles to run (inproc mode)')
    parser.add_argument('--numberOfBuffers', type=int, default=128, help='Number of buffers to allocate')
    parser.add_argument('--warmup', type=int, default=2, help='Warm-up cycles (discarded from leak calculation)')
    parser.add_argument('--linuxPssSlopeLimitMB', type=float, default=2.0, help='Max allowed Linux PSS tail-slope (MB/iter) in inproc mode (requires smaps_rollup)')
    parser.add_argument('--rssSlopeLimitMB', type=float, default=2.0, help='Max allowed RSS slope (MB per iter) in inproc mode')
    parser.add_argument('--privSlopeLimitMB', type=float, default=2.0, help='Max allowed Private Bytes slope (MB per iter, Windows) in inproc mode')
    parser.add_argument('--linuxMallocTrim', default=True, action='store_true', help='On Linux, call malloc_trim(0) after each iteration (test-only)')
    parser.add_argument('--no-linuxMallocTrim', dest='linuxMallocTrim', action='store_false')
    parser.add_argument('--instance', type=int, default=0, help='Instance')
    parser.add_argument('--settleTimeoutSec', type=float, default=10.0, help='Max seconds to wait after iteration for memory to stabilize before sampling AFTER')
    parser.add_argument('--settleStepSec', type=float, default=0.2, help='Polling interval while waiting for memory to stabilize')
    parser.add_argument('--settleStableCount', type=int, default=5, help='How many consecutive stable samples are required')
    parser.add_argument('--settleEpsilonMB', type=float, default=1.0, help='Stability epsilon (MB). If change below this, counts as stable')
    return parser

def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


def _bytes_mb(x: float) -> float:
    return float(x) / (1024.0 * 1024.0)


def _linux_smaps_rollup_bytes() -> Dict[str, int]:
    """
    Linux-only: parse /proc/self/smaps_rollup for rollup metrics.
    Returns dict in BYTES: {'pss':..., 'private_dirty':..., 'rss_rollup':...}
    """
    if not sys.platform.startswith("linux"):
        return {}
    out: Dict[str, int] = {}
    try:
        with open("/proc/self/smaps_rollup", "r", encoding="utf-8") as f:
            for line in f:
                if line.startswith("Pss:"):
                    out["pss"] = int(line.split()[1]) * 1024
                elif line.startswith("Private_Dirty:"):
                    out["private_dirty"] = int(line.split()[1]) * 1024
                elif line.startswith("Rss:"):
                    out["rss_rollup"] = int(line.split()[1]) * 1024
    except Exception:
        return {}
    return out

def _malloc_trim_linux(enabled: bool):
    if not enabled or not sys.platform.startswith("linux"):
        return
    try:
        import ctypes
        libc = ctypes.CDLL("libc.so.6")
        libc.malloc_trim(0)
    except Exception:
        pass


def _mem_snapshot(linux_trim: bool = False) -> Dict[str, int]:
    """
    Return dict with:
      - rss (psutil rss) always
      - Windows: private (if available from psutil memory_full_info)
      - Linux: pss/private_dirty/rss_rollup if /proc/self/smaps_rollup available
      - uss if psutil provides it (sometimes)
    If linux_trim=True, call malloc_trim(0) before taking the snapshot (Linux only).
    """
    if linux_trim:
        _malloc_trim_linux(True)

    p = psutil.Process()
    mi = p.memory_info()
    out: Dict[str, int] = {'rss': int(mi.rss)}

    # Linux rollup
    out.update(_linux_smaps_rollup_bytes())

    # psutil extended info
    try:
        mif = p.memory_full_info()
        if hasattr(mif, 'uss'):
            out['uss'] = int(mif.uss)
        if hasattr(mif, 'private'):
            out['private'] = int(mif.private)
    except Exception:
        pass

    return out


def _print_mem(label: str, snap: Dict[str, int]):
    parts = [f"RSS={_bytes_mb(snap.get('rss', 0)):.2f} MB"]
    if 'pss' in snap:
        parts.append(f"PSS={_bytes_mb(snap['pss']):.2f} MB")
    if 'private_dirty' in snap:
        parts.append(f"PrivateDirty={_bytes_mb(snap['private_dirty']):.2f} MB")
    if 'private' in snap:
        parts.append(f"Private={_bytes_mb(snap['private']):.2f} MB")
    if 'uss' in snap:
        parts.append(f"USS={_bytes_mb(snap['uss']):.2f} MB")
    print(f"{label}: " + ", ".join(parts))


def _slope_per_iter(samples: List[int]) -> float:
    """Simple slope (last - first) / (n-1). Returns 0 if <2 samples."""
    if len(samples) < 2:
        return 0.0
    return float(samples[-1] - samples[0]) / max(1, (len(samples) - 1))


def _tail_slope(samples: List[int], tail: int = 3) -> float:
    """
    Slope over the last 'tail' steps:
    (last - sample[-(tail+1)]) / tail
    Falls back to full slope if not enough samples.
    """
    if len(samples) < tail + 1:
        return _slope_per_iter(samples)
    return float(samples[-1] - samples[-(tail + 1)]) / float(tail)


def _median_delta_per_iter(samples: List[int]) -> float:
    """
    Robust "typical" delta per iteration: median of consecutive differences.
    Great for rejecting one-off spikes.
    """
    if len(samples) < 2:
        return 0.0
    deltas = [samples[i] - samples[i - 1] for i in range(1, len(samples))]
    deltas.sort()
    return float(deltas[len(deltas) // 2])


def _preferred_metric(snap: Dict[str, int]) -> str:
    """
    Metric used ONLY for settle/stability tracking.
    - Windows: USS (best), else Private, else RSS
    - Linux: use RSS (or USS if available) to track allocator/native cleanup stability.
            Do NOT use PSS/PrivateDirty for settle decisions (too noisy for this purpose).
    """
    if os.name == "nt":
        if 'uss' in snap:
            return 'uss'
        if 'private' in snap:
            return 'private'
        return 'rss'
    else:
        # Linux / other Unix
        if 'uss' in snap:
            return 'uss'
        return 'rss'


def _settle_memory(timeout_s: float, step_s: float, stable_needed: int, epsilon_mb: float,
                   linux_trim: bool) -> Dict[str, int]:
    """
    Wait and sample memory repeatedly after an iteration.

    Returns a dict where each metric is the MINIMUM (floor) observed during the window.
    Stability tracking is done using _preferred_metric() (RSS on Linux, USS on Windows if present).

    This avoids Linux regressions where PSS/PrivateDirty noise accidentally drives settle decisions.
    """
    epsilon = epsilon_mb * 1024 * 1024
    start = time.time()
    stable = 0

    last_val: Optional[int] = None
    last_snap: Optional[Dict[str, int]] = None

    # Track per-metric minima (floor)
    min_vals: Dict[str, int] = {}

    keys_to_floor = ('rss', 'uss', 'private', 'pss', 'private_dirty', 'rss_rollup')

    while time.time() - start < timeout_s:
        gc.collect()
        snap = _mem_snapshot(linux_trim=linux_trim)
        last_snap = snap

        # Update per-key minima
        for k in keys_to_floor:
            if k in snap:
                v = int(snap[k])
                if k not in min_vals or v < min_vals[k]:
                    min_vals[k] = v

        # Stability tracking using the chosen settle key (Linux: RSS / Windows: USS if available)
        key = _preferred_metric(snap)
        cur = int(snap.get(key, snap.get('rss', 0)))

        if last_val is not None:
            if abs(cur - last_val) < epsilon:
                stable += 1
            else:
                stable = 0
        last_val = cur

        if stable >= stable_needed:
            break

        time.sleep(step_s)

    # Build result: start from last snapshot (keeps any extra fields),
    # but override floored metrics with the per-key minima.
    if last_snap is None:
        last_snap = _mem_snapshot(linux_trim=linux_trim)

    out = dict(last_snap)
    out.update(min_vals)
    return out


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


def _dump_fingerprint():
    print("\n================= RUN FINGERPRINT =================")
    print("sys.executable:", sys.executable)
    print("sys.version:", sys.version.replace("\n", " "))
    print("platform:", platform.platform())
    print("cwd:", os.getcwd())
    print("pid:", os.getpid(), "ppid:", os.getppid())
    print("argv:", sys.argv)
    print("WithAdapter:", os.environ.get("WithAdapter"))
    print("KAYA_VISION_POINT_PYTHON_PATH:", os.environ.get("KAYA_VISION_POINT_PYTHON_PATH"))
    print("PYTHONPATH:", os.environ.get("PYTHONPATH"))
    try:
        import psutil as _ps
        print("psutil:", _ps.__version__)
    except Exception as e:
        print("psutil import failed:", e)
    print("===================================================\n")


class StreamStruct:
    def __init__(self):
        self.callbackCount = 0

def callbackFunction(buffHandle, userContext):
    if buffHandle == NULL_STREAM_BUFFER_HANDLE or buffHandle == INVALID_STREAM_BUFFER_HANDLE:
        return
    userContext.callbackCount += 1
    try:
        (KYFG_BufferToQueue_status,) = KYFG_BufferToQueue(buffHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT)
    except Exception:
        return

def _one_cycle(device_index: int, numberOfBuffers: int) -> int:
    """
    Run one open->acquire->close cycle across all cameras on the chosen grabber.
    Returns 0 on success (no errors), >0 if errors observed.
    """
    error_count = 0
    (status, device_info) = KY_DeviceInfo(device_index)

    (grabberHandle,) = KYFG_Open(device_index)
    print("-----------------------------------------------------------")
    print(f"Selected grabber: [{device_index}] {device_info.szDeviceDisplayName}, FGHANDLE: {str(grabberHandle)}")
    print("-----------------------------------------------------------\n")

    try:
        (status, cameraList) = KYFG_UpdateCameraList(grabberHandle)
        if not len(cameraList):
            print("-----------------------------------------------------------")
            print('No cameras were found on this grabber')
            print("-----------------------------------------------------------\n")
            return 1

        for cameraIndex, cameraHandle in enumerate(cameraList):
            (status, camInfo) = KYFG_CameraInfo2(cameraHandle)
            (status,) = KYFG_CameraOpen2(cameraHandle, None)

            print("-----------------------------------------------------------")
            print(f"Selected camera: [{cameraIndex}] {camInfo.deviceModelName}, CAMHANDLE: {hex(cameraHandle)}")
            print("-----------------------------------------------------------\n")

            streamHandle = None
            streamBufferHandle = []

            try:
                Reset_camera(cameraHandle, grabberHandle)

                try:
                    if KYFG_IsGrabberValueImplemented(grabberHandle, 'TriggerMode'):
                        KYFG_SetGrabberValueEnum(grabberHandle, "TriggerMode", 0)
                    if KYFG_IsCameraValueImplemented(cameraHandle, "TriggerMode"):
                        KYFG_SetCameraValueEnum(cameraHandle, "TriggerMode", 0)
                    if KYFG_IsCameraValueImplemented(cameraHandle, "SimulationTriggerMode"):
                        KYFG_SetCameraValueEnum(cameraHandle, "SimulationTriggerMode", 0)
                except Exception:
                    pass

                (status, streamHandle) = KYFG_StreamCreate(cameraHandle, 0)
                streamStruct = StreamStruct()
                (status,) = KYFG_StreamBufferCallbackRegister(streamHandle, callbackFunction, streamStruct)

                (status, payload_size, frameDataSize, pInfoType) = \
                    KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_PAYLOAD_SIZE)

                pool_mb = payload_size * numberOfBuffers / (1024.0 * 1024.0)
                print(f"[INFO] payload_size={payload_size} bytes, buffers={numberOfBuffers}, "
                      f"pool_per_camera={pool_mb:.1f} MB")

                (KYFG_StreamGetInfo_status, buf_allignment, frameDataAligment, pInfoType) = \
                    KYFG_StreamGetInfo(streamHandle, KY_STREAM_INFO_CMD.KY_STREAM_INFO_BUF_ALIGNMENT)

                streamBufferHandle = [0] * numberOfBuffers
                for i in range(len(streamBufferHandle)):
                    streamBufferHandle[i] = KYFG_BufferAllocAndAnnounce(streamHandle, payload_size, 0)

                (status,) = KYFG_BufferQueueAll(
                    streamHandle, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_UNQUEUED, KY_ACQ_QUEUE_TYPE.KY_ACQ_QUEUE_INPUT
                )
                (status,) = KYFG_CameraStart(cameraHandle, streamHandle, 0)

                time.sleep(5)

                (status,) = KYFG_CameraStop(cameraHandle)

                try:
                    KYFG_SetGrabberValueInt(grabberHandle, "CameraSelector", cameraIndex)
                    (status, frameCounter) = KYFG_GetGrabberValueInt(grabberHandle, "RXFrameCounter")
                except Exception:
                    frameCounter = 0

                print(f'Results for camera {camInfo.deviceModelName}: ')
                print('frameCounter: ', frameCounter, '\nCallbackCounter: ', streamStruct.callbackCount)
                if frameCounter == 0 or streamStruct.callbackCount == 0:
                    print("Acquisition is not started")
                    error_count += 1

            finally:
                if streamHandle is not None:
                    try:
                        KYFG_StreamBufferCallbackUnregister(streamHandle, callbackFunction)
                    except Exception:
                        pass
                    try:
                        for i in range(len(streamBufferHandle)):
                            KYFG_BufferRevoke(streamHandle, streamBufferHandle[i])
                    except Exception:
                        pass
                    try:
                        KYFG_StreamDelete(streamHandle)
                    except Exception:
                        pass

                try:
                    KYFG_CameraClose(cameraHandle)
                except Exception:
                    pass

    finally:
        try:
            (status,) = KYFG_Close(grabberHandle)
        except Exception:
            pass

    return error_count

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

    iterations = int(args['numberOfTests'])
    warmup = max(0, int(args['warmup']))
    numberOfBuffers = int(args['numberOfBuffers'])
    linux_trim = bool(args.get('linuxMallocTrim', False))

    settle_timeout = float(args.get('settleTimeoutSec', 10.0))
    settle_step = float(args.get('settleStepSec', 0.2))
    settle_stable = int(args.get('settleStableCount', 5))
    settle_eps_mb = float(args.get('settleEpsilonMB', 1.0))

    # Samples (measured AFTER each iteration, after settle)
    rss_samples: List[int] = []
    priv_samples: List[int] = []
    uss_samples: List[int] = []
    linux_pss_samples: List[int] = []
    linux_pd_samples: List[int] = []

    # Warm-up cycles
    for i in range(warmup):
        print(f'\n[WARMUP] Iteration {i + 1}/{warmup}')
        err = _one_cycle(device_index, numberOfBuffers)
        assert err == 0, f'[WARMUP] Acquisition errors observed in warmup iteration {i + 1}'
        gc.collect()

    # Measured cycles
    for i in range(iterations):
        print(f'\n[INPROC] Iteration {i + 1}/{iterations}')

        gc.collect()
        snap_before = _mem_snapshot(linux_trim=False)
        _print_mem('Process memory BEFORE', snap_before)

        err = _one_cycle(device_index, numberOfBuffers)
        assert err == 0, f'[INPROC] Acquisition errors observed in iteration {i + 1}'

        # IMPORTANT: wait for async native cleanup to settle
        snap_after = _settle_memory(
            timeout_s=settle_timeout,
            step_s=settle_step,
            stable_needed=settle_stable,
            epsilon_mb=settle_eps_mb,
            linux_trim=linux_trim
        )
        _print_mem('Process memory AFTER (floor)', snap_after)

        rss_samples.append(snap_after.get('rss', 0))
        if 'private' in snap_after:
            priv_samples.append(snap_after['private'])
        if 'uss' in snap_after:
            uss_samples.append(snap_after['uss'])

        if sys.platform.startswith("linux"):
            if 'pss' in snap_after:
                linux_pss_samples.append(snap_after['pss'])
            if 'private_dirty' in snap_after:
                linux_pd_samples.append(snap_after['private_dirty'])

    # Compute robust stats
    rss_slope = _slope_per_iter(rss_samples)
    rss_med_delta = _median_delta_per_iter(rss_samples)
    rss_tail = _tail_slope(rss_samples, tail=3)

    priv_slope = _slope_per_iter(priv_samples) if priv_samples else 0.0
    priv_med_delta = _median_delta_per_iter(priv_samples) if priv_samples else 0.0
    priv_tail = _tail_slope(priv_samples, tail=3) if priv_samples else 0.0

    uss_slope = _slope_per_iter(uss_samples) if uss_samples else 0.0
    uss_med_delta = _median_delta_per_iter(uss_samples) if uss_samples else 0.0
    uss_tail = _tail_slope(uss_samples, tail=3) if uss_samples else 0.0

    pss_tail = _tail_slope(linux_pss_samples, tail=3) if linux_pss_samples else 0.0
    pd_tail = _tail_slope(linux_pd_samples, tail=3) if linux_pd_samples else 0.0

    print("\n================= In-Process Leak Check Summary =================")
    print(f'Warmup iterations:   {warmup}')
    print(f'Iterations measured: {iterations}')
    print(f'Settle: timeout={settle_timeout}s, step={settle_step}s, stableCount={settle_stable}, eps={settle_eps_mb}MB')

    print(f'RSS slope:           {_bytes_mb(rss_slope):.3f} MB/iter')
    print(f'RSS median delta:    {_bytes_mb(rss_med_delta):.3f} MB/iter')
    print(f'RSS tail slope:      {_bytes_mb(rss_tail):.3f} MB/iter')

    if priv_samples:
        print(f'Private slope:       {_bytes_mb(priv_slope):.3f} MB/iter')
        print(f'Private median delta:{_bytes_mb(priv_med_delta):.3f} MB/iter')
        print(f'Private tail slope:  {_bytes_mb(priv_tail):.3f} MB/iter')
    else:
        print('Private:             N/A on this platform')

    if uss_samples:
        print(f'USS slope:           {_bytes_mb(uss_slope):.3f} MB/iter')
        print(f'USS median delta:    {_bytes_mb(uss_med_delta):.3f} MB/iter')
        print(f'USS tail slope:      {_bytes_mb(uss_tail):.3f} MB/iter')
    else:
        print('USS:                 N/A (psutil did not provide USS)')

    if sys.platform.startswith("linux"):
        if linux_pss_samples:
            print(f'Linux PSS tailSlope: {_bytes_mb(pss_tail):.3f} MB/iter')
        else:
            print('Linux PSS:           N/A (no /proc/self/smaps_rollup)')
        if linux_pd_samples:
            print(f'Linux PrivDirty tailSlope: {_bytes_mb(pd_tail):.3f} MB/iter')
        else:
            print('Linux PrivateDirty:  N/A (no /proc/self/smaps_rollup)')
        print(f'Linux malloc_trim:   {"ON" if linux_trim else "OFF"}')

    print("===============================================================================\n")

    # Limits (MB per iter)
    rss_limit = float(args['rssSlopeLimitMB']) * 1024 * 1024
    priv_limit = float(args['privSlopeLimitMB']) * 1024 * 1024
    linux_pss_tail_limit = float(args['linuxPssSlopeLimitMB']) * 1024 * 1024

    # NEW enforcement policy:
    # - Windows: prefer USS if available (median delta AND tail slope)
    # - Otherwise: use Private if available, else RSS
    # This is robust to one-off spikes and races.
    if os.name == "nt":
        # prefer floor USS (best). this is already the samples you collected (after settle floor).
        if uss_samples:
            # Robust criteria: median delta + tail-median delta (not tail slope)
            # Tail-median: median of last 4 consecutive deltas
            def _tail_median_delta(samples, tail_deltas=4):
                if len(samples) < tail_deltas + 1:
                    return _median_delta_per_iter(samples)
                deltas = [samples[i] - samples[i - 1] for i in range(len(samples) - tail_deltas, len(samples))]
                deltas.sort()
                return float(deltas[len(deltas) // 2])

            uss_tail_med = _tail_median_delta(uss_samples, tail_deltas=4)

            assert uss_med_delta < rss_limit and uss_tail_med < rss_limit, \
                (f"Leak suspected (Windows USS floor): median_delta {_bytes_mb(uss_med_delta):.3f} MB/iter or "
                 f"tail_median_delta {_bytes_mb(uss_tail_med):.3f} MB/iter exceeds limit {args['rssSlopeLimitMB']} MB/iter")
        elif priv_samples:
            assert priv_med_delta < priv_limit and priv_tail < priv_limit, \
                (f"Leak suspected (Windows Private): median_delta {_bytes_mb(priv_med_delta):.3f} MB/iter or "
                 f"tail_slope {_bytes_mb(priv_tail):.3f} MB/iter exceeds limit {args['privSlopeLimitMB']} MB/iter")
        else:
            assert rss_med_delta < rss_limit and rss_tail < rss_limit, \
                (f"Leak suspected (Windows RSS): median_delta {_bytes_mb(rss_med_delta):.3f} MB/iter or "
                 f"tail_slope {_bytes_mb(rss_tail):.3f} MB/iter exceeds limit {args['rssSlopeLimitMB']} MB/iter")
    elif sys.platform.startswith("linux"):
        # Linux: prefer PSS if available, but use robust criteria (median delta + tail slope)
        if linux_pss_samples:
            pss_med = _median_delta_per_iter(linux_pss_samples)
            assert pss_med < linux_pss_tail_limit and pss_tail < linux_pss_tail_limit, \
                (f"Leak suspected (Linux PSS): median_delta {_bytes_mb(pss_med):.3f} MB/iter or "
                 f"tail_slope {_bytes_mb(pss_tail):.3f} MB/iter exceeds limit {args['linuxPssSlopeLimitMB']} MB/iter")
        elif linux_pd_samples:
            pd_med = _median_delta_per_iter(linux_pd_samples)
            assert pd_med < linux_pss_tail_limit and pd_tail < linux_pss_tail_limit, \
                (f"Leak suspected (Linux PrivateDirty): median_delta {_bytes_mb(pd_med):.3f} MB/iter or "
                 f"tail_slope {_bytes_mb(pd_tail):.3f} MB/iter exceeds limit {args['linuxPssSlopeLimitMB']} MB/iter")
        else:
            assert rss_med_delta < rss_limit and rss_tail < rss_limit, \
                (f"Leak suspected (Linux RSS fallback): median_delta {_bytes_mb(rss_med_delta):.3f} MB/iter or "
                 f"tail_slope {_bytes_mb(rss_tail):.3f} MB/iter exceeds limit {args['rssSlopeLimitMB']} MB/iter")
    else:
        # Other OS: robust RSS
        assert rss_med_delta < rss_limit and rss_tail < rss_limit, \
            (f"Leak suspected: median_delta {_bytes_mb(rss_med_delta):.3f} MB/iter or "
             f"tail_slope {_bytes_mb(rss_tail):.3f} MB/iter exceeds limit {args['rssSlopeLimitMB']} MB/iter")

    print('In-process leak check: PASSED (robust + settled).')
    return CaseReturnCode.SUCCESS


if __name__ == "__main__":
    try:
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)

    exit(return_code)
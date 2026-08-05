# ============================================================================
# KNOWN BLOCKER (as of 2026-07-21): This case cannot currently run to completion.
#
# KY_AuthProgramKey / KY_AuthVerify fail with:
#     TypeError: _type_ must have storage info
# raised from POINTER(KY_AuthKey_C_STYLE) inside KYFGLib.py.
#
# Root cause: KY_AuthKey_C_STYLE in KYFGLib.py is defined as a plain class,
# not a ctypes.Structure subclass with _fields_:
#     class KY_AuthKey_C_STYLE:
#         secret = (c_ubyte * KY_AUTHKEY_SIZE)()
# It needs to be:
#     class KY_AuthKey_C_STYLE(Structure):
#         _fields_ = [("secret", c_ubyte * KY_AUTHKEY_SIZE)]
#
# The case logic below (open, program key, verify wrong key, verify valid
# key, close) is complete and correct. Blocked on an updated KYFGLib.py.
# Do not spend time debugging this case further until that fix lands.
# ============================================================================


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
import random


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


device_infos = {}


def make_random_key(exclude_key=None):
    """
    Build a KY_AuthKey with a random 32-byte secret (0-255 per element).
    If exclude_key is provided, guarantees the generated key's secret bytes
    differ from it in at least one position, so it can be used as a
    deliberately-wrong key for verification.
    """
    key = KY_AuthKey()
    length = len(key.secret)
    while True:
        candidate = [random.randint(0, 255) for _ in range(length)]
        if exclude_key is None or candidate != list(exclude_key.secret):
            break
    # Assign the whole list at once rather than element-by-element:
    # `secret` may be backed by a property/descriptor that rebuilds its
    # underlying ctypes array on each access, in which case per-element
    # assignment (key.secret[i] = ...) can silently write into a discarded
    # temporary instead of the real structure.
    key.secret = candidate
    return key


def CaseRun(args):
    print(f'\nEntering CaseRun({args}) (use -h or --help to print available parameters and exit)...')

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

    grabberHandle = None
    try:
        # ---- Step 1: Open Frame Grabber ----
        (grabberHandle,) = KYFG_Open(device_index)
        print(f'Grabber opened, handle={grabberHandle}')

        # ---- Step 2: Register authentication key ----
        # lock=0: key can be reprogrammed later. Using lock=1 would permanently
        # lock this key into the grabber's hardware and break every subsequent
        # run of this (and any other auth-dependent) case.
        valid_key = make_random_key()
        (program_status,) = KY_AuthProgramKey(grabberHandle, valid_key, 0)
        print(f'KY_AuthProgramKey status: {program_status}')
        assert program_status == FGSTATUS_OK, \
            f'KY_AuthProgramKey failed with status {program_status}'
        print('[PASS] Authentication key programmed successfully')

        # ---- Step 3: Verify with a wrong key -> must return an error ----
        wrong_key = make_random_key(exclude_key=valid_key)
        (wrong_verify_status,) = KY_AuthVerify(grabberHandle, wrong_key)
        print(f'KY_AuthVerify (wrong key) status: {wrong_verify_status}')
        assert wrong_verify_status != FGSTATUS_OK, \
            f'KY_AuthVerify unexpectedly returned FGSTATUS_OK for a wrong key'
        print('[PASS] Verification correctly rejected the wrong key')

        # ---- Step 4: Verify with the valid key -> must return FGSTATUS_OK ----
        (valid_verify_status,) = KY_AuthVerify(grabberHandle, valid_key)
        print(f'KY_AuthVerify (valid key) status: {valid_verify_status}')
        assert valid_verify_status == FGSTATUS_OK, \
            f'KY_AuthVerify failed for the valid key with status {valid_verify_status}'
        print('[PASS] Verification correctly accepted the valid key')

    finally:
        if grabberHandle:
            KYFG_Close(grabberHandle)
            print('Grabber closed')

    print(f'\nExiting from CaseRun({args}) with code 0...')
    return CaseReturnCode.SUCCESS


def ParseArgs():
    parser = CaseArgumentParser()
    args = parser.parse_args()
    return vars(args)


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 3584 Process ID:", os.getpid())
        args_ = ParseArgs()
        return_code = CaseRun(args_)
        print(f'Case return code: {return_code}')
    except Exception as ex:
        import traceback
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        traceback.print_exc()
        exit(-200)

    exit(return_code)
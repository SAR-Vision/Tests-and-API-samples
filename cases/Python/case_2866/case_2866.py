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
import time
import shutil
import psutil
import pyautogui
import pygetwindow as gw


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


class CaseReturnCode(IntEnum):
    SUCCESS = 0
    DRIVER_NOT_FOUND = 1
    Driver_Not_Signed = 2


# 1. Set log paths
log_path = r"C:\Users\Public\Documents\SIGVERIF.TXT"
print("log path:", log_path)


# 2. Run SigVerif.exe
subprocess.Popen(["sigverif.exe"])
print("[*] Starting SigVerif...")
time.sleep(1)  # Give it a few seconds to open


# 3. Simulate pressing "TAB" and "ENTER" to start the scan
print("[*] Trying to start scan automatically...")

# Move mouse to center of screen
pyautogui.moveTo(1000, 500)  # Example coordinates, you may need to adjust

# Press "ENTER" to click Start
pyautogui.press('enter')
print("[*] Scan started... waiting for it to complete.")

# 4. Wait for log file to appear
time.sleep(40)

# Wait for "Signature Verification Results" window to appear
# print("[*] Waiting for results window to appear...")
# start_time = time.time()
# timeout = 120  # Max wait in seconds
#
# while True:
#     result_windows = [w for w in gw.getWindowsWithTitle('Signature Verification Results') if w.visible]
#     if result_windows:
#         print("[*] Results window detected.")
#         break
#     if time.time() - start_time > timeout:
#         raise TimeoutError("Timed out waiting for results window.")
#     time.sleep(1)

# Give it a moment before pressing enter
time.sleep(1)
print("[*] SigVerif scan completed.")

# 5. Close "Signature Verification Results" window
pyautogui.press('enter')
time.sleep(2)
# print("[*] Close 'Signature Verification Results' window.")


# 6. Press on "Close": tab, enter - it will close sigverif window
pyautogui.press('tab')
time.sleep(3)
pyautogui.press('enter')
print("[*] sigverif was closed")


# 7. Read the log file
def check_log():
    with open(log_path, "r", encoding="utf-16", errors="ignore") as file:
        content = file.readlines()

    print(f"[*] Loaded {len(content)} lines from {log_path}")

    # 8. Search for "kayakern.sys" and check its status
    target_driver = "kayakern.sys"
    kayakern_status = None

    found_anything = False

    for idx, line in enumerate(content):
        if "kayakern.sys" in line.lower():
            found_anything = True
            print(f"Line #{idx}: {repr(line)}")  # Using repr to show hidden characters!

    if not found_anything:
        print("[!] kayakern was not found even partially in any line.")
        return CaseReturnCode.DRIVER_NOT_FOUND

    for idx, line in enumerate(content):
        if target_driver.lower() in line.lower():
            fields = [field for field in line.split() if field.strip()]
            if len(fields) >= 4:
                kayakern_status = fields[3]  # Field 4 is Status
            else:
                kayakern_status = "Parsing error"
            break

    # 9. Print the status
    if kayakern_status:
        print(f"[*] {target_driver} status: {kayakern_status}")
        if kayakern_status.lower() == "signed":
            print("[+] Verification PASSED: Driver is signed.")
            return CaseReturnCode.SUCCESS
        else:
            print("[!] Verification FAILED: Driver is NOT signed!")
            return CaseReturnCode.Driver_Not_Signed
    else:
        print(f"[!] {target_driver} was not found in the log.")


# The flow starts here
if __name__ == "__main__":
    try:
        print("case 2866 Process ID:", os.getpid())
        return_code = check_log()
        print(f'Case return code: {return_code}')
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {str(ex)}")
        exit(-200)
    exit(return_code)

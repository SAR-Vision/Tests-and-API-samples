import os
import sys
import glob
import time
import shutil

# Same initialization as case_2990
os.environ["WithAdapter"] = "1"
sys.path.insert(0, os.environ["KAYA_VISION_POINT_PYTHON_PATH"])

from KYFGLib import *


DEVICE_INDEX = 0

INCORRECT_JSON = "incorrect_KYHWLib_0x410_emb.json"
ACTIVE_JSON = "KYHWLib_0x410_emb.json"

LOG_PATH = r"C:\ProgramData\KAYA Instruments\Logs"
JSON_PATH = os.environ["KAYA_VISION_POINT_CONF"]


def get_service_log_offsets():
    offsets = {}

    pattern = os.path.join(LOG_PATH, "KAYA_Service*.log")

    for filename in glob.glob(pattern):
        try:
            offsets[filename] = os.path.getsize(filename)
            print(
                f"Log snapshot: {os.path.basename(filename)} "
                f"at byte {offsets[filename]}"
            )
        except OSError as ex:
            print(f"Cannot get size of {filename}: {ex}")

    return offsets


def print_new_service_log(offsets):
    print()
    print("=" * 100)
    print("NEW KAYA_Service LOG CONTENT")
    print("=" * 100)

    pattern = os.path.join(LOG_PATH, "KAYA_Service*.log")
    found_new_data = False

    for filename in glob.glob(pattern):

        # A newly-created service log must be read from the beginning.
        start_offset = offsets.get(filename, 0)

        try:
            current_size = os.path.getsize(filename)

            # Handle log truncation/rotation.
            if current_size < start_offset:
                start_offset = 0

            with open(filename, "rb") as f:
                f.seek(start_offset)
                data = f.read()

        except OSError as ex:
            print(f"Cannot read {filename}: {ex}")
            continue

        if not data:
            continue

        found_new_data = True

        print()
        print(f"FILE: {filename}")
        print(f"FROM BYTE: {start_offset}")
        print(f"TO BYTE:   {current_size}")
        print("-" * 100)

        text = data.decode("utf-8", errors="ignore")
        print(text, end="" if text.endswith("\n") else "\n")

    if not found_new_data:
        print("NO NEW KAYA_Service LOG CONTENT FOUND")

    print("=" * 100)


def main():

    script_path = os.path.dirname(os.path.abspath(__file__))

    incorrect_source = os.path.join(
        script_path,
        INCORRECT_JSON
    )

    active_json = os.path.join(
        JSON_PATH,
        ACTIVE_JSON
    )

    print("Incorrect JSON source:")
    print(incorrect_source)

    print()
    print("Active JSON destination:")
    print(active_json)

    # ------------------------------------------------------------
    # 1. Snapshot Service logs BEFORE replacing JSON
    # ------------------------------------------------------------
    print()
    print("Taking KAYA_Service log snapshot...")

    offsets = get_service_log_offsets()

    # ------------------------------------------------------------
    # 2. Replace active JSON with incorrect JSON
    # ------------------------------------------------------------
    print()
    print("Copying incorrect JSON...")

    shutil.copy2(
        incorrect_source,
        active_json
    )

    print("Incorrect JSON installed.")

    # ------------------------------------------------------------
    # 3. Open grabber
    # ------------------------------------------------------------
    print()
    print("Calling KYFG_Open...")

    grabberHandle = None

    try:
        (grabberHandle,) = KYFG_Open(DEVICE_INDEX)

        print(
            f"KYFG_Open succeeded. "
            f"grabberHandle = {grabberHandle}"
        )

    except Exception as ex:
        print(f"KYFG_Open raised exception: {ex}")

    # Give asynchronous Service logging a moment to flush.
    time.sleep(1)

    # ------------------------------------------------------------
    # 4. Close grabber
    # ------------------------------------------------------------
    if grabberHandle is not None:

        print()
        print("Calling KYFG_Close...")

        try:
            (status,) = KYFG_Close(grabberHandle)
            print(f"KYFG_Close status = {status}")

        except Exception as ex:
            print(f"KYFG_Close raised exception: {ex}")

    # Give Service logger time to finish writing.
    time.sleep(2)

    # ------------------------------------------------------------
    # 5. Print EVERYTHING added to Service logs
    # ------------------------------------------------------------
    print_new_service_log(offsets)


if __name__ == "__main__":
    main()

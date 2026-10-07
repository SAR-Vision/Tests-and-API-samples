# probe_incorrect_json.py — standalone, isolated test of the incorrect-JSON phase only
import sys, os, shutil, platform, time
os.environ["WithAdapter"] = "1"
sys.path.insert(0, os.environ['KAYA_VISION_POINT_PYTHON_PATH'])
from KYFGLib import *

current_folder_path = os.path.dirname(__file__)
system_platform = platform.system().lower()
JSON_FILE_PATH = os.environ['KAYA_VISION_POINT_CONF'] if system_platform == 'windows' else os.environ['KAYA_VISION_POINT_LIB_PATH']
LOG_PATH_ENV = "KAYA_VISION_POINT_LOGS" if not os.environ.get("WithAdapter", "") else "KAYA_VISION_POINT_2_LOGS"
LOG_FILE_PATH = os.environ[LOG_PATH_ENV]

emb_json_incorrect = 'incorrect_KYHWLib_0x410_emb.json'
incorrect_json_dst = os.path.join(JSON_FILE_PATH, emb_json_incorrect.replace('incorrect_', ''))
shutil.copy(os.path.join(current_folder_path, emb_json_incorrect), incorrect_json_dst)
print("Copied incorrect JSON to:", incorrect_json_dst)

def get_log_offsets(path):
    offsets = {}
    for f in os.listdir(path):
        p = os.path.join(path, f)
        if os.path.isfile(p) and f.startswith("KAYA_") and f.endswith(".log"):
            offsets[p] = os.path.getsize(p)
    return offsets

def check_log(path, needle, since_offsets):
    for f in os.listdir(path):
        p = os.path.join(path, f)
        if os.path.isfile(p) and f.startswith("KAYA_") and f.endswith(".log"):
            with open(p, "r", encoding="utf-8", errors="ignore") as log:
                log.seek(since_offsets.get(p, 0))
                if needle in log.read():
                    print("Found in:", f)
                    return True
    return False

offsets_before = get_log_offsets(LOG_FILE_PATH)

open_exception = None
try:
    (status, n) = KY_DeviceScan()
    (grabberHandle,) = KYFG_Open(0)
    KYFG_Close(grabberHandle)
    print("KYFG_Open/Close succeeded with incorrect JSON in place")
except Exception as ex:
    open_exception = ex
    print("KYFG_Open raised:", ex)

time.sleep(5)
print("HW_INVALID_JSON_FILE in logs:", check_log(LOG_FILE_PATH, 'HW_INVALID_JSON_FILE', offsets_before))
print("KYFG_Open raised an exception:", open_exception is not None)

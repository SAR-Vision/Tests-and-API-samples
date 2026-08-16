import argparse
import json
import os
import pathlib
import platform
import shlex
import shutil
import subprocess
import sys
from enum import IntEnum

import pytest


# AUTO-GENERATED BINARY CASE WRAPPER TEMPLATE
# CASENUMBER and AUTOMATION_FILE are replaced by auto_test_runner.py.

python_ver = 'python' if "Windows" in platform.platform() else 'python3'


class CaseReturnCode(IntEnum):
    SUCCESS = 0
    COULD_NOT_RUN = 1
    NO_HW_FOUND = 2
    ASSERT_ERROR = 3
    WRONG_PARAM_VALUE = 4
    FAILED_WITH_EXCEPTION = 5


def get_json_result_file_and_data():
    case_folder = pathlib.Path(__file__).resolve().parent
    json_param_results_file = case_folder / "case_CASENUMBER_param_results.json"
    with json_param_results_file.open('r') as jrf:
        return json_param_results_file, json.load(jrf)


def save_result_to_json(parametrize, result, output):
    json_param_results_file, json_param_results_data = get_json_result_file_and_data()
    if parametrize not in json_param_results_data:
        json_param_results_data[parametrize] = {"status": "Pending", "last output": ""}
    json_param_results_data[parametrize]["status"] = result
    json_param_results_data[parametrize]["last output"] = output
    with json_param_results_file.open('w') as jrf:
        json.dump(json_param_results_data, jrf, indent=4)


def parse_pytest_parametrize(parametrize_str):
    """Return (test_should_fail, native_tokens) while preserving quoted values."""
    tokens = shlex.split(parametrize_str)
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument(
        '--test_should_fail',
        default=False,
        type=lambda x: str(x).lower() in ('true', 'yes', '1'),
    )
    parsed, native_tokens = parser.parse_known_args(tokens)
    return parsed.test_should_fail, native_tokens


def copy_log(parameters, pid):
    """
    Preserve the old per-param log collection when the generic Python binary
    runner creates a KAYA log. Native executables may create their own process
    logs, so absence of a matching Python log is not treated as test failure.
    """
    logs_root = os.environ.get('KAYA_VISION_POINT_LOGS')
    if not logs_root:
        return None

    variable_path = pathlib.Path(logs_root)
    if not variable_path.exists():
        return None

    log_folder = pathlib.Path.cwd() / pathlib.Path(__file__).name.replace('.py', '.log')
    parameters_name_folder = parameters
    for char in r"\/:*?'<>;:|":
        parameters_name_folder = parameters_name_folder.replace(char, '')
    parameters_name_folder = parameters_name_folder[:255]

    parameters_logs_path = log_folder / parameters_name_folder
    parameters_logs_path.mkdir(parents=True, exist_ok=True)

    for next_log in variable_path.iterdir():
        if 'python' in next_log.name.lower() and str(pid) in next_log.name:
            target = parameters_logs_path / next_log.name
            shutil.copy(next_log, target)
            return target
    return None


def result_from_process_return_code(process_return_code):
    try:
        return CaseReturnCode(process_return_code)
    except ValueError:
        return CaseReturnCode.FAILED_WITH_EXCEPTION


def test_case(pytest_parametrize, request):
    test_should_fail, native_tokens = parse_pytest_parametrize(pytest_parametrize)

    device_index = request.config.getoption('--deviceIndex')
    project_root = pathlib.Path(__file__).resolve().parents[2]
    binary_runner = project_root / 'binary_test_executables' / 'binary_case_runner.py'

    if not binary_runner.is_file():
        pytest.fail(f'Binary case runner not found: {binary_runner}')

    command = [
        python_ver,
        str(binary_runner),
        '--automation',
        'AUTOMATION_FILE',
        '--unattended',
        '--deviceIndex',
        str(device_index),
    ]
    command.extend(native_tokens)

    print('\nBinary case command:')
    print(subprocess.list2cmdline(command))

    process = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )
    pid = process.pid
    print('\nProcess ID:', pid, '\n')
    stdout, stderr = process.communicate()

    stdout = stdout or ''
    stderr = stderr or ''
    full_output = stdout.strip()
    if stderr.strip():
        full_output = (full_output + '\n' + stderr.strip()).strip()

    return_code = result_from_process_return_code(process.returncode)

    case_log_file = copy_log(pytest_parametrize, pid)
    error_count = 0
    if case_log_file is not None and case_log_file.exists():
        with case_log_file.open('r', errors='ignore') as lf:
            for next_line in lf:
                if 'error' in next_line.lower():
                    error_count += 1

    full_output = f'ERROR COUNT FROM LOG FILE = {error_count}\n' + full_output

    if return_code == CaseReturnCode.NO_HW_FOUND:
        print(full_output)
        save_result_to_json(pytest_parametrize, "Didn't run", full_output)
        pytest.skip('No hardware found')

    if return_code == CaseReturnCode.COULD_NOT_RUN and 'AssertionError' not in full_output:
        print(full_output)
        save_result_to_json(pytest_parametrize, 'CouldNotRun', full_output)
        pytest.xfail('CouldNotRun')

    print(full_output)

    test_failed = return_code != CaseReturnCode.SUCCESS

    if test_should_fail != test_failed:
        save_result_to_json(pytest_parametrize, 'Failed', full_output)

    assert test_should_fail == test_failed, (
        f'Test expected to fail: {"Yes" if test_should_fail else "No"}, '
        f'actually failed: {"Yes" if test_failed else "No"}'
    )

    if test_should_fail and test_failed:
        print('Test case expectedly failed')
        save_result_to_json(pytest_parametrize, 'Passed', full_output)
        return

    if return_code != CaseReturnCode.SUCCESS:
        save_result_to_json(pytest_parametrize, 'Failed', full_output)
        pytest.fail(f'Binary case returned {int(return_code)}')

    save_result_to_json(pytest_parametrize, 'Passed', full_output)

import argparse
import os
import pathlib
import platform
import re
import sys

import pytest
from py.xml import html


# AUTO-GENERATED BINARY CASE CONFTEST TEMPLATE
# CASENUMBER is replaced by auto_test_runner.py.

sys.path.insert(1, os.environ['KAYA_VISION_POINT_PYTHON_PATH'])
from KYFGLib import *

square_open = re.escape('[')
square_close = re.escape(']')


def pytest_addoption(parser):
    # Binary-specific parameters stay inside --pytest_parametrize and are
    # forwarded unchanged to binary_case_runner.py. Pytest only needs the
    # common runner parameters here.
    parser.addoption('--unattended', action='store', default=False)
    parser.addoption('--deviceIndex', type=int, action='store', default=-1)
    parser.addoption('--pytest_parametrize', action='append', default=[], help='pytest_parametrize')


def pytest_generate_tests(metafunc):
    metafunc.parametrize('pytest_parametrize', metafunc.config.getoption('pytest_parametrize'))


def pytest_html_report_title(report):
    report.title = 'Case CASENUMBER report'


@pytest.hookimpl(optionalhook=True)
def pytest_html_results_table_header(cells):
    cells[1] = html.th('Test parameters')
    del cells[-1]


@pytest.hookimpl(optionalhook=True)
def pytest_html_results_table_row(report, cells):
    match = re.search('%s(.*)%s' % (square_open, square_close), report.head_line)
    cells[1] = html.td(match.group(1) if match else report.head_line)
    del cells[-1]


@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    report = outcome.get_result()
    report.description = str(item.function.__doc__)


def pytest_configure(config):
    host_name = platform.uname()[1]
    (_, software_info) = KY_GetSoftwareVersion()
    vp_version = f'{software_info.Major}.{software_info.Minor}.{software_info.SubMinor}'

    (_, device_count) = KY_DeviceScan()
    installed_frame_grabbers = []
    device_index = config.getoption('--deviceIndex')

    for index in range(device_count):
        (_, device_info) = KY_DeviceInfo(index)
        installed_frame_grabbers.append(device_info.szDeviceDisplayName)

    if 0 <= device_index < len(installed_frame_grabbers):
        actual_frame_grabber = f'[{device_index}] {installed_frame_grabbers[device_index]}'
    else:
        actual_frame_grabber = f'Device index {device_index} not found'

    config._metadata = {
        'Name of Machine': host_name,
        'VP version': vp_version,
        'Hardware Environment': ', '.join(installed_frame_grabbers),
        'Actual frame grabber': actual_frame_grabber,
        'Automation': 'AUTOMATION_FILE',
        'Execution backend': 'binary_case_runner.py',
    }

"""Hardware-free regression checks for camera ownership and sparse CXP links."""
import ast
import contextlib
import io
from pathlib import Path
from types import SimpleNamespace
import unittest
from unittest.mock import Mock, patch


# Load case definitions without importing the vendor DLL or changing sys.path.
source = Path(__file__).with_name('case_3049.py')
tree = ast.parse(source.read_text())
tree.body = [node for node in tree.body
             if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef))
             or isinstance(node, ast.Import)
             or isinstance(node, ast.ImportFrom) and node.module != 'KYFGLib']
case = SimpleNamespace()
exec(compile(tree, str(source), 'exec'), case.__dict__)
case.__dict__.update(__file__=str(source), FGSTATUS_OK=0x3000)


class CameraSelectionTests(unittest.TestCase):
    def run_case(self, links, camera_index=0, master_link=None, open_status=0x3000,
                 counter=5, worker_result=0):
        status = case.FGSTATUS_OK
        setters = Mock(return_value=(status,))
        reset = Mock()
        reset_grabber = Mock()
        camera_close = Mock(return_value=(status,))
        grabber_close = Mock(return_value=(status,))
        def run_worker(*args):
            # A child must never scan while the coordinator owns the grabber.
            camera_close.assert_called_once()
            grabber_close.assert_called_once_with(99)
            return worker_result
        workers = Mock(side_effect=run_worker)
        camera_open = Mock(return_value=(open_status,))
        api = dict(
            KY_DEVICE_PROTOCOL=SimpleNamespace(KY_DEVICE_PROTOCOL_CoaXPress=1),
            KY_DeviceScan=lambda: (status, 1),
            KY_DeviceInfo=lambda index: (status, SimpleNamespace(
                szDeviceDisplayName='mock grabber', m_Protocol=1)),
            KYFG_Open=lambda index: (99,),
            KYFG_UpdateCameraList=lambda handle: (status, list(range(len(links)))),
            KYFG_CameraInfo2=lambda handle: (status, SimpleNamespace(master_link=links[handle])),
            KYFG_CameraOpen2=camera_open, KYFG_CameraClose=camera_close,
            KYFG_Close=grabber_close, Reset_camera=reset, Reset_grabber=reset_grabber,
            KYFG_GetGrabberValueInt=lambda *args: (status, counter),
            run_camera_instance=workers)
        for name in ('KYFG_SetGrabberValueInt', 'KYFG_SetGrabberValueEnum',
                     'KYFG_SetGrabberValueFloat', 'KYFG_SetGrabberValueEnum_ByValueName',
                     'KYFG_SetCameraValueEnum', 'KYFG_SetCameraValueEnum_ByValueName',
                     'KYFG_GrabberExecuteCommand'):
            api[name] = setters
        args = dict(unattended=True, deviceList=False, deviceIndex=0,
                    cameraIndex=camera_index, cameraMasterLink=master_link,
                    number_of_sent_triggers=5)
        with patch.dict(case.__dict__, api), patch.object(case.time, 'sleep'), \
                contextlib.redirect_stdout(io.StringIO()):
            try:
                result = case.CaseRun(args)
            finally:
                grabber_close.assert_called_once_with(99)
                if camera_open.called and open_status == status:
                    camera_close.assert_called_once()
        return result, setters, reset_grabber, camera_open, camera_close, workers

    def test_sparse_worker_uses_local_slot_and_physical_link(self):
        result, setters, reset, opened, closed, _ = self.run_case(
            [0, 2], camera_index=1, master_link=2)
        self.assertEqual(result, 0)
        opened.assert_called_once_with(1, None)
        closed.assert_called_once_with(1)
        reset.assert_not_called()
        setters.assert_any_call(99, 'CameraSelector', 1)
        setters.assert_any_call(99, 'CxpConnectionSelector', 2)
        setters.assert_any_call(99, 'TimerSelector', 2)
        setters.assert_any_call(99, 'CameraTriggerSource', 'KY_TIMER_ACTIVE_2')

    def test_worker_handles_list_compacted_by_other_processes(self):
        result, setters, _, opened, _, workers = self.run_case(
            [2], camera_index=2, master_link=2)
        self.assertEqual(result, 0)
        opened.assert_called_once_with(0, None)
        setters.assert_any_call(99, 'CameraSelector', 0)
        workers.assert_not_called()

    def test_coordinator_assigns_all_sparse_links(self):
        result, _, reset, _, _, workers = self.run_case([0, 1, 3])
        self.assertEqual(result, 0)
        reset.assert_called_once_with(99)
        self.assertEqual(workers.call_count, 2)
        workers.assert_any_call(0, 1, 5, 1)
        workers.assert_any_call(0, 2, 5, 3)

    def test_empty_discovery_closes_grabber(self):
        result, _, _, opened, closed, _ = self.run_case([], master_link=2)
        self.assertEqual(result, 2)
        opened.assert_not_called()
        closed.assert_not_called()

    def test_four_cameras_are_all_tested(self):
        result, _, _, _, _, workers = self.run_case([0, 1, 2, 3])
        self.assertEqual(result, 0)
        self.assertEqual(workers.call_count, 3)
        workers.assert_any_call(0, 3, 5, 3)

    def test_child_failure_is_reported(self):
        with self.assertRaisesRegex(AssertionError, 'Errors while test'):
            self.run_case([0, 2], worker_result=2)

    def test_missing_assigned_link_does_not_open_another_camera(self):
        result, _, _, opened, closed, _ = self.run_case([1], master_link=2)
        self.assertEqual(result, 2)
        opened.assert_not_called()
        closed.assert_not_called()

    def test_invalid_camera_index_closes_grabber(self):
        result, _, _, opened, _, _ = self.run_case([0], camera_index=3)
        self.assertEqual(result, 4)
        opened.assert_not_called()

    def test_open_failure_closes_grabber(self):
        with self.assertRaisesRegex(RuntimeError, 'Camera open failed'):
            self.run_case([0], open_status=0x3003)

    def test_counter_failure_still_closes_grabber(self):
        with self.assertRaisesRegex(AssertionError, 'TriggerSentCount'):
            self.run_case([0], counter=4)


if __name__ == '__main__':
    unittest.main()

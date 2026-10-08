"""
Generic builder/caller for QA native test cases.

Place this file directly in:
    <QA project>/binary_test_executables/binary_case_runner.py

Expected layout:
    binary_test_executables/
        binary_case_runner.py
        case_1234/
            case_1234.cpp          # or .c / .csproj
            case_1234.vcxproj      # Windows C/C++ build, when applicable
            Makefile               # Linux C/C++ build

Typical use:
    python binary_case_runner.py --automation case_5290.cpp --deviceIndex 0 --number_of_tests 100000
    python binary_case_runner.py --automation case_4424.csproj --deviceIndex 0

Arguments unknown to this script are forwarded to the built test executable.
"""

import argparse
import json
import os
import pathlib
import platform as platform_module
import re
import shutil
import subprocess
import sys
import tempfile
from typing import Iterable, Optional
from enum import IntEnum


SUPPORTED_AUTOMATION_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".csproj", ".vcxproj"}
SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".cs"}


class CaseReturnCode(IntEnum):
    SUCCESS = 0
    COULD_NOT_RUN = 1
    NO_HW_FOUND = 2
    ASSERT_ERROR = 3
    WRONG_PARAM_VALUE = 4
    FAILED_WITH_EXCEPTION = 5


class NoHardwareError(RuntimeError):
    pass


class CouldNotRunError(RuntimeError):
    pass



def parse_args():
    parser = argparse.ArgumentParser(
        description="Build and run a C/C++/C# QA case from binary_test_executables."
    )

    # The runner can use either form. --automation is preferred for integration.
    parser.add_argument("automation_positional", nargs="?", help=argparse.SUPPRESS)
    parser.add_argument(
        "--automation",
        dest="automation_option",
        help="Automation file from Redmine, e.g. case_5290.cpp or case_4424.csproj",
    )

    # Common QA arguments are handled here to preserve the behavior of the old
    # per-case Python wrappers. They are then forwarded to the native test.
    parser.add_argument("--unattended", default=False, action="store_true")
    parser.add_argument("--no-unattended", dest="unattended", action="store_false")
    parser.add_argument("--deviceList", default=False, action="store_true")
    parser.add_argument("--deviceIndex", "--device_index", dest="deviceIndex", type=int, default=-1)

    # Generic builder options. These are intentionally lowercase so they are
    # clearly wrapper options, not camera/test parameters.
    parser.add_argument("--configuration", default="Release")
    parser.add_argument("--buildPlatform", default="x64")
    parser.add_argument("--buildTimeout", type=float, default=600.0)
    parser.add_argument("--runTimeout", type=float, default=180.0)
    parser.add_argument("--noBuild", action="store_true")

    args, passthrough = parser.parse_known_args()

    automation = args.automation_option or args.automation_positional
    if not automation:
        parser.error("missing automation file; use --automation case_XXXX.cpp")

    if args.automation_option and args.automation_positional:
        if pathlib.Path(args.automation_option).name != pathlib.Path(args.automation_positional).name:
            parser.error("automation was specified twice with different values")

    args.automation = automation
    return args, passthrough


def load_kyfglib():
    """Load KYFGLib only for common device selection/listing behavior."""
    python_path = os.environ.get("KAYA_VISION_POINT_PYTHON_PATH")
    if not python_path:
        raise EnvironmentError("KAYA_VISION_POINT_PYTHON_PATH is not defined")

    os.environ["WithAdapter"] = "1"
    if python_path not in sys.path:
        sys.path.insert(0, python_path)

    import KYFGLib  # pylint: disable=import-outside-toplevel

    return KYFGLib


def resolve_device_index(args) -> Optional[int]:
    """
    Preserve the common KAYA wrapper behavior without adding case-specific
    protocol checks. Returns None when --deviceList was requested.
    """
    ky = load_kyfglib()

    status, device_count = ky.KY_DeviceScan()
    device_infos = {}

    for index in range(device_count):
        status, device_infos[index] = ky.KY_DeviceInfo(index)
        print(f'Found device [{index}]: "{device_infos[index].szDeviceDisplayName}"')

    if args.deviceList:
        return None

    if device_count <= 0:
        raise NoHardwareError("No frame grabber devices were found")

    device_index = args.deviceIndex
    if device_index < 0:
        if args.unattended:
            device_index = 0
            print(f"\n!!! deviceIndex {device_index} forcibly selected in unattended mode !!!")
        else:
            device_index = int(input(f"Select PCI device to use (0 ... {device_count - 1})"))
            print(f"\ndeviceIndex {device_index} selected")

    if device_index < 0 or device_index >= device_count:
        raise NoHardwareError(
            f"Device with index {device_index} does not exist; valid range is 0 ... {device_count - 1}"
        )

    return device_index


def normalize_automation_name(automation: str) -> pathlib.Path:
    target = pathlib.Path(automation)
    suffix = target.suffix.lower()
    if suffix not in SUPPORTED_AUTOMATION_SUFFIXES:
        raise ValueError(
            f"Unsupported Automation extension '{target.suffix}'. "
            f"Supported: {', '.join(sorted(SUPPORTED_AUTOMATION_SUFFIXES))}"
        )
    return target


def _usable_path(path: pathlib.Path) -> bool:
    lowered = {part.lower() for part in path.parts}
    return "obj" not in lowered and ".git" not in lowered


def locate_case_folder(binary_root: pathlib.Path, automation: pathlib.Path) -> tuple[pathlib.Path, pathlib.Path]:
    """
    Prefer the normal layout binary_test_executables/case_XXXX/.

    Legacy fallback: if the file is not in the expected folder, search the
    immediate case tree for the exact Automation filename. This covers old
    layouts such as a project whose folder name does not match its case number.
    """
    stem = automation.stem
    expected_folder = binary_root / stem

    if expected_folder.is_dir():
        exact = expected_folder / automation.name
        if exact.exists():
            return expected_folder, exact

        # For .c/.cpp Automation, the source might have a slightly different
        # relative location inside the case directory. Search only this case.
        matches = [
            p for p in expected_folder.rglob(automation.name)
            if p.is_file() and _usable_path(p)
        ]
        if len(matches) == 1:
            return expected_folder, matches[0]

        # The Automation filename may represent the case even when the source
        # itself is not needed by the build command. Keep the conventional case folder.
        if automation.suffix.lower() in {".c", ".cc", ".cpp", ".cxx"}:
            return expected_folder, exact

    matches = [
        p for p in binary_root.rglob(automation.name)
        if p.is_file() and _usable_path(p)
    ]

    if len(matches) == 1:
        return matches[0].parent, matches[0]

    if not matches:
        raise FileNotFoundError(
            f"Could not find case folder/file for Automation '{automation.name}' under {binary_root}"
        )

    formatted = "\n".join(f"  - {p}" for p in matches)
    raise RuntimeError(
        f"Automation '{automation.name}' is ambiguous; found multiple matches:\n{formatted}"
    )


def find_msbuild() -> pathlib.Path:
    """Reuse the project configuration across separate binary-case processes."""
    config_path = pathlib.Path(__file__).resolve().parents[2] / "qa_config.json"
    config = {}
    if config_path.is_file():
        with config_path.open(encoding="utf-8-sig") as config_file:
            config = json.load(config_file)
        if not isinstance(config, dict):
            raise ValueError(f"Expected a JSON object in {config_path}")

    # Manually created configs may omit this key or explicitly set it to null.
    configured_path = config.get("msbuild_path") or ""
    if isinstance(configured_path, str) and configured_path.strip():
        msbuild = pathlib.Path(configured_path)
        if not msbuild.is_absolute():
            msbuild = config_path.parent / msbuild
        if msbuild.is_file():
            msbuild = msbuild.resolve()
            os.environ["MSBUILD_EXE_PATH"] = str(msbuild)
            print(f"MSBuild search skipped: using cached path from {config_path}: {msbuild}")
            return msbuild
        print(f"Configured MSBuild no longer exists: {msbuild}; searching again")

    print("MSBuild discovery started: no valid cached msbuild_path in qa_config.json")
    msbuild = discover_msbuild().resolve()
    if not msbuild.is_file():
        raise FileNotFoundError(f"Discovered MSBuild does not exist: {msbuild}")
    config["msbuild_path"] = str(msbuild)

    # Replace atomically so subsequent runners never read a partially written JSON.
    temporary_path = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="w", encoding="utf-8", dir=config_path.parent,
            prefix="qa_config_", suffix=".tmp", delete=False,
        ) as config_file:
            temporary_path = pathlib.Path(config_file.name)
            json.dump(config, config_file, indent=4, ensure_ascii=False)
            config_file.write("\n")
        os.replace(temporary_path, config_path)
    finally:
        if temporary_path is not None:
            temporary_path.unlink(missing_ok=True)

    os.environ["MSBUILD_EXE_PATH"] = str(msbuild)
    print(f"Saved MSBuild path to {config_path}: {msbuild}")
    return msbuild


def discover_msbuild() -> pathlib.Path:
    """Find MSBuild without hardcoding a Visual Studio version."""
    env_path = os.environ.get("MSBUILD_EXE_PATH")
    if env_path and pathlib.Path(env_path).is_file():
        print(f"MSBuild found in MSBUILD_EXE_PATH: {env_path}; PATH/vswhere search skipped")
        return pathlib.Path(env_path)

    print("Searching for MSBuild in PATH and Visual Studio installations...")
    for name in ("MSBuild.exe", "msbuild.exe", "msbuild"):
        found = shutil.which(name)
        if found:
            return pathlib.Path(found)

    program_files_x86 = os.environ.get("ProgramFiles(x86)")
    if program_files_x86:
        vswhere = pathlib.Path(program_files_x86) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
        if vswhere.is_file():
            result = subprocess.run(
                [
                    str(vswhere),
                    "-latest",
                    "-products",
                    "*",
                    "-requires",
                    "Microsoft.Component.MSBuild",
                    "-find",
                    r"MSBuild\**\Bin\MSBuild.exe",
                ],
                capture_output=True,
                text=True,
                check=False,
            )
            candidates = [line.strip() for line in result.stdout.splitlines() if line.strip()]
            if candidates:
                return pathlib.Path(candidates[0])

    raise FileNotFoundError(
        "MSBuild was not found. Install Visual Studio Build Tools or make MSBuild.exe available in PATH."
    )


def find_windows_project(case_folder: pathlib.Path, automation_path: pathlib.Path) -> pathlib.Path:
    suffix = automation_path.suffix.lower()

    if suffix in {".csproj", ".vcxproj"} and automation_path.is_file():
        return automation_path

    stem = automation_path.stem
    preferred = [
        case_folder / f"{stem}.vcxproj",
        case_folder / f"{stem}.csproj",
        case_folder / f"{stem}.sln",
    ]
    for candidate in preferred:
        if candidate.is_file():
            return candidate

    projects = []
    for pattern in ("*.vcxproj", "*.csproj", "*.sln"):
        projects.extend(
            p for p in case_folder.rglob(pattern)
            if p.is_file() and _usable_path(p)
        )

    projects = list(dict.fromkeys(projects))
    if len(projects) == 1:
        return projects[0]

    if not projects:
        raise FileNotFoundError(
            f"No .vcxproj/.csproj/.sln was found in {case_folder} for {automation_path.name}"
        )

    formatted = "\n".join(f"  - {p}" for p in projects)
    raise RuntimeError(
        f"Multiple Windows build projects were found for {automation_path.name}; "
        f"cannot choose automatically:\n{formatted}"
    )


def run_process(command: list[str], cwd: pathlib.Path, timeout: float, title: str) -> int:
    print(f"\n{'=' * 30} {title} {'=' * 30}")
    print("Working directory:", cwd)
    print("Command:", subprocess.list2cmdline([str(x) for x in command]))

    process = subprocess.Popen(
        [str(x) for x in command],
        cwd=str(cwd),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        encoding="utf-8",
        errors="replace",
    )

    try:
        assert process.stdout is not None
        for line in process.stdout:
            print(line, end="")
        return_code = process.wait(timeout=timeout)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait()
        raise TimeoutError(f"{title} timed out after {timeout} seconds")

    print(f"\n{title} return code: {return_code}")
    return return_code


def build_windows(
    case_folder: pathlib.Path,
    automation_path: pathlib.Path,
    configuration: str,
    build_platform: str,
    timeout: float,
):
    project = find_windows_project(case_folder, automation_path)
    msbuild = find_msbuild()

    print("MSBuild:", msbuild)
    print("Project:", project)

    command = [
        str(msbuild),
        str(project),
        f"/p:Configuration={configuration}",
        f"/p:Platform={build_platform}",
        "/t:Rebuild",
        "/m",
    ]

    return_code = run_process(command, case_folder, timeout, "WINDOWS BUILD")
    if return_code != 0:
        raise RuntimeError(f"MSBuild failed with code {return_code}")


def find_case_makefile(case_folder: pathlib.Path) -> pathlib.Path:
    """The Makefile is per case folder; there is intentionally no top-level Makefile."""
    for name in ("Makefile", "makefile", "GNUmakefile"):
        candidate = case_folder / name
        if candidate.is_file():
            return candidate

    raise FileNotFoundError(
        f"No Makefile found inside case folder: {case_folder}. "
        "Linux builds are expected to use one Makefile per case_N folder."
    )


def build_linux(case_folder: pathlib.Path, automation_path: pathlib.Path, configuration: str, timeout: float):
    suffix = automation_path.suffix.lower()

    if suffix == ".csproj":
        dotnet = shutil.which("dotnet")
        if not dotnet:
            raise FileNotFoundError("dotnet was not found for .csproj build on Linux")
        command = [dotnet, "build", str(automation_path), "-c", configuration]
        return_code = run_process(command, case_folder, timeout, "DOTNET BUILD")
        if return_code != 0:
            raise RuntimeError(f"dotnet build failed with code {return_code}")
        return

    # C/C++ Linux build: always use the Makefile in this specific case folder.
    makefile = find_case_makefile(case_folder)
    print("Case Makefile:", makefile)

    make = shutil.which("make") or "make"
    return_code = run_process([make], case_folder, timeout, "LINUX BUILD")
    if return_code != 0:
        raise RuntimeError(f"make failed with code {return_code}")


def _candidate_score(path: pathlib.Path, stem: str) -> tuple[int, float]:
    parts = [part.lower() for part in path.parts]
    name = path.name.lower()
    score = 0

    if path.stem.lower() == stem.lower():
        score += 100
    if "release" in parts:
        score += 20
    if "x64" in parts or "amd64" in parts:
        score += 10
    if "debug" in parts:
        score -= 20
    if "obj" in parts:
        score -= 100
    if name.endswith(".vshost.exe"):
        score -= 100

    try:
        mtime = path.stat().st_mtime
    except OSError:
        mtime = 0.0
    return score, mtime


def find_built_program(case_folder: pathlib.Path, binary_root: pathlib.Path, stem: str):
    if platform_module.system() == "Windows":
        candidates = [
            p for p in case_folder.rglob(f"{stem}.exe")
            if p.is_file() and _usable_path(p)
        ]
        if candidates:
            return max(candidates, key=lambda p: _candidate_score(p, stem)), None

        # Support SDK-style C# projects that produce a DLL.
        dlls = [
            p for p in case_folder.rglob(f"{stem}.dll")
            if p.is_file() and _usable_path(p)
        ]
        if dlls:
            return max(dlls, key=lambda p: _candidate_score(p, stem)), "dotnet"

        # Some legacy Visual Studio projects place output outside the case folder.
        root_candidates = [
            p for p in binary_root.rglob(f"{stem}.exe")
            if p.is_file() and _usable_path(p)
        ]
        if root_candidates:
            return max(root_candidates, key=lambda p: _candidate_score(p, stem)), None

        raise FileNotFoundError(
            f"Built executable '{stem}.exe' was not found under {case_folder} or {binary_root}"
        )

    # Linux C/C++ executable. Prefer the exact case root, then recurse.
    exact = case_folder / stem
    if exact.is_file():
        return exact, None

    candidates = [
        p for p in case_folder.rglob(stem)
        if p.is_file() and _usable_path(p) and os.access(p, os.X_OK)
    ]
    if candidates:
        return max(candidates, key=lambda p: _candidate_score(p, stem)), None

    # Linux C# output.
    dlls = [
        p for p in case_folder.rglob(f"{stem}.dll")
        if p.is_file() and _usable_path(p)
    ]
    if dlls:
        return max(dlls, key=lambda p: _candidate_score(p, stem)), "dotnet"

    root_candidates = [
        p for p in binary_root.rglob(stem)
        if p.is_file() and _usable_path(p) and os.access(p, os.X_OK)
    ]
    if root_candidates:
        return max(root_candidates, key=lambda p: _candidate_score(p, stem)), None

    raise FileNotFoundError(
        f"Built executable '{stem}' was not found under {case_folder} or {binary_root}"
    )


def detect_device_index_option(case_folder: pathlib.Path, automation_path: pathlib.Path) -> str:
    """
    Existing native cases are not fully consistent: most C/C++ wrappers forward
    --device_index, while the C# example forwards --deviceIndex. Detect the spelling
    from source when possible, with a language-based fallback.
    """
    found_camel = False
    found_snake = False

    files: Iterable[pathlib.Path]
    if automation_path.is_file() and automation_path.suffix.lower() in SOURCE_SUFFIXES:
        files = [automation_path]
    else:
        files = (
            p for p in case_folder.rglob("*")
            if p.is_file() and p.suffix.lower() in SOURCE_SUFFIXES and _usable_path(p)
        )

    for source in files:
        try:
            text = source.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        if "--deviceIndex" in text or "deviceIndex" in text:
            found_camel = True
        if "--device_index" in text or "device_index" in text:
            found_snake = True

    if found_snake and not found_camel:
        return "--device_index"
    if found_camel and not found_snake:
        return "--deviceIndex"

    if automation_path.suffix.lower() == ".csproj":
        return "--deviceIndex"
    return "--device_index"


def normalize_passthrough(passthrough: list[str], device_index: int) -> list[str]:
    """Remove device-index aliases from passthrough; the resolved value is appended once later."""
    result = []
    i = 0
    while i < len(passthrough):
        token = passthrough[i]

        if token in ("--deviceIndex", "--device_index"):
            i += 2
            continue

        if token.startswith("--deviceIndex=") or token.startswith("--device_index="):
            i += 1
            continue

        # Common Python wrapper flags are converted to the native form separately.
        if token in ("--unattended", "--no-unattended", "--deviceList"):
            i += 1
            continue

        result.append(token)
        i += 1

    return result


def configure_adapter_environment():
    """Use the same adapter selection for KYFGLib and native MSBuild projects."""
    with_adapter = os.environ.get("WithAdapter", "").strip() == "1"
    suffix = "A" if with_adapter else ""
    os.environ["KYFGLIB_ADAPTER_SUFFIX"] = suffix
    print(f"Native library: KYFGLib{suffix}_vc141.lib (WithAdapter={'1' if with_adapter else '0'})")


def run_binary_case(args, passthrough: list[str]) -> int:
    binary_root = pathlib.Path(__file__).resolve().parent
    automation = normalize_automation_name(args.automation)

    print("Binary tests root:", binary_root)
    print("Automation:", automation.name)

    case_folder, automation_path = locate_case_folder(binary_root, automation)
    stem = automation.stem

    print("Case folder:", case_folder)
    print("Automation path:", automation_path)

    device_index = resolve_device_index(args)
    if device_index is None:
        return 0

    configure_adapter_environment()

    if not args.noBuild:
        if platform_module.system() == "Windows":
            build_windows(
                case_folder,
                automation_path,
                args.configuration,
                args.buildPlatform,
                args.buildTimeout,
            )
        else:
            build_linux(
                case_folder,
                automation_path,
                args.configuration,
                args.buildTimeout,
            )
    else:
        print("Build skipped (--noBuild)")

    program, launcher = find_built_program(case_folder, binary_root, stem)
    print("Built program:", program)

    device_option = detect_device_index_option(case_folder, automation_path)
    forwarded = normalize_passthrough(passthrough, device_index)

    # Preserve old wrapper behavior: native binaries are always launched in unattended mode.
    native_args = ["--unattended", "1", device_option, str(device_index)] + forwarded

    if launcher == "dotnet":
        dotnet = shutil.which("dotnet")
        if not dotnet:
            raise FileNotFoundError("dotnet was not found to run the built C# assembly")
        command = [dotnet, str(program)] + native_args
    else:
        command = [str(program)] + native_args

    return_code = run_process(command, case_folder, args.runTimeout, "CASE RUN")

    # Preserve the native QA return code when it is one of the standard case
    # codes so the generated pytest wrapper can distinguish CouldNotRun,
    # No HW, ordinary failure, etc. Unknown non-zero codes are normalized to
    # FAILED_WITH_EXCEPTION instead of becoming Python's platform-specific -200.
    if return_code == 0:
        return int(CaseReturnCode.SUCCESS)
    if return_code in {int(code) for code in CaseReturnCode}:
        return int(return_code)

    print(f"Native case returned non-standard code {return_code}")
    return int(CaseReturnCode.FAILED_WITH_EXCEPTION)


def main():
    args, passthrough = parse_args()
    return run_binary_case(args, passthrough)


if __name__ == "__main__":
    try:
        return_code = main()
    except NoHardwareError as ex:
        print(f"No hardware: {ex}")
        return_code = int(CaseReturnCode.NO_HW_FOUND)
    except (FileNotFoundError, ValueError) as ex:
        print(f"Configuration error: {ex}")
        return_code = int(CaseReturnCode.WRONG_PARAM_VALUE)
    except TimeoutError as ex:
        print(f"Timeout: {ex}")
        return_code = int(CaseReturnCode.FAILED_WITH_EXCEPTION)
    except Exception as ex:
        print(f"Exception of type {type(ex)} occurred: {ex}")
        return_code = int(CaseReturnCode.FAILED_WITH_EXCEPTION)

    print(f"Case return code: {return_code}")
    sys.exit(return_code)

import os
import sys
import platform
import pybind11
import sysconfig

# Project names
APP_NAME = "financio"
TRADER_NAME = "tradio"

# Build
BUILD_DIR = ".build"
CXX_STANDARD = 23

# Feature flags
ENABLE_ASSERTS = True
ENABLE_DEBUG_LOGGING = True

# APP flags
PLATFORM = platform.system()

ENG_PLATFORM_MACOS = PLATFORM == "Darwin"
ENG_PLATFORM_WINDOWS = PLATFORM == "Windows"
ENG_PLATFORM_LINUX = PLATFORM == "Linux"


def get_python_info():
    py_exec = sys.executable
    py_version = f"{sys.version_info.major}.{sys.version_info.minor}"
    include_dir = sysconfig.get_path("include")
    lib_dir = sysconfig.get_config_var("LIBDIR")

    if platform.system() == "Windows":
        lib_path = os.path.join(lib_dir, f"python{py_version.replace('.', '')}.lib")
    elif platform.system() == "Darwin":
        lib_path = os.path.join(lib_dir, f"libpython{py_version}.dylib")
    else:
        lib_path = os.path.join(lib_dir, f"libpython{py_version}.so")

    pybind_dir = pybind11.get_cmake_dir()

    return {
        "exec": py_exec,
        "include": include_dir,
        "lib": lib_path,
        "version": py_version,
        "pybind": pybind_dir
    }


class CMakeArgs:
    def __init__(self, source=".", build_dir="build"):
        self.source = source
        self.build_dir = build_dir
        self._defines = {}

    def __setitem__(self, key, value):
        """Allows args['CMAKE_BUILD_TYPE'] = 'Debug'."""
        self._defines[key] = value

    def __getitem__(self, key):
        return self._defines[key]

    def __contains__(self, key):
        return key in self._defines

    def merge(self, other: "CMakeArgs"):
        """Merge another instance (right-hand overrides left)."""
        self._defines.update(other._defines)
        return self

    def as_list(self):
        """Serialize into a list usable by subprocess.run."""
        args = ["cmake", "-S", self.source, "-B", self.build_dir]
        args += [f"-D{k}={v}" for k, v in self._defines.items()]
        return args

    def __repr__(self):
        lines = [f"{k}={v}" for k, v in self._defines.items()]
        return "CMakeArgs(\n  " + "\n  ".join(lines) + "\n)"

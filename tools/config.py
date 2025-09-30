import sys
import platform

# Project names
ENGINE_NAME = "enginio"
APP_NAME = "financio"

# Build
BUILD_DIR = ".build"
CXX_STANDARD = 23

# Feature flags
ENABLE_ASSERTS = True
ENABLE_DEBUG_LOGGING = True

# APP flags
PLATFORM = platform.system()

FNC_PLATFORM_MACOS = PLATFORM == "Darwin"
FNC_PLATFORM_WINDOWS = PLATFORM == "Windows"
FNC_PLATFORM_LINUX = PLATFORM == "Linux"

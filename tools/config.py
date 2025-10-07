import sys
import platform

# Project names
ENGINE_NAME = "enginio"
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

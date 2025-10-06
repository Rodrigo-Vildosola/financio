cmake_minimum_required(VERSION 3.16)

project(FinancioProject)

# # Prevent CMake from finding system-wide Abseil packages
# set(absl_DIR "${CMAKE_SOURCE_DIR}/external/protobuf/third_party/abseil-cpp/cmake" CACHE PATH "Abseil path override" FORCE)
# set(ABSL_USE_EXTERNAL_PROJECT OFF CACHE BOOL "" FORCE)


# Use passed-in vars from Python
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(ENGINE_ASSETS_DIR "${CMAKE_SOURCE_DIR}/engine/assets")
set(APP_ASSETS_DIR "${CMAKE_SOURCE_DIR}/app/assets")


# Pass to submodules
set(ENGINE_NAME ${ENGINE_NAME})
set(APP_NAME ${APP_NAME})
set(TRADER_NAME ${TRADER_NAME})


# Engine Deps
add_subdirectory(external/glfw)
add_subdirectory(external/glfw3webgpu)
add_subdirectory(external/webgpu)
add_subdirectory(external/glm)
add_subdirectory(external/stb)
add_subdirectory(external/nlohmann)
add_subdirectory(external/spdlog)

file(GLOB IMGUI_SRC
    external/imgui/*.cpp
    external/imgui/*.h
)
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC external/imgui)


# IPC and Trading Deps
add_subdirectory(external/ibkr)

# Protobuf compilation
add_subdirectory(proto)

# Targets
add_subdirectory(engine)
add_subdirectory(app)
add_subdirectory(trader)

# Add optimization flags for GCC and Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O2 -Wall -Wextra -Wpedantic)
endif()

# Add optimization flags for MSVC
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/O2 /W4)
endif()



if (FNC_ENABLE_ASSERTS)
    target_compile_definitions(${ENGINE_NAME} PUBLIC FNC_ENABLE_ASSERTS)
endif()

if (FNC_ENABLE_DEBUG_LOGGING)
    target_compile_definitions(${ENGINE_NAME} PUBLIC FNC_ENABLE_DEBUG_LOGGING)
endif()



string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)

if (BUILD_TYPE_UPPER STREQUAL "RELEASE" OR BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    target_compile_definitions(${ENGINE_NAME} PUBLIC FNC_RELEASE=1)
elseif (BUILD_TYPE_UPPER STREQUAL "TESTING")
    target_compile_definitions(${ENGINE_NAME} PUBLIC FNC_TESTING=1)
endif()


# add_executable(example_glfw_wgpu
#     external/imgui/examples/example_glfw_wgpu/main.cpp
#     external/imgui/backends/imgui_impl_glfw.cpp
#     external/imgui/backends/imgui_impl_wgpu.cpp
# )

# target_compile_definitions(example_glfw_wgpu PUBLIC IMGUI_IMPL_WEBGPU_BACKEND_DAWN)

# target_include_directories(example_glfw_wgpu PUBLIC
#     external/imgui
#     external/imgui/backends
# )

# target_link_libraries(example_glfw_wgpu PUBLIC
#     imgui
#     glfw
#     webgpu
#     glfw3webgpu
# )

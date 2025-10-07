cmake_minimum_required(VERSION 3.16)

project(FinancioProject)

set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(ENGINE_ASSETS_DIR "${CMAKE_SOURCE_DIR}/engine/assets")
set(APP_ASSETS_DIR "${CMAKE_SOURCE_DIR}/app/assets")


# Names passed in from tooling or cache
set(ENGINE_NAME ${ENGINE_NAME})
set(APP_NAME ${APP_NAME})
set(TRADER_NAME ${TRADER_NAME})


# ---------- Dependencies ----------
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


# IBKR and proto
add_subdirectory(external/ibkr)
add_subdirectory(proto)


# ---------- Engine + Apps ----------
add_subdirectory(engine)
add_subdirectory(app)
add_subdirectory(trader)

# ---------- Global compile flags ----------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-O2 -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/O2 /W4)
endif()



# ---------- Feature flags ----------
if (ENG_ENABLE_ASSERTS)
  target_compile_definitions(eng-core PUBLIC ENG_ENABLE_ASSERTS)
  target_compile_definitions(eng-frontend PUBLIC ENG_ENABLE_ASSERTS)
endif()

if (ENG_ENABLE_DEBUG_LOGGING)
  target_compile_definitions(eng-core PUBLIC ENG_ENABLE_DEBUG_LOGGING)
  target_compile_definitions(eng-frontend PUBLIC ENG_ENABLE_DEBUG_LOGGING)
endif()



string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
if (BUILD_TYPE_UPPER STREQUAL "RELEASE" OR BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    target_compile_definitions(eng-core PUBLIC ENG_RELEASE=1)
    target_compile_definitions(eng-frontend PUBLIC ENG_RELEASE=1)
elseif (BUILD_TYPE_UPPER STREQUAL "TESTING")
    target_compile_definitions(eng-core PUBLIC ENG_TESTING=1)
    target_compile_definitions(eng-frontend PUBLIC ENG_TESTING=1)
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

cmake_minimum_required(VERSION 3.16)

# Options
option(ENG_BUILD_NET "Build gRPC networking" OFF)

# ---- eng-core ----
add_library(eng-core STATIC
  src/core/layer_stack.cpp
  src/core/logger.cpp
  src/core/timer.cpp
  src/runtime/application_base.cpp
)
target_include_directories(eng-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(eng-core PUBLIC spdlog::spdlog)
target_compile_definitions(eng-core PUBLIC ENGINE_ASSETS_DIR="${ENGINE_ASSETS_DIR}")



# ---- eng-frontend ----
add_library(eng-frontend STATIC
  src/runtime/application.cpp
  src/platform/window.cpp
  src/platform/macos/input.cpp
  src/platform/macos/window.cpp
  src/renderer/context/command_queue.cpp
  src/renderer/context/context.cpp
  src/renderer/context/context_utils.cpp
  src/renderer/buffer.cpp
  src/renderer/pipeline.cpp
  src/renderer/render_pass.cpp
  src/renderer/renderer.cpp
  src/renderer/renderer_api.cpp
  src/renderer/shader.cpp
  src/ui/build.cpp
  src/ui/imgui_layer.cpp
  src/ui/implot.cpp
  src/ui/implot_items.cpp
)

target_include_directories(eng-frontend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(eng-frontend
  PUBLIC eng-core webgpu glfw3webgpu glm imgui   # PUBLIC, not PRIVATE
  PRIVATE glfw
)
target_compile_definitions(eng-frontend PUBLIC IMGUI_DEFINE_MATH_OPERATORS ENG_FRONTEND ENG_WITH_WEBGPU)

# ---- eng-net (optional) ----
if(ENG_BUILD_NET)
  find_package(gRPC CONFIG REQUIRED)
  find_package(Protobuf CONFIG REQUIRED)
  file(GLOB_RECURSE ENG_NET_SRC CONFIGURE_DEPENDS
    src/net/*.cpp
  )
  add_library(eng-net STATIC ${ENG_NET_SRC})
  target_include_directories(eng-net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
  target_link_libraries(eng-net PUBLIC
    eng-core
    gRPC::grpc++
    protobuf::libprotobuf
  )
endif()

# Convenience alias if you still want ${ENGINE_NAME} to exist
if(DEFINED ENGINE_NAME AND NOT TARGET ${ENGINE_NAME})
  add_library(${ENGINE_NAME} INTERFACE)
  target_link_libraries(${ENGINE_NAME} INTERFACE eng-core eng-frontend)
endif()


# # ---- apps ----
# # Headless: links ONLY eng-core
# add_executable(app_headless
#   src/main_headless.cpp
# )
# target_link_libraries(app_headless PRIVATE eng-core)
# target_include_directories(app_headless PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# # GUI: links eng-frontend (which brings glfw/webgpu/imgui)
# add_executable(app_gui
#   src/main_gui.cpp
# )
# target_link_libraries(app_gui PRIVATE eng-frontend)
# target_include_directories(app_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# # Convenience alias if desired
# if(DEFINED ENGINE_NAME AND NOT TARGET ${ENGINE_NAME})
#   add_library(${ENGINE_NAME} INTERFACE)
#   target_link_libraries(${ENGINE_NAME} INTERFACE eng-core eng-frontend)
# endif()

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    src/*.cpp
    include/*.h
)

add_library(${ENGINE_NAME} STATIC ${ENGINE_SRC})

target_include_directories(${ENGINE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/glm
)

target_link_libraries(${ENGINE_NAME}
    glfw
    spdlog::spdlog
    glm
    imgui
)


target_compile_definitions(${ENGINE_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

target_compile_definitions(${ENGINE_NAME} PUBLIC ENGINE_ASSETS_DIR="${ENGINE_ASSETS_DIR}")
target_compile_definitions(${ENGINE_NAME} PUBLIC APP_ASSETS_DIR="${APP_ASSETS_DIR}")

# target_compile_definitions(${ENGINE_NAME} PUBLIC FNC_RELEASE)

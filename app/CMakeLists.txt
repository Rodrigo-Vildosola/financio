file(GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${APP_NAME} ${APP_SRC})

target_link_libraries(${APP_NAME} ${ENGINE_NAME})

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/app/src
    ${CMAKE_SOURCE_DIR}/external/glm  # <-- Add this if GLM is local
)

target_link_libraries(${APP_NAME}
    imgui
    glm
    # nlohmann_json::nlohmann_json
    # stb
)

target_compile_definitions(${APP_NAME} PUBLIC APP_ASSETS_DIR="${APP_ASSETS_DIR}")

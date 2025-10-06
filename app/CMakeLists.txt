file(GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS
    src/*.cpp
    main.cpp
)

add_executable(${APP_NAME} ${APP_SRC})

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/app/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/ibkr/src/cppclient
)

target_link_libraries(${APP_NAME} PRIVATE
    ${ENGINE_NAME}
    imgui
    glm
    twsapi
    trading_proto
)

target_compile_definitions(${APP_NAME} PUBLIC APP_ASSETS_DIR="${APP_ASSETS_DIR}")

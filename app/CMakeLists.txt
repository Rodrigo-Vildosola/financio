cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS
    src/*.cpp
    main.cpp
)

add_executable(${APP_NAME} ${APP_SRC})

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/app/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/ibkr/src/cppclient
    ${CMAKE_SOURCE_DIR}/shared/proto/generated
)

# Link only the frontend engine; it pulls eng-core internally
target_link_libraries(${APP_NAME} PRIVATE
    eng-frontend
    # imgui               # optional; eng-frontend already links it PRIVATELY
    # glm                 # optional; eng-frontend already links it PRIVATELY
    twsapi
    trading_proto
)

target_compile_definitions(${APP_NAME} PUBLIC
  APP_ASSETS_DIR="${APP_ASSETS_DIR}"
)

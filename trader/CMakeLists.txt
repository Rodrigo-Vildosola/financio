file(GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS
    src/*.cpp
    main.cpp
)

add_executable(${APP_NAME} ${APP_SRC})

target_link_libraries(${APP_NAME} ${ENGINE_NAME})

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/trader/include
    ${CMAKE_SOURCE_DIR}/external/ibkr/source/cppclient
)

target_link_libraries(${APP_NAME}
    twsapi
)

file(GLOB_RECURSE TRADER_SRC CONFIGURE_DEPENDS
    src/*.cpp
    main.cpp
)

add_executable(${TRADER_NAME} ${TRADER_SRC})

target_include_directories(${TRADER_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/trader/include
    ${CMAKE_SOURCE_DIR}/external/ibkr/src
    ${CMAKE_SOURCE_DIR}/proto/generated
)



target_link_libraries(${TRADER_NAME} PRIVATE
    ${ENGINE_NAME}
    twsapi
    trading_proto
)

file(GLOB_RECURSE TRADER_SRC CONFIGURE_DEPENDS
    src/*.cpp
    main.cpp
)

add_executable(${TRADER_NAME} ${TRADER_SRC})

target_include_directories(${TRADER_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/trader/include
    ${CMAKE_SOURCE_DIR}/external/ibkr/src
)



target_link_libraries(${TRADER_NAME} PRIVATE
    twsapi
    trading_proto
)

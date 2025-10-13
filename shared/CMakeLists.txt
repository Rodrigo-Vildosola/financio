cmake_minimum_required(VERSION 3.16)
project(financio_proto)

find_package(gRPC REQUIRED)

file(GLOB_RECURSE GENERATED_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/generated/**/*.cc"
)
file(GLOB_RECURSE GENERATED_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/generated/**/*.h"
)

add_library(trading_proto STATIC ${GENERATED_SRCS} ${GENERATED_HDRS})

target_include_directories(trading_proto PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

target_link_libraries(trading_proto
    PUBLIC protobuf::libprotobuf gRPC::grpc++
)

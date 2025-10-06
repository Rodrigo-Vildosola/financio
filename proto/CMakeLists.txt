# proto/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

find_package(gRPC REQUIRED)

# include(GrpcGenerate)

set(PROTO_BASE ${CMAKE_CURRENT_SOURCE_DIR})
set(PROTO_DIR  ${PROTO_BASE}/trading)

set(PROTO_FILES
    ${PROTO_DIR}/common.proto
    ${PROTO_DIR}/requests.proto
    ${PROTO_DIR}/events.proto
    ${PROTO_DIR}/control.proto
    ${PROTO_DIR}/trader_service.proto
)

# Where generated C++ code will be placed (inside build tree)
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_DIR})

if (NOT DEFINED gRPC_CPP_PLUGIN_EXECUTABLE)
    find_program(gRPC_CPP_PLUGIN_EXECUTABLE NAMES grpc_cpp_plugin)
    if (NOT gRPC_CPP_PLUGIN_EXECUTABLE)
        message(FATAL_ERROR "grpc_cpp_plugin not found. Install gRPC CLI tools or add to PATH.")
    endif()
endif()

# ------------------------------------------------------------------
# Manual fallback for grpc_generate_cpp() if gRPC didn't install it
# ------------------------------------------------------------------
function(grpc_generate_cpp SRCS HDRS)
  set(_srcs "")
  set(_hdrs "")
  foreach(FIL IN LISTS ARGN)
    # Get base filename and relative directory
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(FIL_PATH ${FIL} PATH)
    file(RELATIVE_PATH REL_PATH ${PROTO_BASE} ${FIL_PATH})

    set(_out_dir "${GENERATED_DIR}/${REL_PATH}")
    file(MAKE_DIRECTORY ${_out_dir})

    set(_src "${_out_dir}/${FIL_WE}.grpc.pb.cc")
    set(_hdr "${_out_dir}/${FIL_WE}.grpc.pb.h")

    add_custom_command(
      OUTPUT ${_src} ${_hdr}
      COMMAND ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --grpc_out ${GENERATED_DIR}
           --plugin=protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}
           -I ${PROTO_BASE}
           ${FIL}
      DEPENDS ${FIL}
      COMMENT "Generating gRPC sources for ${FIL_WE}"
      VERBATIM)
    list(APPEND _srcs ${_src})
    list(APPEND _hdrs ${_hdr})
  endforeach()
  set(${SRCS} ${_srcs} PARENT_SCOPE)
  set(${HDRS} ${_hdrs} PARENT_SCOPE)
endfunction()
# ------------------------------------------------------------------

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    ${PROTO_FILES}
    PROTOC_OUT_DIR ${GENERATED_DIR}
    IMPORT_DIRS ${PROTO_BASE}
)

grpc_generate_cpp(GRPC_SRCS GRPC_HDRS
    ${PROTO_DIR}/trader_service.proto
)

add_library(trading_proto STATIC
    ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
)

target_include_directories(trading_proto PUBLIC
    ${GENERATED_DIR}
)

target_link_libraries(trading_proto
    PUBLIC protobuf::libprotobuf gRPC::grpc++ 
)

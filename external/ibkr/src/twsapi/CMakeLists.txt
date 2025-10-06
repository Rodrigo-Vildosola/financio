file(GLOB SOURCES "*.cpp" "protobufUnix/*.cc")
file(GLOB HEADERS "*.h")

set(twsapi_VERSION 9.79.02)
if (NOT TARGET protobuf::libprotobuf)
    find_package(Protobuf REQUIRED)
endif()
find_package(absl REQUIRED COMPONENTS log check hash strings)

add_library(twsapi STATIC ${SOURCES})

target_include_directories(twsapi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/protobufUnix
        ${Protobuf_INCLUDE_DIRS}
)


# Link libraries
if(WIN32)
    target_link_libraries(twsapi PUBLIC ws2_32)
endif()

add_library(bid STATIC IMPORTED GLOBAL)
set_target_properties(bid PROPERTIES
    IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libbid.a
)

target_link_libraries(twsapi PRIVATE
    protobuf::libprotobuf
    absl::log
    absl::check
    absl::hash
    absl::strings
    bid
)

if(MSVC)
    target_compile_options(twsapi PRIVATE /W3)
else()
    target_compile_options(twsapi PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)
endif()

set_target_properties(twsapi PROPERTIES
    PREFIX "lib"
    DEBUG_POSTFIX "d"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PUBLIC_HEADER "${HEADERS}"
)

# install(TARGETS twsapi EXPORT twsapi-targets
#     PUBLIC_HEADER DESTINATION include/twsapi
#     LIBRARY DESTINATION lib
# )


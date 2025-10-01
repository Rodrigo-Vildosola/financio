file(GLOB SOURCES "*.cpp" "protobufUnix/*.cc")
file(GLOB HEADERS "*.h" "protobufUnix/*.h")

set(twsapi_VERSION 9.79.02)
add_library(twsapi SHARED ${SOURCES})

# Include dirs for normal headers + protobuf
target_include_directories(twsapi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/protobufUnix
)

# Link libraries
if(WIN32)
    target_link_libraries(twsapi PUBLIC ws2_32)
endif()

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED COMPONENTS log check hash strings)

target_link_libraries(twsapi PRIVATE
    protobuf::libprotobuf
    absl::log
    absl::check
    absl::hash
    absl::strings
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

install(TARGETS twsapi EXPORT twsapi-targets
    PUBLIC_HEADER DESTINATION include/twsapi
    LIBRARY DESTINATION lib
)


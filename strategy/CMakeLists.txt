cmake_minimum_required(VERSION 3.20)
project(feature_extractor LANGUAGES CXX)

find_package(nlohmann_json REQUIRED)

file(GLOB_RECURSE FEATURE_EXTRACTOR_SRC
    CONFIGURE_DEPENDS
    "src/*.cpp"
)

add_library(feature_extractor STATIC ${FEATURE_EXTRACTOR_SRC})

target_include_directories(feature_extractor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(feature_extractor PUBLIC
    nlohmann_json::nlohmann_json
)

add_executable(test_feature_extractor examples/test_feature_extractor.cpp)
target_link_libraries(test_feature_extractor PRIVATE feature_extractor)

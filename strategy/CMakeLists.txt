cmake_minimum_required(VERSION 3.20)
project(strategy LANGUAGES CXX)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)

message(STATUS "Python3 found:")
message(STATUS "  Executable    : ${Python3_EXECUTABLE}")
message(STATUS "  Version       : ${Python3_VERSION}")
message(STATUS "  Include dirs  : ${Python3_INCLUDE_DIRS}")
message(STATUS "  Library       : ${Python3_LIBRARIES}")


file(GLOB_RECURSE STRAT_SRC
    CONFIGURE_DEPENDS
    "src/*.cpp"
)

add_library(strategy STATIC ${STRAT_SRC})

target_include_directories(strategy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(strategy PUBLIC
    pybind11::embed
    nlohmann_json::nlohmann_json
)

add_executable(test_feature_extractor examples/test_feature_extractor.cpp)
target_link_libraries(test_feature_extractor PRIVATE strategy)

add_executable(test_model_python examples/test_model_python.cpp)
target_link_libraries(test_model_python PRIVATE strategy)

add_executable(test_feature_model examples/test_feature_model.cpp)
target_link_libraries(test_feature_model PRIVATE strategy)

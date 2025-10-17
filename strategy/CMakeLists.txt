cmake_minimum_required(VERSION 3.20)
project(strategy LANGUAGES CXX)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(strategy STATIC
    src/strategy_runtime.cpp
    src/strategy_runtime_python.cpp
    src/strategy_context.cpp
    src/bindings/binding_context.cpp
    src/bindings/binding_decorators.cpp
    src/bindings/binding_registry.cpp
)

target_include_directories(strategy PUBLIC include)
target_link_libraries(strategy
    PUBLIC
        pybind11::embed
        Python3::Python
        nlohmann_json::nlohmann_json
)

add_custom_command(
    TARGET strategy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/python
            $<TARGET_FILE_DIR:strategy>/python
)

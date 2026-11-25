cmake_minimum_required(VERSION 3.16)
project(yaml_cpp_include LANGUAGES CXX)

find_package(yaml-cpp CONFIG REQUIRED)

add_library(yaml_include src/yaml_include.cpp)
target_include_directories(yaml_include PUBLIC include)
target_compile_features(yaml_include PUBLIC cxx_std_17)
target_link_libraries(yaml_include PUBLIC yaml-cpp::yaml-cpp)

add_executable(yaml_include_example src/main.cpp)
target_link_libraries(yaml_include_example PRIVATE yaml_include)

include(CTest)
if(BUILD_TESTING)
  add_executable(yaml_include_tests tests/yaml_include_tests.cpp)
  target_link_libraries(yaml_include_tests PRIVATE yaml_include)
  add_test(NAME yaml_include_tests COMMAND yaml_include_tests
    "${CMAKE_CURRENT_SOURCE_DIR}/examples"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures")
  add_test(NAME yaml_include_cli COMMAND "${CMAKE_COMMAND}"
    "-DEXECUTABLE=$<TARGET_FILE:yaml_include_example>"
    "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/examples/config.yaml"
    "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/combined-test.yaml"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cli_test.cmake")
endif()

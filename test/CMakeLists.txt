cmake_minimum_required(VERSION 3.14)

if(NOT TARGET GTest::gtest_main)
    find_package(GTest REQUIRED)
endif()
enable_testing()
add_compile_options(-fprofile-arcs -ftest-coverage)
add_link_options(-fprofile-arcs)

include(GoogleTest)
file(GLOB_RECURSE TEST_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")
add_library(test_proto "${CMAKE_CURRENT_SOURCE_DIR}/proto/test_pb2.proto")
target_link_libraries(test_proto protobuf::libprotobuf)
protobuf_generate(TARGET test_proto
    IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/proto"
    PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(test_proto PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
foreach(SRC ${TEST_SRCS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" TARGET_NAME ${SRC})
    string(REPLACE "/" "_" TARGET_NAME ${TARGET_NAME})
    string(REPLACE "." "_" TARGET_NAME ${TARGET_NAME})
    add_executable("${TARGET_NAME}" "${SRC}")
    message(STATUS "Adding test target ${TARGET_NAME} for ${SRC}")
    target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    target_include_directories("${TARGET_NAME}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/proto")
    target_link_libraries("${TARGET_NAME}" youya)
    target_link_libraries("${TARGET_NAME}" test_proto)
    target_link_libraries("${TARGET_NAME}" GTest::gtest_main)
    gtest_discover_tests("${TARGET_NAME}")
endforeach()
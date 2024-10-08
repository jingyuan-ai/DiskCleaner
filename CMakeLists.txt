cmake_minimum_required(VERSION 3.14)

project(youya VERSION 1.2.0)
cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)

include(CTest)                      # for BUILD_TESTING option
include(CMakePackageConfigHelpers)  # for write_basic_package_version_file

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(REVISION_VERSION 0)
set(BUILD_VERSION 0)

include(${PROJECT_ROOT_DIR}/cmake/version.cmake)
TAG_VERSION()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-local-typedefs -Wno-unused-private-field -g -O3")

# deps
option(BUILD_DEPS "Use FetchContent download and build dependencies" ON)
option(BUILD_ABSL "Build and install ABSL" ON)
option(BUILD_PROTOBUF "Build and install Google Protocol Buffers" ON)
option(BUILD_GTEST "Build and install Google Test libraries" ON)
option(BUILD_OPENSSL "Build and install OpenSSL" ON)
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_DEPS)
  include(FetchContent)
  set(FETCHCONTENT_QUIET OFF)
  set(extra_builds "")
  FetchContent_Declare(
    boost
    URL "https://github.com/boostorg/boost/releases/download/boost-1.83.0/boost-1.83.0.tar.gz"
    URL_HASH SHA256=0c6049764e80aa32754acd7d4f179fd5551d8172a83b71532ae093e7384e98da
    EXCLUDE_FROM_ALL
  )
  list(APPEND extra_builds "boost")
  # fetchcontent gflags
  FetchContent_Declare(
    gflags
    URL "https://github.com/gflags/gflags/archive/refs/tags/v2.2.2.tar.gz"
    URL_HASH SHA256=34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf
  )
  list(APPEND extra_builds "gflags")
  FetchContent_Declare(
    glog
    URL "https://github.com/google/glog/archive/refs/tags/v0.7.1.tar.gz"
    URL_HASH SHA256=00e4a87e87b7e7612f519a41e491f16623b12423620006f59f5688bfd8d13b08
  )
  list(APPEND extra_builds "glog")
  add_definitions(-DGOOGLE_GLOG_DLL_DECL)
  if (BUILD_ABSL)
    FetchContent_Declare(
      absl
      URL "https://github.com/abseil/abseil-cpp/archive/refs/tags/20230802.1.tar.gz"
      URL_HASH SHA256=987ce98f02eefbaf930d6e38ab16aa05737234d7afbab2d5c4ea7adbe50c28ed
    )
    list(APPEND extra_builds "absl")
  endif(BUILD_ABSL)
  if (BUILD_PROTOBUF)
    FetchContent_Declare(
        protobuf
        URL "https://github.com/protocolbuffers/protobuf/archive/refs/tags/v25.3.tar.gz"
        URL_HASH SHA256=d19643d265b978383352b3143f04c0641eea75a75235c111cc01a1350173180e
    )
    list(APPEND extra_builds "protobuf")
  endif(BUILD_PROTOBUF)
  if (BUILD_OPENSSL)
    find_package(OpenSSL REQUIRED)
    # list(APPEND extra_builds OpenSSL::SSL OpenSSL::Crypto)
  endif()
  if (BUILD_GTEST)
    FetchContent_Declare(
        googletest
        URL "https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz"
        URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7
    )
  endif(BUILD_GTEST)

  if(BUILD_GTEST AND BUILD_TESTS AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    FetchContent_MakeAvailable(googletest)
  endif()
  set(protobuf_BUILD_TESTS OFF)
  set(ABSL_ENABLE_INSTALL ON)
  set(ABSL_PROPAGATE_CXX_STD ON)
  set(protobuf_BUILD_TESTS OFF)
  set(BOOST_INCLUDE_LIBRARIES preprocessor spirit)
  set(ABSL_ROOT_DIR ${CMAKE_BINARY_DIR}/_deps/absl-src)
  message("Using FetchContent to download and build dependencies")
  FetchContent_MakeAvailable(${extra_builds})
  if (BUILD_PROTOBUF)
    include("${protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake")
  endif(BUILD_PROTOBUF)
endif()

# 增加openssl的include路径
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(src)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(apps)
file(GLOB_RECURSE srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(target_name "youya")
add_library(${target_name} SHARED ${srcs})
target_link_libraries(${target_name} PUBLIC OpenSSL::SSL OpenSSL::Crypto glog gflags)
set_target_properties(${main_target_name} PROPERTIES VERSION ${SDK_VERSION} SOVERSION ${MAJOR_VERSION})

install(TARGETS ${target_name} EXPORT youya)
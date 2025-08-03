cmake_minimum_required(VERSION 3.29)
project(wordlebot)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

add_executable(prog)
include("src/filelist.cmake")
target_compile_features(prog PRIVATE cxx_std_23)

if (DEFINED MY_COMPILE_OPTS) # defined by CMakePresets.json sometimes
	separate_arguments(MY_CXX_FLAGS UNIX_COMMAND ${MY_COMPILE_OPTS})
	target_compile_options(prog PRIVATE ${MY_CXX_FLAGS})
	target_link_options(prog PRIVATE ${MY_CXX_FLAGS})
endif()


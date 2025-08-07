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

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)

CPMAddPackage(
	NAME progressbar
	GITHUB_REPOSITORY doches/progressbar
	GIT_TAG ac56232610abf58cc2db2bc86efc8fcba7dfe8c2
	VERSION 0.1 # doesn't actually have versions, so this is a placeholder
)
# fix progressbar's broken CMake config
target_link_libraries(progressbar PUBLIC ${CURSES_LIBRARIES})

add_executable(prog)
include("src/filelist.cmake")
target_compile_options(prog PRIVATE "-march=native")
target_compile_features(prog PRIVATE cxx_std_23)
target_compile_features(prog PRIVATE c_std_23)
target_link_libraries(prog progressbar)
target_include_directories(prog PRIVATE ${progressbar_SOURCE_DIR}/include)

if (DEFINED MY_COMPILE_OPTS) # defined by CMakePresets.json sometimes
	separate_arguments(MY_CXX_FLAGS UNIX_COMMAND ${MY_COMPILE_OPTS})
	target_compile_options(prog PRIVATE ${MY_CXX_FLAGS})
	target_link_options(prog PRIVATE ${MY_CXX_FLAGS})
endif()


cmake_minimum_required(VERSION 3.29)
project(wordlebot)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
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

CPMAddPackage(
	NAME c_hashmap
	GITHUB_REPOSITORY Mashpoe/c-hashmap
	GIT_TAG 98bbfe006640a411a9d8c949a46a712edbbbf891
	VERSION 1.1 # again, a placeholder version
)
# has no cmakelists, so we have to do it ourselves
add_library(c_hashmap ${c_hashmap_SOURCE_DIR}/map.c)

CPMAddPackage(
	NAME argparse
	GITHUB_REPOSITORY cofyc/argparse
	GIT_TAG 5559200a91a334e15391e8285571145f6b6cc002
	VERSION 2.0 # grabbed from main, no actual version 2.0 exists
	OPTIONS "ARGPARSE_SHARED OFF"
)

CPMAddPackage(
	NAME jansson
	GITHUB_REPOSITORY akheron/jansson
	GIT_TAG 154f8919fb0d3778282b797789ad985e3d99fdc8
	VERSION 2.14.1.2 # not a release, grabbed from master to get unreleased CMake config fixes
	OPTIONS
		"JANSSON_BUILD_DOCS OFF"
		"JANSSON_BUILD_SHARED_LIBS OFF"
		"JANSSON_EXAMPLES OFF"
		"JANSSON_WITHOUT_TESTS ON"
		"JANSSON_INSTALL OFF"
)

CPMAddPackage(
	NAME Unity
	GITHUB_REPOSITORY ThrowTheSwitch/Unity
	VERSION 2.6.1
)

# all the files that don't have main functions
add_library(modules)
include("src/filelist.cmake")

# supply config here, so it propagates to both targets
target_compile_options(modules PUBLIC "-march=native") # makes things faster, if less portable
target_compile_features(modules PUBLIC cxx_std_23)
target_compile_features(modules PUBLIC c_std_23)
target_link_libraries(modules progressbar)
target_link_libraries(modules c_hashmap)
target_link_libraries(modules argparse_static)
target_link_libraries(modules jansson)
target_include_directories(modules PRIVATE ${progressbar_SOURCE_DIR}/include)
# also has other files, but is the least messy solution
target_include_directories(modules PRIVATE ${c_hashmap_SOURCE_DIR})
target_include_directories(modules PRIVATE ${argparse_SOURCE_DIR})

if (DEFINED MY_COMPILE_OPTS) # defined by CMakePresets.json sometimes
	separate_arguments(MY_CXX_FLAGS UNIX_COMMAND ${MY_COMPILE_OPTS})
	target_compile_options(modules PUBLIC ${MY_CXX_FLAGS})
	target_link_options(modules PUBLIC ${MY_CXX_FLAGS})
endif()

add_executable(prog src/main.c)
target_link_libraries(prog modules)

add_executable(test-bin test/test.c)
target_link_libraries(test-bin modules)
target_link_libraries(test-bin unity::framework)

add_custom_target(test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test-bin)


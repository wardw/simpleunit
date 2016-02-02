cmake_minimum_required(VERSION 2.8)
project(simpleunit)

# Tested with GTest 1.7.0 on OS X

# Set GTest path
option(BUILD_GTEST "Build GTest with this project" ON)
SET(GTEST_ROOT $ENV{GTEST_ROOT} CACHE PATH "Path to GTest")
message("Using GTest at ${GTEST_ROOT}")

# Build
include_directories(".")
set(gtest_src "simpleunit/UnitTest.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_executable(simpleunit ${gtest_src})

# Link Gtest
if(BUILD_GTEST)
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/gtest")
	add_subdirectory("${GTEST_ROOT}" "${CMAKE_BINARY_DIR}/gtest")
	include_directories("${GTEST_ROOT}/include")
	target_link_libraries(simpleunit gtest gtest_main pthread)
else()
	# Use existing libs
	find_package(GTest REQUIRED)
	include_directories("${GTEST_INCLUDE_DIRS}")
	target_link_libraries(simpleunit ${GTEST_BOTH_LIBRARIES})
endif()


# CTest. Doing it this way ensures 'make check' depends on the simpleunit build target
enable_testing()

add_test(NAME all COMMAND simpleunit)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS simpleunit)

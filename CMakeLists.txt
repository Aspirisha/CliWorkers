cmake_minimum_required(VERSION 3.4)

project(cliworkers)

set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_STANDARD 11)

if(MSVC)
	add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif(MSVC)

add_subdirectory(src)
add_subdirectory(test)
cmake_policy(SET CMP0048 OLD)
project(ae_graphics)
cmake_minimum_required(VERSION 2.8.6)

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "major version" FORCE)
set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING "minor version" FORCE)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR} CACHE STRING "version" FORCE)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3  -funroll-loops -ftree-vectorize -fopt-info-vec-optimized -ftree-vectorizer-verbose=3")

set(INCLUDE_DEST "include/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}")
set(LIB_DEST "include/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}/${CMAKE_BUILD_TYPE}")

set(GRAPHICS_LIBS_PREFIX "" CACHE PATH "Path to installation of graphics libs")

option(GROUP_ACCELERATION_NODES "Group acceleration tree nodes for faster traversal" ON)
option(PAD_BOUNDS "Pad bounding boxes for faster 4-wide SIMD operations" ON)

if (GROUP_ACCELERATION_NODES)
add_definitions(-DGROUP_ACCELERATION_NODES)
endif()

if (PAD_BOUNDS)
add_definitions(-DPAD_BOUNDS)
endif()

include_directories(${GRAPHICS_LIBS_PREFIX}/include)
link_directories(${GRAPHICS_LIBS_PREFIX}/lib)

set(ENV{PKG_CONFIG_PATH} "${GRAPHICS_LIBS_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
find_package(PkgConfig)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
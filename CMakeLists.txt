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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -ftree-vectorize")

add_subdirectory(ext/tinyobjloader)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -fopt-info-vec-optimized -ftree-vectorizer-verbose=3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(INCLUDE_DEST "include/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}")
set(LIB_DEST "include/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}/${CMAKE_BUILD_TYPE}")

add_subdirectory(src)

enable_testing()

add_executable(spatial_acceleration_test test/spatial_acceleration.cc)
target_link_libraries(spatial_acceleration_test ae_graphics tinyobjloader)
add_test(
  NAME bvh_sah_build_bunny 
  COMMAND build/spatial_acceleration_test objs/bunny.obj 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(
  NAME bvh_sah_build_dragon 
  COMMAND build/spatial_acceleration_test objs/dragon.obj 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
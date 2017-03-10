
add_definitions(-DEXRDISPLAY_PATH="${GRAPHICS_LIBS_PREFIX}/bin/exrdisplay")

add_executable(load_obj load_obj.cc)
target_link_libraries(load_obj 
  ae_graphics 
  tinyobjloader)

add_executable(tree_build tree_build.cc)
target_link_libraries(tree_build 
  ae_graphics 
  tinyobjloader)

add_executable(depth_trace depth_trace.cc)
target_link_libraries(depth_trace 
  ae_graphics 
  tinyobjloader 
  IlmImf-2_2)

add_test(
  NAME load_square_obj
  COMMAND build/test/load_obj objs/square.obj
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME load_bunny_obj
  COMMAND build/test/load_obj objs/bunny.obj
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME load_dragon_obj
  COMMAND build/test/load_obj objs/dragon.obj
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME bvh_sah_build_square
  COMMAND build/test/tree_build objs/square.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME bvh_sah_build_bunny 
  COMMAND build/test/tree_build objs/bunny.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME bvh_sah_build_dragon 
  COMMAND build/test/tree_build objs/dragon.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME depth_trace_square 
  COMMAND build/test/depth_trace objs/square.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME depth_trace_bunny 
  COMMAND build/test/depth_trace objs/bunny.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME depth_trace_dragon 
  COMMAND build/test/depth_trace objs/dragon.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(load_obj load_obj.cc)
target_link_libraries(load_obj ae_graphics tinyobjloader)

add_executable(tree_build tree_build.cc)
target_link_libraries(tree_build ae_graphics tinyobjloader)

add_executable(trace trace.cc)
target_link_libraries(trace ae_graphics tinyobjloader)

add_test(
  NAME load_bunny_obj
  COMMAND build/test/load_obj objs/bunny.obj
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME load_dragon_obj
  COMMAND build/test/load_obj objs/dragon.obj
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
  NAME trace_bunny 
  COMMAND build/test/trace objs/bunny.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(
  NAME trace_dragon 
  COMMAND build/test/trace objs/dragon.obj 
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
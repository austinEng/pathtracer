
file(GLOB ae_graphics_headers
  core/*.h
  geometry/*.h
  spatial_acceleration/*.h  
  intersection/*.h
)

file(GLOB ae_graphics_source
  core/*.cc
  geometry/*.cc
  spatial_acceleration/*.cc  
  intersection/*.cc
)

configure_file("config.inl" "${CMAKE_CURRENT_BINARY_DIR}/config_impl.h")
add_library(ae_graphics ${ae_graphics_headers} ${ae_graphics_source})
set_target_properties(ae_graphics PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ae_graphics PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for config_impl.hpp when building
  $<INSTALL_INTERFACE:${INCLUDE_DEST}> # for client in install mode
  $<INSTALL_INTERFACE:${LIB_DEST}> # for config_impl.hpp in install mode)
)

install(TARGETS ae_graphics EXPORT ae_graphics DESTINATION "${LIB_DEST}")
install(FILES ae_graphics-config.cmake DESTINATION ${LIB_DEST})
install(EXPORT ae_graphics DESTINATION "${LIB_DEST}")
install(FILES ${ae_graphics_headers} DESTINATION "${INCLUDE_DEST}")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config_impl.h DESTINATION "${LIB_DEST}")
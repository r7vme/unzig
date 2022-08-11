macro(unzig_add_executable target path)
  get_filename_component(filename ${path} NAME)
  get_filename_component(filename_no_ext ${path} NAME_WE)

  set(unzig_exe ${CMAKE_CURRENT_BINARY_DIR}/unzig)
  set(orig_source_path ${CMAKE_CURRENT_SOURCE_DIR}/${path})
  set(source_path ${CMAKE_CURRENT_BINARY_DIR}/${filename})
  set(output_file ${CMAKE_CURRENT_BINARY_DIR}/${filename_no_ext})

  add_custom_command(
    OUTPUT ${output_file}
    DEPENDS ${orig_source_path}
    COMMAND cp ${orig_source_path} ${source_path}
    COMMAND ${unzig_exe} ${source_path}
  )
  add_custom_target(${target}_unzig_executable ALL DEPENDS ${output_file})
  add_dependencies(${target}_unzig_executable unzig)
endmacro()

macro(unzig_add_test target path)
  unzig_add_executable(${target} ${path})
  add_test(NAME ${target} COMMAND ${target})
endmacro()

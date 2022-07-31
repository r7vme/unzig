macro(unzig_build_executables)
  foreach(path IN ITEMS ${ARGN})
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
     add_custom_target(unzig_executable_${filename_no_ext} ALL DEPENDS ${output_file})
     add_dependencies(unzig_executable_${filename_no_ext} unzig)
  endforeach()
endmacro()

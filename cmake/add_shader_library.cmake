
find_package(PythonInterp)

macro(setup_folders target)
if(IS_MULTICONFIG)
  file(RELATIVE_PATH relative_offset ${vulkan_fem_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR})
  set_property(TARGET ${target} PROPERTY FOLDER ${relative_offset})
endif()
endmacro()

set(CMAKE_GLSL_COMPILER "glslc" CACHE STRING "Which glsl compiler to use")

# Compiles the given GLSL shader through glslc.
function(compile_glsl_using_glslc shader output_file)
  get_filename_component(input_file ${shader} ABSOLUTE)
  add_custom_command (
    OUTPUT ${output_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling SPIR-V binary ${shader}"
    DEPENDS ${shader} ${FILE_DEPS}
      ${VulkanTestApplications_SOURCE_DIR}/cmake/generate_cmake_dep.py
    COMMAND ${CMAKE_GLSL_COMPILER} -mfmt=c -o ${output_file} -c ${input_file} -MD
      ${ADDITIONAL_ARGS}
    COMMAND ${PYTHON_EXECUTABLE}
      ${VulkanTestApplications_SOURCE_DIR}/cmake/generate_cmake_dep.py
      ${output_file}.d
    COMMAND ${CMAKE_COMMAND} -E
      copy_if_different
        ${output_file}.d.cmake.tmp
        ${output_file}.d.cmake
  )
endfunction(compile_glsl_using_glslc)

# Compiles the given HLSL shader through glslc.
# The name of the resulting file is also written to the 'result' variable.
function(compile_hlsl_using_glslc shader output_file result)
  get_filename_component(input_file ${shader} ABSOLUTE)
  string(REPLACE ".hlsl." ".glslc.hlsl." glslc_hlsl_filename ${output_file})
  add_custom_command (
    OUTPUT ${glslc_hlsl_filename}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling HLSL to SPIR-V binary using glslc: ${shader}"
    DEPENDS ${shader} ${FILE_DEPS}
      ${VulkanTestApplications_SOURCE_DIR}/cmake/generate_cmake_dep.py
    COMMAND ${CMAKE_GLSL_COMPILER} -mfmt=c -x hlsl -o ${glslc_hlsl_filename} -c ${input_file} -MD
    COMMAND ${PYTHON_EXECUTABLE}
      ${VulkanTestApplications_SOURCE_DIR}/cmake/generate_cmake_dep.py
      ${glslc_hlsl_filename}.d
    COMMAND ${CMAKE_COMMAND} -E
      copy_if_different
        ${glslc_hlsl_filename}.d.cmake.tmp
        ${glslc_hlsl_filename}.d.cmake
  )
  set(${result} ${glslc_hlsl_filename} PARENT_SCOPE)
endfunction(compile_hlsl_using_glslc)


macro(add_vulkan_subdirectory dir)
  if (NOT ANDROID OR NOT CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
    add_subdirectory(${dir})
  else()
    if (NOT next_sub_index)
      set(next_sub_index 0)
    endif()
    add_subdirectory(${dir} ${next_sub_index})
    math(EXPR next_sub_index "${next_sub_index} + 1")
  endif()
endmacro()

function(add_shader_library target)
cmake_parse_arguments(LIB "" "" "SOURCES;SHADER_DEPS" ${ARGN})
if (BUILD_APKS)
  add_custom_target(${target})
  set(ABSOLUTE_SOURCES)
  foreach(SOURCE ${LIB_SOURCES})
    get_filename_component(TEMP ${SOURCE} ABSOLUTE)
    list(APPEND ABSOLUTE_SOURCES ${TEMP})
  endforeach()
  set_target_properties(${target} PROPERTIES LIB_SRCS "${ABSOLUTE_SOURCES}")
  if (LIB_SHADER_DEPS)
    foreach(SHADER_DEP ${LIB_SHADER_DEPS})
      get_filename_component(TEMP ${SHADER_DEP} ABSOLUTE)
      set_target_properties(${target} PROPERTIES LIB_DEPS "${TEMP}")
    endforeach()
  else()
    set_target_properties(${target} PROPERTIES LIB_DEPS "")
  endif()
else()
  set(output_files)
  foreach(shader ${LIB_SOURCES})
    get_filename_component(suffix ${shader} EXT)
    if (${suffix} MATCHES "\\.vert|\\.frag|\\.geom|\\.comp|\\.tesc|\\.tese")
      get_filename_component(temp ${shader} ABSOLUTE)
      file(RELATIVE_PATH rel_pos ${CMAKE_CURRENT_SOURCE_DIR} ${temp})
      set(output_file ${CMAKE_CURRENT_BINARY_DIR}/${rel_pos}.spv)
      get_filename_component(output_file ${output_file} ABSOLUTE)


      if (NOT EXISTS ${output_file}.d.cmake)
          execute_process(
            COMMAND
              ${CMAKE_COMMAND} -E copy
              ${vulkan_fem_SOURCE_DIR}/cmake/empty_cmake_dep.in.cmake
              ${output_file}.d.cmake)
      endif()

      include(${output_file}.d.cmake)

      set(ADDITIONAL_ARGS "")
      if (LIB_SHADER_DEPS)
        foreach(DEP ${LIB_SHADER_DEPS})
          if(NOT TARGET ${DEP})
            message(FATAL_ERROR "Could not find dependent shader library ${DEP}")
          endif()
          get_target_property(SRC_DIR ${DEP} SHADER_SOURCE_DIR)
          if(NOT SRC_DIR)
            message(FATAL_ERROR "Could not get shader source dir for ${DEP}")
          endif()
          list(APPEND ADDITIONAL_ARGS "-I${SRC_DIR}")
        endforeach()
      endif()

      get_filename_component(shader_name ${shader} NAME)

      if (${shader_name} MATCHES ".*\\.hlsl\\..*")
        # If an HLSL shader is found, compile it using glslc and DXC
        compile_hlsl_using_dxc(${shader} ${output_file} result)
        list(APPEND output_files ${result})
        compile_hlsl_using_glslc(${shader} ${output_file} result)
        list(APPEND output_files ${result})
      else()
        # Compile GLSL shaders through glslc
        compile_glsl_using_glslc(${shader} ${output_file})
        list(APPEND output_files ${output_file})
      endif()
    endif()
  endforeach()
  add_custom_target(${target}
    DEPENDS ${output_files})
  setup_folders(${target})
  set_target_properties(${target} PROPERTIES SHADER_OUT_FILES
    "${output_files}")
  set_target_properties(${target} PROPERTIES SHADER_LIB_DIR
    "${CMAKE_CURRENT_BINARY_DIR}")
  get_filename_component(TEMP ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
  set_target_properties(${target} PROPERTIES SHADER_SOURCE_DIR
    "${TEMP}")
endif()
endfunction()

function(add_shader_library target)
  cmake_parse_arguments(LIB "" "" "SOURCES;SHADER_DEPS" ${ARGN})
  if (BUILD_APKS)
    add_custom_target(${target})
    set(ABSOLUTE_SOURCES)
    foreach(SOURCE ${LIB_SOURCES})
      get_filename_component(TEMP ${SOURCE} ABSOLUTE)
      list(APPEND ABSOLUTE_SOURCES ${TEMP})
    endforeach()
    set_target_properties(${target} PROPERTIES LIB_SRCS "${ABSOLUTE_SOURCES}")
    if (LIB_SHADER_DEPS)
      foreach(SHADER_DEP ${LIB_SHADER_DEPS})
        get_filename_component(TEMP ${SHADER_DEP} ABSOLUTE)
        set_target_properties(${target} PROPERTIES LIB_DEPS "${TEMP}")
      endforeach()
    else()
      set_target_properties(${target} PROPERTIES LIB_DEPS "")
    endif()
  else()
    set(output_files)
    foreach(shader ${LIB_SOURCES})
      get_filename_component(suffix ${shader} EXT)
      if (${suffix} MATCHES "\\.vert|\\.frag|\\.geom|\\.comp|\\.tesc|\\.tese")
        get_filename_component(temp ${shader} ABSOLUTE)
        file(RELATIVE_PATH rel_pos ${CMAKE_CURRENT_SOURCE_DIR} ${temp})
        set(output_file ${CMAKE_CURRENT_BINARY_DIR}/${rel_pos}.spv)
        get_filename_component(output_file ${output_file} ABSOLUTE)


        if (NOT EXISTS ${output_file}.d.cmake)
            execute_process(
              COMMAND
                ${CMAKE_COMMAND} -E copy
                ${vulkan_fem_SOURCE_DIR}/cmake/empty_cmake_dep.in.cmake
                ${output_file}.d.cmake)
        endif()

        include(${output_file}.d.cmake)

        set(ADDITIONAL_ARGS "")
        if (LIB_SHADER_DEPS)
          foreach(DEP ${LIB_SHADER_DEPS})
            if(NOT TARGET ${DEP})
              message(FATAL_ERROR "Could not find dependent shader library ${DEP}")
            endif()
            get_target_property(SRC_DIR ${DEP} SHADER_SOURCE_DIR)
            if(NOT SRC_DIR)
              message(FATAL_ERROR "Could not get shader source dir for ${DEP}")
            endif()
            list(APPEND ADDITIONAL_ARGS "-I${SRC_DIR}")
          endforeach()
        endif()

        get_filename_component(shader_name ${shader} NAME)

        if (${shader_name} MATCHES ".*\\.hlsl\\..*")
          # If an HLSL shader is found, compile it using glslc and DXC
          compile_hlsl_using_dxc(${shader} ${output_file} result)
          list(APPEND output_files ${result})
          compile_hlsl_using_glslc(${shader} ${output_file} result)
          list(APPEND output_files ${result})
        else()
          # Compile GLSL shaders through glslc
          compile_glsl_using_glslc(${shader} ${output_file})
          list(APPEND output_files ${output_file})
        endif()
      endif()
    endforeach()
    add_custom_target(${target}
      DEPENDS ${output_files})
    setup_folders(${target})
    set_target_properties(${target} PROPERTIES SHADER_OUT_FILES
      "${output_files}")
    set_target_properties(${target} PROPERTIES SHADER_LIB_DIR
      "${CMAKE_CURRENT_BINARY_DIR}")
    get_filename_component(TEMP ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
    set_target_properties(${target} PROPERTIES SHADER_SOURCE_DIR
      "${TEMP}")
  endif()
endfunction()

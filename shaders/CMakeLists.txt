cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(shaders VERSION 0.1.0)

set(GLSL_VALIDATOR "glslangValidator")

file(GLOB_RECURSE GLSL_SOURCE_FILES
  "*.frag"
  "*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    shaders_build
    DEPENDS ${SPIRV_BINARY_FILES}
    )

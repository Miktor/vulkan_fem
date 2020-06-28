cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(vulkan_fem VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-DWIN32_LEAN_AND_MEAN)
ELSE()
    add_compile_options ("-Wall" "-Wextra" "-Wpedantic" "-Werror")
ENDIF()

IF(MSVC)
    add_definitions(-DNOMINMAX)
ENDIF()

find_package(Vulkan REQUIRED)
find_package(GLM REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(Vulkan::Vulkan)
include_directories(${GLM_INCLUDE_DIR})

add_executable(vulkan_fem src/main.cpp)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_compile_definitions(vulkan_fem PRIVATE VK_USE_PLATFORM_WIN32_KHR)
ELSE()
    target_compile_definitions(vulkan_fem PRIVATE VK_USE_PLATFORM_MACOS_MVK)
ENDIF()

target_link_libraries(vulkan_fem PRIVATE Vulkan::Vulkan)
target_link_libraries(vulkan_fem PRIVATE glfw)
target_link_libraries(vulkan_fem PRIVATE Eigen3::Eigen)


set(GLSL_VALIDATOR "glslangValidator")

file(GLOB_RECURSE GLSL_SOURCE_FILES
  "shaders/*.frag"
  "shaders/*.vert"
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
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(vulkan_fem Shaders)

add_custom_command(TARGET vulkan_fem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:vulkan_fem>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:vulkan_fem>/shaders"
        )

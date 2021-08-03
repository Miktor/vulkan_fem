cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(vulkan_fem VERSION 0.1.0)

enable_language(CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_compile_options(-std=c++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-DWIN32_LEAN_AND_MEAN)
ELSEIF(APPLE)
    add_compile_options ( "-Wno-nullability-extension" "-Wno-missing-field-initializers")  ## "-Wall" "-Wextra" "-Wpedantic" "-Werror"
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_MACOS_MVK")
ENDIF()


IF(MSVC)
    add_definitions(-DNOMINMAX)
ENDIF()

find_package(Vulkan REQUIRED)
find_package(GLM REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(spdlog REQUIRED)

include_directories(${GLM_INCLUDE_DIR})

file(GLOB VULKAN_FEM_SRC
     "*.h"
     "*.cpp"
)

AUX_SOURCE_DIRECTORY(src VULKAN_FEM_SRC)
add_executable(vulkan_fem ${VULKAN_FEM_SRC})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_compile_definitions(vulkan_fem PRIVATE VK_USE_PLATFORM_WIN32_KHR)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
ELSE()
    target_compile_definitions(vulkan_fem PRIVATE VK_USE_PLATFORM_MACOS_MVK)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_MACOS_MVK)
ENDIF()

target_include_directories(vulkan_fem PRIVATE Vulkan::Vulkan)

target_link_libraries(vulkan_fem PRIVATE Vulkan::Vulkan)
target_link_libraries(vulkan_fem PRIVATE glfw)
target_link_libraries(vulkan_fem PRIVATE Eigen3::Eigen)
target_link_libraries(vulkan_fem PRIVATE spdlog::spdlog)

add_subdirectory(shaders)

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)

add_custom_target(
        clangformat
        COMMAND /usr/local/opt/llvm@11/bin/clang-format 
        -i
        ${ALL_SOURCE_FILES}
)
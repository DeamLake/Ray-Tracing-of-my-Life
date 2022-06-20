# CMakeList.txt: 顶层 CMageoke 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.15)

set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(object_dir ${curr_dir}/src/Object)
set(render_dir ${curr_dir}/src/Render)
set(utils_dir ${curr_dir}/src/Utils)

project(tinyRaytracing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Debug和Release output dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/out)

# external library
include_directories(${CMAKE_SOURCE_DIR}/3rdParty)

# Add header file include directories
include_directories(
    ${object_dir}
    ${render_dir}
    ${utils_dir}
)

# Add block directories
add_subdirectory(src/Object)
add_subdirectory(src/Render)
add_subdirectory(src/Utils)

# Target
add_executable(Sandbox "src/SandBox.cpp")
target_link_libraries(Sandbox object render utils "../../../lib/tbb1/tbb12_debug" "../../../lib/tbb2/tbb12")


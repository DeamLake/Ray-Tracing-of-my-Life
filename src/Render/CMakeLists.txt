set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${curr_dir} Render_SRCS)

add_library(render ${Render_SRCS}  "ray.h")
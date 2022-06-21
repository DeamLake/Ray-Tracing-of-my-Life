set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${curr_dir} Utils_SRCS)

add_library(utils ${Utils_SRCS} "stb_raw_image.h")
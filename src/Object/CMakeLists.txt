set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${curr_dir} Object_SRCS)

add_library(object ${Object_SRCS} "material.h" "sphere.h" "texture.h" "aarect.h" "perlin.h")
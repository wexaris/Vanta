cmake_minimum_required(VERSION 3.20)

add_library(filewatch INTERFACE IMPORTED GLOBAL)
set_target_properties(filewatch PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

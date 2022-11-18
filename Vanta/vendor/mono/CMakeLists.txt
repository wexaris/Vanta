cmake_minimum_required(VERSION 3.20)

add_library(mono STATIC IMPORTED GLOBAL)
set_target_properties(mono PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_CONFIGURATION_TYPES}/libmono-static-sgen.lib"
    INTERFACE_LINK_LIBRARIES "Ws2_32.lib;Winmm.lib;Version.lib;Bcrypt.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

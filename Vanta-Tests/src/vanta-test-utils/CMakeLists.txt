add_library(VantaTestUtils STATIC
    src/TestHarness.cpp
)

target_include_directories(VantaTestUtils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(VantaTestUtils PUBLIC Vanta)

set_target_properties(VantaTestUtils PROPERTIES
    FOLDER "Tests"
)

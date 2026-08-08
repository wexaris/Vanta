add_executable(NativeScriptTests "NativeScriptTests.cpp")
add_executable(NativeScriptNoComponentTests "NativeScriptNoComponentTests.cpp")

vanta_configure_test_target(NativeScriptTests)
vanta_configure_test_target(NativeScriptNoComponentTests)

vanta_add_native_script_module(NativeScriptTestAssembly
    TYPE SHARED
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/NativeScriptTestAssembly.cpp"
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../../../Vanta-ScriptCore-Native"
    WHOLE_ARCHIVE_LINK_LIBRARIES Vanta-ScriptCore-Native
    OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

vanta_add_native_script_module(NoComponentUsageNativeScriptAssembly
    TYPE SHARED
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/NoComponentUsageNativeScriptAssembly.cpp"
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../../../Vanta-ScriptCore-Native"
    WHOLE_ARCHIVE_LINK_LIBRARIES Vanta-ScriptCore-Native
    OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

add_dependencies(NativeScriptTests NativeScriptTestAssembly)
add_dependencies(NativeScriptTests NoComponentUsageNativeScriptAssembly)
add_dependencies(NativeScriptNoComponentTests NoComponentUsageNativeScriptAssembly)

add_test(
    NAME NativeScriptBuildFixture
    COMMAND "${CMAKE_COMMAND}"
    -D VANTA_LIB_DIR:PATH=${CMAKE_BINARY_DIR}/bin/Resources/Scripts/Native
    -D VANTA_INCLUDE_DIR:PATH=${CMAKE_BINARY_DIR}/bin/Resources/Scripts/Native/include
    -D FIXTURE_ROOT:PATH=${CMAKE_CURRENT_SOURCE_DIR}/../../fixtures/NativeScriptBuildFixture
    -D FIXTURE_BUILD_DIR:PATH=${CMAKE_CURRENT_SOURCE_DIR}/../../fixtures/NativeScriptBuildFixture/build/${CMAKE_BUILD_TYPE}
    -D CMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -D CMAKE_GENERATOR:STRING=${CMAKE_GENERATOR}
    -D CMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM}
    -D CMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -D CMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -D CMAKE_RC_COMPILER:FILEPATH=${CMAKE_RC_COMPILER}
    -D CMAKE_MT:FILEPATH=${CMAKE_MT}
    -P "${CMAKE_CURRENT_SOURCE_DIR}/../../fixtures/RunNativeScriptFixture.cmake"
)

add_test(
    NAME NativeScriptRuntimeMetadata
    COMMAND $<TARGET_FILE:NativeScriptTests>
)

add_test(
    NAME NativeScriptNoComponentMetadata
    COMMAND $<TARGET_FILE:NativeScriptNoComponentTests>
)

set_tests_properties(NativeScriptRuntimeMetadata PROPERTIES
    DEPENDS NativeScriptBuildFixture
)

set_tests_properties(NativeScriptNoComponentMetadata PROPERTIES
    DEPENDS NativeScriptBuildFixture
)
if(NOT DEFINED VANTA_LIB_DIR)
    message(FATAL_ERROR "VANTA_LIB_DIR must be provided")
endif()

if(NOT DEFINED VANTA_INCLUDE_DIR)
    message(FATAL_ERROR "VANTA_INCLUDE_DIR must be provided")
endif()

if(NOT DEFINED FIXTURE_ROOT)
    message(FATAL_ERROR "FIXTURE_ROOT must be provided")
endif()

if(NOT DEFINED FIXTURE_BUILD_DIR)
    message(FATAL_ERROR "FIXTURE_BUILD_DIR must be provided")
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(EXISTS "${FIXTURE_BUILD_DIR}")
    file(REMOVE_RECURSE "${FIXTURE_BUILD_DIR}")
endif()

if(WIN32)
    set(vsdevcmd "C:/Program Files/Microsoft Visual Studio/18/Community/Common7/Tools/VsDevCmd.bat")
    if(NOT EXISTS "${vsdevcmd}")
        message(FATAL_ERROR "VsDevCmd not found at expected path: ${vsdevcmd}")
    endif()

    file(MAKE_DIRECTORY "${FIXTURE_BUILD_DIR}")
    set(driver_bat "${FIXTURE_BUILD_DIR}/run_fixture_build.bat")
    file(WRITE "${driver_bat}" "@echo off\n")
    file(APPEND "${driver_bat}" "call \"${vsdevcmd}\" -arch=x64 -host_arch=x64\n")
    file(APPEND "${driver_bat}" "if errorlevel 1 exit /b 1\n")
    file(APPEND "${driver_bat}" "\"${CMAKE_COMMAND}\" -S \"${FIXTURE_ROOT}\" -B \"${FIXTURE_BUILD_DIR}\" -DVANTA_LIB:STRING=${VANTA_LIB_DIR} -DVANTA_INCLUDE:STRING=${VANTA_INCLUDE_DIR} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -G \"${CMAKE_GENERATOR}\"\n")
    file(APPEND "${driver_bat}" "if errorlevel 1 exit /b 1\n")
    file(APPEND "${driver_bat}" "\"${CMAKE_COMMAND}\" --build \"${FIXTURE_BUILD_DIR}\" --config ${CMAKE_BUILD_TYPE}\n")
    file(APPEND "${driver_bat}" "exit /b %errorlevel%\n")

    execute_process(
        COMMAND cmd /c "${driver_bat}"
        RESULT_VARIABLE fixture_result
        OUTPUT_VARIABLE fixture_output
        ERROR_VARIABLE fixture_error
    )

    if(NOT fixture_result EQUAL 0)
        message(FATAL_ERROR "Fixture configure/build failed:\n${fixture_output}\n${fixture_error}")
    endif()
else()
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -S "${FIXTURE_ROOT}" -B "${FIXTURE_BUILD_DIR}" -DVANTA_LIB:STRING=${VANTA_LIB_DIR} -DVANTA_INCLUDE:STRING=${VANTA_INCLUDE_DIR} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -G "${CMAKE_GENERATOR}" -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM} -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER} -DCMAKE_RC_COMPILER:FILEPATH=${CMAKE_RC_COMPILER} -DCMAKE_MT:FILEPATH=${CMAKE_MT}
        RESULT_VARIABLE configure_result
        OUTPUT_VARIABLE configure_output
        ERROR_VARIABLE configure_error
    )

    if(NOT configure_result EQUAL 0)
        message(FATAL_ERROR "Fixture configure failed:\n${configure_output}\n${configure_error}")
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${FIXTURE_BUILD_DIR}" --config ${CMAKE_BUILD_TYPE}
        RESULT_VARIABLE build_result
        OUTPUT_VARIABLE build_output
        ERROR_VARIABLE build_error
    )

    if(NOT build_result EQUAL 0)
        message(FATAL_ERROR "Fixture build failed:\n${build_output}\n${build_error}")
    endif()
endif()

if(NOT EXISTS "${FIXTURE_BUILD_DIR}/Binaries/Scripts_Native.dll")
    message(FATAL_ERROR "Fixture output missing: ${FIXTURE_BUILD_DIR}/Binaries/Scripts_Native.dll")
endif()

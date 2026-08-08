function(vanta_add_native_script_module target)
    set(options)
    set(oneValueArgs TYPE OUTPUT_DIRECTORY)
    set(multiValueArgs SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES WHOLE_ARCHIVE_LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(VANTA_SCRIPT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT VANTA_SCRIPT_TYPE)
        set(VANTA_SCRIPT_TYPE SHARED)
    endif()

    add_library(${target} ${VANTA_SCRIPT_TYPE} ${VANTA_SCRIPT_SOURCES})

    if(VANTA_SCRIPT_INCLUDE_DIRECTORIES)
        target_include_directories(${target} PUBLIC ${VANTA_SCRIPT_INCLUDE_DIRECTORIES})
    endif()

    if(VANTA_SCRIPT_LINK_LIBRARIES)
        target_link_libraries(${target} PRIVATE ${VANTA_SCRIPT_LINK_LIBRARIES})
    endif()

    if(VANTA_SCRIPT_WHOLE_ARCHIVE_LINK_LIBRARIES)
        foreach(lib IN LISTS VANTA_SCRIPT_WHOLE_ARCHIVE_LINK_LIBRARIES)
            if(TARGET ${lib})
                set(lib_path "$<TARGET_FILE:${lib}>")
                target_link_libraries(${target} PRIVATE ${lib})
            else()
                set(lib_path "${lib}")
            endif()

            if(MSVC)
                target_link_options(${target} PRIVATE "/WHOLEARCHIVE:${lib_path}")
            elseif(APPLE)
                target_link_options(${target} PRIVATE "-Wl,-force_load,${lib_path}")
            else()
                target_link_options(${target} PRIVATE "-Wl,--whole-archive" "${lib_path}" "-Wl,--no-whole-archive")
            endif()
        endforeach()
    endif()

    target_compile_definitions(${target} PUBLIC VANTA_MODULE)

    if(VANTA_SCRIPT_COMPILE_DEFINITIONS)
        target_compile_definitions(${target} PRIVATE ${VANTA_SCRIPT_COMPILE_DEFINITIONS})
    endif()

    if(VANTA_SCRIPT_OUTPUT_DIRECTORY)
        set(output_dir "${VANTA_SCRIPT_OUTPUT_DIRECTORY}")
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${output_dir}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${output_dir}"
            ARCHIVE_OUTPUT_DIRECTORY "${output_dir}"
            ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${output_dir}"
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${output_dir}"
            LIBRARY_OUTPUT_DIRECTORY "${output_dir}"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${output_dir}"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${output_dir}"
        )
    endif()
endfunction()

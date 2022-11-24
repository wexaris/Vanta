cmake_minimum_required(VERSION 3.20)
project(Vanta-Script VERSION 0.1.0 LANGUAGES CSharp)

set(${PROJECT_NAME}_SOURCE
    "Source/Components.cs"
    "Source/Entity.cs"
    "Source/Input.cs"
    "Source/Internal.cs"
    "Source/KeyCodes.cs"
    "Source/Log.cs"
    "Source/Math.cs"
    "Source/Player.cs"
)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCE})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DOTNET_REFERENCES
    "Microsoft.CSharp"
    "PresentationCore"
    "PresentationFramework"
    "System"
    "System.Core"
    "System.Data"
    "System.Data.DataSetExtensions"
    "System.Net.Http"
    "System.Xaml"
    "System.Xml"
    "System.Xml.Linq"
    "WindowsBase"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"

    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"

    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Resources/Scripts"
)

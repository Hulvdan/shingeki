cmake_minimum_required(VERSION 3.12)
project(game CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(GRAPHICS_API_OPENGL_43)

#-----------------------------------------------------------------------------------
# Dependencies.
#-----------------------------------------------------------------------------------

set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# SHIET REMOVE ME
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/libraries/raygui")

#-----------------------------------------------------------------------------------
# Game.
#-----------------------------------------------------------------------------------
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/vendor/libraries/doctest")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    DEPENDS ${PROJECT_NAME})

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib raygui_cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    DOCTEST_CONFIG_DISABLE
)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC "-sUSE_GLFW=3")
endif()

if (WIN32)
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VS_DEBUGGER_COMMAND           "$<TARGET_FILE:${PROJECT_NAME}>"
        VS_DEBUGGER_ENVIRONMENT       "PATH=%PATH%;${CMAKE_PREFIX_PATH}/bin"
    )
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

#-----------------------------------------------------------------------------------
# Tests.
#-----------------------------------------------------------------------------------
add_executable(tests src/tests.cpp)
target_include_directories(tests PRIVATE "${PROJECT_SOURCE_DIR}/vendor/libraries/doctest")
target_compile_definitions(tests PRIVATE TESTS)

set_target_properties(tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

#set(raylib_VERBOSE 1)
target_link_libraries(tests raylib raygui_cpp)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(tests "-framework IOKit")
    target_link_libraries(tests "-framework Cocoa")
    target_link_libraries(tests "-framework OpenGL")
endif()

#-----------------------------------------------------------------------------------
# Enabling Linting On Win32.
#-----------------------------------------------------------------------------------
IF(WIN32 AND CMAKE_GENERATOR STREQUAL Ninja)
    include_directories("${PROJECT_SOURCE_DIR}/vendor/libraries/doctest")
ENDIf()


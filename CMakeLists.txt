cmake_minimum_required(VERSION 3.10)
project(KolosalDesktop)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Windows-specific configurations
if(WIN32)
    # Disable min/max macros from Windows.h
    add_definitions(-DNOMINMAX)
    # Use Unicode character set
    add_definitions(-DUNICODE -D_UNICODE)
    # Prevent warning about unsafe strcpy etc.
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Set OpenGL preference
set(OpenGL_GL_PREFERENCE GLVND)

# Find OpenGL
find_package(OpenGL REQUIRED)
if(NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found")
else()
    message(STATUS "Found OpenGL: ${OpenGL_INCLUDE_DIR}")
endif()

# GLFW options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Only enable Wayland and X11 on non-Windows platforms
if(NOT WIN32)
    option(GLFW_BUILD_WAYLAND "Build GLFW with Wayland support" ON)
    option(GLFW_BUILD_X11 "Build GLFW with X11 support" ON)
endif()

# Add GLFW
add_subdirectory(external/glfw)

# Add GLAD
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# Add ImGui source files
set(IMGUI_DIR external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Create Kolosal static library
add_library(kolosal_lib STATIC kolosal.cpp ${IMGUI_SOURCES})

# Include directories for Kolosal library
target_include_directories(kolosal_lib PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${OPENGL_INCLUDE_DIR}
    external/glad/include
)

# Link libraries to Kolosal library
target_link_libraries(kolosal_lib PUBLIC
    glfw
    ${OPENGL_LIBRARIES}
    glad
)

# Windows-specific libraries
if(WIN32)
    target_link_libraries(kolosal_lib PUBLIC
        opengl32
        gdi32
        shell32
        user32
    )
endif()

# Add compile definitions for ImGui
target_compile_definitions(kolosal_lib PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

# Define the font path relative to the executable
if(WIN32)
    target_compile_definitions(kolosal_lib PUBLIC
        IMGUI_FONT_PATH="./Inter-Regular.ttf"
    )
else()
    target_compile_definitions(kolosal_lib PUBLIC
        IMGUI_FONT_PATH="Inter-Regular.ttf"
    )
endif()

# Create the main executable
add_executable(KolosalDesktop main.cpp)

# Windows-specific executable properties
if(WIN32)
    # Set the Windows subsystem to GUI application
    set_target_properties(KolosalDesktop PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Add a Windows resource file if you have one
    # set(RC_FILE "path/to/your/resource.rc")
    # target_sources(KolosalDesktop PRIVATE ${RC_FILE})
endif()

# Include directories for the main executable
target_include_directories(KolosalDesktop PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${OPENGL_INCLUDE_DIR}
    external/glad/include
)

# Link the Kolosal library to the main executable
target_link_libraries(KolosalDesktop PRIVATE kolosal_lib)

# Path to the font file
set(FONT_FILE "${CMAKE_SOURCE_DIR}/external/fonts/Inter-Regular.ttf")

# Add custom command to copy the font to the build directory
add_custom_command(
    TARGET KolosalDesktop POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE}" "$<TARGET_FILE_DIR:KolosalDesktop>/Inter-Regular.ttf"
)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(tests)
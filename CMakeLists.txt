cmake_minimum_required(VERSION 3.10)
project(KolosalDesktop)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# Wayland and X11 options
option(GLFW_BUILD_WAYLAND "Build GLFW with Wayland support" ON)
option(GLFW_BUILD_X11 "Build GLFW with X11 support" ON)

# Add GLFW
add_subdirectory(external/glfw)

# Add GLAD
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# Add NFD
add_subdirectory(external/nativefiledialog-extended)

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
    external/icons
    external/nlohmann
    external/nativefiledialog-extended/src/include  # Corrected path to NFD
)

# Link libraries to Kolosal library
target_link_libraries(kolosal_lib PUBLIC
    glfw
    ${OPENGL_LIBRARIES}
    glad
    nfd  # Make sure to link the NFD library
)

# Add compile definitions for ImGui
target_compile_definitions(kolosal_lib PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

# Define the font paths relative to the executable
set(FONT_FILE_INTER_REGULAR "${CMAKE_SOURCE_DIR}/external/fonts/Inter-Regular.ttf")
set(FONT_FILE_FIRACODE_REGULAR "${CMAKE_SOURCE_DIR}/external/fonts/FiraCode-Regular.ttf")
set(FONT_FILE_INTER_BOLD "${CMAKE_SOURCE_DIR}/external/fonts/Inter-Bold.ttf")
set(FONT_FILE_INTER_BOLDITALIC "${CMAKE_SOURCE_DIR}/external/fonts/Inter-BoldItalic.ttf")
set(FONT_FILE_INTER_ITALIC "${CMAKE_SOURCE_DIR}/external/fonts/Inter-Italic.ttf")
set(FONT_FILE_FA_BRANDS "${CMAKE_SOURCE_DIR}/external/fonts/fa-brands-400.ttf")
set(FONT_FILE_FA_REGULAR "${CMAKE_SOURCE_DIR}/external/fonts/fa-regular-400.ttf")
set(FONT_FILE_FA_SOLID "${CMAKE_SOURCE_DIR}/external/fonts/fa-solid-900.ttf")

# Add compile definitions for each font
target_compile_definitions(kolosal_lib PUBLIC
    IMGUI_FONT_PATH_INTER_REGULAR="Inter-Regular.ttf"
    IMGUI_FONT_PATH_FIRACODE_REGULAR="FiraCode-Regular.ttf"
    IMGUI_FONT_PATH_INTER_BOLD="Inter-Bold.ttf"
    IMGUI_FONT_PATH_INTER_BOLDITALIC="Inter-BoldItalic.ttf"
    IMGUI_FONT_PATH_INTER_ITALIC="Inter-Italic.ttf"
    IMGUI_FONT_PATH_FA_BRANDS="fa-brands-400.ttf"
    IMGUI_FONT_PATH_FA_REGULAR="fa-regular-400.ttf"
    IMGUI_FONT_PATH_FA_SOLID="fa-solid-900.ttf"
)

# Create the main executable
add_executable(KolosalDesktop main.cpp)

# Include directories for the main executable
target_include_directories(KolosalDesktop PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${OPENGL_INCLUDE_DIR}
    external/glad/include
    external/icons
    external/nlohmann
    external/nativefiledialog-extended/src/include  # Corrected path for NFD
)

# Link libraries to the main executable
target_link_libraries(KolosalDesktop PRIVATE
    kolosal_lib
)

# Add custom commands to copy the fonts to the build directory
add_custom_command(
    TARGET KolosalDesktop POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_INTER_REGULAR}" "$<TARGET_FILE_DIR:KolosalDesktop>/Inter-Regular.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_FIRACODE_REGULAR}" "$<TARGET_FILE_DIR:KolosalDesktop>/FiraCode-Regular.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_INTER_BOLD}" "$<TARGET_FILE_DIR:KolosalDesktop>/Inter-Bold.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_INTER_BOLDITALIC}" "$<TARGET_FILE_DIR:KolosalDesktop>/Inter-BoldItalic.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_INTER_ITALIC}" "$<TARGET_FILE_DIR:KolosalDesktop>/Inter-Italic.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_FA_BRANDS}" "$<TARGET_FILE_DIR:KolosalDesktop>/fa-brands-400.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_FA_REGULAR}" "$<TARGET_FILE_DIR:KolosalDesktop>/fa-regular-400.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${FONT_FILE_FA_SOLID}" "$<TARGET_FILE_DIR:KolosalDesktop>/fa-solid-900.ttf"
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

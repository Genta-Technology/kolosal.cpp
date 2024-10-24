# Add the include directories for GLFW, GLAD, ImGui, and the source directory
include_directories(
    ${CMAKE_SOURCE_DIR}/external/glfw/include
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_SOURCE_DIR}  # For kolosal.h
)

# Add the test executable
add_executable(kolosal_test kolosal_test.cpp)

# Link the necessary libraries: GoogleTest and kolosal_lib
target_link_libraries(kolosal_test
    gtest_main
    gmock_main
    kolosal_lib  # Link against the Kolosal library
)

# Ensure the target uses C++17
set_target_properties(kolosal_test PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Include GoogleTest and enable the discovery of tests
include(GoogleTest)
gtest_discover_tests(kolosal_test)

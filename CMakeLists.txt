cmake_minimum_required(VERSION 3.10)
project(VPSSimulator CXX)

set(CMAKE_CXX_STANDARD 17)

# Find system libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# ImGui source files
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Create executable
add_executable(${PROJECT_NAME} 
src/main.cpp 
src/physics/HeatSolver.cpp
src/profile/ReflowProfile.cpp
 ${IMGUI_SOURCES})

# Include directories so we can #include "imgui.h" etc
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)
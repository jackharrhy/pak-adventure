cmake_minimum_required(VERSION 3.10)
project(PakViewer)

set(CMAKE_CXX_STANDARD 23)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)

# Add libzip from submodule
add_subdirectory(third_party/libzip)

# Add spdlog from submodule
add_subdirectory(third_party/spdlog)

# Add ImGui library
add_library(imgui
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
    third_party/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/third_party/imgui
)

# Add tinyfiledialogs library
add_library(tinyfiledialogs
    third_party/tinyfiledialogs/tinyfiledialogs.c
)

target_include_directories(tinyfiledialogs PUBLIC
    ${CMAKE_SOURCE_DIR}/third_party/tinyfiledialogs
)

# Link GLFW with ImGui
target_link_libraries(imgui PUBLIC glfw)

add_executable(PakViewer
    src/main.cpp
    src/parser/pcx.cpp
    src/parser/pak.cpp
    src/parser/pkzip.cpp
    src/parser/wal.cpp
    src/parser/text.cpp
    src/parser/binary.cpp
    src/parser/registry.cpp
    src/parser/stb.cpp
)
target_include_directories(PakViewer PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/parser
    ${CMAKE_SOURCE_DIR}/third_party/libzip/lib
    ${CMAKE_SOURCE_DIR}/third_party/stb
)
target_link_libraries(PakViewer glfw OpenGL::GL ZLIB::ZLIB zip imgui tinyfiledialogs spdlog::spdlog_header_only)

cmake_minimum_required(VERSION 3.5)
project(boo)

add_subdirectory(lib/oead)
add_subdirectory(lib/raylib)
add_subdirectory(lib/nativefiledialog-extended)

add_library(imgui STATIC
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui_raylib/imgui_impl_raylib.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)

include_directories(src/include)
include_directories(lib/inih)
include_directories(lib/raylib/src)
include_directories(lib/imgui_raylib)
include_directories(lib/imgui)
include_directories(lib/imgui/backends)
include_directories(lib/nativefiledialog-extended/src/include)

add_executable(boo src/boo.cpp src/config.cpp src/editor.cpp src/objparamdb.cpp src/stage.cpp src/ui.cpp src/include/boo/config.h src/include/boo/editor.h src/include/boo/localization.h src/include/boo/objparamdb.h src/include/boo/stage.h src/include/boo/ui.h)
target_link_libraries(boo PUBLIC oead)
target_link_libraries(boo PUBLIC raylib)
target_link_libraries(boo PUBLIC imgui)
target_link_libraries(boo PRIVATE nfd)

file(COPY res DESTINATION .)
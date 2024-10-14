cmake_minimum_required(VERSION 3.27)

project(Notes)

find_package(fmt REQUIRED)
find_package(SDL3 REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory(external/glad/cmake)

FILE(GLOB IMGUI_SRC
    "external/imgui/im*.*"
    "external/imgui/backends/imgui_impl_sdl*3.*"
    "external/imgui/backends/imgui_impl_opengl3.*"
    "external/imgui/misc/cpp/imgui_stdlib.*"

    "external/imgui_md/imgui_md*"
    
    "external/md4c/src/md4c.*"
)

add_library(ImGui STATIC ${IMGUI_SRC})
target_include_directories(ImGui PUBLIC "include" "external/imgui" "external/imgui/backends" "external/imgui/misc/cpp" "external/md4c/src" "external/imgui_md")
target_link_libraries(ImGui PRIVATE SDL3::SDL3)

glad_add_library(ogl ALIAS API gl:core=4.6)

add_executable(Notes)

target_sources(Notes PRIVATE "src/main.cpp"
    "include/editor/editor.hpp" "src/editor/editor.cpp"
    "include/editor/window.hpp" "src/editor/window.cpp"
)
target_include_directories(Notes PRIVATE "include")
target_link_libraries(Notes PRIVATE fmt::fmt SDL3::SDL3 Eigen3::Eigen ogl ImGui )

set_target_properties(Notes PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)

if(MSVC)
    target_compile_options(editor PRIVATE /W4 /WX)
else()
    target_compile_options(Notes PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-error=unused-parameter -Wno-error=unused-variable)

    # target_compile_options(Notes PRIVATE -fsanitize=undefined -fsanitize=address)
    # target_link_options(Notes PUBLIC -fsanitize=undefined -fsanitize=address)
endif()
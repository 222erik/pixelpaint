cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Pixelpaint LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(pixelpaint
    src/main.c
    src/window.c
    src/canva.c
)

target_include_directories(pixelpaint PRIVATE include)

find_package(SDL3 REQUIRED)
target_link_libraries(pixelpaint PRIVATE SDL3::SDL3)

target_link_options(pixelpaint PRIVATE -dynamic)

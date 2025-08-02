cmake_minimum_required(VERSION 3.10)

project(chip8 C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(chip8 main.c)
target_link_libraries(chip8 ncurses)

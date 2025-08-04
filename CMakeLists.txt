cmake_minimum_required(VERSION 3.10)

project(chip8 C)

set(CMAKE_C_STANDARD 99)
add_compile_options(-Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Raylib installation
set(RL_VERSION 5.5)
find_package(raylib ${RL_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RL_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

add_executable(chip8 main.c)
target_link_libraries(chip8 raylib)

cmake_minimum_required(VERSION 3.7)
project(cdocgen C)
set(CMAKE_C_STANDARD 11)

# FILE(GLOB_RECURSE OsoyanLibSrc osoyanlib/*.c)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -DDEBUG)

add_compile_options(-Wall -Wextra -Wpedantic)

add_compile_options("--coverage")
link_libraries("--coverage")

add_subdirectory(osoyanlib)
add_executable(cdocgen main.c)
target_link_libraries(cdocgen osoyanlib)
cmake_minimum_required(VERSION 3.9.2)
project(cdocgen C)
set(CMAKE_C_STANDARD 11)

FILE(GLOB_RECURSE OsoyanLibSrc osoyanlib/*.c)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -DDEBUG)

add_executable(cdocgen ${OsoyanLibSrc} main.c)
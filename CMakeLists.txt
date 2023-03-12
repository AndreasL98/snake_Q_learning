cmake_minimum_required(VERSION 3.12)
project(snake5)

set(CMAKE_C_STANDARD 99)

# Use the package PkgConfig to detect GTK+ headers/library files
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})

add_executable(snake5 main.c snake.h snake.c)
TARGET_LINK_LIBRARIES(snake5 ${GTK3_LIBRARIES})

add_executable(test snake.h test.c)

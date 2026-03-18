cmake_minimum_required(VERSION 3.10)
project(Simulador-Mini-MIPS-8bits LANGUAGES C)

if(MSVC)
    add_compile_options(/utf-8)
endif()

file(GLOB SOURCES "src/*.c")
add_executable(simulador-mini-mips-8bits ${SOURCES})

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)


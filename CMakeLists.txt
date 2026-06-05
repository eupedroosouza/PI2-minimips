cmake_minimum_required(VERSION 3.10)
project(Simulador-Mini-MIPS-8bits LANGUAGES C)

if (MSVC)
    add_compile_options(/utf-8)
endif ()

file(GLOB SOURCES "src/*.c")
add_executable(simulador-mini-mips-8bits ${SOURCES})

# Curses
if (WIN32)
    find_path(PDCURSES_INCLUDE_DIR NAMES curses.h)
    find_library(PDCURSES_LIBRARY NAMES pdcurses curses)

    if (PDCURSES_INCLUDE_DIR AND PDCURSES_LIBRARY)
        target_include_directories(simulador-mini-mips-8bits PRIVATE ${PDCURSES_INCLUDE_DIR})
        target_link_libraries(simulador-mini-mips-8bits PRIVATE ${PDCURSES_LIBRARY})

        target_compile_definitions(simulador-mini-mips-8bits PRIVATE PDC_DLL_BUILD)
    endif ()
else ()
    set(CURSES_NEED_NCURSES TRUE)
    set(CURSES_NEED_WIDE TRUE)
    find_package(Curses REQUIRED)

    target_link_libraries(simulador-mini-mips-8bits PRIVATE ${CURSES_LIBRARIES})
    if (CURSES_INCLUDE_DIRS)
        target_include_directories(simulador-mini-mips-8bits PRIVATE ${CURSES_INCLUDE_DIRS})
    endif ()
endif ()


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)


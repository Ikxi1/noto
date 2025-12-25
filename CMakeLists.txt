cmake_minimum_required(VERSION 3.16)
project(noto C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")

include_directories(include)

# Find all sources
file(GLOB_RECURSE C_SRC source/*.c)
file(GLOB_RECURSE ASM_SRC source/*.asm)

# Custom rule for NASM
foreach(ASM ${ASM_SRC})
    get_filename_component(OBJ ${ASM} NAME_WE)
    set(OBJ_PATH ${CMAKE_CURRENT_BINARY_DIR}/${OBJ}.o)

    add_custom_command(
        OUTPUT ${OBJ_PATH}
        COMMAND nasm -g -F dwarf -f elf64 -Isource/asm/ ${ASM} -o ${OBJ_PATH}
        DEPENDS ${ASM}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Assembling ${ASM}"
    )

    list(APPEND ASM_OBJ ${OBJ_PATH})
endforeach()

add_executable(noto
    ${C_SRC}
    ${ASM_OBJ}
)

target_link_libraries(noto ncursesw)

cmake_minimum_required(VERSION 3.31)
project(noto C)

set(CMAKE_C_STANDARD 99)


set(SOURCE_FILES
        source/main.c
)

file(GLOB_RECURSE ASM_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.asm)

set(ASM_OBJECTS "")
foreach(ASM_FILE IN LISTS ASM_SOURCES)
    get_filename_component(ASM_NAME ${ASM_FILE} NAME_WE)
    set(OBJ_FILE ${CMAKE_CURRENT_BINARY_DIR}/${ASM_NAME}.o)

    add_custom_command(
            OUTPUT ${OBJ_FILE}
            COMMAND nasm -g -f elf64 -I${CMAKE_CURRENT_SOURCE_DIR}/source/asm ${ASM_FILE} -o ${OBJ_FILE}
            DEPENDS ${ASM_FILE}
            COMMENT "Assembling ${ASM_FILE}"
    )

    list(APPEND ASM_OBJECTS ${OBJ_FILE})
endforeach()

add_custom_target(asm_objects ALL DEPENDS ${ASM_OBJECTS})

add_executable(noto ${SOURCE_FILES})

add_dependencies(noto asm_objects)
target_link_libraries(noto PRIVATE ${ASM_OBJECTS})

target_include_directories(noto PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# for copying settings files, currently none
#add_custom_command(
#        TARGET noto POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_CURRENT_SOURCE_DIR}/source/settings
#        ${CMAKE_CURRENT_BINARY_DIR}/settings
#        COMMENT "Copying settings directory to build directory"
#)

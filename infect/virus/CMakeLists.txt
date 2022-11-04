# cmake -S ./virus -B ./build_virus
cmake_minimum_required (VERSION 3.24)

project (virus)

set(SOURCES 
    virus.c 
    infect.c
    parse.c
    code.c
)

add_executable(virus ${SOURCES})

target_include_directories(
    virus
    PRIVATE ${PROJECT_SOURCE_DIR}/../include
)
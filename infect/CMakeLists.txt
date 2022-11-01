cmake_minimum_required (VERSION 3.24)

project (virus)

set(SOURCES 
    src/virus.c 
    src/get_api.c
    src/infect.c
    src/inject.c
    src/parse.c
)

add_executable(virus ${SOURCES})

target_include_directories(
    virus
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)
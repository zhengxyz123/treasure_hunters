cmake_minimum_required(VERSION 3.15)
project(treasure_hunters C)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

if(MSVC)
    add_compile_options(/Wall)
    add_compile_definitions(_AMD64_)
    add_link_options(/SUBSYSTEM:WINDOWS)
else()
    add_compile_options(-Wall -Werror -O3)
endif()

set(CJSON_DIR ${CMAKE_SOURCE_DIR}/extern/cjson)

add_library(cjson ${CJSON_DIR}/cJSON.c ${CJSON_DIR}/cJSON.h)
include_directories(${CMAKE_SOURCE_DIR}/extern)

aux_source_directory(src SRC_DIR)
aux_source_directory(src/cjson CJSON_DIR)
aux_source_directory(src/map MAP_DIR)
aux_source_directory(src/resources RES_DIR)
aux_source_directory(src/scenes SCENES_DIR)
aux_source_directory(src/ui UI_DIR)

add_executable(main ${SRC_DIR} ${CJSON_DIR} ${MAP_DIR} ${RES_DIR} ${SCENES_DIR} ${UI_DIR})
target_link_libraries(main SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer cjson)

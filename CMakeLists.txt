cmake_minimum_required(VERSION 3.15)
project(treasure_hunters C)

find_package(SDL2 REQUIRED)
if(NOT PSP)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
endif()

if(MSVC)
    add_compile_options(/Wall)
    add_compile_definitions(_AMD64_)
    add_link_options(/SUBSYSTEM:WINDOWS)
else()
    add_compile_options(-Wall -Werror)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CJSON_DIR ${CMAKE_SOURCE_DIR}/external/cjson)

add_library(cjson ${CJSON_DIR}/cJSON.c ${CJSON_DIR}/cJSON.h)
include_directories(${CMAKE_SOURCE_DIR}/external)

aux_source_directory(src SRC_DIR)
aux_source_directory(src/map MAP_DIR)
aux_source_directory(src/resources RES_DIR)
aux_source_directory(src/scenes SCENES_DIR)
aux_source_directory(src/ui UI_DIR)

add_executable(${PROJECT_NAME} ${SRC_DIR} ${MAP_DIR} ${RES_DIR} ${SCENES_DIR} ${UI_DIR})
if(NOT PSP)
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer cjson)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2_image SDL2_mixer SDL2 png jpeg z
        vorbisfile vorbis ogg modplug stdc++
    )
    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH NULL
        BACKGROUND_PATH NULL
        PREVIEW_PATH NULL
        TITLE ${PROJECT_NAME}
    )
endif()

cmake_minimum_required(VERSION 3.15)

option(BUILD_VITA "Build executable files for PS Vita" OFF)
if(BUILD_VITA)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(treasure_hunters C)

if(VITA)
    set(VITA_APP_NAME "Treasure Hunters")
    set(VITA_TITLEID  "TRHT10001")
    set(VITA_VERSION  "01.00")
    set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")
    link_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

if(NOT PSP AND NOT VITA)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
else()
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2)
    pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
    pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
endif()

if(MSVC)
    add_compile_options(/Wall)
    add_compile_definitions(_AMD64_)
    add_link_options(/SUBSYSTEM:WINDOWS)
else()
    add_compile_options(-Wall -Werror)
endif()
if(PSP)
    # generate little-endian code
    add_compile_options(-EL)
    add_compile_definitions(TH_FALLBACK_TO_BITMAP_FONT)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CJSON_DIR ${CMAKE_SOURCE_DIR}/external/cjson)
add_library(cjson ${CJSON_DIR}/cJSON.c ${CJSON_DIR}/cJSON.h)

include_directories(${CMAKE_SOURCE_DIR}/external)
aux_source_directory(src BASE_DIR)
aux_source_directory(src/entities ENTITIES_DIR)
aux_source_directory(src/image IMAGE_DIR)
aux_source_directory(src/resources RES_DIR)
aux_source_directory(src/scenes SCENES_DIR)
aux_source_directory(src/ui UI_DIR)
aux_source_directory(src/ui/text UI_TEXT_DIR)

if(PSP)
    set(RESPACK_FLAGS "--psp")
else()
    set(RESPACK_FLAGS "")
endif()
add_custom_target(
    generate_respack
    COMMAND ${CMAKE_SOURCE_DIR}/tools/respack.py gen ${RESPACK_FLAGS} ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/assets.rpkg
    COMMENT "Generate resource pack"
    VERBATIM
)

add_executable(${PROJECT_NAME} ${BASE_DIR} ${ENTITIES_DIR} ${IMAGE_DIR} ${RES_DIR} ${SCENES_DIR} ${UI_DIR} ${UI_TEXT_DIR})
add_dependencies(${PROJECT_NAME} generate_respack)
if(NOT PSP AND NOT VITA)
    target_link_libraries(${PROJECT_NAME} 
        SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf cjson
    )
else()
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        cjson
    )
    if(PSP)
        create_pbp_file(
            TARGET ${PROJECT_NAME}
            ICON_PATH NULL
            BACKGROUND_PATH NULL
            PREVIEW_PATH NULL
            TITLE "Treasure Hunters"
            VERSION 1.0
        )
    else()
        vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
        vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
            VERSION ${VITA_VERSION}
            NAME ${VITA_APP_NAME}
            FILE ${CMAKE_BINARY_DIR}/assets.rpkg assets.rpkg
        )
    endif()
endif()

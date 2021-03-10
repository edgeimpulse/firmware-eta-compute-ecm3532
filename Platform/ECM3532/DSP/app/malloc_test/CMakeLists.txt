cmake_minimum_required(VERSION 3.12.1)
set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(base_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(APP ${base_name})

#Below line should point to CMakeList of project base directory(dsp directory)
#if application directory in not inside dsp/app, this need to be changed
include(../../CMakeLists.txt)
project(${APP} C ASM)

file (GLOB app "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
foreach (appf ${app})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${appf})
endforeach()

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc")

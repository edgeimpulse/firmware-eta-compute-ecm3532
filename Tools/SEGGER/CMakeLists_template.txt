cmake_minimum_required(VERSION 3.12.1)
set(APP_DIR ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(base_name ${CMAKE_CURRENT_LIST_DIR} NAME)
set(APP ${base_name})

#Below line should point to CMakeList of project base directory(freeertos directory)
set(MCU_DIR "../../Platform/ECM3532/M3")
include(${MCU_DIR}/CMakeLists.txt)
project(${APP} C CXX ASM)

file (GLOB app "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
foreach (appf ${app})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${appf})
endforeach()
target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include")

# Generate SES project files
generate_ses_project(${PROJECT_NAME}.elf)
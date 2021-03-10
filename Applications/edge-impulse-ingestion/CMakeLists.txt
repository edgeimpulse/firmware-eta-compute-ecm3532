cmake_minimum_required(VERSION 3.12.1)
set(APP_DIR ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(base_name ${CMAKE_CURRENT_LIST_DIR} NAME)
set(APP ${base_name})

add_definitions(-DECM3532 -DUSE_PDM_MIC -DUSE_BLE -DUSE_SPIFLASH -DFLASH -DPROJ_NAME=\"edge_impulse_ingestion\" -DEI_SENSOR_AQ_STREAM=FILE -DEIDSP_USE_CMSIS_DSP=1 -DEIDSP_QUANTIZE_FILTERBANK=0 -DARM_MATH_LOOPUNROL -DEI_CLASSIFIER_ALLOCATION_STATIC)

include_directories (../../Thirdparty/edge_impulse/)
include_directories (../../Thirdparty/edge_impulse/ingestion-sdk-platform/eta-compute)
include_directories (../../Thirdparty/edge_impulse/repl)
include_directories (../../Thirdparty/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Include)
include_directories (../../Thirdparty/edge_impulse/edge-impulse-sdk/CMSIS/Core/Include)
include_directories (../../Platform/ECM3532/M3/hw/hal/common/include)
include_directories (../../Platform/ECM3532/Common/framework/inc)
include_directories (../../eta_ai_bsp/inc)
include_directories (${APP_DIR}/src/sensors)
include_directories (../../Platform/ECM3532/M3/framework/executor/include/pub)
include_directories (../../Platform/ECM3532/M3/NN_kernels/include)

#Below line should point to CMakeList of project base directory(freeertos directory)
set(MCU_DIR "../../Platform/ECM3532/M3")
include(${MCU_DIR}/CMakeLists.txt)
project(${APP} C CXX ASM)

file (GLOB app "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
				"${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
file (GLOB sensors "${CMAKE_CURRENT_LIST_DIR}/src/sensors/*.c"
				"${CMAKE_CURRENT_LIST_DIR}/src/sensors/*.cpp")
foreach (appf ${app} ${sensors})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${appf})
endforeach()
target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include")

# Generate SES project files
generate_ses_project(${PROJECT_NAME}.elf)

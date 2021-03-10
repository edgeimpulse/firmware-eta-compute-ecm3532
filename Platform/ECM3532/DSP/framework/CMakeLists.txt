target_sources(${PROJECT_NAME}.elf PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src/m3Interface.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/utility.c")

set(cfgvar "CONFIG_FRAMEWORK")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y _cmp)
if (_cmp)
target_sources(${PROJECT_NAME}.elf PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src/dsp_msg.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/dsp_task.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/workQ_common.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/dsp_malloc.c")
endif()


set(cfgvar "CONFIG_SPI_TIMER")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y _cmp)
if (_cmp)
    target_sources(${PROJECT_NAME}.elf PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src/timer.c")
endif()

target_include_directories(${PROJECT_NAME}.elf PUBLIC
	#    "${CMAKE_CURRENT_LIST_DIR}/inc/common"
    "${CMAKE_CURRENT_LIST_DIR}/inc")

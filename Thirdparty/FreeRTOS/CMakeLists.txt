file(GLOB RTOS "${CMAKE_CURRENT_LIST_DIR}/Source/*.c")
file(GLOB RTOS_PORT "${CMAKE_CURRENT_LIST_DIR}/Source/portable/GCC/ARM_CM3/*.c")
file(GLOB RTOS_MEM_MANG "${CMAKE_CURRENT_LIST_DIR}/Source/portable/MemMang/heap_3.c")
set(getsoc "CONFIG_SOC")
CONFIGVAL(soc ${getsoc})
foreach (rtossrc ${RTOS})
    target_sources(freertos PRIVATE ${rtossrc})
endforeach()

foreach (rtosport ${RTOS_PORT})
    target_sources(freertos PRIVATE ${rtosport})
endforeach()

foreach (rtosmmang ${RTOS_MEM_MANG})
    target_sources(freertos PRIVATE ${rtosmmang})
endforeach()

target_include_directories(freertos PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/Source/include")

target_include_directories(freertos PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/Source/portable/GCC/ARM_CM3")

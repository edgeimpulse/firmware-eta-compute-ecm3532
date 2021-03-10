file (GLOB SYS "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.s")
foreach (syssrc ${SYS})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${syssrc})
endforeach()

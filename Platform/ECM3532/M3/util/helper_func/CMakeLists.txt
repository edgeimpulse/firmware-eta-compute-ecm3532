file (GLOB cli "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
foreach (clif ${cli})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${clif})
endforeach()

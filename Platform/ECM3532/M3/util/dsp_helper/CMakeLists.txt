GETCONFIG(cfgvaren "RPC")
string(COMPARE EQUAL "${cfgvaren}" y _cmp)
if (_cmp)
    file (GLOB var "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
    foreach (varf ${var})
        target_sources(${PROJECT_NAME}.elf PRIVATE ${varf})
    endforeach()

    target_include_directories(${PROJECT_NAME}.elf PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include")
endif()

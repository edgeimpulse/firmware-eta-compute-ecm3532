GETCONFIG(clien "CLI")
string(COMPARE EQUAL "${clien}" y _cmp)
if (_cmp)
    file (GLOB cli "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
foreach (clif ${cli})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${clif})

    target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include")
endforeach()
else()
    target_sources(${PROJECT_NAME}.elf PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src/printf-stdarg.c")
endif()


file(GLOB CLI
    "${CMAKE_CURRENT_LIST_DIR}/*c")
foreach (clisrc ${CLI})
    target_sources(freertos PRIVATE ${clisrc})
endforeach()
target_include_directories(freertos PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}")

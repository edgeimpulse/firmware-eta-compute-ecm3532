file (GLOB cspcommon "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
                     "${CMAKE_CURRENT_LIST_DIR}/src/*.s")
foreach (appf ${cspcommon})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${appf})
endforeach()

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/inc"
    "${CMAKE_CURRENT_LIST_DIR}/mathlib/inc")

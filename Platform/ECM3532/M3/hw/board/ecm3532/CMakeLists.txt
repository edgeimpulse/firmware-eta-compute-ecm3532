set(getboard "CONFIG_ECM3532_TARGET_BOARD")
CONFIGVAL(BRD ${getboard})
message("board : ${BRD}")
file (GLOB BSRC "${CMAKE_CURRENT_LIST_DIR}/${BRD}/src/*.c")
foreach (bsrc ${BSRC})
    target_sources(${PROJECT_NAME}.elf PRIVATE ${bsrc})
endforeach()

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/${BRD}/include")

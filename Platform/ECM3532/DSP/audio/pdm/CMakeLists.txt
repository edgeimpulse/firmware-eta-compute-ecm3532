set(var "CONFIG_PDM")
CONFIGVAL(varen ${var})
message("----Audio")
string(COMPARE EQUAL "${varen}" y _en)
if (_en)
    file (GLOB aud "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
    foreach (audf ${aud})
        message("----adding file ${audf}")
        target_sources(${PROJECT_NAME}.elf PRIVATE "${audf}")
    endforeach()
endif()

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/inc")

GETCONFIG(varen "CONFIG_DISPLAY_NHD0216")
string(COMPARE EQUAL "${varen}" y _cmp)
if (_cmp)
    target_sources(iodev PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/eta_devices_nhd0216.c")
    target_include_directories(${PROJECT_NAME}.elf PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}")
endif()

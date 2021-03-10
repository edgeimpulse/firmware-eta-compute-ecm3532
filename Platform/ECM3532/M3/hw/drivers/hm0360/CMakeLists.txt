GETCONFIG(varen "CONFIG_CAM_HM0360")
string(COMPARE EQUAL "${varen}" y _cmp)
if (_cmp)
    target_sources(iodev PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/eta_devices_hm0360.c")
    target_include_directories(${PROJECT_NAME}.elf PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}")
endif()

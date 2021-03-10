set(cfgvar "CONFIG_SENSOR_MANAGER")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y _cmp_outer)
if (_cmp_outer)
    target_sources(${PROJECT_NAME}.elf PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sensor.c")

    set(cfgvar "CONFIG_I2C_2")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(${PROJECT_NAME}.elf PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/dspI2c.c")
    endif()
endif() #_cmp_outer

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/inc")
    #    "${CMAKE_CURRENT_SOURCE_DIR}/inc/common")

##SRC packaging
GETCONFIG(varen "SM_DSP_SRC_PACKAGE")
string(COMPARE EQUAL "${varen}" y _cmp1)
if (NOT _cmp1)
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property
    "${ignore_list};sensor_mgr/src")
endif()



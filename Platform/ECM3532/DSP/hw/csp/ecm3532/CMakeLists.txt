###Including peripharal csp files based on which peripheral is enabled.

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
    ##Include dma, isr and io are always included.
    target_sources(csp PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src/eta_csp_io.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/eta_csp_mailbox.c"
        "${CMAKE_CURRENT_LIST_DIR}/src/eta_csp_isr.c")

    set(cfgvar "CONFIG_DSP_HAL_DMA")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(csp PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_csp_dma.c")
    endif()

    set(cfgvar "CONFIG_I2C_2")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(csp PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_csp_i2c.c")
    endif()

    set(cfgvar "CONFIG_SPI_DSP_MASTER")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(csp PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_csp_spi.c")
    endif()

    target_include_directories(csp PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/inc"
        "${CMAKE_CURRENT_LIST_DIR}/nxp/inc"
        "${CMAKE_CURRENT_LIST_DIR}/reg/inc"
        "${CMAKE_CURRENT_LIST_DIR}/../common/inc"
        "${CMAKE_CURRENT_LIST_DIR}/../common/mathlib/inc"
        )

endif () #if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")

target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/inc"
    "${CMAKE_CURRENT_LIST_DIR}/nxp/inc"
    "${CMAKE_CURRENT_LIST_DIR}/../common/mathlib/inc"
    "${CMAKE_CURRENT_LIST_DIR}/reg/inc")

####src packaging options
GETCONFIG(varen "CSP_DSP_SRC_PACKAGE")
string(COMPARE EQUAL "${varen}" y _cmp1)
if (NOT _cmp1)
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property
    "${ignore_list};ecm3532/src")
endif()





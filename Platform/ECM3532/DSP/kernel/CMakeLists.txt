set(cfgvar "CONFIG_NN_KERNELS")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y _cmp_outer)
if (_cmp_outer)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
    set(cfgvar "CONFIG_CONV_RELU_AVGPOOL_2X2")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_convolve_CHW_q7_ker2x2_stride1_pad0_relu_avgPool_2x2_stride2_pad0_dsp.c")
    endif()

    set(cfgvar "CONFIG_CONV_RELU_AVGPOOL_3X3")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_convolve_CHW_q7_ker3x3_stride1_pad0_relu_avgPool_2x2_stride2_pad0_dsp.c")
    endif()

    set(cfgvar "CONFIG_PW_CONV_FAST")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_pw_conv2d_q7.c")
    endif()

    set(cfgvar "CONFIG_CONV_RELU_3X3_STRIDE2")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_conv2d_q7_chw_ker3x3_stride2_pad0_relu.c")
    endif()

    set(cfgvar "CONFIG_DS_CONV_RELU_3X3_STRIDE2")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_ds_conv2d_q7_chw_ker3x3_stride2_pad0_relu.c")
    endif()

    set(cfgvar "CONFIG_CONV_RELU_2X2_STRIDE2")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_conv2d_q7_chw_ker2x2_stride2_pad0_relu.c")
    endif()

    set(cfgvar "CONFIG_CONV_RELU_3X3_STRIDE1")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu.c")
    endif()

    set(cfgvar "CONFIG_DS_CONV_RELU_3X3_STRIDE1")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_ds_conv2d_q7_chw_ker3x3_stride1_pad0_relu.c")
    endif()

    set(cfgvar "CONFIG_CONV_RELU_3X3_STRIDE1_EX")
    CONFIGVAL(cfgvaren ${cfgvar})
    string(COMPARE EQUAL "${cfgvaren}" y _cmp)
    if (_cmp)
        target_sources(dspkernel PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/src/eta_conv2d_q7_chw_ker3x3_stride1_pad0_relu_ex.c")
    endif()

target_include_directories(dspkernel PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/inc"
    "${CMAKE_CURRENT_LIST_DIR}/inc/common")


endif() #if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
endif() #_cmp_outer


target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/inc"
    "${CMAKE_CURRENT_LIST_DIR}/inc/common")

###src packaging
GETCONFIG(varen "NN_KERNEL_DSP_SRC_PACKAGE")
string(COMPARE EQUAL "${varen}" y _cmp1)
if (NOT _cmp1)
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property
    "${ignore_list};kernel/src")
endif()




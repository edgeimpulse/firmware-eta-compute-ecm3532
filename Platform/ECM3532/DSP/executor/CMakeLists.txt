set(cfgvar "CONFIG_EXECUTOR")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y _cmp_outer)
if (_cmp_outer)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
   target_sources(executor PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src/executor.c")

    target_include_directories(executor PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/inc")

    target_include_directories(executor PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/inc/tmp_test_vec")

    target_include_directories(executor PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../framework/inc")

    target_include_directories(executor PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../kernel/inc")

    target_include_directories(executor PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/../eta_math/inc")

endif() # if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
endif() #_cmp_outer


target_include_directories(${PROJECT_NAME}.elf PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/inc")

set(cfgvar "CONFIG_EXECUTOR_UNIT_TEST")
CONFIGVAL(cfgvaren ${cfgvar})
string(COMPARE EQUAL "${cfgvaren}" y _cmp)
if (_cmp)
    target_include_directories(${PROJECT_NAME}.elf PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/inc/tmp_test_vec")
endif()

## SRC packaging
GETCONFIG(varen "DSP_EXECUTOR_SRC_PACKAGE")
string(COMPARE EQUAL "${varen}" y _cmp1)
if (NOT _cmp1)
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property
    "${ignore_list};executor/src")
endif()



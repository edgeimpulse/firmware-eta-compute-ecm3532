set(getsoc "CONFIG_SOC")
CONFIGVAL(soc ${getsoc})

if (EXISTS "${PROJ_DIR}/hw/hal/${soc}/uart/src" OR
        EXISTS "${PROJ_DIR}/hw/hal/common/uart/src")
    target_include_directories(hal PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
endif ()

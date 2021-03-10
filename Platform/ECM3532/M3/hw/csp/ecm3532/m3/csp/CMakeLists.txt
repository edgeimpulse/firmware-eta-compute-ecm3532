set(getsoc "CONFIG_SOC")
CONFIGVAL(soc ${getsoc})

if("${soc}" STREQUAL "ecm3532")

    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src")
        target_include_directories(csp PUBLIC
            "${CMAKE_CURRENT_LIST_DIR}/../../../common/inc")
        target_include_directories(csp PUBLIC "${CMAKE_CURRENT_LIST_DIR}/inc")
        file (GLOB cspsrc "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
        list(REMOVE_ITEM cspsrc ${CMAKE_CURRENT_LIST_DIR}/src/eta_csp_io.c)
        foreach (cspf ${cspsrc})
            target_sources(csp PRIVATE ${cspf})
        endforeach()
    else ()
        include_directories("${CMAKE_CURRENT_LIST_DIR}/inc")
    endif ()

    GETCONFIG(varen "CSP_SRC_PACKAGE")
    string(COMPARE EQUAL "${varen}" y _cmp1)
else ()
    set(_cmp1 0)
endif ()

if (NOT _cmp1) 
    get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
    set_property(GLOBAL PROPERTY ignore_list_property
    "${ignore_list};csp/src")
endif()

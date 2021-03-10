set(getsoc "CONFIG_SOC")
CONFIGVAL(soc ${getsoc})
get_property(ignore_list GLOBAL PROPERTY ignore_list_property)
SUBDIRLIST(SDIRS ${CMAKE_CURRENT_LIST_DIR})
FOREACH(subdir ${SDIRS})
    if("${subdir}" STREQUAL ${soc} OR
            "${subdir}" STREQUAL "common")
        ADD_SUBDIRECTORY(${subdir})
    else ()
        set_property(GLOBAL PROPERTY ignore_list_property
            "${ignore_list};${subdir}")
    endif ()
ENDFOREACH()
